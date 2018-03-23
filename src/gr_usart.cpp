#include "gr_usart.h"
#include "gr_gpio.h"

GRUsart::GRUsart(USART_TypeDef * p,u32 b,u8 pclk)
	:m_rec_flag(false),m_rec_enable(false),m_usart_baud(b),m_usart_port(p)
{
	if(!pclk)m_usart_pclk=GRCore::SystemFreq();//如果等于0则取当前主频
	init(m_usart_port,m_usart_baud,m_usart_pclk);
	switch((u32)p)
	{
		case USART1_BASE:
			if(GRMacro::USART1_REC_ENABLE){/*注册中断*/}
			break;
		case USART2_BASE:
			if(GRMacro::USART2_REC_ENABLE){/*注册中断*/}
			break;
		case USART3_BASE:
			if(GRMacro::USART3_REC_ENABLE){/*注册中断*/}
			break;
	}
}

void GRUsart::init(USART_TypeDef* p,u32 b,u8 pclk)
{
	bool rec_int=false;//选择是否使能串口接收中断
	u8 irq_ch;
	//时钟及端口初始化
	switch((u32)p)
	{
		case USART1_BASE:
			if(GRMacro::AFIO_AUTO_INIT)//如果使用自动初始化
				//建议不使用自动初始化，因为会大大增加端口冲突的可能性
			{
				GRGpio::init(GPIOA,9,GRGpio::AFPushPull,GRGpio::Speed_50MHz);
				GRGpio::init(GPIOA,10,GRGpio::NormalInput);
			}
			m_usart_rcc=14;
			RCC->APB2ENR|=1<<m_usart_rcc;
		
			rec_int=GRMacro::USART1_REC_ENABLE;
			irq_ch=USART1_IRQn;
		
			GRCore::CIRQReg(GRCore::Usart1,this);//注册类中断
		
			break;
		case USART2_BASE:
			m_usart_pclk>>=1;//时钟频率减半
			if(GRMacro::AFIO_AUTO_INIT)
			{
				GRGpio::init(GPIOA,2,GRGpio::AFPushPull,GRGpio::Speed_50MHz);
				GRGpio::init(GPIOA,3,GRGpio::NormalInput);
			}
			m_usart_rcc=17;
			RCC->APB1ENR|=1<<m_usart_rcc;
		
			rec_int=GRMacro::USART2_REC_ENABLE;
			irq_ch=USART2_IRQn;
		
			GRCore::CIRQReg(GRCore::Usart2,this);//注册类中断
		
			break;
		case USART3_BASE:
			m_usart_pclk>>=1;
			if(GRMacro::AFIO_AUTO_INIT)
			{
				GRGpio::init(GPIOB,10,GRGpio::AFPushPull,GRGpio::Speed_50MHz);
				GRGpio::init(GPIOB,11,GRGpio::NormalInput);
			}
			m_usart_rcc=18;
			RCC->APB1ENR|=1<<m_usart_rcc;
		
			rec_int=GRMacro::USART3_REC_ENABLE;
			irq_ch=USART2_IRQn;
		
			GRCore::CIRQReg(GRCore::Usart3,this);//注册类中断
		
			break;
		default:return;//错误
	}
	
	setBaud(b);//设置波特率
	
	if(rec_int)
	{
		m_usart_port->CR1|=1<<8;	//PE中断使能
		m_usart_port->CR1|=1<<5;	//接收缓冲区非空中断使能
		GRCore::NvicInit(3,1,irq_ch,2);
		m_rec_enable=true;
	}
}
void GRUsart::setBaud(u32 baud)
{
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)((float)m_usart_pclk*1000000)/(baud*16);//得到USARTDIV
	mantissa=(u16)temp;				 //得到整数部分
	fraction=(temp-mantissa)*16; //得到小数部分	 
  mantissa<<=4;
	mantissa+=fraction; 
	
	if((u32)m_usart_port==USART1_BASE)
	{
		RCC->APB2RSTR|=1<<m_usart_rcc;   //复位串口1
		RCC->APB2RSTR&=~(1<<m_usart_rcc);//停止复位	  
	}
	else
	{
		RCC->APB1RSTR|=1<<m_usart_rcc;   //复位串口2、3
		RCC->APB1RSTR&=~(1<<m_usart_rcc);//停止复位	 
	}
	//波特率设置
 	m_usart_port->BRR=mantissa; // 波特率设置	 
	m_usart_port->CR1|=0X200C;  //1位停止,无校验位.
}

void GRUsart::sendByte(u8 byte)
{
	while((m_usart_port->SR&0X40)==0);
	m_usart_port->DR=byte;
}

void GRUsart::sendBuf(u8* buf,u32 size)
{
	for(u32 i=0;i<size;++i)
	{
		sendByte(buf[i]);
	}
}

void GRUsart::sendStr(const char* str)
{
	while(*str != '\0')
	{
		sendByte(*str++);
	}
}

//////数据包解析////////
GRUsartPkg::GRUsartPkg(USART_TypeDef * p,u32 b,u8 pclk)
	:GRUsart(p,b,pclk)
{
	m_recv_format_sheet=new GRLinkedList<Block>;
	recv_data=new GRLinkedList<u8>;
	//recv_format_sheet->rstPtr(0);
	return;
}

GRUsartPkg::~GRUsartPkg()
{
	delete m_recv_format_sheet;
	delete recv_data;
}

void GRUsartPkg::add_format(Format t_b,u8 data)
{
	Block tmp;
	tmp.data=data;
	tmp.type=t_b;	
	m_recv_format_sheet->addToTail(tmp);
}

void GRUsartPkg::clear_format_sheet()
{
	m_recv_format_sheet->clear();
}

void GRUsartPkg::CIRQ()
{
	if(m_recv_format_sheet->isEmpty())//如果没有设置格式表
		return;//直接跳出
	
	//static u32	cnt=0;
	static bool	f_SOF=false;
	static GRLinkedList<Block>::iterator it = m_recv_format_sheet->head();//链表迭代器，初始化为链表的头部
	u8 DR=getByte();
	//Block		tmp;

	/*等待SOF*/
	if(!f_SOF)
	{
		if((*it).data==DR)//SOF首字节正确
		{
			f_SOF=true;//标记收到SOF的第一个byte
			recv_data->clear();//清空接收区
			resetRecFlag();//清除接收标志
			return;
		}
		else
		{
			recv_data->clear();//清空接收区
			return;
		}
	}
	else//已经收到过SOF（第一字节）
	{
		++it;
		if(it.hasNext())//历遍样式表
		{
			if((*it).type==SOF || (*it).type==EOF)//如果收到SOF或EOF
			{
				if((*it).data!=DR)//错误的SOF或EOF
				{
					it = m_recv_format_sheet->head();//复位格式表迭代器
					recv_data->clear();//清空接收区
					f_SOF=false;
					return;
				}
			}
			else if((*it).type==Check)
			{
				recv_ver(DR);//调用虚校验方法
			}
			else
			{
				recv_data->addToTail(DR);//保存拆掉校验位、S/EOF的数据包
			}
		}
		else
		{
			setRecFlag();//置位接收完成标志
			disableRec();//关闭接收
			acknowledge();//应答
			it = m_recv_format_sheet->head();//复位格式表迭代器
			f_SOF=false;
			return;
		}
	}
	
}

/*********中断服务入口定义**********/
/*实现从硬件中断跳转到类中断*/
GRCFunc void USART1_IRQHandler(void)
{
	if(USART1->SR&(1<<5))
	{	
		if((u32)GRCore::CIRQPtr(GRCore::Usart1))//确认指针已经分配了内存以防死机
			if(((GRUsart*)GRCore::CIRQPtr(GRCore::Usart1))->isRecEnable())
				((GRUsart*)GRCore::CIRQPtr(GRCore::Usart1))->CIRQ();//跳入注册的类中断
	}
	USART1->SR&=~(1<<5);
	return;
}

GRCFunc void USART2_IRQHandler(void)
{
	if(USART2->SR&(1<<5))
	{	
		if(GRCore::CIRQPtr(GRCore::Usart2))//确认指针已经分配了内存以防死机
			if(((GRUsart*)GRCore::CIRQPtr(GRCore::Usart2))->isRecEnable())
				((GRUsart*)GRCore::CIRQPtr(GRCore::Usart2))->CIRQ();//跳入注册的类中断
	}
	USART2->SR&=~(1<<5);
	return;
}

GRCFunc void USART3_IRQHandler(void)
{
	if(USART3->SR&(1<<5))
	{	
		if(GRCore::CIRQPtr(GRCore::Usart3))//确认指针已经分配了内存以防死机
			if(((GRUsart*)GRCore::CIRQPtr(GRCore::Usart3))->isRecEnable())
					((GRUsart*)GRCore::CIRQPtr(GRCore::Usart3))->CIRQ();//跳入注册的类中断
	}
	USART3->SR&=~(1<<5);
	return;
}

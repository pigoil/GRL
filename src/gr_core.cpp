#include "gr_core.h"

u8	GRCore::pll=GRMacro::SYSTEM_PLL;
u8	GRCore::sys_freq=grRound(GRMacro::XTAL * GRMacro::SYSTEM_PLL);
u8	GRCore::ticks_per_us=sys_freq / 8;
u16	GRCore::ticks_per_ms=ticks_per_us * 1000;

void* GRCore::CIRQ_table[GRCore::CH_CNT]={(void*)0};

u8 GRCore::SystemPll(){return pll;}
u8 GRCore::SystemFreq(){return sys_freq;}

//系统时钟初始化,在main之前会调用，保证主频设置最先执行
GRCFunc void SystemInit (void)
{
	GRCore::SystemClockInit(GRMacro::SYSTEM_PLL);
	GRCore::SystemIoInit();
	return;
}

//时钟初始化
void GRCore::SystemClockInit(u8 p)
{
	pll=p;//记录PLL
	sys_freq=grRound(GRMacro::XTAL * p);//计算主频
	unsigned char temp=0;   
	RccDeInit();		  //复位并配置向量表
 	RCC->CR|=0x00010000;  //外部高速时钟使能HSEON
	while(!(RCC->CR>>17));//等待外部时钟就绪
	while(!((RCC->CR>>1) & 0x01));//等待外部时钟就绪
	RCC->CFGR=0X00000400; //APB1=DIV2;APB2=DIV1;AHB=DIV1;
	p-=2;//抵消2个单位
	RCC->CFGR|=p<<18;   //设置PLL值 2~16
	RCC->CFGR|=1<<16;	  //PLLSRC ON 
	FLASH->ACR|=0x32;	  //FLASH 2个延时周期

	RCC->CR|=0x01000000;  //PLLON
	while(!(RCC->CR>>25));//等待PLL锁定
	RCC->CFGR|=0x00000002;//PLL作为系统时钟
	while(temp!=0x02)     //等待PLL作为系统时钟设置成功
	{   
		temp=RCC->CFGR>>2;
		temp&=0x03;
	}
	
	SysTick->CTRL&=~(1<<2);			//SYSTICK使用外部时钟源
	ticks_per_us=sys_freq / 8;		//每微秒滴答数
	ticks_per_ms=ticks_per_us * 1000;		//每毫秒滴答数
	SysTick->LOAD=GRMacro::SYSTICK_RELOAD;//设置装载值
	SysTick->CTRL|=1<<0;   	//开启SYSTICK
}

void GRCore::SleepUs(u32 nus)
{
	u32 reload=SysTick->LOAD;
	uc32 ENABLE_BIT=0x01;
	uc32 CNTFLAB_BIT=1<<16;
	u32 tnow,told=SysTick->VAL;
	u32 escape=nus*ticks_per_us;
	u32 cnt=0;
	while(1)
	{
		u32 temp = SysTick->CTRL;
		tnow = SysTick->VAL;//读取当前计数器值
		if((temp&CNTFLAB_BIT) == 0)//微秒延时优先考虑无越0
		{
			cnt+=(told-tnow);
		}
			else
			{
				cnt+=(reload-tnow+told);
			}
		told=tnow;//更新told
		if(cnt>=escape)break;
		if((temp&ENABLE_BIT)== 0)break;
	}
}

void GRCore::SleepMs(u32 nms)
{
	u32 reload=SysTick->LOAD;
	uc32 ENABLE_BIT=0x01;
	uc32 CNTFLAB_BIT=1<<16;
	u32 tnow,told=SysTick->VAL;
	for(;nms>0;--nms)
	{
		u32 cnt=0;
		while(1)
		{
			u32 temp = SysTick->CTRL;
			tnow = SysTick->VAL;//读取当前计数器值
			if(temp&CNTFLAB_BIT)//如果越零
			{
				cnt+=(reload-tnow+told);
			}
				else if(tnow!=told)//没越零,tnow有变化
				{
					cnt+=(told-tnow);
				}
					else continue;//tnow无变化
			told=tnow;//更新told
			if(cnt>=ticks_per_ms)break;//
			if((temp&ENABLE_BIT)== 0)break;
		}
	}
}

void GRCore::SleepSec(double time)
{
	u32 sec=(u32)time;
	u32 us=(u32)((time-(double)sec)*1000000.0);
	u16 ms=us/1000;
	us%=1000;
	for(;sec>0;--sec)
	{
		SleepMs(1000);
	}
	SleepMs(ms);
	SleepUs(us);
}

void GRCore::SleepNop(u32 n)
{
	for(u32 i=n*sys_freq;i>0;--i);
}

void GRCore::SleepInt(vu32 n)
{
	for(u32 i=n*sys_freq;i>0;--i);
}
//设置中断向量表
void GRCore::NvicSetVectorTable(u32 NVIC_VectTab, u32 Offset)
{
	SCB->VTOR = NVIC_VectTab|(Offset & (u32)0x1FFFFF80);//设置NVIC的向量表偏移寄存器
	//用于标识向量表是在CODE区还是在RAM区
}
//
void GRCore::NvicExConfig(GPIO_TypeDef* GPIOx,u8 BITx,TriMode TRIM)
{
	u8 EXTADDR;
	u8 EXTOFFSET;
	u32 port=((u32)GPIOx-(u32)GPIOA)/0x400;
	EXTADDR=BITx/4;//得到中断寄存器组的编号
	EXTOFFSET=(BITx%4)*4;
						   
	RCC->APB2ENR|=0x01;//使能io复用时钟

	AFIO->EXTICR[EXTADDR]&=~(0x000F<<EXTOFFSET);//清除原来设置！！！
	AFIO->EXTICR[EXTADDR]|=port<<EXTOFFSET;//EXTI.BITx映射到GPIOx.BITx
	
	//自动设置
	EXTI->IMR|=1<<BITx;//  开启line BITx上的中断
	//EXTI->EMR|=1<<BITx;//不屏蔽line BITx上的事件 (如果不屏蔽这句,在硬件上是可以的,但是在软件仿真的时候无法进入中断!)
 	if(TRIM&0x01)EXTI->FTSR|=1<<BITx;//line BITx上事件下降沿触发
	if(TRIM&0x02)EXTI->RTSR|=1<<BITx;//line BITx上事件上升降沿触发
}
//
void GRCore::NvicPriorityGroupConfig(u8 NVIC_Group)
{
	u32 temp,temp1;	  
	temp1=(~NVIC_Group)&0x07;//取后三位
	temp1<<=8;
	temp=SCB->AIRCR;  //读取先前的设置
	temp&=0X0000F8FF; //清空先前分组
	temp|=0X05FA0000; //写入钥匙
	temp|=temp1;	   
	SCB->AIRCR=temp;  //设置分组
}
//
void GRCore::NvicInit(u8 NVIC_PreemptionPriority,u8 NVIC_SubPriority,u8 NVIC_Channel,u8 NVIC_Group)
{
	u32 temp;	
	u8 IPRADDR=NVIC_Channel/4;  //每组只能存4个,得到组地址
	u8 IPROFFSET=NVIC_Channel%4;//在组内的偏移
	IPROFFSET=IPROFFSET*8+4;    //得到偏移的确切位置
	NvicPriorityGroupConfig(NVIC_Group);//设置分组
	temp=NVIC_PreemptionPriority<<(4-NVIC_Group);	  
	temp|=NVIC_SubPriority&(0x0f>>NVIC_Group);
	temp&=0xf;//取低四位

	if(NVIC_Channel<32)NVIC->ISER[0]|=1<<NVIC_Channel;//使能中断位(要清除的话,相反操作就OK)
	else NVIC->ISER[1]|=1<<(NVIC_Channel-32);    
	NVIC->IP[IPRADDR]|=temp<<IPROFFSET;//设置响应优先级和抢断优先级
}
//复位所有时钟寄存器
void GRCore::RccDeInit()
{
 	RCC->APB1RSTR = 0x00000000;//复位结束
	RCC->APB2RSTR = 0x00000000; 
	  
	RCC->AHBENR = 0x00000014;  //睡眠模式闪存和SRAM时钟使能.其他关闭.
	RCC->APB2ENR = 0x00000000; //外设时钟关闭.
	RCC->APB1ENR = 0x00000000;   
	RCC->CR |= 0x00000001;     //使能内部高速时钟HSION
	RCC->CFGR &= 0xF8FF0000;   //复位SW[1:0],HPRE[3:0],PPRE1[2:0],PPRE2[2:0],ADCPRE[1:0],MCO[2:0]
	RCC->CR &= 0xFEF6FFFF;     //复位HSEON,CSSON,PLLON
	RCC->CR &= 0xFFFBFFFF;     //复位HSEBYP
	RCC->CFGR &= 0xFF80FFFF;   //复位PLLSRC, PLLXTPRE, PLLMUL[3:0] and USBPRE
	RCC->CIR = 0x00000000;     //关闭所有中断
	//配置向量表
#ifdef  VECT_TAB_RAM
	NvicSetVectorTable(SRAM_BASE, 0x0);
#else   
	NvicSetVectorTable(FLASH_BASE,0x0);
#endif
}
////开启所有中断
//__asm void GRCore::ASM_INTX_ENABLE()
//{
//	CPSIE I;	
//}
////关闭所有中断
//__asm void GRCore::ASM_INTX_DISABLE()
//{
//	CPSID I;
//}
////WFI指令
//__asm void GRCore::ASM_WFI()
//{
//	WFI;
//}
////设置栈顶地址
//__asm void GRCore::ASM_MSR_MSP(u32 addr)
//{
//	MSR MSP, r0 			//set Main Stack value
//	BX r14
//}

//开启所有中断
void GRCore::ASM_INTX_ENABLE()
{
	__asm("cpsie i");	
}
////关闭所有中断
void GRCore::ASM_INTX_DISABLE()
{
	__asm("cpsid i");
}
//WFI指令
void GRCore::ASM_WFI()
{
	__asm("WFI");
}
////设置栈顶地址
//void GRCore::ASM_MSR_MSP(u32 addr)
//{
//	__asm("MSR MSP,r0"); 			//set Main Stack value
//	__asm("bx r14");
//}

//
void GRCore::JtagSet(JtagMode mode)
{
	u32 temp;
	temp=(u32)mode;
	temp<<=25;
	RCC->APB2ENR|=1<<0;     //开启辅助时钟
	AFIO->MAPR&=0XF8FFFFFF; //清除MAPR的[26:24]
	AFIO->MAPR|=temp;       //设置jtag模式
}

u8 GRCore::CIRQReg(CIRQChannel ch,void *ptr)
{
	if(ch>=CH_CNT)return 1;
	CIRQ_table[ch]=ptr;
	return 0;
}


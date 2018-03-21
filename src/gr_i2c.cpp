#include "gr_i2c.h"

GRI2c::GRI2c(GRGpio& _sck,GRGpio& _sda)
	:iic_sck(_sck),iic_sda(_sda)
{
	sda_port=iic_sda.port();
	sda_pin=iic_sda.pin();
}

void GRI2c::sda_out()
{
	u8 p=sda_pin;
	if(p<8)
	{
		sda_port->CRL&=~(0xF	<<	(p<<2));
		sda_port->CRL|=	3	<<	(p<<2);
	}
	else
	{
		p-=8;
		sda_port->CRH&=~(0xF	<<	(p<<2));
		sda_port->CRH|=	3	<<	(p<<2);
	}
}

void GRI2c::sda_in()
{
	u8 p=sda_pin;
	if(p<8)
	{
		sda_port->CRL&=~(0xF	<<	(p<<2));
		sda_port->CRL|=	8	<<	(p<<2);
	}
	else
	{
		p-=8;
		sda_port->CRH&=~(0xF	<<	(p<<2));
		sda_port->CRH|=	8	<<	(p<<2);
	}
}

void GRI2c::iic_start()
{
	sda_out();     //sda线输出
	iic_sda=1;	  	  
	iic_sck=1;
	GRCore::SleepUs(GRMacro::IIC_US);
 	iic_sda=0;//START:when CLK is high,DATA change form high to low 
	GRCore::SleepUs(GRMacro::IIC_US);
	iic_sck=0;//钳住I2C总线，准备发送或接收数据 
}	  

//产生IIC停止信号
void GRI2c::iic_stop()
{
	sda_out();     //sda线输出
	iic_sck=1;
	iic_sda=0;//STOP:when CLK is high DATA change form low to high
 	GRCore::SleepUs(GRMacro::IIC_US);
	iic_sda=1;//发送I2C总线结束信号
	GRCore::SleepUs(GRMacro::IIC_US);		
	//iic_sck=1; 
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 GRI2c::iic_wait_ack()
{
	u8 ucErrTime=0;
	sda_in();      //SDA设置为输入  
	iic_sck=1;GRCore::SleepUs(GRMacro::IIC_US); 
	iic_sck=1;GRCore::SleepUs(GRMacro::IIC_US);
	while(iic_sda)
	{
		++ucErrTime;
		if(ucErrTime>250)
		{
			iic_stop();
			return 1;
		}
	}
	iic_sck=0;//时钟输出0 	   
	return 0;  
} 
//产生ACK应答
void GRI2c::iic_ack()
{
	iic_sck=0;
	sda_out();
	iic_sda=0;
	GRCore::SleepUs(GRMacro::IIC_US);
	iic_sck=1;
	GRCore::SleepUs(GRMacro::IIC_US);
	iic_sck=0;
}
//不产生ACK应答		    
void GRI2c::iic_nack(void)
{
	iic_sck=0;
	sda_out();
	iic_sda=1;
	GRCore::SleepUs(GRMacro::IIC_US);
	iic_sck=1;
	GRCore::SleepUs(GRMacro::IIC_US);
	iic_sck=0;
}					 				     
//IIC发送一个字节
void GRI2c::sendByte(u8 txd)
{                        
	u8 t;   
	sda_out(); 	    
	iic_sck=0;//拉低时钟开始数据传输
	for(t=0;t<8;t++)
	{              
			iic_sda=(txd&0x80)>>7;
			txd<<=1; 	  
			GRCore::SleepUs(GRMacro::IIC_US);   //对TEA5767这三个延时都是必须的
			iic_sck=1;
			GRCore::SleepUs(GRMacro::IIC_US); 
			iic_sck=0;	
			GRCore::SleepUs(GRMacro::IIC_US);
	}	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
u8 GRI2c::readByte(AckMode ack)
{
	unsigned char i,receive=0;
	sda_in();//SDA设置为输入
  for(i=0;i<8;i++ )
	{
		iic_sck=0; 
		GRCore::SleepUs(GRMacro::IIC_US);
		iic_sck=1;
		receive<<=1;
		if(iic_sda)receive++;   
		GRCore::SleepUs(GRMacro::IIC_US); 
  }					 
	if (ack==Nack)
			iic_nack();//发送nACK
	else
			iic_ack(); //发送ACK   
	return receive;
}

//单字节写入*******************************************

u8 GRI2c::write(unsigned char SlaveAddress,unsigned char REG_Address,unsigned char REG_data)	
{
	iic_start();
	sendByte(SlaveAddress);   //发送设备地址+写信号
	if(iic_wait_ack())return 1;
	sendByte(REG_Address);   //设置低起始地址      
	if(iic_wait_ack())return 1;
	sendByte(REG_data);//发送数据
	if(iic_wait_ack())return 1;
	iic_stop(); 
	//delay_ms(10);
	return 0;
}

//单字节读取*****************************************
u8 GRI2c::read(unsigned char SlaveAddress,unsigned char REG_Address)
{  
	unsigned char REG_data=0;     	
	iic_start();
	sendByte(SlaveAddress); //发送设备地址+写信号
	iic_wait_ack();
	sendByte((u8) REG_Address);   //设置低起始地址      
	iic_wait_ack();
	iic_start();
	sendByte(SlaveAddress+1);//发送设备地址+读信号
	iic_wait_ack();
	REG_data=readByte(Nack);//读数据
	iic_stop();
	return REG_data;
}						      

//连续读写

u8 GRI2c::write(unsigned char SlaveAddress,unsigned char REG_Address,unsigned char* REG_data,u32 cnt)
{  
	iic_start();
	sendByte(SlaveAddress);	   //发送写命令
	iic_wait_ack();
	sendByte(REG_Address);   //发送地址
	iic_wait_ack ();
	
	for(u32 i=0;i<cnt;++i)
	{
		sendByte(REG_data[i]); 
		iic_wait_ack(); 
	}
	
	iic_stop();//产生一个停止条件
	return 0;
}

u8 GRI2c::read(unsigned char SlaveAddress,unsigned char REG_Address,unsigned char* REG_data,u32 cnt)
{
	iic_start();
	sendByte(SlaveAddress);
	iic_wait_ack();
	sendByte(REG_Address);
	iic_wait_ack();
	iic_start();
	sendByte(SlaveAddress+1);
	iic_wait_ack();
	
	for(u32 i=0;i<cnt-1;++i)
	{
		REG_data[i]=readByte(GRI2c::Ack);
	}
	REG_data[cnt-1]=readByte(GRI2c::Nack);
	
	iic_stop();
	return 0;
}

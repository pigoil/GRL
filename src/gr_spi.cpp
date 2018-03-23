#include "gr_spi.h"
#include "gr_gpio.h"

GRSpi::GRSpi(SPI_TypeDef* port,ClockSpeed speed,\
				ClockPolarity cpol,ClockPhase cpha,\
				DataFormat d_f,FrameFormat f_f)
	:m_spi_port(port)
{
	init(port,speed,cpol,cpha,d_f,f_f);
}

GRSpi::~GRSpi()
{}

void GRSpi::init(SPI_TypeDef* port,ClockSpeed speed,\
				ClockPolarity cpol,ClockPhase cpha,\
				DataFormat d_f,FrameFormat f_f)
{	
	switch((u32)port)
	{
		case SPI1_BASE:
			RCC->APB2ENR|=1<<12;
			if(GRMacro::AFIO_AUTO_INIT)
			{
				GRGpio(GPIOA,5,GRGpio::AFPushPull);
				GRGpio(GPIOA,6,GRGpio::AFPushPull);
				GRGpio(GPIOA,7,GRGpio::AFPushPull);
			}
			break;
		case SPI2_BASE:
			RCC->APB1ENR|=1<<14;
			if(GRMacro::AFIO_AUTO_INIT)
			{
				GRGpio(GPIOB,13,GRGpio::AFPushPull);
				GRGpio(GPIOB,14,GRGpio::AFPushPull);
				GRGpio(GPIOB,15,GRGpio::AFPushPull);
			}
			break;
		case SPI3_BASE:
			RCC->APB1ENR|=1<<15;
			if(GRMacro::AFIO_AUTO_INIT)
			{
				GRGpio(GPIOB,3,GRGpio::AFPushPull);
				GRGpio(GPIOB,4,GRGpio::AFPushPull);
				GRGpio(GPIOB,5,GRGpio::AFPushPull);
			}
			break;
		default:return;
	}
	port->CR1|=0<<10;//全双工
	port->CR1|=1<<9; //软件nss
	port->CR1|=1<<8;
	
	port->CR1|=1<<2; //SPI主机
	port->CR1|=d_f<<11;//8/16bit
	port->CR1|=f_f<<7;//MSN/LSB
	port->CR1|=cpol<<1;//CPOL
	port->CR1|=cpha<<0;//CPHA
	port->CR1|=1<<6;
	speedSet(speed);//Speed
	transByte(0xff);
}

void GRSpi::speedSet(ClockSpeed speed)
{
	m_spi_port->CR1&=0xFFC7;//清空
	m_spi_port->CR1|=speed<<3;
	SPI2->CR1|=1<<6; //SPI设备使能	  
}

u16 GRSpi::transByte(u16 data)
{
	u16 retry=0;				 
	while((m_spi_port->SR&1<<1)==0)//等待发送区空	
	{
		retry++;
		//if(retry>500)return 0;
	}			  
	m_spi_port->DR=data;	 	  //发送一个byte 
	retry=0;
	while((m_spi_port->SR&1<<0)==0) //等待接收完一个byte  
	{
		retry++;
		//if(retry>500)return 0;
	}	  						    
	return m_spi_port->DR;          //返回收到的数据		
}

#include "gr_gpio.h"

//单个GPIO
//输出模式
GRGpio::GRGpio(GPIO_TypeDef* port,u8 pin,OutputMode mode,OutputSpeed speed)
	:io_port(port),io_pin(pin),io_mode(mode<<2 | speed)
{
	init(port,pin,mode,speed);
}
//输入模式
GRGpio::GRGpio(GPIO_TypeDef* port,u8 pin,InputMode mode)
	:io_port(port),io_pin(pin),io_mode(mode<<2)
{
	init(port,pin,mode);
}

void GRGpio::init(GPIO_TypeDef* port,u8 pin,OutputMode mode,OutputSpeed speed)
{
	if(mode == GRGpio::AFPushPull || mode == GRGpio::AFOpenDrain)RCC->APB2ENR|=1<<0;
	u8 rcc=((vu32)port - (vu32)GPIOA)>>10;
	RCC->APB2ENR|=1<<(rcc+2);
	u8 temp = mode<<2 | speed;
	if(pin<8)
	{
		port->CRL&=~(0xF	<<	(pin<<2));
		port->CRL|=	temp	<<	(pin<<2);
	}
	else
	{
		pin-=8;
		port->CRH&=~(0xF	<<	(pin<<2));
		port->CRH|=	temp	<<	(pin<<2);
	}
}

void GRGpio::init(GPIO_TypeDef* port,u8 pin,InputMode mode)
{
	u8 rcc=((vu32)port - (vu32)GPIOA)>>10;
	RCC->APB2ENR|=1<<(rcc+2);
	u8 temp = mode<<2;
	if(pin<8)
	{
		port->CRL&=~(0xF	<<	(pin<<2));
		port->CRL|=	temp	<<	(pin<<2);
	}
	else
	{
		pin-=8;
		port->CRH&=~(0xF	<<	(pin<<2));
		port->CRH|=	temp	<<	(pin<<2);
	}
}

bool GRGpio::read()const
{
	return *BIT_ADDR((vu32)io_port+8,io_pin);
}

void GRGpio::write(bool b)const
{
	*BIT_ADDR((vu32)io_port+12,io_pin)=b;
}

bool GRGpio::StableHigh(u8 delay,u8 cnt)const
{
	for(;cnt>0;--cnt)
	{
		if(!read())return false;
		GRCore::SleepNop(delay);
	}
	return true;
}

bool GRGpio::StableLow(u8 delay,u8 cnt)const
{
	for(;cnt>0;--cnt)
	{
		if(read())return false;
		GRCore::SleepNop(delay);
	}
	return true;
}

GRGpio& GRGpio::operator= (bool b)
{
	write(b);
	return *this;
}

GRGpio::operator bool()const
{
	return read();
}

/**************************************************************/
//一组GPIO

GRGpioPort::GRGpioPort(GPIO_TypeDef* port):
		m_io_port(port)
{
	u8 rcc=((vu32)port - (vu32)GPIOA)>>10;
	RCC->APB2ENR|=1<<(rcc+2);
}

void GRGpioPort::init()
{
	m_io_port->CRL &= m_io_inv_crl;
	m_io_port->CRL |= m_io_crl;
	m_io_port->CRH &= m_io_inv_crh;
	m_io_port->CRH |= m_io_crh;
}

GRGpio GRGpioPort::pin(int p)
{
	return GRGpio(m_io_port,p);
}

GRGpioPort& GRGpioPort::operator<< (GRGpio::InputMode m)
{
	m_io_crl=0,m_io_crh=0;
	m_io_inv_crl=0xFFFFFFFF,m_io_inv_crh=0xFFFFFFFF;
	m_mode = m<<2;
	return *this;
}

GRGpioPort& GRGpioPort::operator<< (GRGpio::OutputMode m)
{
	m_io_crl=0,m_io_crh=0;
	m_io_inv_crl=0xFFFFFFFF,m_io_inv_crh=0xFFFFFFFF;
	m_mode = m<<2|GRGpio::Speed_50MHz;//默认50MHz速度
	return *this;
}

GRGpioPort& GRGpioPort::operator<< (GRGpio::OutputSpeed m)
{
	m_mode &= 0xC;
	m_mode |= m;//默认推挽输出
	return *this;
}

GRGpioPort& GRGpioPort::operator<< (int pin)
{
	if(pin<8)
	{
		m_io_crl |= (m_mode<<(pin<<2)); 
		m_io_inv_crl &= ~(0xF<<(pin<<2));
	}
	else
	{
		pin-=8;
		m_io_crh |= (m_mode<<(pin<<2));
		m_io_inv_crh &= ~(0xF<<(pin<<2));
	}
	return *this;
}

GRGpio GRGpioPort::operator[](int p)
{
	return pin(p);
}


#include "gr_adc.h"
//单次模式：
GRAdc::GRAdc(ADC_TypeDef* p ,AdcMode m)
	:m_port(p),m_mode(m)
{
	init(p,m);
	calibrate();//校准
}

void GRAdc::calibrate()
{
	m_port->CR2|=1<<3;
	while(ADC1->CR2&1<<3); //等待校准结束 
	m_port->CR2|=1<<2;        //开启AD校准	   
	while(ADC1->CR2&1<<2);  //等待校准结束	
}

void GRAdc::init(ADC_TypeDef* p ,AdcMode m)
{
	u8 rcc_bit;
	switch((u32)p)
	{
		case ADC1_BASE:
			rcc_bit=9;
			break;
		case ADC2_BASE:
			rcc_bit=10;
			break;
		case ADC3_BASE:
			rcc_bit=15;
			break;
		default: break;
	}

	RCC->APB2ENR|=1<<rcc_bit;
	RCC->APB2RSTR|=1<<rcc_bit;
	RCC->APB2RSTR&=~(1<<rcc_bit);
	RCC->CFGR&=~(3<<14);
	RCC->CFGR|=2<<14; //六分频
	
	p->CR1&=0XF0FFFF;//独立模式
	p->CR1|=0<<16;
	
	p->CR2&=~(1<<8);// 单通道/扫描模式
	p->CR2|=m;
	
	p->CR2&=~(1<<1);    //单次转换模式
	p->CR2&=~(7<<17);	   
	p->CR2|=7<<17;	   //软件控制转换  
	p->CR2|=1<<20;      //使用用外部触发(SWSTART)!!!	必须使用一个事件来触发
	p->CR2&=~(1<<11);   //右对齐	
	
	if(m==Single)
	{
		p->SQR1&=~(0XF<<20);
		p->SQR1|=0<<20;
	}
	p->CR2|=1<<0;	   //ADC上电
}

u16 GRAdc::result(AdcChannel ch, SampleTime t)
{
	u16 res=0;
	if(m_mode == Single)
	{
		if((u8)ch > 9)
		{
			m_port->SMPR1 &= ~(7<<(3*((u8)ch-10)));
			m_port->SMPR1 |= t<<(3*((u8)ch-10));
		}else
		{
			m_port->SMPR2 &= ~(7<<(3*((u8)ch)));
			m_port->SMPR2 |= t<<(3*((u8)ch));
		}

		m_port->SQR3  &= 0XFFFFFFE0;
		m_port->SQR3|=ch;
		m_port->CR2|=1<<22;
		while(!(ADC1->SR&1<<1));
		res = ADC1->DR;
	}
	return res;
}

float GRAdc::voltage(AdcChannel ch ,SampleTime t)
{
	u16 res=result(ch ,t);
	return (float)res/4096.0 * GRMacro::VREF;
}

u32 GRAdc::trueRand(AdcChannel ch)
{
	u32 res=0;
	for(u8 i=0;i<32;++i)
	{
		res|=((result(ch,Cycle1_5)&0x08)>>3)<<i;
	}
	return res;
}

/////温度传感器和电压参考

GRInnerTemperature::GRInnerTemperature()
	:GRAdc(ADC1,Single)
{
}

float GRInnerTemperature::temperature()
{
	float temp;
	ADC1->CR2|=1<<23;
	GRCore::SleepNop(10);
	temp=((GRMacro::TEMP_V25 - voltage(Ch16,Cycle71_5)) / GRMacro::TEMP_SLOPE) + 25;
	ADC1->CR2&=~(1<<23);
	return temp;
}

float GRInnerTemperature::vRefInt()
{
	float temp;
	ADC1->CR2|=1<<23;
	temp=voltage(Ch17,Cycle71_5);
	ADC1->CR2&=~(1<<23);
	return temp;
}

#include "gr_timer.h"

GRTimer::GRTimer(TIM_TypeDef* p,u16 arr,u16 psc,bool ui_en)
	:timer_port(p),timer_arr(arr),timer_psc(psc)
{
	init(p,arr,psc,ui_en);
}

void GRTimer::init(TIM_TypeDef* p,u16 arr,u16 psc,bool ui_en)
{
	u8 int_channel=0;
	GRCore::CIRQChannel cirq_ch;
	switch((u32)p)//时钟使能
	{
//		case TIM1_BASE:    ///高级定时器的中断？
//			RCC->APB2ENR|=1<<11;
//			break;
		case TIM2_BASE:
			RCC->APB1ENR|=1<<0;
			cirq_ch=GRCore::Tim2;
			int_channel=TIM2_IRQn;
			break;
		case TIM3_BASE:
			RCC->APB1ENR|=1<<1;
			cirq_ch=GRCore::Tim3;
			int_channel=TIM3_IRQn;
			break;
		case TIM4_BASE:
			RCC->APB1ENR|=1<<2;
			cirq_ch=GRCore::Tim4;
			int_channel=TIM4_IRQn;
			break;
		case TIM5_BASE:
			RCC->APB1ENR|=1<<3;
			cirq_ch=GRCore::Tim5;
			int_channel=TIM5_IRQn;
			break;
		case TIM6_BASE:
			RCC->APB1ENR|=1<<4;
			cirq_ch=GRCore::Tim6;
			int_channel=TIM6_IRQn;
			break;
		case TIM7_BASE:
			RCC->APB1ENR|=1<<5;
			cirq_ch=GRCore::Tim7;
			int_channel=TIM7_IRQn;
			break;
//		case TIM8_BASE:
//			RCC->APB2ENR|=1<<13;
//			break;
		default:
			return;
	}
	setArr(arr);
	setPsc(psc);
	if(ui_en)
	{
		timer_port->DIER|=1<<0;
		GRCore::NvicInit(1,3,int_channel,2);//如何控制优先级？
		/*抢占优先级和响应优先级相同时按照中断表中的顺序*/
		GRCore::CIRQReg(cirq_ch,this);
	}
	//不启动
}

void GRTimer::setArr(u16 a)
{
	timer_port->ARR=a;
	timer_arr=a;
}

void GRTimer::setPsc(u16 p)
{
	timer_port->PSC=p;
	timer_psc=p;
}

u32 GRTimer::freq()
{
	return (u32)GRCore::SystemFreq()*1000000/(timer_psc+1)/(timer_arr+1);
}

double GRTimer::period()
{
	double f=freq();//Hz
	f/=1000000.0;//MHz;
	return 1.0/(double)f;
}
/*******************PWM发生器*****************/
GRPwmGenerator::GRPwmGenerator(TIM_TypeDef* p,u16 arr,u16 psc,bool ui_en)
	:GRTimer(p,arr,psc,ui_en)
{
	
}

void GRPwmGenerator::outputConfig(OutputCh ch,bool o_en,PwmMode mode,OutPolarity pol)
{
	switch(ch)
	{
		case CH1:
			timer_port->CCMR1|=mode<<4;
			timer_port->CCMR1|=1<<3;//预装载使能
			timer_port->CCER|=(pol|o_en)<<0;
			break;
		case CH2:
			timer_port->CCMR1|=mode<<12;
			timer_port->CCMR1|=1<<11;//预装载使能
		timer_port->CCER|=(pol|o_en)<<4;
			break;
		case CH3:
			timer_port->CCMR2|=mode<<4;
			timer_port->CCMR2|=1<<3;//预装载使能
			timer_port->CCER|=(pol|o_en)<<8;
			break;
		case CH4:
			timer_port->CCMR2|=mode<<12;
			timer_port->CCMR2|=1<<11;//预装载使能
			timer_port->CCER|=(pol|o_en)<<12;
			break;
		default:return;
	}
}

void GRPwmGenerator::setCompareVal(OutputCh ch,u16 val)
{
	switch(ch)
	{
		case CH1:
			timer_port->CCR1=val;
			break;
		case CH2:
			timer_port->CCR2=val;
			break;
		case CH3:
			timer_port->CCR3=val;
			break;
		case CH4:
			timer_port->CCR4=val;
			break;
		default: return;
	}
}

u16 GRPwmGenerator::compareVal(OutputCh ch)
{
	switch(ch)
	{
		case CH1:
			return timer_port->CCR1;
		case CH2:
			return timer_port->CCR2;
		case CH3:
			return timer_port->CCR3;
		case CH4:
			return timer_port->CCR4;
		default: return 0;
	}
}

void GRPwmGenerator::setDuty(OutputCh ch,double duty)
{
	u16 tmp=duty*(timer_arr+1);
	setCompareVal(ch,tmp);
}

/********************中断服务入口*************************/
GRCFunc void TIM2_IRQHandler(void)
{
	if(TIM2->SR&(1<<0))
	{	
		if(GRCore::CIRQPtr(GRCore::Tim2))
			((GRTimer*)GRCore::CIRQPtr(GRCore::Tim2))->CIRQ();
	}
	TIM2->SR&=~(1<<0);
	return;
}

GRCFunc void TIM3_IRQHandler(void)
{
	if(TIM3->SR&(1<<0))
	{	
		if(GRCore::CIRQPtr(GRCore::Tim3))
			((GRTimer*)GRCore::CIRQPtr(GRCore::Tim3))->CIRQ();
	}
	TIM3->SR&=~(1<<0);
	return;
}

GRCFunc void TIM4_IRQHandler(void)
{
	if(TIM4->SR&(1<<0))
	{	
		if(GRCore::CIRQPtr(GRCore::Tim4))
			((GRTimer*)GRCore::CIRQPtr(GRCore::Tim4))->CIRQ();
	}
	TIM4->SR&=~(1<<0);
	return;
}

GRCFunc void TIM5_IRQHandler(void)
{
	if(TIM2->SR&(1<<0))
	{	
		if(GRCore::CIRQPtr(GRCore::Tim5))
			((GRTimer*)GRCore::CIRQPtr(GRCore::Tim5))->CIRQ();
	}
	TIM5->SR&=~(1<<0);
	return;
}

GRCFunc void TIM6_IRQHandler(void)
{
	if(TIM6->SR&(1<<0))
	{	
		if(GRCore::CIRQPtr(GRCore::Tim6))
			((GRTimer*)GRCore::CIRQPtr(GRCore::Tim6))->CIRQ();
	}
	TIM6->SR&=~(1<<0);
	return;
}

GRCFunc void TIM7_IRQHandler(void)
{
	if(TIM7->SR&(1<<0))
	{	
		if(GRCore::CIRQPtr(GRCore::Tim7))
			((GRTimer*)GRCore::CIRQPtr(GRCore::Tim7))->CIRQ();
	}
	TIM7->SR&=~(1<<0);
	return;
}

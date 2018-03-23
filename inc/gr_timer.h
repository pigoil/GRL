#ifndef GR_TIMER_H
#define GR_TIMER_H

#include "gr_core.h"

GR_BEGIN_NAMESPACE

class GRTimer;
class GRPwmGenerator;

//定时器
class GRTimer
{
public:
	GRTimer(TIM_TypeDef*,u16 arr,u16 psc,bool ui_en);

	virtual ~GRTimer(){};

	TIM_TypeDef* 	port(){return timer_port;};
	u16 					arr(){return timer_arr;}
	u16 					psc(){return timer_psc;}
	void 					setArr(u16 a);
	void 					setPsc(u16 p);
	void 					start(){timer_port->CR1|=1;};
	void 					stop(){timer_port->CR1&=~1;};
	u32						freq();//返回计数频率，单位Hz
	double				period();//返回计数周期，单位us
	u16						cnt(){return timer_port->CNT;};//返回计数值
	
	virtual void CIRQ(){return;};
protected:
	TIM_TypeDef* timer_port;
	u16 timer_arr;
	u16 timer_psc;

	void init(TIM_TypeDef*,u16 arr,u16 psc,bool ui_en);
};

class GRPwmGenerator : public GRTimer
{
public:
	enum PwmMode{Pwm1=0x06,Pwm2=0x07};
	enum OutPolarity{High=0,Low=2};
	enum OutputCh{CH1,CH2,CH3,CH4};

	GRPwmGenerator(TIM_TypeDef*,u16 arr,u16 psc,bool ui_en=false);
	void 	outputConfig(OutputCh ch,bool o_en=true,PwmMode mode=Pwm1,OutPolarity pol=High);
	void 	setCompareVal(OutputCh ch,u16);
	void 	setDuty(OutputCh ch,double);//设置占空比
	u16		compareVal(OutputCh ch);
};

GR_END_NAMESPACE

#endif

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
	//中断服务函数作为友元，以保护CIRQ函数不被意外调用
	friend void TIM2_IRQHandler(void);
	friend void TIM3_IRQHandler(void);
	friend void TIM4_IRQHandler(void);
	friend void TIM5_IRQHandler(void);
	friend void TIM6_IRQHandler(void);
	friend void TIM7_IRQHandler(void);

	GRTimer(TIM_TypeDef*,u16 arr,u16 psc,bool ui_en);

	virtual ~GRTimer(){};

	TIM_TypeDef* 	port(){return m_timer_port;};
	u16 			arr()const{return m_timer_arr;}
	u16 			psc()const{return m_timer_psc;}
	void 			setArr(u16 a);
	void 			setPsc(u16 p);
	void 			start(){m_timer_port->CR1|=1;};
	void 			stop(){m_timer_port->CR1&=~1;};
	u32				freq()const;//返回计数频率，单位Hz
	double			period()const;//返回计数周期，单位us
	u16				cnt()const{return m_timer_port->CNT;};//返回计数值
	
protected:
	TIM_TypeDef* m_timer_port;
	u16 m_timer_arr;
	u16 m_timer_psc;

	void init(TIM_TypeDef*,u16 arr,u16 psc,bool ui_en);
	virtual void CIRQ(){return;};
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

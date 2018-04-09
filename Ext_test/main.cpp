#include <GRCore>
#include <GRGpio>
#include <GRUsart>
#include <GRTimer>

#include "at24c.h"
int a = 0;

class shit : public GRTimer
{
public:
	GRGpio LED;

	shit():
		GRTimer(TIM2,7199,999,true),
		LED(GPIOB,9,GRGpio::PushPull)
	{}


protected:
	void CIRQ()
	{
		LED = !LED;
	}

};

void GRCore::SystemIoInit()
{	
	GRGpio::init(GPIOA,2,GRGpio::AFPushPull,GRGpio::Speed_50MHz);//串口2 AFIO初始化
	GRGpio::init(GPIOA,3,GRGpio::NormalInput);
}

int main()
{
	//在进入主函数之前，系统会完成两件事情：
	// 1.主时钟初始化
	// 2.调用用户自定义的SystemIoInit函数进行IO初始化
	//完成后才初始化静态变量，随后进入主函数
	
	shit fuck;
	fuck.start();
	int b;
	while(1)
	{
		b=a;
		GRCore::SleepMs(1);
	}
}

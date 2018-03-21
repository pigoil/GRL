#include <GRCore>
#include <GRGpio>
#include <GRUsart>

#include "at24c.h"

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
	
	GRGpio LED(GPIOB,9,GRGpio::PushPull);//LED初始化，闪灯用，PB5推挽输出
	GRGpio KEY(GPIOB,10,GRGpio::PushPull);
	LED=1;
	GRCore::SleepMs(100);

	LED=0;
	GRCore::SleepMs(100);
	LED=1;
	
	Setting setting;
	while(!setting.check())
	{
		LED=0;
		GRCore::SleepMs(50);
		LED=1;
		GRCore::SleepMs(150);
	}
	GRUsart com1(USART2,115200);
	com1.sendStr("Welcome!");
}

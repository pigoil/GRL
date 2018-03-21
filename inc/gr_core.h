#ifndef GR_CORE_H
#define GR_CORE_H

#include "gr_global.h"

GR_BEGIN_NAMESPACE

class GRCore;

class GRCore
{
public:
	enum TriMode{FallTri=0x01,RiseTri=0X02,AnyLevel=0X03};//外部中断触发模式
	enum JtagMode{JtagSwdEnable=0x00,SwdEnable=0x01,JtagSwdDisable=0x02};//Jtag模式
	enum CIRQChannel
		{
			Usart1,Usart2,Usart3,
			Tim1,Tim2,Tim3,Tim4,
			Tim5,Tim6,Tim7,Tim8,
			Sdio,
			CH_CNT};//中断通道
	//虚析构函数，声明该类为抽象类，不允许继承
	virtual ~GRCore()=0;
	//时钟相关函数
	static void SystemIoInit();
	static void	SystemClockInit(u8 p);
	static u8	SystemPll();
	static u8	SystemFreq();
	static void RccDeInit();
	//延时函数
	static void SleepUs	(u32		t);//微秒级精确延时,整形，取值不可超过U32_MAX/(SysFreq/8),72M主频下为477218588，10us以下延时效果不理想
	static void SleepMs	(u32		t);//毫秒级精确延时，整形 ，当reload值足够大时，十秒误差为1ms
	static void	SleepSec(double t);//秒级精确延时，浮点，可以延时很长时间，但无法做到非常准确，理论传入数精确到0.0000001s,但基本只能精确到10ms
	static void SleepNop(u32		c);//粗略延时，执行c*SysFreq个空循环,传入100的情况下，延时为700us左右
	static void SleepInt(vu32		c);//粗略延时，执行c*SysFreq个空循环，但务必在中断中专用
	//NVIC相关函数
	static void NvicSetVectorTable(u32 NVIC_VectTab, u32 Offset);
	static void NvicPriorityGroupConfig(u8 NVIC_Group);
	static void NvicInit(u8 NVIC_PreemptionPriority,u8 NVIC_SubPriority,u8 NVIC_Channel,u8 NVIC_Group);
	static void NvicExConfig(GPIO_TypeDef* GPIOx,u8 BITx,TriMode TRIM);//外部中断配置函数(只对GPIOA~G)
	//JTAG设置函数
	static void JtagSet(JtagMode mode);
	//汇编函数
	static void ASM_WFI();		//执行WFI指令
	static void ASM_INTX_DISABLE();//关闭所有中断
	static void ASM_INTX_ENABLE();	//开启所有中断
	static void ASM_MSR_MSP(u32 addr);	//设置堆栈地址
	//类中断注册
	static u8 		CIRQReg(CIRQChannel ch,void* ptr);//注册类中断
	static void* 	CIRQPtr(CIRQChannel ch){return CIRQ_table[ch];};//获取类中断向量
	
private:
	GRCore();//不允许生成实例
	static u8	pll;
	static u8	sys_freq;
	static u8	ticks_per_us;
	static u16	ticks_per_ms;
	static void* CIRQ_table[GRCore::CH_CNT];
};


GR_END_NAMESPACE

#endif

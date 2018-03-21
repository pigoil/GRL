#ifndef GR_ADC_H
#define GR_ADC_H

#include "gr_core.h"

GR_BEGIN_NAMESPACE

class GRAdc;
class GRInnerTemperature;

class GRAdc
{
public:
	enum AdcChannel{Ch0,Ch1,Ch2,Ch3,Ch4,Ch5,Ch6,Ch7,Ch8,Ch9,Ch10,Ch11,Ch12,Ch13,Ch14,Ch15,Ch16,Ch17,CH_CNT};
	enum AdcMode{Single,Scan};
	enum SampleTime{Cycle1_5,Cycle7_5,Cycle13_5,Cycle28_5,Cycle41_5,Cycle55_5,Cycle71_5,Cycle239_5};
	
	GRAdc(ADC_TypeDef*,AdcMode);
	static void init(ADC_TypeDef*,AdcMode);
	void calibrate();
	
	GRAdc& operator +=(AdcChannel);//连续模式用
	GRAdc& operator -=(AdcChannel);

	u16 result(AdcChannel,SampleTime);
	float voltage(AdcChannel,SampleTime);
	u32 trueRand(AdcChannel);//利用ADC电平噪声返回一个32位真随机数,应使用悬空的通道，
	//可结合伪随机生成接口grRand，使用真随机数接口生成种子
	
	virtual void CIRQ(){};
	
private:
	ADC_TypeDef* port;
	AdcMode mode;
	u8 sq_cnt;
	u16 mea_data[16];//十六个序列的转换结果
	u8 sq_ch[CH_CNT];//十八个通道在转换序列中的顺序，255为不在转换序列中
};

class GRInnerTemperature : public GRAdc
{
public:
	GRInnerTemperature();
	float temperature();//内核温度
	float vRefInt();//内部参照电压 典型值=1.20V
};

GR_END_NAMESPACE

#endif

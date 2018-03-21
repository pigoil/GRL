#ifndef GR_USER_h
#define GR_USER_h

#if GR_USE_NAMESPACE == 1 //是否使用命名空间
#define GR_BEGIN_NAMESPACE namespace GRL{
#define GR_END_NAMESPACE 								}
#else
#define GR_BEGIN_NAMESPACE
#define GR_END_NAMESPACE
#endif

#define USE_STDIO 1//是否使用stdio

//用户宏设定
namespace GRMacro{

const unsigned char SYSTEM_PLL=9;
const float XTAL = 8.000f; //晶振频率，单位MHz
const unsigned long SYSTICK_RELOAD = 9000000;//最好不要设太小 //72M/8=9M,最好设9的倍数
const bool AFIO_AUTO_INIT = false;//外设的AFIO自动初始化
	
const bool USART1_REC_ENABLE = true;//串口接收开关
const bool USART2_REC_ENABLE = true;
const bool USART3_REC_ENABLE = true;
	
const unsigned char IIC_US = 2;//模拟IIC延时时间
const unsigned long PRINTF_BUF_SIZE = 256;
	
const float VREF = 3.3000;//参考电压
const float TEMP_V25=1.43000;
const float TEMP_SLOPE=0.0043;
} 

#endif

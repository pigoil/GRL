#ifndef GR_GPIO_H
#define GR_GPIO_H

#include "gr_core.h"

GR_BEGIN_NAMESPACE

class GRGpio;
class GRGpioPort;

class GRGpio
{
public:
	enum InputMode{Analog=0x00,Floating=0x01,NormalInput=0x02,Reserve=0x03};
	enum OutputMode{PushPull=0x00,OpenDrain=0x01,AFPushPull=0x02,AFOpenDrain=0x03};
	enum OutputSpeed{Speed_10MHz=0x01,Speed_2MHz=0x02,Speed_50MHz=0x03};
	
	GRGpio(GPIO_TypeDef* port,u8 pin):io_port(port),io_pin(pin),io_mode(0){};//不进行外设初始化
	GRGpio(GPIO_TypeDef* port,u8 pin,OutputMode mode,OutputSpeed speed=Speed_50MHz);
	GRGpio(GPIO_TypeDef* port,u8 pin,InputMode mode);

	virtual ~GRGpio(){};

	static void init(GPIO_TypeDef* port,u8 pin,OutputMode mode,OutputSpeed speed=Speed_50MHz);
	static void init(GPIO_TypeDef* port,u8 pin,InputMode mode);
	
	GPIO_TypeDef* port()const	{return io_port;}
	u8 pin()const	{return io_pin;}
	u8 mode()const {return io_mode;}
	
	bool read()const;
	void write(bool)const;
	bool StableHigh(u8 nop=20,u8 cnt=3)const;
	bool StableLow(u8 nop=20,u8 cnt=3)const;
	
	GRGpio& operator= (bool);
	operator bool()const;

private:
	GPIO_TypeDef* io_port;
	u8 io_pin;
	u8 io_mode;

protected:
	virtual void CIRQ(){};//待实现
};

class GRGpioPort
{
public:
	GRGpioPort(GPIO_TypeDef* port);

	virtual ~GRGpioPort(){};

	void init();
	GRGpio pin(int);

	GRGpioPort& operator<<(int);
	GRGpioPort& operator<<(GRGpio::InputMode);
	GRGpioPort& operator<<(GRGpio::OutputMode);
	GRGpioPort& operator<<(GRGpio::OutputSpeed);
	GRGpio			operator[](int);

private:
	u32 m_io_crl;
	u32 m_io_crh;
	u32 m_io_inv_crl;
	u32 m_io_inv_crh;
	u8 	m_mode;
	GPIO_TypeDef* m_io_port;
};

GR_END_NAMESPACE


#endif

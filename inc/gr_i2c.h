#ifndef GR_I2C_H
#define GR_I2C_H

#include "gr_core.h"
#include "gr_gpio.h"

GR_BEGIN_NAMESPACE

class GRI2c;

class GRI2c
{
public:
	enum AckMode{Ack,Nack};

	GRI2c(GRGpio& sck,GRGpio& sda);

	virtual ~GRI2c(){};

	void 	sendByte(u8);
	u8		readByte(AckMode ack);//应答类型
	u8		write(u8 sAdd,u8 rAdd,u8 data);
	u8 		read(u8 sAdd,u8 rAdd);
	u8 		write(u8 sAdd,u8 rAdd,u8* data,u32 cnt);
	u8 		read(u8 sAdd,u8 rAdd,u8*data,u32 cnt);
	
private:
	GRGpio			&m_iic_sck,&m_iic_sda;
	GPIO_TypeDef*	m_sda_port;
	u8				m_sda_pin;

protected:
	void sda_out();
	void sda_in();

	void iic_start();
	void iic_stop();
	u8	 iic_wait_ack();
	void iic_ack();
	void iic_nack();
};

GR_END_NAMESPACE

#endif

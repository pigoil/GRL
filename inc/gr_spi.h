#ifndef GR_SPI_H
#define GR_SPI_H

#include "gr_core.h"

GR_BEGIN_NAMESPACE

class GRSpi;

class GRSpi
{
public:
	enum DataFormat{Data_8bit,Data_16bit};
	enum FrameFormat{MSB,LSB};
	enum ClockPolarity{Low,High};
	enum ClockPhase{FirstEdge,SecondEdge};
	enum ClockSpeed{Speed_2,Speed_4,Speed_8,Speed_16,Speed_32,Speed_64,Speed_128,Speed_256};
	
	GRSpi(SPI_TypeDef* port,ClockSpeed speed=Speed_256,\
				ClockPolarity cpol=High,ClockPhase=SecondEdge,\
				DataFormat d_f=Data_8bit,FrameFormat f_f=MSB);
	~GRSpi();
	
	void speedSet(ClockSpeed);
	u16 transByte(u16 data);

private:
	void init(SPI_TypeDef* port,ClockSpeed speed,\
				ClockPolarity cpol,ClockPhase,\
				DataFormat d_f,FrameFormat f_f);
	SPI_TypeDef* spi_port;
};

GR_END_NAMESPACE

#endif

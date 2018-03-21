#include <GRLcd>
#include <GRGpio>
#include <GRSpi>

class SpiLcd;

class SpiLcd : public GRAbstractLCD
{
public:
	SpiLcd();
	~SpiLcd();
	virtual void drawPoint(u16 x,u16 y);//使用前景色画点
	virtual void drawPoint(u16 x,u16 y,GRColor_16 color);
	virtual GRColor_16 readPoint(u16 x,u16 y);
	
	void setRegion(u16 x_start,u16 y_start,u16 x_end,u16 y_end);
	void setXY(u16 x,u16 y);
	void clearDisp(GRColor_16 color);

	void paintImage(u16 x,u16 y);

private :
	GRSpi* 	device;
	GRGpio* lcd_cs;
	GRGpio* lcd_rs;
	GRGpio* lcd_rst;
	GRGpio* lcd_led;

	void write_data_16(u16 data);
	void write_data_8(u8 data);
	void write_index(u8 index);
	void write_reg(u8 Index,u8 Data);
	void reset();
};

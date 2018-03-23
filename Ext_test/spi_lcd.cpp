#include "spi_lcd.h"
#include "image.h"

SpiLcd::SpiLcd()
{
	LCD_H=160,LCD_W=128;
	MAX_CHAR_POSX=128,MAX_CHAR_POSY=160;
	
	device = new GRSpi(SPI1,GRSpi::Speed_2);//SPI高速
	
	lcd_cs = new GRGpio(GPIOC,2	,GRGpio::PushPull);
	lcd_rs = new GRGpio(GPIOC,1,GRGpio::PushPull);
	lcd_rst = new GRGpio(GPIOC,0,GRGpio::PushPull);
	//lcd_led = new GRGpio(GPIOA,3,GRGpio::PushPull);
	//*lcd_led = 1;
	reset();
	{//LCD Init For 1.44Inch LCD Panel with ST7735R.
		write_index(0x11);//Sleep exit 
		GRCore::SleepMs(5);
			
		//ST7735R Frame Rate
		write_index(0xB1); 
		write_data_8(0x01); 
		write_data_8(0x2C); 
		write_data_8(0x2D); 

		write_index(0xB2); 
		write_data_8(0x01); 
		write_data_8(0x2C); 
		write_data_8(0x2D); 

		write_index(0xB3); 
		write_data_8(0x01); 
		write_data_8(0x2C); 
		write_data_8(0x2D); 
		write_data_8(0x01); 
		write_data_8(0x2C); 
		write_data_8(0x2D); 
		
		write_index(0xB4); //Column inversion 
		write_data_8(0x07); 
		
		//ST7735R Power Sequence
		write_index(0xC0); 
		write_data_8(0xA2); 
		write_data_8(0x02); 
		write_data_8(0x84); 
		write_index(0xC1); 
		write_data_8(0xC5); 

		write_index(0xC2); 
		write_data_8(0x0A); 
		write_data_8(0x00); 

		write_index(0xC3); 
		write_data_8(0x8A); 
		write_data_8(0x2A); 
		write_index(0xC4); 
		write_data_8(0x8A); 
		write_data_8(0xEE); 
		
		write_index(0xC5); //VCOM 
		write_data_8(0x0E); 
		
		write_index(0x36); //MX, MY, RGB mode 
		write_data_8(0xC0); 
		
		//ST7735R Gamma Sequence
		write_index(0xe0); 
		write_data_8(0x0f); 
		write_data_8(0x1a); 
		write_data_8(0x0f); 
		write_data_8(0x18); 
		write_data_8(0x2f); 
		write_data_8(0x28); 
		write_data_8(0x20); 
		write_data_8(0x22); 
		write_data_8(0x1f); 
		write_data_8(0x1b); 
		write_data_8(0x23); 
		write_data_8(0x37); 
		write_data_8(0x00); 	
		write_data_8(0x07); 
		write_data_8(0x02); 
		write_data_8(0x10); 

		write_index(0xe1); 
		write_data_8(0x0f); 
		write_data_8(0x1b); 
		write_data_8(0x0f); 
		write_data_8(0x17); 
		write_data_8(0x33); 
		write_data_8(0x2c); 
		write_data_8(0x29); 
		write_data_8(0x2e); 
		write_data_8(0x30); 
		write_data_8(0x30); 
		write_data_8(0x39); 
		write_data_8(0x3f); 
		write_data_8(0x00); 
		write_data_8(0x07); 
		write_data_8(0x03); 
		write_data_8(0x10);  
		
		write_index(0x2a);
		write_data_8(0x00);
		write_data_8(0x00);
		write_data_8(0x00);
		write_data_8(0x7f);

		write_index(0x2b);
		write_data_8(0x00);
		write_data_8(0x00);
		write_data_8(0x00);
		write_data_8(0x9f);
		
		write_index(0xF0); //Enable test command  
		write_data_8(0x01); 
		write_index(0xF6); //Disable ram power save mode 
		write_data_8(0x00); 
		
		write_index(0x3A); //65k mode 
		write_data_8(0x05); 
		
		
		write_index(0x29);//Display on	 
	}
	clearDisp(WHITE);
}

SpiLcd::~SpiLcd()
{
	delete device;
	delete lcd_cs;
	delete lcd_rs;
	delete lcd_rst;
	delete lcd_led;
}

void SpiLcd::drawPoint(u16 x,u16 y)
{
	setRegion(x,y,x+1,y+1);
	write_data_16(POINT_COLOR);
}

void SpiLcd::drawPoint(u16 x,u16 y,GRColor_16 c)
{
	setRegion(x,y,x+1,y+1);
	write_data_16(c);
}

GRColor_16 SpiLcd::readPoint(u16 x,u16 y)
{
  u16 Data = 0;
  setXY(x,y);
  write_data_8(0xff);
  return Data;
}

void SpiLcd::setRegion(u16 x_start,u16 y_start,u16 x_end,u16 y_end)
{
	write_index(0x2a);
	write_data_8(0x00);
	write_data_8(x_start+2);
	write_data_8(0x00);
	write_data_8(x_end+2);

	write_index(0x2b);
	write_data_8(0x00);
	write_data_8(y_start+1);
	write_data_8(0x00);
	write_data_8(y_end+1);
	
	write_index(0x2c);
}

void SpiLcd::setXY(u16 x,u16 y)
{
	setRegion(x,y,x,y);
}

void SpiLcd::clearDisp(GRColor_16 color)
{
	u8 i,m;
	setRegion(0,0,128-1,160-1);
	//LCD_WriteIndex(0x2C);
	for(i=0;i<LCD_W;i++)
	{
		for(m=0;m<LCD_H;m++)
		{	
			write_data_16(color);
		}  
	}
	BACK_COLOR=color;
}

//向液晶屏写一个16位数据
void SpiLcd::write_data_16(u16 Data)
{
	*lcd_cs=0;
	*lcd_rs=1;
	device->transByte(Data>>8);
	device->transByte(Data);
	*lcd_cs=1; 
}

void SpiLcd::write_index(u8 Index)
{
   //SPI 写命令时序开始
   *lcd_cs=0;
   *lcd_rs=0;
	 device->transByte(Index);
	 *lcd_cs=1;
}

//向液晶屏写一个8位数据
void SpiLcd::write_data_8(u8 Data)
{
   *lcd_cs=0;
   *lcd_rs=1;
   device->transByte(Data);
   *lcd_cs=1; 
}

void SpiLcd::reset(void)
{
	*lcd_rst=0;
	GRCore::SleepMs(100);
	*lcd_rst=1;
	GRCore::SleepMs(100);
}

void SpiLcd::paintImage(u16 x,u16 y)
{
//	setRegion(20,75,100-1,155-1);
//	u16 tmp;
//	for(u16 i=0;i<12800;i+=2)
//	{
//		tmp=gImage_1[i+1];
//		tmp<<=8;
//		tmp|=gImage_1[i];
//		write_data_16(tmp);
//	}
	u16 w=gimp_image.width;
	u16 h=gimp_image.height;
	u16 xe=x+w;
	u16 ye=y+h;
	if(xe>LCD_W)xe=LCD_W;
	if(ye>LCD_H)ye=LCD_H;
	
	setRegion(x,y,xe-1,ye-1);
	u16 tmp;
	for(u16 i=0;i<ye-y;++i)
		for(u16 j=0;j<(xe-x)<<1;j+=2)
		{
			tmp=gimp_image.pixel_data[(i*gimp_image.width<<1)+j+1];
			tmp<<=8;
			tmp|=gimp_image.pixel_data[(i*gimp_image.width<<1)+j];
			write_data_16(tmp);
		}
	
}

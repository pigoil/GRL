#ifndef GR_LCD_H
#define GR_LCD_H

#include "gr_core.h"

GR_BEGIN_NAMESPACE

class GRAbstractLCD;

typedef u16 GRColor_16;

//画笔颜色
#define WHITE     0xFFFF
#define BLACK     0x0000	  
#define BLUE      0x001F  
#define BRED      0XF81F
#define GRED 			0XFFE0
#define GBLUE			0X07FF
#define RED       0xF800
#define MAGENTA   0xF81F
#define GREEN     0x07E0
#define CYAN      0x7FFF
#define YELLOW    0xFFE0
#define BROWN 		0XBC40 //棕色
#define BRRED 		0XFC07 //棕红色
#define GRAY  		0X8430 //灰色
//GUI颜色

#define DARKBLUE      	 0X01CF	//深蓝色
#define LIGHTBLUE      	 0X7D7C	//浅蓝色  
#define GRAYBLUE       	 0X5458 //灰蓝色
//以上三色为PANEL的颜色 
 
#define LIGHTGREEN     	 0X841F //浅绿色
//#define LIGHTGRAY        0XEF5B //浅灰色(PANNEL)
#define LGRAY 			 0XC618 //浅灰色(PANNEL),窗体背景色

#define LGRAYBLUE        0XA651 //浅灰蓝色(中间层颜色)
#define LBBLUE           0X2B12 //浅棕蓝色(选择条目的反色)

class GRAbstractLCD
{
public:
	GRAbstractLCD();

	virtual ~GRAbstractLCD()=0;

	virtual void drawPoint(u16 x,u16 y)=0;//使用前景色画点
	virtual void drawPoint(u16 x,u16 y,GRColor_16 color)=0;
	virtual u16 readPoint(u16 x,u16 y)=0;
	
	void clearDisp(GRColor_16 color);
		
	void drawLine(u16 x1, u16 y1, u16 x2, u16 y2);
	void drawRect(u16 x1, u16 y1, u16 x2, u16 y2);
	void drawCircle(u16 x0,u16 y0,u8 r);
	

	void showChar(u16 x,u16 y,u8 num,u8 size,u8 mode);
	void showNum(u16 x,u16 y,s32 num,u8 len,u8 size,bool zero=false);//zero选择是否填充前置0
	void showFloat(u16 x,u16 y,float num,u8 len,u8 digits,u8 size);//len为整数部分长度，digits为小数部分长度
	void showString(u16 x,u16 y,const char *p);
	
	void setPointColor(GRColor_16 color){POINT_COLOR=color;};
	GRColor_16 pointColor(){return POINT_COLOR;};
	
	void setBackColor(GRColor_16 color){BACK_COLOR=color;};
	GRColor_16 backColor(){return BACK_COLOR;};

protected:
	GRColor_16 POINT_COLOR;
	GRColor_16 BACK_COLOR;  
	u16 LCD_W,LCD_H;
	u16 MAX_CHAR_POSX,MAX_CHAR_POSY;
};

GR_END_NAMESPACE

#endif

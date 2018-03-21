#include "gr_lcd.h"
#include "FONT.H"

GRAbstractLCD::GRAbstractLCD()
	:	POINT_COLOR(BLACK),BACK_COLOR(WHITE),
		LCD_W(0),LCD_H(0),
		MAX_CHAR_POSX(0),MAX_CHAR_POSY(0)
{
	
}

void GRAbstractLCD::clearDisp(GRColor_16 color)
{
	for(u16 i=0;i<LCD_H;++i)
		for(u16 j=0;j<LCD_W;++j)
			drawPoint(i,j,color);
}

void GRAbstractLCD::drawLine(u16 x1, u16 y1, u16 x2, u16 y2)
{
	u16 t; 
	s16 xerr=0,yerr=0,delta_x,delta_y,distance; 
	s16 incx,incy,uRow,uCol; 

	delta_x=x2-x1; //计算坐标增量 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //设置单步方向 
	else if(delta_x==0)incx=0;//垂直线 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//水平线 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//画线输出 
	{  
		drawPoint(uRow,uCol);//画点 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
}

void GRAbstractLCD::drawRect(u16 x1, u16 y1, u16 x2, u16 y2)
{
	drawLine(x1,y1,x2,y1);
	drawLine(x1,y1,x1,y2);
	drawLine(x1,y2,x2,y2);
	drawLine(x2,y1,x2,y2);
}

void GRAbstractLCD::drawCircle(u16 x0,u16 y0,u8 r)
{
	s16 a,b;
	s16 di;
	a=0;b=r;	  
	di=3-(r<<1);             //判断下个点位置的标志
	while(a<=b)
	{
		drawPoint(x0-b,y0-a);             //3           
		drawPoint(x0+b,y0-a);             //0           
		drawPoint(x0-a,y0+b);             //1       
		drawPoint(x0-b,y0-a);             //7           
		drawPoint(x0-a,y0-b);             //2             
		drawPoint(x0+b,y0+a);             //4               
		drawPoint(x0+a,y0-b);             //5
		drawPoint(x0+a,y0+b);             //6 
		drawPoint(x0-b,y0+a);             
		a++;
		//使用Bresenham算法画圆     
		if(di<0)di +=4*a+6;	  
		else
		{
			di+=10+4*(a-b);   
			b--;
		} 
		drawPoint(x0+a,y0+b);
	}
}

void GRAbstractLCD::showChar(u16 x,u16 y,u8 num,u8 size,u8 mode)
{
  u8 temp;
  u8 pos,t;
	u16 x0=x;
	u16 colortemp=POINT_COLOR;      
    if(x>MAX_CHAR_POSX||y>MAX_CHAR_POSY)return; 
	//设置窗口		   
	num=num-' ';//得到偏移后的值
	if(!mode) //非叠加方式
	{
		for(pos=0;pos<size;pos++)
		{
			if(size==12)temp=asc2_1206[num][pos];//调用1206字体
			else temp=asc2_1608[num][pos];		 //调用1608字体
			for(t=0;t<size/2;t++)
		    {                 
		        if(temp&0x01)POINT_COLOR=colortemp;
				else POINT_COLOR=BACK_COLOR;
				drawPoint(x,y);	
				temp>>=1; 
				x++;
		    }
			x=x0;
			y++;
		}	
	}else//叠加方式
	{
		for(pos=0;pos<size;pos++)
		{
			if(size==12)temp=asc2_1206[num][pos];//调用1206字体
			else temp=asc2_1608[num][pos];		 //调用1608字体
			for(t=0;t<size/2;t++)
		    {                 
		        if(temp&0x01)drawPoint(x+t,y+pos);//画一个点     
		        temp>>=1; 
		    }
		}
	}
	POINT_COLOR=colortemp;	    
}

//m^n函数
u32 mypow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}			

void GRAbstractLCD::showNum(u16 x,u16 y,s32 num,u8 len,u8 size,bool zero)
{
	u8 t,temp;
	u8 enshow=0;
	if(num<0)
	{
		showChar(x,y,'-',size,0);
		x+=size/2;
		len-=1;
		num=-num;
	}
	
	for(t=0;t<len;t++)
	{
		temp=(num/mypow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				if(zero)showChar(x+(size/2)*t,y,'0',size,0);
				else showChar(x+(size/2)*t,y,' ',size,0);
				continue;
			}else enshow=1; 
		 	 
		}
	 	showChar(x+(size/2)*t,y,temp+'0',size,0); 
	}
}
void GRAbstractLCD::showString(u16 x,u16 y,const char *p)
{
    while(*p!='\0')
    {       
        if(x+6>=MAX_CHAR_POSX){x=0;y+=12;}
//        if(y>MAX_CHAR_POSY)
//						{y=0,x=0;
//							LCD_Clear(BLUE);}
        showChar(x,y,*p,12,0);
        x+=6;
        p++;
    }  
}

void GRAbstractLCD::showFloat(u16 x,u16 y,float num,u8 len,u8 digits,u8 size)
{
	s32 integer=(s32)num;
	u32 decimal=(num-integer)*mypow(10,digits);
	
	showNum(x,y,integer,len,size);
	x+=len*size/2;
	showChar(x,y,'.',size,0);
	x+=size/2;
	showNum(x,y,decimal,digits,size,true);
}

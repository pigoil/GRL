#include "at24c.h"

const u8 AT24C_ADD = 0xA0;
const u8 SIG_REG = 0xFF;
const u8 SIGNATURE = 0x55;
const u8 CLEAR_DATA  = 0x55;

GRGpio sck(GPIOB,6,GRGpio::PushPull);
GRGpio sda(GPIOB,7,GRGpio::PushPull);

AT24C::AT24C()
	:GRI2c(sck,sda)
{
}

//检测24C02状态
bool AT24C::check()
{
	if(read(AT24C_ADD,SIG_REG)==SIGNATURE)return true;
	else write(AT24C_ADD,SIG_REG,SIGNATURE);
	if(read(AT24C_ADD,SIG_REG)==SIGNATURE)return true;
	else return false;
}

void AT24C::write_eight(u8 address,u8* buff,u8 cnt)
{
	u8 i;
	iic_start();
	sendByte(AT24C_ADD);   //发送设备地址+写信号
	iic_wait_ack();
	sendByte(address);   //设置低起始地址      
	iic_wait_ack();	
	for(i=0;i<cnt;i++)
	{
		sendByte(*(buff+i));//发送数据
		iic_wait_ack();  
	}
	iic_stop(); 
	GRCore::SleepMs(100);
}

void AT24C::writeBuf(u8 address,u8* buff,u8 cnt)
{
	u8 i;
	u8 temp_add=address;
	if(address%8+cnt>8)
	{
		write_eight(address,buff,8-address%8);
		buff+=(8-address%8);
		address+=(8-address%8);
		for(i=0;i<cnt/8-1;i++)
		{
			write_eight(address,buff,8);
			address+=8,buff+=8;
		}
		write_eight(address,buff,cnt-address+temp_add);
	}
	else
		write_eight(address,buff,cnt);
}

void AT24C::readBuf(u8 address,u8* buff,u8 cnt)
{
	u8 i;
	iic_start();
	sendByte(AT24C_ADD);   //发送设备地址+写信号
	iic_wait_ack();
	sendByte(address);   //设置低起始地址      
	iic_wait_ack();	
	iic_start();
	sendByte(AT24C_ADD+1);   //发送设备地址+写信号
	iic_wait_ack();	
	for(i=0;i<cnt-1;i++)
	{
		*(buff)=readByte(GRI2c::Ack);
		buff++;
	}
	*(buff)=readByte(GRI2c::Nack);
	iic_stop();
}
void AT24C::clear(u8 address,u8 cnt)
{
	u8 i;
	for(i=0;i<cnt;i++)
	write(AT24C_ADD,address+i,CLEAR_DATA);
}

/*设置记录*/

void Setting::writeSetting(Key k,double w)
{
	union{float f;u8 c[4];} u;
	u.f=w;
	writeBuf((u8)k<<2,u.c,4);
}


void Setting::writeSetting(Key k,int w)
{
	union{s32 f;u8 c[4];} u;
	u.f=w;
	writeBuf((u8)k<<2,u.c,4);
}

void Setting::writeSetting(Key k,bool w)
{
	union{s32 f;u8 c[4];} u;
	u.f=w;
	writeBuf((u8)k<<2,u.c,4);
}

void Setting::readSetting(Key k,float& r)
{
	union{float f;u8 c[4];} u;
	readBuf((u8)k<<2,u.c,4);
	r=u.f;
}

void Setting::readSetting(Key k,s32& r)
{
	union{s32 f;u8 c[4];} u;
	readBuf((u8)k<<2,u.c,4);
	r=u.f;
}

void Setting::readSetting(Key k,bool& r)
{
	union{s32 f;u8 c[4];} u;
	readBuf((u8)k<<2,u.c,4);
	r=u.f;
}

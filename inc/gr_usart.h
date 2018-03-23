#ifndef GR_USART_H
#define GR_USART_H

#include "gr_core.h"

GR_BEGIN_NAMESPACE

class GRUsart;
class GRUsartPkg;

class GRUsart
{
public:
	GRUsart(USART_TypeDef * p,u32 b,u8 pclk=0);//pclk=0则使用当前主频
	virtual ~GRUsart(){};
	void init(USART_TypeDef *,u32,u8);
	void setBaud(u32 baud);
	void sendByte(u8 byte);
	void sendBuf(u8* buf,u32 size);
	void sendStr(const char* str);
	u8 getByte(){return usart_port->DR;};
	
	bool isReceived(){return rec_flag;};
	void setRecFlag(){rec_flag=true;}
	void resetRecFlag(){rec_flag=false;};
	bool isRecEnable(){return rec_enable;};
	void enableRec(){rec_flag=false;rec_enable=true;};
	void disableRec(){rec_enable=false;}
	
	virtual void CIRQ(){u8 no_use=getByte();rec_flag=true;};//读取并丢弃数据
private:
	bool rec_flag;
	bool rec_enable;
	u32 usart_baud;
	u8 usart_pclk;
	u8 usart_rcc;
	USART_TypeDef* usart_port;
};

class GRUsartPkg : public GRUsart
{
	public:
	GRUsartPkg(USART_TypeDef * p,u32 b,u8 pclk=0);
	virtual ~GRUsartPkg();
	virtual void CIRQ();//类中断
	
protected:
	enum Format{SOF,EOF,Check,Data};
	
	void add_format(Format type_of_block,u8 data);
	void clear_format_sheet();
	virtual u8	recv_ver(u8 data){return 0;};//接收校验方法
	virtual void acknowledge(){return;};//从机应答方法
	GRLinkedList<u8>	*recv_data;

private:
	struct Block
	{
		Format	type;
		u8			data;
	};
	GRLinkedList<Block> *recv_format_sheet;
};

GR_END_NAMESPACE

#endif

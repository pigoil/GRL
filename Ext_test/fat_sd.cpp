#include "fat_sd.h"
#include <stdio.h>

#include <GRGpio>

// SD卡类型定义  
#define SD_TYPE_ERR     0X00
#define SD_TYPE_MMC     0X01
#define SD_TYPE_V1      0X02
#define SD_TYPE_V2      0X04
#define SD_TYPE_V2HC    0X06	   
// SD卡指令表  	   
#define CMD0    0       //卡复位
#define CMD1    1
#define CMD8    8       //命令8 ，SEND_IF_COND
#define CMD9    9       //命令9 ，读CSD数据
#define CMD10   10      //命令10，读CID数据
#define CMD12   12      //命令12，停止数据传输
#define CMD16   16      //命令16，设置SectorSize 应返回0x00
#define CMD17   17      //命令17，读sector
#define CMD18   18      //命令18，读Multi sector
#define CMD23   23      //命令23，设置多sector写入前预先擦除N个block
#define CMD24   24      //命令24，写sector
#define CMD25   25      //命令25，写Multi sector
#define CMD41   41      //命令41，应返回0x00
#define CMD55   55      //命令55，应返回0x01
#define CMD58   58      //命令58，读OCR信息
#define CMD59   59      //命令59，使能/禁止CRC，应返回0x00
//数据写入回应字意义
#define MSD_DATA_OK                0x05
#define MSD_DATA_CRC_ERROR         0x0B
#define MSD_DATA_WRITE_ERROR       0x0D
#define MSD_DATA_OTHER_ERROR       0xFF
//SD卡回应标记字
#define MSD_RESPONSE_NO_ERROR      0x00
#define MSD_IN_IDLE_STATE          0x01
#define MSD_ERASE_RESET            0x02
#define MSD_ILLEGAL_COMMAND        0x04
#define MSD_COM_CRC_ERROR          0x08
#define MSD_ERASE_SEQUENCE_ERROR   0x10
#define MSD_ADDRESS_ERROR          0x20
#define MSD_PARAMETER_ERROR        0x40
#define MSD_RESPONSE_FAILURE       0xFF

GRGpio cs(GPIOD,2,GRGpio::PushPull);
//SPI SD卡驱动
SDCard::SDCard(SPI_TypeDef* port)
	:GRSpi(port)
{
	//nothing to do
}

u8 SDCard::init()
{
	u8 r1;      // 存放SD卡的返回值
  u8 buf[4];  
	u8 i;
 	for(i=0;i<10;i++)transByte(0XFF);//发送最少74个脉冲
	
	u16 retry=20;
	do
	{
		r1=send_cmd(CMD0,0,0x95);//进入IDLE状态
	}while((r1!=0X01) && retry--);
 	sd_type=0;//默认无卡
	if(r1==0X01)
	{
		if(send_cmd(CMD8,0x1AA,0x87)==1)//SD V2.0
		{
			for(i=0;i<4;i++)buf[i]=transByte(0XFF);	//Get trailing return value of R7 resp
			if(buf[2]==0X01&&buf[3]==0XAA)//卡是否支持2.7~3.6V
			{
				retry=0XFFFE;
				do
				{
					send_cmd(CMD55,0,0X01);	//发送CMD55
					r1=send_cmd(CMD41,0x40000000,0X01);//发送CMD41
				}while(r1&&retry--);
				if(retry&&send_cmd(CMD58,0,0X01)==0)//鉴别SD2.0卡版本开始
				{
					for(i=0;i<4;i++)buf[i]=transByte(0XFF);//得到OCR值
					if(buf[0]&0x40)sd_type=SD_TYPE_V2HC;    //检查CCS
					else sd_type=SD_TYPE_V2;   
				}
			}
		}else//SD V1.x/ MMC	V3
		{
			send_cmd(CMD55,0,0X01);		//发送CMD55
			r1=send_cmd(CMD41,0,0X01);	//发送CMD41
			if(r1<=1)
			{		
				sd_type=SD_TYPE_V1;
				retry=0XFFFE;
				do //等待退出IDLE模式
				{
					send_cmd(CMD55,0,0X01);	//发送CMD55
					r1=send_cmd(CMD41,0,0X01);//发送CMD41
				}while(r1&&retry--);
			}else//MMC卡不支持CMD55+CMD41识别
			{
				sd_type=SD_TYPE_MMC;//MMC V3
				retry=0XFFFE;
				do //等待退出IDLE模式
				{											    
					r1=send_cmd(CMD1,0,0X01);//发送CMD1
				}while(r1&&retry--);  
			}
			if(retry==0||send_cmd(CMD16,512,0X01)!=0)sd_type=SD_TYPE_ERR;//错误的卡
		}
	}
	disselect();//取消片选
	//进入高速
	if(sd_type)return 0;
	else if(r1)return r1; 	   
	return 0xaa;//其他错误
}

u8 SDCard::sync()
{
	u8 res;
	cs=0;
	if(wait_ready()==0)res=0;
	else res=1;
	cs=1;
	return res;
}

u8 SDCard::getCid(u8* cid_data)
{
	u8 r1;	   
	//发CMD10命令，读CID
	r1=send_cmd(CMD10,0,0x01);
  if(r1==0x00)
	{
		r1=recv_data(cid_data,16);//接收16个字节的数据	 
	}
	disselect();//取消片选
	if(r1)return 1;
	else return 0;
}

u8 SDCard::getCsd(u8* csd_data)
{
	u8 r1;	 
	r1=send_cmd(CMD9,0,0x01);//发CMD9命令，读CSD
	if(r1==0)
	{
		r1=recv_data(csd_data, 16);//接收16个字节的数据 
	}
	disselect();//取消片选
	if(r1)return 1;
	else return 0;
}

u32 SDCard::getSectorCnt()
{
	u8 csd[16];
	u32 Capacity;  
	u8 n;
	u16 csize;  					    
	//取CSD信息，如果期间出错，返回0
	if(getCsd(csd)!=0) return 0;	    
	//如果为SDHC卡，按照下面方式计算
	if((csd[0]&0xC0)==0x40)	 //V2.00的卡
	{	
	csize = csd[9] + ((u16)csd[8] << 8) + 1;
	Capacity = (u32)csize << 10;//得到扇区数	 		   
	}else//V1.XX的卡
	{	
	n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
	csize = (csd[8] >> 6) + ((u16)csd[7] << 2) + ((u16)(csd[6] & 3) << 10) + 1;
	Capacity= (u32)csize << (n - 9);//得到扇区数   
	}
	return Capacity;
}

u8 SDCard::readDisk(u8*buf,u32 sector,u8 cnt)
{
	u8 r1;
	if(sd_type!=SD_TYPE_V2HC)sector <<= 9;//转换为字节地址
	if(cnt==1)
	{
		r1=send_cmd(CMD17,sector,0X01);//读命令
		if(r1==0)//指令发送成功
		{
			r1=recv_data(buf,512);//接收512个字节	   
		}
	}else
	{
		r1=send_cmd(CMD18,sector,0X01);//连续读命令
		do
		{
			r1=recv_data(buf,512);//接收512个字节	 
			buf+=512;  
		}while(--cnt && r1==0); 	
		send_cmd(CMD12,0,0X01);	//发送停止命令
	}   
	disselect();//取消片选
	return r1;//
}

u8 SDCard::writeDisk(u8*buf,u32 sector,u8 cnt)
{
	u8 r1;
	if(sd_type!=SD_TYPE_V2HC)sector *= 512;//转换为字节地址
	if(cnt==1)
	{
		r1=send_cmd(CMD24,sector,0X01);//读命令
		if(r1==0)//指令发送成功
		{
			r1=send_data(buf,0xFE);//写512个字节	   
		}
	}else
	{
		if(sd_type!=SD_TYPE_MMC)
		{
			send_cmd(CMD55,0,0X01);	
			send_cmd(CMD23,cnt,0X01);//发送指令	
		}
 		r1=send_cmd(CMD25,sector,0X01);//连续读命令
		if(r1==0)
		{
			do
			{
				r1=send_data(buf,0xFC);//接收512个字节	 
				buf+=512;  
			}while(--cnt && r1==0);
			r1=send_data(0,0xFD);//接收512个字节 
		}
	}   
	disselect();//取消片选
	return r1;//
}

//private functions

u8 SDCard::select()
{
	cs=0;
	if(wait_ready()==0)return 0;//等待成功
	disselect();
	return 1;//等待失败
}

void SDCard::disselect()
{
	cs=1;
 	transByte(0xff);//提供额外的8个时钟
}

u8 SDCard::wait_ready()
{
	u32 t=0;
	do
	{
		if(transByte(0XFF)==0XFF)return 0;//OK
		t++;		  	
	}while(t<0XFFFFFF);//等待 
	return 1;
}

u8 SDCard::get_response(u8 res)
{
	u16 Count=0xFFFF;//等待次数	   						  
	while ((transByte(0XFF)!=res)&&Count)Count--;//等待得到准确的回应  	  
	if (Count==0)return MSD_RESPONSE_FAILURE;//得到回应失败   
	else return MSD_RESPONSE_NO_ERROR;//正确回应
}

u8 SDCard::recv_data(u8* buf,u16 len)
{
	if(get_response(0xFE))return 1;//等待SD卡发回数据起始令牌0xFE
	while(len--)//开始接收数据
	{
			*buf=transByte(0xFF);
			buf++;
	}
	//下面是2个伪CRC（dummy CRC）
	transByte(0xFF);
	transByte(0xFF);									  					    
	return 0;//读取成功
}

u8 SDCard::send_data(u8* buf,u8 cmd)
{
	u16 t;		  	  
	if(wait_ready())return 1;//等待准备失效
	transByte(cmd);
	if(cmd!=0XFD)//不是结束指令
	{
		for(t=0;t<512;t++)transByte(buf[t]);//提高速度,减少函数传参时间
	    transByte(0xFF);//忽略crc
	    transByte(0xFF);
		t=transByte(0xFF);//接收响应
		if((t&0x1F)!=0x05)return 2;//响应错误									  					    
	}						 									  					    
    return 0;//写入成功
}

u8 SDCard::send_cmd(u8 cmd,u32 arg,u8 crc)
{
  u8 r1;	
	u8 Retry=0; 
	disselect();//取消上次片选
	if(select())return 0XFF;//片选失效 
	//发送
	transByte(cmd | 0x40);//分别写入命令
	transByte(arg >> 24);
	transByte(arg >> 16);
	transByte(arg >> 8);
	transByte(arg);	  
	transByte(crc); 
	if(cmd==CMD12)transByte(0xff);//Skip a stuff byte when stop reading
    //等待响应，或超时退出
	Retry=0X1F;
	do
	{
		r1=transByte(0xFF);
	}while((r1&0X80) && Retry--);	 
	//返回状态值
  return r1;
}

//FAT相关重载

FatSD::FatSD()
{
	card=new GRSdio;
}

u8 FatSD::disk_initialize(u8 pdrv)
{
	u8 res=card->init();
	if(res)return STA_NOINIT;
	else return 0;
} 

u8 FatSD::disk_status(u8 pdrv)
{
	return 0;
}

u8 FatSD::disk_ioctl(u8 pdrv, u8 ctrl, void* buff)
{
	u8 res=0;
	switch(ctrl)
	{
		case CTRL_SYNC:
				res=RES_OK;
				break;	 
		case GET_SECTOR_SIZE:
				*(WORD*)buff = 512;
				res = RES_OK;
				break;	 
		case GET_BLOCK_SIZE:
				*(WORD*)buff = 8;
				res = RES_OK;
				break;	 
		case GET_SECTOR_COUNT:
				*(DWORD*)buff = card->cardInfo.CardCapacity/512;
				res = RES_OK;
				break;
		default:
				res = RES_PARERR;
				break;
	}
	return res;
}

u8 FatSD::disk_read(u8 pdrv, u8* buff, u32 sector, u32 count)
{
	u8 res=card->readDisk(buff,sector,count);
	while(res)
	{
		card->init();
		res=card->readDisk(buff,sector,count);
	}
	if(res==0)return RES_OK;
	else return RES_ERROR;
}

u8 FatSD::disk_write(u8 pdrv, const u8* buff, u32 sector, u32 count)
{
	u8 res=0;
	if(!count)return RES_PARERR;
	
	res=card->writeDisk((u8*)buff,sector,count);
	while(res)
	{
		card->init();
		res=card->writeDisk((u8*)buff,sector,count);
	}
	
	if(res==0)return RES_OK;
		else return RES_ERROR;
}


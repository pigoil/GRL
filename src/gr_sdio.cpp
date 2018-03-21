#include "gr_sdio.h"
#include "string.h"	 

//typedef unsigned int uint32_t;

//SDIO相关标志位
#define SDIO_FLAG_CCRCFAIL                  ((uint32_t)0x00000001)
#define SDIO_FLAG_DCRCFAIL                  ((uint32_t)0x00000002)
#define SDIO_FLAG_CTIMEOUT                  ((uint32_t)0x00000004)
#define SDIO_FLAG_DTIMEOUT                  ((uint32_t)0x00000008)
#define SDIO_FLAG_TXUNDERR                  ((uint32_t)0x00000010)
#define SDIO_FLAG_RXOVERR                   ((uint32_t)0x00000020)
#define SDIO_FLAG_CMDREND                   ((uint32_t)0x00000040)
#define SDIO_FLAG_CMDSENT                   ((uint32_t)0x00000080)
#define SDIO_FLAG_DATAEND                   ((uint32_t)0x00000100)
#define SDIO_FLAG_STBITERR                  ((uint32_t)0x00000200)
#define SDIO_FLAG_DBCKEND                   ((uint32_t)0x00000400)
#define SDIO_FLAG_CMDACT                    ((uint32_t)0x00000800)
#define SDIO_FLAG_TXACT                     ((uint32_t)0x00001000)
#define SDIO_FLAG_RXACT                     ((uint32_t)0x00002000)
#define SDIO_FLAG_TXFIFOHE                  ((uint32_t)0x00004000)
#define SDIO_FLAG_RXFIFOHF                  ((uint32_t)0x00008000)
#define SDIO_FLAG_TXFIFOF                   ((uint32_t)0x00010000)
#define SDIO_FLAG_RXFIFOF                   ((uint32_t)0x00020000)
#define SDIO_FLAG_TXFIFOE                   ((uint32_t)0x00040000)
#define SDIO_FLAG_RXFIFOE                   ((uint32_t)0x00080000)
#define SDIO_FLAG_TXDAVL                    ((uint32_t)0x00100000)
#define SDIO_FLAG_RXDAVL                    ((uint32_t)0x00200000)
#define SDIO_FLAG_SDIOIT                    ((uint32_t)0x00400000)
#define SDIO_FLAG_CEATAEND                  ((uint32_t)0x00800000)
////////////////////////////////////////////////////////////////////////////////////////////////////
//SDIO 指令集
#define SD_CMD_GO_IDLE_STATE                       ((u8)0)
#define SD_CMD_SEND_OP_COND                        ((u8)1)
#define SD_CMD_ALL_SEND_CID                        ((u8)2)
#define SD_CMD_SET_REL_ADDR                        ((u8)3) /*!< SDIO_SEND_REL_ADDR for SD Card */
#define SD_CMD_SET_DSR                             ((u8)4)
#define SD_CMD_SDIO_SEN_OP_COND                    ((u8)5)
#define SD_CMD_HS_SWITCH                           ((u8)6)
#define SD_CMD_SEL_DESEL_CARD                      ((u8)7)
#define SD_CMD_HS_SEND_EXT_CSD                     ((u8)8)
#define SD_CMD_SEND_CSD                            ((u8)9)
#define SD_CMD_SEND_CID                            ((u8)10)
#define SD_CMD_READ_DAT_UNTIL_STOP                 ((u8)11) /*!< SD Card doesn't support it */
#define SD_CMD_STOP_TRANSMISSION                   ((u8)12)
#define SD_CMD_SEND_STATUS                         ((u8)13)
#define SD_CMD_HS_BUSTEST_READ                     ((u8)14)
#define SD_CMD_GO_INACTIVE_STATE                   ((u8)15)
#define SD_CMD_SET_BLOCKLEN                        ((u8)16)
#define SD_CMD_READ_SINGLE_BLOCK                   ((u8)17)
#define SD_CMD_READ_MULT_BLOCK                     ((u8)18)
#define SD_CMD_HS_BUSTEST_WRITE                    ((u8)19)
#define SD_CMD_WRITE_DAT_UNTIL_STOP                ((u8)20) 
#define SD_CMD_SET_BLOCK_COUNT                     ((u8)23) 
#define SD_CMD_WRITE_SINGLE_BLOCK                  ((u8)24)
#define SD_CMD_WRITE_MULT_BLOCK                    ((u8)25)
#define SD_CMD_PROG_CID                            ((u8)26)
#define SD_CMD_PROG_CSD                            ((u8)27)
#define SD_CMD_SET_WRITE_PROT                      ((u8)28)
#define SD_CMD_CLR_WRITE_PROT                      ((u8)29)
#define SD_CMD_SEND_WRITE_PROT                     ((u8)30)
#define SD_CMD_SD_ERASE_GRP_START                  ((u8)32) /*!< To set the address of the first write
                                                                  block to be erased. (For SD card only) */
#define SD_CMD_SD_ERASE_GRP_END                    ((u8)33) /*!< To set the address of the last write block of the
                                                                  continuous range to be erased. (For SD card only) */
#define SD_CMD_ERASE_GRP_START                     ((u8)35) /*!< To set the address of the first write block to be erased.
                                                                  (For MMC card only spec 3.31) */

#define SD_CMD_ERASE_GRP_END                       ((u8)36) /*!< To set the address of the last write block of the
                                                                  continuous range to be erased. (For MMC card only spec 3.31) */

#define SD_CMD_ERASE                               ((u8)38)
#define SD_CMD_FAST_IO                             ((u8)39) /*!< SD Card doesn't support it */
#define SD_CMD_GO_IRQ_STATE                        ((u8)40) /*!< SD Card doesn't support it */
#define SD_CMD_LOCK_UNLOCK                         ((u8)42)
#define SD_CMD_APP_CMD                             ((u8)55)
#define SD_CMD_GEN_CMD                             ((u8)56)
#define SD_CMD_NO_CMD                              ((u8)64)

/** 
  * @brief Following commands are SD Card Specific commands.
  *        SDIO_APP_CMD ：CMD55 should be sent before sending these commands. 
  */
#define SD_CMD_APP_SD_SET_BUSWIDTH                 ((u8)6)  /*!< For SD Card only */
#define SD_CMD_SD_APP_STAUS                        ((u8)13) /*!< For SD Card only */
#define SD_CMD_SD_APP_SEND_NUM_WRITE_BLOCKS        ((u8)22) /*!< For SD Card only */
#define SD_CMD_SD_APP_OP_COND                      ((u8)41) /*!< For SD Card only */
#define SD_CMD_SD_APP_SET_CLR_CARD_DETECT          ((u8)42) /*!< For SD Card only */
#define SD_CMD_SD_APP_SEND_SCR                     ((u8)51) /*!< For SD Card only */
#define SD_CMD_SDIO_RW_DIRECT                      ((u8)52) /*!< For SD I/O Card only */
#define SD_CMD_SDIO_RW_EXTENDED                    ((u8)53) /*!< For SD I/O Card only */

/** 
  * @brief Following commands are SD Card Specific security commands.
  *        SDIO_APP_CMD should be sent before sending these commands. 
  */
#define SD_CMD_SD_APP_GET_MKB                      ((u8)43) /*!< For SD Card only */
#define SD_CMD_SD_APP_GET_MID                      ((u8)44) /*!< For SD Card only */
#define SD_CMD_SD_APP_SET_CER_RN1                  ((u8)45) /*!< For SD Card only */
#define SD_CMD_SD_APP_GET_CER_RN2                  ((u8)46) /*!< For SD Card only */
#define SD_CMD_SD_APP_SET_CER_RES2                 ((u8)47) /*!< For SD Card only */
#define SD_CMD_SD_APP_GET_CER_RES1                 ((u8)48) /*!< For SD Card only */
#define SD_CMD_SD_APP_SECURE_READ_MULTIPLE_BLOCK   ((u8)18) /*!< For SD Card only */
#define SD_CMD_SD_APP_SECURE_WRITE_MULTIPLE_BLOCK  ((u8)25) /*!< For SD Card only */
#define SD_CMD_SD_APP_SECURE_ERASE                 ((u8)38) /*!< For SD Card only */
#define SD_CMD_SD_APP_CHANGE_SECURE_AREA           ((u8)49) /*!< For SD Card only */
#define SD_CMD_SD_APP_SECURE_WRITE_MKB             ((u8)48) /*!< For SD Card only */
  			   
//支持的SD卡定义
#define SDIO_STD_CAPACITY_SD_CARD_V1_1             ((u32)0x00000000)
#define SDIO_STD_CAPACITY_SD_CARD_V2_0             ((u32)0x00000001)
#define SDIO_HIGH_CAPACITY_SD_CARD                 ((u32)0x00000002)
#define SDIO_MULTIMEDIA_CARD                       ((u32)0x00000003)
#define SDIO_SECURE_DIGITAL_IO_CARD                ((u32)0x00000004)
#define SDIO_HIGH_SPEED_MULTIMEDIA_CARD            ((u32)0x00000005)
#define SDIO_SECURE_DIGITAL_IO_COMBO_CARD          ((u32)0x00000006)
#define SDIO_HIGH_CAPACITY_MMC_CARD                ((u32)0x00000007)

//SDIO相关参数定义
#define NULL 0
#define SDIO_STATIC_FLAGS               ((u32)0x000005FF)
#define SDIO_CMD0TIMEOUT                ((u32)0x00002710)	  
#define SDIO_DATATIMEOUT                ((u32)0x0001FFFF)	  
#define SDIO_FIFO_Address               ((u32)0x40018080)

//Mask for errors Card Status R1 (OCR Register)  
#define SD_OCR_ADDR_OUT_OF_RANGE        ((u32)0x80000000)
#define SD_OCR_ADDR_MISALIGNED          ((u32)0x40000000)
#define SD_OCR_BLOCK_LEN_ERR            ((u32)0x20000000)
#define SD_OCR_ERASE_SEQ_ERR            ((u32)0x10000000)
#define SD_OCR_BAD_ERASE_PARAM          ((u32)0x08000000)
#define SD_OCR_WRITE_PROT_VIOLATION     ((u32)0x04000000)
#define SD_OCR_LOCK_UNLOCK_FAILED       ((u32)0x01000000)
#define SD_OCR_COM_CRC_FAILED           ((u32)0x00800000)
#define SD_OCR_ILLEGAL_CMD              ((u32)0x00400000)
#define SD_OCR_CARD_ECC_FAILED          ((u32)0x00200000)
#define SD_OCR_CC_ERROR                 ((u32)0x00100000)
#define SD_OCR_GENERAL_UNKNOWN_ERROR    ((u32)0x00080000)
#define SD_OCR_STREAM_READ_UNDERRUN     ((u32)0x00040000)
#define SD_OCR_STREAM_WRITE_OVERRUN     ((u32)0x00020000)
#define SD_OCR_CID_CSD_OVERWRIETE       ((u32)0x00010000)
#define SD_OCR_WP_ERASE_SKIP            ((u32)0x00008000)
#define SD_OCR_CARD_ECC_DISABLED        ((u32)0x00004000)
#define SD_OCR_ERASE_RESET              ((u32)0x00002000)
#define SD_OCR_AKE_SEQ_ERROR            ((u32)0x00000008)
#define SD_OCR_ERRORBITS                ((u32)0xFDFFE008)

//Masks for R6 Response 
#define SD_R6_GENERAL_UNKNOWN_ERROR     ((u32)0x00002000)
#define SD_R6_ILLEGAL_CMD               ((u32)0x00004000)
#define SD_R6_COM_CRC_FAILED            ((u32)0x00008000)

#define SD_VOLTAGE_WINDOW_SD            ((u32)0x80100000)
#define SD_HIGH_CAPACITY                ((u32)0x40000000)
#define SD_STD_CAPACITY                 ((u32)0x00000000)
#define SD_CHECK_PATTERN                ((u32)0x000001AA)
#define SD_VOLTAGE_WINDOW_MMC           ((u32)0x80FF8000)

#define SD_MAX_VOLT_TRIAL               ((u32)0x0000FFFF)
#define SD_ALLZERO                      ((u32)0x00000000)

#define SD_WIDE_BUS_SUPPORT             ((u32)0x00040000)
#define SD_SINGLE_BUS_SUPPORT           ((u32)0x00010000)
#define SD_CARD_LOCKED                  ((u32)0x02000000)
#define SD_CARD_PROGRAMMING             ((u32)0x00000007)
#define SD_CARD_RECEIVING               ((u32)0x00000006)
#define SD_DATATIMEOUT                  ((u32)0x000FFFFF)
#define SD_0TO7BITS                     ((u32)0x000000FF)
#define SD_8TO15BITS                    ((u32)0x0000FF00)
#define SD_16TO23BITS                   ((u32)0x00FF0000)
#define SD_24TO31BITS                   ((u32)0xFF000000)
#define SD_MAX_DATA_LENGTH              ((u32)0x01FFFFFF)

#define SD_HALFFIFO                     ((u32)0x00000008)
#define SD_HALFFIFOBYTES                ((u32)0x00000020)

//Command Class Supported  
#define SD_CCCC_LOCK_UNLOCK             ((u32)0x00000080)
#define SD_CCCC_WRITE_PROT              ((u32)0x00000040)
#define SD_CCCC_ERASE                   ((u32)0x00000020)
																	 
//CMD8指令
#define SDIO_SEND_IF_COND               ((u32)0x00000008)


//用户配置区			  
//SDIO时钟计算公式:SDIO_CK时钟=SDIOCLK/[clkdiv+2];其中,SDIOCLK一般为72Mhz
//使用DMA模式的时候,传输速率可以到24Mhz,不过如果你的卡不是高速卡,可能也会出错
//出错就请降低时钟,使用查询模式的话,推荐SDIO_TRANSFER_CLK_DIV设置为3或者更大
#define SDIO_INIT_CLK_DIV        0xB2 		//SDIO初始化频率，最大400Kh  
#define SDIO_TRANSFER_CLK_DIV    0x04		//SDIO传输频率,该值太小可能会导致读写文件出错 
//SDIO工作模式定义,通过SD_SetDeviceMode函数设置.
#define SD_POLLING_MODE    	0  	//查询模式,该模式下,如果读写有问题,建议增大SDIO_TRANSFER_CLK_DIV的设置.
#define SD_DMA_MODE    		1	//DMA模式,该模式下,如果读写有问题,建议增大SDIO_TRANSFER_CLK_DIV的设置.   

static u8 CardType=SDIO_STD_CAPACITY_SD_CARD_V1_1;		//SD卡类型（默认为1.x卡）
static u32 CSD_Tab[4],CID_Tab[4],RCA=0;					//SD卡CSD,CID以及相对地址(RCA)数据
static u8 DeviceMode=SD_DMA_MODE;		   				//工作模式,注意,工作模式必须通过SD_SetDeviceMode,后才算数.这里只是定义一个默认的模式(SD_DMA_MODE)
static u8 StopCondition=0; 								//是否发送停止传输标志位,DMA多块读写的时候用到  
volatile GRSdio::SdErr TransferError=GRSdio::SD_OK;					//数据传输错误标志,DMA读写时使用	    
volatile u8 TransferEnd=0;								//传输结束标志,DMA读写时使用

#pragma pack(4)
u8 SDIO_DATA_BUFFER[512];
#pragma pack()

GRSdio::GRSdio()
{
	GRCore::CIRQReg(GRCore::Sdio,this);
}

GRSdio::SdErr GRSdio::init()
{
	u8 clkdiv=0;
	SdErr errorstatus=SD_OK;	   
	//SDIO IO口初始化

  RCC->AHBENR|=1<<10;    	//使能SDIO时钟	   	 
 	RCC->AHBENR|=1<<1;    	//使能DMA2时钟

	RCC->APB2ENR|=1<<4;    	//使能PORTC时钟	   	 
	RCC->APB2ENR|=1<<5;    	//使能PORTD时钟
	GPIOC->CRH&=0XFFF00000; 
	GPIOC->CRH|=0X000BBBBB;	//PC.8~12 复用输出

	GPIOD->CRL&=0XFFFFF0FF; 
	GPIOD->CRL|=0X00000B00;	//PD2复用输出,PD7 上拉输入
 	//SDIO外设寄存器设置为默认值 			   
	SDIO->POWER=0x00000000;
	SDIO->CLKCR=0x00000000;
	SDIO->ARG=0x00000000;
	SDIO->CMD=0x00000000;
	SDIO->DTIMER=0x00000000;
	SDIO->DLEN=0x00000000;
	SDIO->DCTRL=0x00000000;
	SDIO->ICR=0x00C007FF;
	SDIO->MASK=0x00000000;	  
 	GRCore::NvicInit(0,0,SDIO_IRQn,2);		//SDIO中断配置
   	errorstatus=power_on();			//SD卡上电
 	if(errorstatus==SD_OK)errorstatus=init_card();			//初始化SD卡														  
  	if(errorstatus==SD_OK)errorstatus=get_card_info(&cardInfo);	//获取卡信息
 	if(errorstatus==SD_OK)errorstatus=select_deselect((u32)(cardInfo.RCA<<16));//选中SD卡   
   	if(errorstatus==SD_OK)errorstatus=enable_wide_bus_operation(1);	//4位宽度,如果是MMC卡,则不能用4位模式 
  	if((errorstatus==SD_OK)||(SDIO_MULTIMEDIA_CARD==CardType))
	{  		    
		if(cardInfo.CardType==SDIO_STD_CAPACITY_SD_CARD_V1_1||cardInfo.CardType==SDIO_STD_CAPACITY_SD_CARD_V2_0)
		{
			clkdiv=SDIO_TRANSFER_CLK_DIV+6;	//V1.1/V2.0卡，设置最高72/12=6Mhz
		}else clkdiv=SDIO_TRANSFER_CLK_DIV;	//SDHC等其他卡，设置最高72/6=12Mhz
		clk_set(clkdiv);				//设置时钟频率,SDIO时钟计算公式:SDIO_CK时钟=SDIOCLK/[clkdiv+2];其中,SDIOCLK固定为48Mhz 
		//errorstatus=SD_SetDeviceMode(SD_DMA_MODE);	//设置为DMA模式
		errorstatus=set_device_mode(SD_POLLING_MODE);	//设置为查询模式
 	}
	return errorstatus;		
}

u8 GRSdio::readDisk(u8*buf,u32 sector,u8 cnt)
{
	u8 sta=SD_OK;
	long long lsector=sector;
	u8 n;
	lsector<<=9;
	if((u32)buf%4!=0)
	{
	 	for(n=0;n<cnt;n++)
		{
		 	sta=read_block(SDIO_DATA_BUFFER,lsector+512*n,512);//单个sector的读操作
			memcpy(buf,SDIO_DATA_BUFFER,512);
			buf+=512;
		} 
	}else
	{
		if(cnt==1)sta=read_block(buf,lsector,512);    	//单个sector的读操作
		else sta=read_multi_block(buf,lsector,512,cnt);//多个sector  
	}
	return sta;
}

u8 GRSdio::writeDisk(u8*buf,u32 sector,u8 cnt)
{
	u8 sta=SD_OK;
	u8 n;
	long long lsector=sector;
	lsector<<=9;
	if((u32)buf%4!=0)
	{
	 	for(n=0;n<cnt;n++)
		{
			memcpy(SDIO_DATA_BUFFER,buf,512);
		 	sta=write_block(SDIO_DATA_BUFFER,lsector+512*n,512);//单个sector的写操作
			buf+=512;
		} 
	}else
	{
		if(cnt==1)sta=write_block(buf,lsector,512);    	//单个sector的写操作
		else sta=write_multi_block(buf,lsector,512,cnt);	//多个sector
	}
	return sta;
}

void GRSdio::clk_set(u8 clkdiv)
{
	u32 tmpreg=SDIO->CLKCR; 
  tmpreg&=0XFFFFFF00; 
 	tmpreg|=clkdiv;   
	SDIO->CLKCR=tmpreg;
}

void GRSdio::send_cmd(u8 cmdindex,u8 waitrsp,u32 arg)
{
	u32 tmpreg;
	SDIO->ARG=arg;
	tmpreg=SDIO->CMD; 
	tmpreg&=0XFFFFF800;		//清除index和waitrsp
	tmpreg|=cmdindex&0X3F;	//设置新的index			 
	tmpreg|=waitrsp<<6;		//设置新的wait rsp 
	tmpreg|=0<<8;			//无等待
  tmpreg|=1<<10;			//命令通道状态机使能
	SDIO->CMD=tmpreg;
}

void GRSdio::send_data_cfg(u32 datatimeout,u32 datalen,u8 blksize,u8 dir)
{
	u32 tmpreg;
	SDIO->DTIMER=datatimeout;
  SDIO->DLEN=datalen&0X1FFFFFF;	//低25位有效
	tmpreg=SDIO->DCTRL; 
	tmpreg&=0xFFFFFF08;		//清除之前的设置.
	tmpreg|=blksize<<4;		//设置块大小
	tmpreg|=0<<2;			//块数据传输
	tmpreg|=(dir&0X01)<<1;	//方向控制
	tmpreg|=1<<0;			//数据传输使能,DPSM状态机
	SDIO->DCTRL=tmpreg;		
}

GRSdio::SdErr GRSdio::power_on()
{
 	u8 i=0;
	SdErr errorstatus=SD_OK;
	u32 response=0,count=0,validvoltage=0;
	u32 SDType=SD_STD_CAPACITY;
	//配置CLKCR寄存器 
	SDIO->CLKCR=0;				//清空CLKCR之前的设置
	SDIO->CLKCR|=0<<9;			//非省电模式
	SDIO->CLKCR|=0<<10;			//关闭旁路,CK根据分频设置输出
	SDIO->CLKCR|=0<<11;			//1位数据宽度
	SDIO->CLKCR|=0<<13;			//SDIOCLK上升沿产生SDIOCK
	SDIO->CLKCR|=0<<14;			//关闭硬件流控制    
	clk_set(SDIO_INIT_CLK_DIV);//设置时钟频率(初始化的时候,不能超过400Khz)			 
 	SDIO->POWER=0X03;			//上电状态,开启卡时钟    
  	SDIO->CLKCR|=1<<8;			//SDIOCK使能   
   	for(i=0;i<74;i++)
	{
		send_cmd(SD_CMD_GO_IDLE_STATE,0,0);//发送CMD0进入IDLE STAGE模式命令.												  
		errorstatus=cmd_error();
		if(errorstatus==SD_OK)break;
 	}
 	if(errorstatus)return errorstatus;//返回错误状态
	send_cmd(SDIO_SEND_IF_COND,1,SD_CHECK_PATTERN);//发送CMD8,短响应,检查SD卡接口特性.
 														//arg[11:8]:01,支持电压范围,2.7~3.6V
														//arg[7:0]:默认0XAA
														//返回响应7
  	errorstatus=cmd_res7_error();						//等待R7响应
 	if(errorstatus==SD_OK) 								//R7响应正常
	{
		CardType=SDIO_STD_CAPACITY_SD_CARD_V2_0;		//SD 2.0卡
		SDType=SD_HIGH_CAPACITY;			   			//高容量卡
	}
	send_cmd(SD_CMD_APP_CMD,1,0);					//发送CMD55,短响应	 
	errorstatus=cmd_res1_error(SD_CMD_APP_CMD); 		 	//等待R1响应   
	if(errorstatus==SD_OK)//SD2.0/SD 1.1,否则为MMC卡
	{																  
		//SD卡,发送ACMD41 SD_APP_OP_COND,参数为:0x80100000 
		while((!validvoltage)&&(count<SD_MAX_VOLT_TRIAL))
		{	   										   
			send_cmd(SD_CMD_APP_CMD,1,0);				//发送CMD55,短响应	 
			errorstatus=cmd_res1_error(SD_CMD_APP_CMD); 	 	//等待R1响应   
 			if(errorstatus!=SD_OK)return errorstatus;   	//响应错误
			send_cmd(SD_CMD_SD_APP_OP_COND,1,SD_VOLTAGE_WINDOW_SD|SDType);//发送ACMD41,短响应	 
			errorstatus=cmd_res3_error(); 					//等待R3响应   
 			if(errorstatus!=SD_OK)return errorstatus;   	//响应错误  
			response=SDIO->RESP1;;			   				//得到响应
			validvoltage=(((response>>31)==1)?1:0);			//判断SD卡上电是否完成
			count++;
		}
		if(count>=SD_MAX_VOLT_TRIAL)
		{
			errorstatus=SD_INVALID_VOLTRANGE;
			return errorstatus;
		}	 
		if(response&=SD_HIGH_CAPACITY)
		{
			CardType=SDIO_HIGH_CAPACITY_SD_CARD;
		}
 	}else//MMC卡
	{
		//MMC卡,发送CMD1 SDIO_SEND_OP_COND,参数为:0x80FF8000 
		while((!validvoltage)&&(count<SD_MAX_VOLT_TRIAL))
		{	   										   				   
			send_cmd(SD_CMD_SEND_OP_COND,1,SD_VOLTAGE_WINDOW_MMC);//发送CMD1,短响应	 
			errorstatus=cmd_res3_error(); 					//等待R3响应   
 			if(errorstatus!=SD_OK)return errorstatus;   	//响应错误  
			response=SDIO->RESP1;;			   				//得到响应
			validvoltage=(((response>>31)==1)?1:0);
			count++;
		}
		if(count>=SD_MAX_VOLT_TRIAL)
		{
			errorstatus=SD_INVALID_VOLTRANGE;
			return errorstatus;
		}	 			    
		CardType=SDIO_MULTIMEDIA_CARD;	  
  	}  
  	return(errorstatus);		
}

GRSdio::SdErr GRSdio::power_off()
{
  SDIO->POWER&=~(3<<0);//SDIO电源关闭,时钟停止	
	return SD_OK;		 
}

GRSdio::SdErr GRSdio::init_card()
{
 	SdErr errorstatus=SD_OK;
	u16 rca = 0x01;
 	if((SDIO->POWER&0X03)==0)return SD_REQUEST_NOT_APPLICABLE;//检查电源状态,确保为上电状态
 	if(SDIO_SECURE_DIGITAL_IO_CARD!=CardType)			//非SECURE_DIGITAL_IO_CARD
	{
		send_cmd(SD_CMD_ALL_SEND_CID,3,0);			//发送CMD2,取得CID,长响应	 
		errorstatus=cmd_res2_error(); 					//等待R2响应   
		if(errorstatus!=SD_OK)return errorstatus;   	//响应错误		    
 		CID_Tab[0]=SDIO->RESP1;
		CID_Tab[1]=SDIO->RESP2;
		CID_Tab[2]=SDIO->RESP3;
		CID_Tab[3]=SDIO->RESP4;
	}
	if((SDIO_STD_CAPACITY_SD_CARD_V1_1==CardType)||(SDIO_STD_CAPACITY_SD_CARD_V2_0==CardType)||(SDIO_SECURE_DIGITAL_IO_COMBO_CARD==CardType)||(SDIO_HIGH_CAPACITY_SD_CARD==CardType))//判断卡类型
	{
		send_cmd(SD_CMD_SET_REL_ADDR,1,0);			//发送CMD3,短响应 
		errorstatus=cmd_res6_error(SD_CMD_SET_REL_ADDR,&rca);//等待R6响应 
		if(errorstatus!=SD_OK)return errorstatus;   	//响应错误		    
	}   
    if (SDIO_MULTIMEDIA_CARD==CardType)
    {
 		send_cmd(SD_CMD_SET_REL_ADDR,1,(u32)(rca<<16));//发送CMD3,短响应 	   
		errorstatus=cmd_res2_error(); 					//等待R2响应   
		if(errorstatus!=SD_OK)return errorstatus;   	//响应错误	 
    }
	if (SDIO_SECURE_DIGITAL_IO_CARD!=CardType)			//非SECURE_DIGITAL_IO_CARD
	{
		RCA = rca;
		send_cmd(SD_CMD_SEND_CSD,3,(u32)(rca<<16));//发送CMD9+卡RCA,取得CSD,长响应 	   
		errorstatus=cmd_res2_error(); 					//等待R2响应   
		if(errorstatus!=SD_OK)return errorstatus;   	//响应错误		    
  		CSD_Tab[0]=SDIO->RESP1;
		CSD_Tab[1]=SDIO->RESP2;
		CSD_Tab[2]=SDIO->RESP3;						
		CSD_Tab[3]=SDIO->RESP4;					    
	}
	return SD_OK;//卡初始化成功
}

GRSdio::SdErr GRSdio::get_card_info(GRSdio::SdInfo* cardinfo)
{
 	SdErr errorstatus=SD_OK;
	u8 tmp=0;	   
	cardinfo->CardType=(u8)CardType; 				//卡类型
	cardinfo->RCA=(u16)RCA;							//卡RCA值
	tmp=(u8)((CSD_Tab[0]&0xFF000000)>>24);
	cardinfo->SD_csd.CSDStruct=(tmp&0xC0)>>6;		//CSD结构
	cardinfo->SD_csd.SysSpecVersion=(tmp&0x3C)>>2;	//2.0协议还没定义这部分(为保留),应该是后续协议定义的
	cardinfo->SD_csd.Reserved1=tmp&0x03;			//2个保留位  
	tmp=(u8)((CSD_Tab[0]&0x00FF0000)>>16);			//第1个字节
	cardinfo->SD_csd.TAAC=tmp;				   		//数据读时间1
	tmp=(u8)((CSD_Tab[0]&0x0000FF00)>>8);	  		//第2个字节
	cardinfo->SD_csd.NSAC=tmp;		  				//数据读时间2
	tmp=(u8)(CSD_Tab[0]&0x000000FF);				//第3个字节
	cardinfo->SD_csd.MaxBusClkFrec=tmp;		  		//传输速度	   
	tmp=(u8)((CSD_Tab[1]&0xFF000000)>>24);			//第4个字节
	cardinfo->SD_csd.CardComdClasses=tmp<<4;    	//卡指令类高四位
	tmp=(u8)((CSD_Tab[1]&0x00FF0000)>>16);	 		//第5个字节
	cardinfo->SD_csd.CardComdClasses|=(tmp&0xF0)>>4;//卡指令类低四位
	cardinfo->SD_csd.RdBlockLen=tmp&0x0F;	    	//最大读取数据长度
	tmp=(u8)((CSD_Tab[1]&0x0000FF00)>>8);			//第6个字节
	cardinfo->SD_csd.PartBlockRead=(tmp&0x80)>>7;	//允许分块读
	cardinfo->SD_csd.WrBlockMisalign=(tmp&0x40)>>6;	//写块错位
	cardinfo->SD_csd.RdBlockMisalign=(tmp&0x20)>>5;	//读块错位
	cardinfo->SD_csd.DSRImpl=(tmp&0x10)>>4;
	cardinfo->SD_csd.Reserved2=0; 					//保留
 	if((CardType==SDIO_STD_CAPACITY_SD_CARD_V1_1)||(CardType==SDIO_STD_CAPACITY_SD_CARD_V2_0)||(SDIO_MULTIMEDIA_CARD==CardType))//标准1.1/2.0卡/MMC卡
	{
		cardinfo->SD_csd.DeviceSize=(tmp&0x03)<<10;	//C_SIZE(12位)
	 	tmp=(u8)(CSD_Tab[1]&0x000000FF); 			//第7个字节	
		cardinfo->SD_csd.DeviceSize|=(tmp)<<2;
 		tmp=(u8)((CSD_Tab[2]&0xFF000000)>>24);		//第8个字节	
		cardinfo->SD_csd.DeviceSize|=(tmp&0xC0)>>6;
 		cardinfo->SD_csd.MaxRdCurrentVDDMin=(tmp&0x38)>>3;
		cardinfo->SD_csd.MaxRdCurrentVDDMax=(tmp&0x07);
 		tmp=(u8)((CSD_Tab[2]&0x00FF0000)>>16);		//第9个字节	
		cardinfo->SD_csd.MaxWrCurrentVDDMin=(tmp&0xE0)>>5;
		cardinfo->SD_csd.MaxWrCurrentVDDMax=(tmp&0x1C)>>2;
		cardinfo->SD_csd.DeviceSizeMul=(tmp&0x03)<<1;//C_SIZE_MULT
 		tmp=(u8)((CSD_Tab[2]&0x0000FF00)>>8);	  	//第10个字节	
		cardinfo->SD_csd.DeviceSizeMul|=(tmp&0x80)>>7;
 		cardinfo->CardCapacity=(cardinfo->SD_csd.DeviceSize+1);//计算卡容量
		cardinfo->CardCapacity*=(1<<(cardinfo->SD_csd.DeviceSizeMul+2));
		cardinfo->CardBlockSize=1<<(cardinfo->SD_csd.RdBlockLen);//块大小
		cardinfo->CardCapacity*=cardinfo->CardBlockSize;
	}else if(CardType==SDIO_HIGH_CAPACITY_SD_CARD)	//高容量卡
	{
 		tmp=(u8)(CSD_Tab[1]&0x000000FF); 		//第7个字节	
		cardinfo->SD_csd.DeviceSize=(tmp&0x3F)<<16;//C_SIZE
 		tmp=(u8)((CSD_Tab[2]&0xFF000000)>>24); 	//第8个字节	
 		cardinfo->SD_csd.DeviceSize|=(tmp<<8);
 		tmp=(u8)((CSD_Tab[2]&0x00FF0000)>>16);	//第9个字节	
 		cardinfo->SD_csd.DeviceSize|=(tmp);
 		tmp=(u8)((CSD_Tab[2]&0x0000FF00)>>8); 	//第10个字节	
 		cardinfo->CardCapacity=(long long)(cardinfo->SD_csd.DeviceSize+1)*512*1024;//计算卡容量
		cardinfo->CardBlockSize=512; 			//块大小固定为512字节
	}	  
	cardinfo->SD_csd.EraseGrSize=(tmp&0x40)>>6;
	cardinfo->SD_csd.EraseGrMul=(tmp&0x3F)<<1;	   
	tmp=(u8)(CSD_Tab[2]&0x000000FF);			//第11个字节	
	cardinfo->SD_csd.EraseGrMul|=(tmp&0x80)>>7;
	cardinfo->SD_csd.WrProtectGrSize=(tmp&0x7F);
 	tmp=(u8)((CSD_Tab[3]&0xFF000000)>>24);		//第12个字节	
	cardinfo->SD_csd.WrProtectGrEnable=(tmp&0x80)>>7;
	cardinfo->SD_csd.ManDeflECC=(tmp&0x60)>>5;
	cardinfo->SD_csd.WrSpeedFact=(tmp&0x1C)>>2;
	cardinfo->SD_csd.MaxWrBlockLen=(tmp&0x03)<<2;	 
	tmp=(u8)((CSD_Tab[3]&0x00FF0000)>>16);		//第13个字节
	cardinfo->SD_csd.MaxWrBlockLen|=(tmp&0xC0)>>6;
	cardinfo->SD_csd.WriteBlockPaPartial=(tmp&0x20)>>5;
	cardinfo->SD_csd.Reserved3=0;
	cardinfo->SD_csd.ContentProtectAppli=(tmp&0x01);  
	tmp=(u8)((CSD_Tab[3]&0x0000FF00)>>8);		//第14个字节
	cardinfo->SD_csd.FileFormatGrouop=(tmp&0x80)>>7;
	cardinfo->SD_csd.CopyFlag=(tmp&0x40)>>6;
	cardinfo->SD_csd.PermWrProtect=(tmp&0x20)>>5;
	cardinfo->SD_csd.TempWrProtect=(tmp&0x10)>>4;
	cardinfo->SD_csd.FileFormat=(tmp&0x0C)>>2;
	cardinfo->SD_csd.ECC=(tmp&0x03);  
	tmp=(u8)(CSD_Tab[3]&0x000000FF);			//第15个字节
	cardinfo->SD_csd.CSD_CRC=(tmp&0xFE)>>1;
	cardinfo->SD_csd.Reserved4=1;		 
	tmp=(u8)((CID_Tab[0]&0xFF000000)>>24);		//第0个字节
	cardinfo->SD_cid.ManufacturerID=tmp;		    
	tmp=(u8)((CID_Tab[0]&0x00FF0000)>>16);		//第1个字节
	cardinfo->SD_cid.OEM_AppliID=tmp<<8;	  
	tmp=(u8)((CID_Tab[0]&0x000000FF00)>>8);		//第2个字节
	cardinfo->SD_cid.OEM_AppliID|=tmp;	    
	tmp=(u8)(CID_Tab[0]&0x000000FF);			//第3个字节	
	cardinfo->SD_cid.ProdName1=tmp<<24;				  
	tmp=(u8)((CID_Tab[1]&0xFF000000)>>24); 		//第4个字节
	cardinfo->SD_cid.ProdName1|=tmp<<16;	  
	tmp=(u8)((CID_Tab[1]&0x00FF0000)>>16);	   	//第5个字节
	cardinfo->SD_cid.ProdName1|=tmp<<8;		 
	tmp=(u8)((CID_Tab[1]&0x0000FF00)>>8);		//第6个字节
	cardinfo->SD_cid.ProdName1|=tmp;		   
	tmp=(u8)(CID_Tab[1]&0x000000FF);	  		//第7个字节
	cardinfo->SD_cid.ProdName2=tmp;			  
	tmp=(u8)((CID_Tab[2]&0xFF000000)>>24); 		//第8个字节
	cardinfo->SD_cid.ProdRev=tmp;		 
	tmp=(u8)((CID_Tab[2]&0x00FF0000)>>16);		//第9个字节
	cardinfo->SD_cid.ProdSN=tmp<<24;	   
	tmp=(u8)((CID_Tab[2]&0x0000FF00)>>8); 		//第10个字节
	cardinfo->SD_cid.ProdSN|=tmp<<16;	   
	tmp=(u8)(CID_Tab[2]&0x000000FF);   			//第11个字节
	cardinfo->SD_cid.ProdSN|=tmp<<8;		   
	tmp=(u8)((CID_Tab[3]&0xFF000000)>>24); 		//第12个字节
	cardinfo->SD_cid.ProdSN|=tmp;			     
	tmp=(u8)((CID_Tab[3]&0x00FF0000)>>16);	 	//第13个字节
	cardinfo->SD_cid.Reserved1|=(tmp&0xF0)>>4;
	cardinfo->SD_cid.ManufactDate=(tmp&0x0F)<<8;    
	tmp=(u8)((CID_Tab[3]&0x0000FF00)>>8);		//第14个字节
	cardinfo->SD_cid.ManufactDate|=tmp;		 	  
	tmp=(u8)(CID_Tab[3]&0x000000FF);			//第15个字节
	cardinfo->SD_cid.CID_CRC=(tmp&0xFE)>>1;
	cardinfo->SD_cid.Reserved2=1;	 
	return errorstatus;
}

GRSdio::SdErr GRSdio::enable_wide_bus_operation(u32 wmode)
{
  SdErr errorstatus=SD_OK;
 	if(SDIO_MULTIMEDIA_CARD==CardType)return SD_UNSUPPORTED_FEATURE;//MMC卡不支持
 	else if((SDIO_STD_CAPACITY_SD_CARD_V1_1==CardType)||(SDIO_STD_CAPACITY_SD_CARD_V2_0==CardType)||(SDIO_HIGH_CAPACITY_SD_CARD==CardType))
	{
		if(wmode>=2)return SD_UNSUPPORTED_FEATURE;//不支持8位模式
 		else   
		{
			errorstatus=en_wide_bus(wmode);
 			if(SD_OK==errorstatus)
			{
				SDIO->CLKCR&=~(3<<11);		//清除之前的位宽设置    
				SDIO->CLKCR|=(u16)wmode<<11;//1位/4位总线宽度 
				SDIO->CLKCR|=0<<14;			//不开启硬件流控制
			}
		}  
	}
	return errorstatus; 
}

GRSdio::SdErr GRSdio::set_device_mode(u32 Mode)
{
	SdErr errorstatus = SD_OK;
 	if((Mode==SD_DMA_MODE)||(Mode==SD_POLLING_MODE))DeviceMode=Mode;
	else errorstatus=SD_INVALID_PARAMETER;
	return errorstatus;	    
}

GRSdio::SdErr GRSdio::select_deselect(u32 addr)
{
 	send_cmd(SD_CMD_SEL_DESEL_CARD,1,addr);	//发送CMD7,选择卡,短响应	 	   
   	return cmd_res1_error(SD_CMD_SEL_DESEL_CARD);	  
}

GRSdio::SdErr GRSdio::send_status(unsigned int* pcardstatus)
{
	SdErr errorstatus = SD_OK;
	if(pcardstatus==NULL)
	{
		errorstatus=SD_INVALID_PARAMETER;
		return errorstatus;
	}
 	send_cmd(SD_CMD_SEND_STATUS,1,RCA<<16);	//发送CMD13,短响应		 
	errorstatus=cmd_res1_error(SD_CMD_SEND_STATUS);	//查询响应状态 
	if(errorstatus!=SD_OK)return errorstatus;
	*pcardstatus=SDIO->RESP1;//读取响应值
	return errorstatus;
}

GRSdio::SdCardState GRSdio::get_status()
{
	unsigned int resp1=0;
	if(send_status(&resp1)!=SD_OK)return SD_CARD_ERROR;
	else return (SdCardState)((resp1>>9) & 0x0F);
}

GRSdio::SdErr GRSdio::read_block(u8 *buf,long long addr,u16 blksize)
{
	SdErr errorstatus=SD_OK;
	u8 power;
  u32 count=0,*tempbuff=(u32*)buf;//转换为u32指针 
	u32 timeout=SDIO_DATATIMEOUT;   
   	if(NULL==buf)return SD_INVALID_PARAMETER; 
   	SDIO->DCTRL=0x0;	//数据控制寄存器清零(关DMA)   
	if(CardType==SDIO_HIGH_CAPACITY_SD_CARD)//大容量卡
	{
		blksize=512;
		addr>>=9;
	}   
  	send_data_cfg(SD_DATATIMEOUT,0,0,0);	//清除DPSM状态机配置
	if(SDIO->RESP1&SD_CARD_LOCKED)return SD_LOCK_UNLOCK_FAILED;//卡锁了
	if((blksize>0)&&(blksize<=2048)&&((blksize&(blksize-1))==0))
	{
		power=convert_from_bytes_to_power_of_two(blksize);	    	   
		send_cmd(SD_CMD_SET_BLOCKLEN,1,blksize);	//发送CMD16+设置数据长度为blksize,短响应 	   
		errorstatus=cmd_res1_error(SD_CMD_SET_BLOCKLEN);	//等待R1响应   
		if(errorstatus!=SD_OK)return errorstatus;   	//响应错误	 
	}else return SD_INVALID_PARAMETER;	  	  									    
  	send_data_cfg(SD_DATATIMEOUT,blksize,power,1);	//blksize,卡到控制器	  
   	send_cmd(SD_CMD_READ_SINGLE_BLOCK,1,addr);		//发送CMD17+从addr地址出读取数据,短响应 	   
	errorstatus=cmd_res1_error(SD_CMD_READ_SINGLE_BLOCK);//等待R1响应   
	if(errorstatus!=SD_OK)return errorstatus;   		//响应错误	 
 	if(DeviceMode==SD_POLLING_MODE)						//查询模式,轮询数据	 
	{
 		GRCore::ASM_INTX_DISABLE();//关闭总中断(POLLING模式,严禁中断打断SDIO读写操作!!!)
		while(!(SDIO->STA&((1<<5)|(1<<1)|(1<<3)|(1<<10)|(1<<9))))//无上溢/CRC/超时/完成(标志)/起始位错误
		{
			if(SDIO->STA&(1<<15))						//接收区半满,表示至少存了8个字
			{
				for(count=0;count<8;count++)			//循环读取数据
				{
					*(tempbuff+count)=SDIO->FIFO;
				}
				tempbuff+=8;	 
				timeout=0X7FFFFF; 	//读数据溢出时间
			}else 	//处理超时
			{
				if(timeout==0)return SD_DATA_TIMEOUT;
				timeout--;
			}
		} 
		if(SDIO->STA&(1<<3))		//数据超时错误
		{										   
	 		SDIO->ICR|=1<<3; 		//清错误标志
			return SD_DATA_TIMEOUT;
	 	}else if(SDIO->STA&(1<<1))	//数据块CRC错误
		{
	 		SDIO->ICR|=1<<1; 		//清错误标志
			return SD_DATA_CRC_FAIL;		   
		}else if(SDIO->STA&(1<<5)) 	//接收fifo上溢错误
		{
	 		SDIO->ICR|=1<<5; 		//清错误标志
			return SD_RX_OVERRUN;		 
		}else if(SDIO->STA&(1<<9)) 	//接收起始位错误
		{
	 		SDIO->ICR|=1<<9; 		//清错误标志
			return SD_START_BIT_ERR;		 
		}   
		while(SDIO->STA&(1<<21))	//FIFO里面,还存在可用数据
		{
			*tempbuff=SDIO->FIFO;	//循环读取数据
			tempbuff++;
		}
		GRCore::ASM_INTX_ENABLE();//开启总中断
		SDIO->ICR=0X5FF;	 		//清除所有标记
	}else if(DeviceMode==SD_DMA_MODE)
	{
  	    sd_dma_config((u32*)buf,blksize,0); 
		TransferError=SD_OK;
		StopCondition=0;			//单块读,不需要发送停止传输指令
		TransferEnd=0;				//传输结束标置位，在中断服务置1
		SDIO->MASK|=(1<<1)|(1<<3)|(1<<8)|(1<<5)|(1<<9);	//配置需要的中断 
	 	SDIO->DCTRL|=1<<3;		 	//SDIO DMA使能 
 		while(((DMA2->ISR&0X2000)==RESET)&&(TransferEnd==0)&&(TransferError==SD_OK)&&timeout)timeout--;//等待传输完成 
		if(timeout==0)return SD_DATA_TIMEOUT;//超时
		if(TransferError!=SD_OK)errorstatus=TransferError;  
    }   
 	return errorstatus; 
}

#pragma pack(4)
u32 *tempbuff;
#pragma pack()

GRSdio::SdErr GRSdio::read_multi_block(u8 *buf,long long  addr,u16 blksize,u32 nblks)
{
	SdErr errorstatus=SD_OK;
	u8 power;
   	u32 count=0;
	u32 timeout=SDIO_DATATIMEOUT;  
	tempbuff=(u32*)buf;//转换为u32指针
	
    SDIO->DCTRL=0x0;		//数据控制寄存器清零(关DMA)   
	if(CardType==SDIO_HIGH_CAPACITY_SD_CARD)//大容量卡
	{
		blksize=512;
		addr>>=9;
	}  
   	send_data_cfg(SD_DATATIMEOUT,0,0,0);	//清除DPSM状态机配置
	if(SDIO->RESP1&SD_CARD_LOCKED)return SD_LOCK_UNLOCK_FAILED;//卡锁了
	if((blksize>0)&&(blksize<=2048)&&((blksize&(blksize-1))==0))
	{
		power=convert_from_bytes_to_power_of_two(blksize);	    
		send_cmd(SD_CMD_SET_BLOCKLEN,1,blksize);	//发送CMD16+设置数据长度为blksize,短响应 	   
		errorstatus=cmd_res1_error(SD_CMD_SET_BLOCKLEN);	//等待R1响应   
		if(errorstatus!=SD_OK)return errorstatus;   	//响应错误	 
	}else return SD_INVALID_PARAMETER;	  
	if(nblks>1)											//多块读  
	{									    
 	  	if(nblks*blksize>SD_MAX_DATA_LENGTH)return SD_INVALID_PARAMETER;//判断是否超过最大接收长度
		send_data_cfg(SD_DATATIMEOUT,nblks*blksize,power,1);//nblks*blksize,512块大小,卡到控制器	  
	  	send_cmd(SD_CMD_READ_MULT_BLOCK,1,addr);	//发送CMD18+从addr地址出读取数据,短响应 	   
		errorstatus=cmd_res1_error(SD_CMD_READ_MULT_BLOCK);//等待R1响应   
		if(errorstatus!=SD_OK)return errorstatus;   	//响应错误	  
 		if(DeviceMode==SD_POLLING_MODE)
		{
			GRCore::ASM_INTX_DISABLE();//关闭总中断(POLLING模式,严禁中断打断SDIO读写操作!!!)
			while(!(SDIO->STA&((1<<5)|(1<<1)|(1<<3)|(1<<8)|(1<<9))))//无上溢/CRC/超时/完成(标志)/起始位错误
			{
				if(SDIO->STA&(1<<15))						//接收区半满,表示至少存了8个字
				{
					for(count=0;count<8;count++)			//循环读取数据
					{
						*(tempbuff+count)=SDIO->FIFO;
					}
					tempbuff+=8;	 
					timeout=0X7FFFFF; 	//读数据溢出时间
				}else 	//处理超时
				{
					if(timeout==0)return SD_DATA_TIMEOUT;
					timeout--;
				}
			}  
			if(SDIO->STA&(1<<3))		//数据超时错误
			{										   
		 		SDIO->ICR|=1<<3; 		//清错误标志
				return SD_DATA_TIMEOUT;
		 	}else if(SDIO->STA&(1<<1))	//数据块CRC错误
			{
		 		SDIO->ICR|=1<<1; 		//清错误标志
				return SD_DATA_CRC_FAIL;		   
			}else if(SDIO->STA&(1<<5)) 	//接收fifo上溢错误
			{
		 		SDIO->ICR|=1<<5; 		//清错误标志
				return SD_RX_OVERRUN;		 
			}else if(SDIO->STA&(1<<9)) 	//接收起始位错误
			{
		 		SDIO->ICR|=1<<9; 		//清错误标志
				return SD_START_BIT_ERR;		 
			}   
			while(SDIO->STA&(1<<21))	//FIFO里面,还存在可用数据
			{
				*tempbuff=SDIO->FIFO;	//循环读取数据
				tempbuff++;
			}
	 		if(SDIO->STA&(1<<8))		//接收结束
			{
				if((SDIO_STD_CAPACITY_SD_CARD_V1_1==CardType)||(SDIO_STD_CAPACITY_SD_CARD_V2_0==CardType)||(SDIO_HIGH_CAPACITY_SD_CARD==CardType))
				{
					send_cmd(SD_CMD_STOP_TRANSMISSION,1,0);		//发送CMD12+结束传输 	   
					errorstatus=cmd_res1_error(SD_CMD_STOP_TRANSMISSION);//等待R1响应   
					if(errorstatus!=SD_OK)return errorstatus;	 
				}
 			}
			GRCore::ASM_INTX_ENABLE();//开启总中断
	 		SDIO->ICR=0X5FF;	 		//清除所有标记 
 		}else if(DeviceMode==SD_DMA_MODE)
		{
	 	    sd_dma_config((u32*)buf,nblks*blksize,0); 
	   		TransferError=SD_OK;
			StopCondition=1;			//多块读,需要发送停止传输指令 
			TransferEnd=0;				//传输结束标置位，在中断服务置1
			SDIO->MASK|=(1<<1)|(1<<3)|(1<<8)|(1<<5)|(1<<9);	//配置需要的中断 
		 	SDIO->DCTRL|=1<<3;		 						//SDIO DMA使能 
	 		while(((DMA2->ISR&0X2000)==RESET)&&timeout)timeout--;//等待传输完成 
			if(timeout==0)return SD_DATA_TIMEOUT;//超时
			while((TransferEnd==0)&&(TransferError==SD_OK)); 
			if(TransferError!=SD_OK)errorstatus=TransferError;  	 
		}		 
  	}
	return errorstatus;
}

GRSdio::SdErr GRSdio::write_block(u8 *buf,long long addr,  u16 blksize)
{
	SdErr errorstatus = SD_OK;
	u8  power=0,cardstate=0;
	u32 timeout=0,bytestransferred=0;
	u32 cardstatus=0,count=0,restwords=0;
	u32	tlen=blksize;						//总长度(字节)
	u32*tempbuff=(u32*)buf;								 
 	if(buf==NULL)return SD_INVALID_PARAMETER;//参数错误   
  	SDIO->DCTRL=0x0;							//数据控制寄存器清零(关DMA)   
  	send_data_cfg(SD_DATATIMEOUT,0,0,0);	//清除DPSM状态机配置
	if(SDIO->RESP1&SD_CARD_LOCKED)return SD_LOCK_UNLOCK_FAILED;//卡锁了
 	if(CardType==SDIO_HIGH_CAPACITY_SD_CARD)	//大容量卡
	{
		blksize=512;
		addr>>=9;
	}    
	if((blksize>0)&&(blksize<=2048)&&((blksize&(blksize-1))==0))
	{
		power=convert_from_bytes_to_power_of_two(blksize);	    
		send_cmd(SD_CMD_SET_BLOCKLEN,1,blksize);	//发送CMD16+设置数据长度为blksize,短响应 	   
		errorstatus=cmd_res1_error(SD_CMD_SET_BLOCKLEN);	//等待R1响应   
		if(errorstatus!=SD_OK)return errorstatus;   	//响应错误	 
	}else return SD_INVALID_PARAMETER;	 
   	send_cmd(SD_CMD_SEND_STATUS,1,(u32)RCA<<16);	//发送CMD13,查询卡的状态,短响应 	   
	errorstatus=cmd_res1_error(SD_CMD_SEND_STATUS);		//等待R1响应   		   
	if(errorstatus!=SD_OK)return errorstatus;
	cardstatus=SDIO->RESP1;													  
	timeout=SD_DATATIMEOUT;
   	while(((cardstatus&0x00000100)==0)&&(timeout>0)) 	//检查READY_FOR_DATA位是否置位
	{
		timeout--;
	   	send_cmd(SD_CMD_SEND_STATUS,1,(u32)RCA<<16);//发送CMD13,查询卡的状态,短响应 	   
		errorstatus=cmd_res1_error(SD_CMD_SEND_STATUS);	//等待R1响应   		   
		if(errorstatus!=SD_OK)return errorstatus;				    
		cardstatus=SDIO->RESP1;													  
	}
	if(timeout==0)return SD_ERROR;
   	send_cmd(SD_CMD_WRITE_SINGLE_BLOCK,1,addr);	//发送CMD24,写单块指令,短响应 	   
	errorstatus=cmd_res1_error(SD_CMD_WRITE_SINGLE_BLOCK);//等待R1响应   		   
	if(errorstatus!=SD_OK)return errorstatus;   	  
	StopCondition=0;									//单块写,不需要发送停止传输指令 
 	send_data_cfg(SD_DATATIMEOUT,blksize,power,0);	//blksize, 控制器到卡	  
	timeout=SDIO_DATATIMEOUT;
	if (DeviceMode == SD_POLLING_MODE)
	{
		GRCore::ASM_INTX_DISABLE();//关闭总中断(POLLING模式,严禁中断打断SDIO读写操作!!!)
		while(!(SDIO->STA&((1<<10)|(1<<4)|(1<<1)|(1<<3)|(1<<9))))//数据块发送成功/下溢/CRC/超时/起始位错误
		{
			if(SDIO->STA&(1<<14))							//发送区半空,表示至少存了8个字
			{
				if((tlen-bytestransferred)<SD_HALFFIFOBYTES)//不够32字节了
				{
					restwords=((tlen-bytestransferred)%4==0)?((tlen-bytestransferred)/4):((tlen-bytestransferred)/4+1);
					
					for(count=0;count<restwords;count++,tempbuff++,bytestransferred+=4)
					{
						SDIO->FIFO=*tempbuff;
					}
				}else
				{
					for(count=0;count<8;count++)
					{
						SDIO->FIFO=*(tempbuff+count);
					}
					tempbuff+=8;
					bytestransferred+=32;
				}
				timeout=0X3FFFFFFF;	//写数据溢出时间
			}else
			{
				if(timeout==0)return SD_DATA_TIMEOUT;
				timeout--;
			}
		} 
		if(SDIO->STA&(1<<3))		//数据超时错误
		{										   
	 		SDIO->ICR|=1<<3; 		//清错误标志
			return SD_DATA_TIMEOUT;
	 	}else if(SDIO->STA&(1<<1))	//数据块CRC错误
		{
	 		SDIO->ICR|=1<<1; 		//清错误标志
			return SD_DATA_CRC_FAIL;		   
		}else if(SDIO->STA&(1<<4)) 	//接收fifo下溢错误
		{
	 		SDIO->ICR|=1<<4; 		//清错误标志
			return SD_TX_UNDERRUN;		 
		}else if(SDIO->STA&(1<<9)) 	//接收起始位错误
		{
	 		SDIO->ICR|=1<<9; 		//清错误标志
			return SD_START_BIT_ERR;		 
		}   
		GRCore::ASM_INTX_ENABLE();//开启总中断
		SDIO->ICR=0X5FF;	 		//清除所有标记	  
	}else if(DeviceMode==SD_DMA_MODE)
	{
		sd_dma_config((u32*)buf,blksize,1);//SDIO DMA配置
   		TransferError=SD_OK;
		StopCondition=0;			//单块写,不需要发送停止传输指令 
		TransferEnd=0;				//传输结束标置位，在中断服务置1
		SDIO->MASK|=(1<<1)|(1<<3)|(1<<8)|(1<<4)|(1<<9);	//配置产生数据接收完成中断
 	 	SDIO->DCTRL|=1<<3;								//SDIO DMA使能.  
 		while(((DMA2->ISR&0X2000)==RESET)&&timeout)timeout--;//等待传输完成 
		if(timeout==0)
		{
  			init();	 					//重新初始化SD卡,可以解决写入死机的问题
			return SD_DATA_TIMEOUT;			//超时	 
 		}
		timeout=SDIO_DATATIMEOUT;
		while((TransferEnd==0)&&(TransferError==SD_OK)&&timeout)timeout--;
 		if(timeout==0)return SD_DATA_TIMEOUT;			//超时	 
  		if(TransferError!=SD_OK)return TransferError;
 	}  
 	SDIO->ICR=0X5FF;	 		//清除所有标记
 	errorstatus=is_card_programming(&cardstate);
 	while((errorstatus==SD_OK)&&((cardstate==SD_CARD_PROGRAMMING)||(cardstate==SD_CARD_RECEIVING)))
	{
		errorstatus=is_card_programming(&cardstate);
	}   
	return errorstatus;
}

GRSdio::SdErr GRSdio::write_multi_block(u8 *buf,long long addr,u16 blksize,u32 nblks)
{
	SdErr errorstatus = SD_OK;
	u8  power = 0, cardstate = 0;
	u32 timeout=0,bytestransferred=0;
	u32 count = 0, restwords = 0;
	u32 tlen=nblks*blksize;				//总长度(字节)
	u32 *tempbuff = (u32*)buf;  
  	if(buf==NULL)return SD_INVALID_PARAMETER; //参数错误  
  	SDIO->DCTRL=0x0;							//数据控制寄存器清零(关DMA)   
  	send_data_cfg(SD_DATATIMEOUT,0,0,0);	//清除DPSM状态机配置
	if(SDIO->RESP1&SD_CARD_LOCKED)return SD_LOCK_UNLOCK_FAILED;//卡锁了
 	if(CardType==SDIO_HIGH_CAPACITY_SD_CARD)//大容量卡
	{
		blksize=512;
		addr>>=9;
	}    
	if((blksize>0)&&(blksize<=2048)&&((blksize&(blksize-1))==0))
	{
		power=convert_from_bytes_to_power_of_two(blksize);	    
		send_cmd(SD_CMD_SET_BLOCKLEN,1,blksize);	//发送CMD16+设置数据长度为blksize,短响应 	   
		errorstatus=cmd_res1_error(SD_CMD_SET_BLOCKLEN);	//等待R1响应   
		if(errorstatus!=SD_OK)return errorstatus;   	//响应错误	 
	}else return SD_INVALID_PARAMETER;	 
	if(nblks>1)
	{					  
		if(nblks*blksize>SD_MAX_DATA_LENGTH)return SD_INVALID_PARAMETER;   
     	if((SDIO_STD_CAPACITY_SD_CARD_V1_1==CardType)||(SDIO_STD_CAPACITY_SD_CARD_V2_0==CardType)||(SDIO_HIGH_CAPACITY_SD_CARD==CardType))
    	{
			//提高性能
	 	   	send_cmd(SD_CMD_APP_CMD,1,(u32)RCA<<16);	//发送ACMD55,短响应 	   
			errorstatus=cmd_res1_error(SD_CMD_APP_CMD);		//等待R1响应   		   
			if(errorstatus!=SD_OK)return errorstatus;				    
	 	   	send_cmd(SD_CMD_SET_BLOCK_COUNT,1,nblks);	//发送CMD23,设置块数量,短响应 	   
			errorstatus=cmd_res1_error(SD_CMD_SET_BLOCK_COUNT);//等待R1响应   		   
			if(errorstatus!=SD_OK)return errorstatus;				    
		} 
		send_cmd(SD_CMD_WRITE_MULT_BLOCK,1,addr);		//发送CMD25,多块写指令,短响应 	   
		errorstatus=cmd_res1_error(SD_CMD_WRITE_MULT_BLOCK);	//等待R1响应   		   
		if(errorstatus!=SD_OK)return errorstatus;
 	 	send_data_cfg(SD_DATATIMEOUT,nblks*blksize,power,0);//blksize, 控制器到卡	
		if(DeviceMode==SD_POLLING_MODE)
	    {
			timeout=SDIO_DATATIMEOUT;
				GRCore::ASM_INTX_DISABLE();//关闭总中断(POLLING模式,严禁中断打断SDIO读写操作!!!)
			while(!(SDIO->STA&((1<<4)|(1<<1)|(1<<8)|(1<<3)|(1<<9))))//下溢/CRC/数据结束/超时/起始位错误
			{
				if(SDIO->STA&(1<<14))							//发送区半空,表示至少存了8字(32字节)
				{	  
					if((tlen-bytestransferred)<SD_HALFFIFOBYTES)//不够32字节了
					{
						restwords=((tlen-bytestransferred)%4==0)?((tlen-bytestransferred)/4):((tlen-bytestransferred)/4+1);
						for(count=0;count<restwords;count++,tempbuff++,bytestransferred+=4)
						{
							SDIO->FIFO=*tempbuff;
						}
					}else 										//发送区半空,可以发送至少8字(32字节)数据
					{
						for(count=0;count<SD_HALFFIFO;count++)
						{
							SDIO->FIFO=*(tempbuff+count);
						}
						tempbuff+=SD_HALFFIFO;
						bytestransferred+=SD_HALFFIFOBYTES;
					}
					timeout=0X3FFFFFFF;	//写数据溢出时间
				}else
				{
					if(timeout==0)return SD_DATA_TIMEOUT; 
					timeout--;
				}
			} 
			if(SDIO->STA&(1<<3))		//数据超时错误
			{										   
		 		SDIO->ICR|=1<<3; 		//清错误标志
				return SD_DATA_TIMEOUT;
		 	}else if(SDIO->STA&(1<<1))	//数据块CRC错误
			{
		 		SDIO->ICR|=1<<1; 		//清错误标志
				return SD_DATA_CRC_FAIL;		   
			}else if(SDIO->STA&(1<<4)) 	//接收fifo下溢错误
			{
		 		SDIO->ICR|=1<<4; 		//清错误标志
				return SD_TX_UNDERRUN;		 
			}else if(SDIO->STA&(1<<9)) 	//接收起始位错误
			{
		 		SDIO->ICR|=1<<9; 		//清错误标志
				return SD_START_BIT_ERR;		 
			}   										   
			if(SDIO->STA&(1<<8))		//发送结束
			{															 
				if((SDIO_STD_CAPACITY_SD_CARD_V1_1==CardType)||(SDIO_STD_CAPACITY_SD_CARD_V2_0==CardType)||(SDIO_HIGH_CAPACITY_SD_CARD==CardType))
				{
					send_cmd(SD_CMD_STOP_TRANSMISSION,1,0);		//发送CMD12+结束传输 	   
					errorstatus=cmd_res1_error(SD_CMD_STOP_TRANSMISSION);//等待R1响应   
					if(errorstatus!=SD_OK)return errorstatus;	 
				}
			}
			GRCore::ASM_INTX_ENABLE();//开启总中断
	 		SDIO->ICR=0X5FF;	 		//清除所有标记 
	    }else if(DeviceMode==SD_DMA_MODE)
		{
			sd_dma_config((u32*)buf,nblks*blksize,1);//SDIO DMA配置
	   		TransferError=SD_OK;
			StopCondition=1;			//多块写,需要发送停止传输指令 
			TransferEnd=0;				//传输结束标置位，在中断服务置1
			SDIO->MASK|=(1<<1)|(1<<3)|(1<<8)|(1<<4)|(1<<9);	//配置产生数据接收完成中断
	 	 	SDIO->DCTRL|=1<<3;								//SDIO DMA使能. 
			timeout=SDIO_DATATIMEOUT;
	 		while(((DMA2->ISR&0X2000)==RESET)&&timeout)timeout--;//等待传输完成 
			if(timeout==0)	 								//超时
			{									  
  				init();	 					//重新初始化SD卡,可以解决写入死机的问题
	 			return SD_DATA_TIMEOUT;			//超时	 
	 		}
			timeout=SDIO_DATATIMEOUT;
			while((TransferEnd==0)&&(TransferError==SD_OK)&&timeout)timeout--;
	 		if(timeout==0)return SD_DATA_TIMEOUT;			//超时	 
	 		if(TransferError!=SD_OK)return TransferError;	 
		}
  	}
 	SDIO->ICR=0X5FF;	 		//清除所有标记
 	errorstatus=is_card_programming(&cardstate);
 	while((errorstatus==SD_OK)&&((cardstate==SD_CARD_PROGRAMMING)||(cardstate==SD_CARD_RECEIVING)))
	{
		errorstatus=is_card_programming(&cardstate);
	}   
	return errorstatus;	  
}

void GRSdio::CIRQ()
{
	if(SDIO->STA&(1<<8))//接收完成中断
	{	 
		if (StopCondition==1)
		{
			send_cmd(SD_CMD_STOP_TRANSMISSION,1,0);		//发送CMD12,结束传输 	   
			TransferError=cmd_res1_error(SD_CMD_STOP_TRANSMISSION);
		}else TransferError = SD_OK;	
 		SDIO->ICR|=1<<8;//清除完成中断标记
		SDIO->MASK&=~((1<<1)|(1<<3)|(1<<8)|(1<<14)|(1<<15)|(1<<4)|(1<<5)|(1<<9));//关闭相关中断
 		TransferEnd = 1;
		//return(TransferError);
	}
 	if(SDIO->STA&(1<<1))//数据CRC错误
	{
		SDIO->ICR|=1<<1;//清除中断标记
		SDIO->MASK&=~((1<<1)|(1<<3)|(1<<8)|(1<<14)|(1<<15)|(1<<4)|(1<<5)|(1<<9));//关闭相关中断
	    TransferError = SD_DATA_CRC_FAIL;
	    //return(SD_DATA_CRC_FAIL);
	}
 	if(SDIO->STA&(1<<3))//数据超时错误
	{
		SDIO->ICR|=1<<3;//清除中断标记
		SDIO->MASK&=~((1<<1)|(1<<3)|(1<<8)|(1<<14)|(1<<15)|(1<<4)|(1<<5)|(1<<9));//关闭相关中断
	    TransferError = SD_DATA_TIMEOUT;
	    //return(SD_DATA_TIMEOUT);
	}
  	if(SDIO->STA&(1<<5))//FIFO上溢错误
	{
		SDIO->ICR|=1<<5;//清除中断标记
		SDIO->MASK&=~((1<<1)|(1<<3)|(1<<8)|(1<<14)|(1<<15)|(1<<4)|(1<<5)|(1<<9));//关闭相关中断
	    TransferError = SD_RX_OVERRUN;
	    //return(SD_RX_OVERRUN);
	}
   	if(SDIO->STA&(1<<4))//FIFO下溢错误
	{
		SDIO->ICR|=1<<4;//清除中断标记
		SDIO->MASK&=~((1<<1)|(1<<3)|(1<<8)|(1<<14)|(1<<15)|(1<<4)|(1<<5)|(1<<9));//关闭相关中断
	    TransferError = SD_TX_UNDERRUN;
	    //return(SD_TX_UNDERRUN);
	}
	if(SDIO->STA&(1<<9))//起始位错误
	{
		SDIO->ICR|=1<<9;//清除中断标记
		SDIO->MASK&=~((1<<1)|(1<<3)|(1<<8)|(1<<14)|(1<<15)|(1<<4)|(1<<5)|(1<<9));//关闭相关中断
	    TransferError = SD_START_BIT_ERR;
	    //return(SD_START_BIT_ERR);
	}
	//return(SD_OK);
}

GRSdio::SdErr GRSdio::cmd_error()
{
	SdErr errorstatus = SD_OK;
	u32 timeout=SDIO_CMD0TIMEOUT;	   
	while(timeout--)
	{
		if(SDIO->STA&(1<<7))break;	//命令已发送(无需响应)	 
	}	    
	if(timeout==0)return SD_CMD_RSP_TIMEOUT;  
	SDIO->ICR=0X5FF;				//清除标记
	return errorstatus;
}

GRSdio::SdErr GRSdio::cmd_res1_error(u8 cmd)
{
  u32 status; 
	while(1)
	{
		status=SDIO->STA;
		if(status&((1<<0)|(1<<2)|(1<<6)))break;//CRC错误/命令响应超时/已经收到响应(CRC校验成功)
	} 
	if(status&(1<<2))					//响应超时
	{																				    
 		SDIO->ICR=1<<2;					//清除命令响应超时标志
		return SD_CMD_RSP_TIMEOUT;
	}	
 	if(status&(1<<0))					//CRC错误
	{																				    
 		SDIO->ICR=1<<0;					//清除标志
		return SD_CMD_CRC_FAIL;
	}		
	if(SDIO->RESPCMD!=cmd)return SD_ILLEGAL_CMD;//命令不匹配 
  	SDIO->ICR=0X5FF;	 				//清除标记
	return (SdErr)(SDIO->RESP1&SD_OCR_ERRORBITS);//返回卡响应
}

GRSdio::SdErr GRSdio::cmd_res2_error()
{
	SdErr errorstatus=SD_OK;
	u32 status;
	u32 timeout=SDIO_CMD0TIMEOUT;
 	while(timeout--)
	{
		status=SDIO->STA;
		if(status&((1<<0)|(1<<2)|(1<<6)))break;//CRC错误/命令响应超时/已经收到响应(CRC校验成功)	
	}
  	if((timeout==0)||(status&(1<<2)))	//响应超时
	{																				    
		errorstatus=SD_CMD_RSP_TIMEOUT; 
		SDIO->ICR|=1<<2;				//清除命令响应超时标志
		return errorstatus;
	}	 
	if(status&1<<0)						//CRC错误
	{								   
		errorstatus=SD_CMD_CRC_FAIL;
		SDIO->ICR|=1<<0;				//清除响应标志
 	}
	SDIO->ICR=0X5FF;	 				//清除标记
 	return errorstatus;		
}

GRSdio::SdErr GRSdio::cmd_res3_error()
{
	u32 status;						 
 	while(1)
	{
		status=SDIO->STA;
		if(status&((1<<0)|(1<<2)|(1<<6)))break;//CRC错误/命令响应超时/已经收到响应(CRC校验成功)	
	}
 	if(status&(1<<2))					//响应超时
	{											 
		SDIO->ICR|=1<<2;				//清除命令响应超时标志
		return SD_CMD_RSP_TIMEOUT;
	}	 
   	SDIO->ICR=0X5FF;	 				//清除标记
 	return SD_OK;		
}

GRSdio::SdErr GRSdio::cmd_res6_error(u8 cmd,u16*prca)
{
	SdErr errorstatus=SD_OK;
	u32 status;					    
	u32 rspr1;
 	while(1)
	{
		status=SDIO->STA;
		if(status&((1<<0)|(1<<2)|(1<<6)))break;//CRC错误/命令响应超时/已经收到响应(CRC校验成功)	
	}
	if(status&(1<<2))					//响应超时
	{																				    
 		SDIO->ICR|=1<<2;				//清除命令响应超时标志
		return SD_CMD_RSP_TIMEOUT;
	}	 	 
	if(status&1<<0)						//CRC错误
	{								   
		SDIO->ICR|=1<<0;				//清除响应标志
 		return SD_CMD_CRC_FAIL;
	}
	if(SDIO->RESPCMD!=cmd)				//判断是否响应cmd命令
	{
 		return SD_ILLEGAL_CMD; 		
	}	    
	SDIO->ICR=0X5FF;	 				//清除所有标记
	rspr1=SDIO->RESP1;					//得到响应 	 
	if(SD_ALLZERO==(rspr1&(SD_R6_GENERAL_UNKNOWN_ERROR|SD_R6_ILLEGAL_CMD|SD_R6_COM_CRC_FAILED)))
	{
		*prca=(u16)(rspr1>>16);			//右移16位得到,rca
		return errorstatus;
	}
   	if(rspr1&SD_R6_GENERAL_UNKNOWN_ERROR)return SD_GENERAL_UNKNOWN_ERROR;
   	if(rspr1&SD_R6_ILLEGAL_CMD)return SD_ILLEGAL_CMD;
   	if(rspr1&SD_R6_COM_CRC_FAILED)return SD_COM_CRC_FAILED;
	return errorstatus;
}

GRSdio::SdErr GRSdio::cmd_res7_error()
{
	SdErr errorstatus=SD_OK;
	u32 status;
	u32 timeout=SDIO_CMD0TIMEOUT;
 	while(timeout--)
	{
		status=SDIO->STA;
		if(status&((1<<0)|(1<<2)|(1<<6)))break;//CRC错误/命令响应超时/已经收到响应(CRC校验成功)	
	}
 	if((timeout==0)||(status&(1<<2)))	//响应超时
	{																				    
		errorstatus=SD_CMD_RSP_TIMEOUT;	//当前卡不是2.0兼容卡,或者不支持设定的电压范围
		SDIO->ICR|=1<<2;				//清除命令响应超时标志
		return errorstatus;
	}	 
	if(status&1<<6)						//成功接收到响应
	{								   
		errorstatus=SD_OK;
		SDIO->ICR|=1<<6;				//清除响应标志
 	}
	return errorstatus;
}

GRSdio::SdErr GRSdio::en_wide_bus(u8 enx)
{
	SdErr errorstatus = SD_OK;
 	u32 scr[2]={0,0};
	u8 arg=0X00;
	if(enx)arg=0X02;
	else arg=0X00;
 	if(SDIO->RESP1&SD_CARD_LOCKED)return SD_LOCK_UNLOCK_FAILED;//SD卡处于LOCKED状态		    
 	errorstatus=find_scr(RCA,scr);						//得到SCR寄存器数据
 	if(errorstatus!=SD_OK)return errorstatus;
	if((scr[1]&SD_WIDE_BUS_SUPPORT)!=SD_ALLZERO)		//支持宽总线
	{
	 	send_cmd(SD_CMD_APP_CMD,1,(u32)RCA<<16);	//发送CMD55+RCA,短响应											  
	 	errorstatus=cmd_res1_error(SD_CMD_APP_CMD);
	 	if(errorstatus!=SD_OK)return errorstatus; 
	 	send_cmd(SD_CMD_APP_SD_SET_BUSWIDTH,1,arg);//发送ACMD6,短响应,参数:10,4位;00,1位.											  
		errorstatus=cmd_res1_error(SD_CMD_APP_SD_SET_BUSWIDTH);
		return errorstatus;
	}else return SD_REQUEST_NOT_APPLICABLE;				//不支持宽总线设置 	 
}

GRSdio::SdErr GRSdio::is_card_programming(u8 *pstatus)
{
 	vu32 respR1 = 0, status = 0; 
  	send_cmd(SD_CMD_SEND_STATUS,1,(u32)RCA<<16);		//发送CMD13 	   
  	status=SDIO->STA;
	while(!(status&((1<<0)|(1<<6)|(1<<2))))status=SDIO->STA;//等待操作完成
   	if(status&(1<<0))			//CRC检测失败
	{
		SDIO->ICR|=1<<0;		//清除错误标记
		return SD_CMD_CRC_FAIL;
	}
   	if(status&(1<<2))			//命令超时 
	{
		SDIO->ICR|=1<<2;		//清除错误标记
		return SD_CMD_RSP_TIMEOUT;
	}
 	if(SDIO->RESPCMD!=SD_CMD_SEND_STATUS)return SD_ILLEGAL_CMD;
	SDIO->ICR=0X5FF;	 		//清除所有标记
	respR1=SDIO->RESP1;
	*pstatus=(u8)((respR1>>9)&0x0000000F);
	return SD_OK;
}

GRSdio::SdErr GRSdio::find_scr(u16 rca,u32 *pscr)
{
	u32 index = 0; 
	SdErr errorstatus = SD_OK;
	u32 tempscr[2]={0,0};  
 	send_cmd(SD_CMD_SET_BLOCKLEN,1,8);			//发送CMD16,短响应,设置Block Size为8字节											  
 	errorstatus=cmd_res1_error(SD_CMD_SET_BLOCKLEN);
 	if(errorstatus!=SD_OK)return errorstatus;	    
  	send_cmd(SD_CMD_APP_CMD,1,(u32)rca<<16);	//发送CMD55,短响应 									  
 	errorstatus=cmd_res1_error(SD_CMD_APP_CMD);
 	if(errorstatus!=SD_OK)return errorstatus;
	send_data_cfg(SD_DATATIMEOUT,8,3,1);		//8个字节长度,block为8字节,SD卡到SDIO.
   	send_cmd(SD_CMD_SD_APP_SEND_SCR,1,0);		//发送ACMD51,短响应,参数为0											  
 	errorstatus=cmd_res1_error(SD_CMD_SD_APP_SEND_SCR);
 	if(errorstatus!=SD_OK)return errorstatus;							   
 	while(!(SDIO->STA&(SDIO_FLAG_RXOVERR|SDIO_FLAG_DCRCFAIL|SDIO_FLAG_DTIMEOUT|SDIO_FLAG_DBCKEND|SDIO_FLAG_STBITERR)))
	{ 
		if(SDIO->STA&(1<<21))//接收FIFO数据可用
		{
			*(tempscr+index)=SDIO->FIFO;	//读取FIFO内容
			index++;
			if(index>=2)break;
		}
	}
 	if(SDIO->STA&(1<<3))		//接收数据超时
	{										 
 		SDIO->ICR|=1<<3;		//清除标记
		return SD_DATA_TIMEOUT;
	}
	else if(SDIO->STA&(1<<1))	//已发送/接收的数据块CRC校验错误
	{
 		SDIO->ICR|=1<<1;		//清除标记
		return SD_DATA_CRC_FAIL;   
	}
	else if(SDIO->STA&(1<<5))	//接收FIFO溢出
	{
 		SDIO->ICR|=1<<5;		//清除标记
		return SD_RX_OVERRUN;   	   
	}
	else if(SDIO->STA&(1<<9))	//起始位检测错误
	{
 		SDIO->ICR|=1<<9;		//清除标记
		return SD_START_BIT_ERR;    
	}
   	SDIO->ICR=0X5FF;	 		//清除标记	 
	//把数据顺序按8位为单位倒过来.   	
	*(pscr+1)=((tempscr[0]&SD_0TO7BITS)<<24)|((tempscr[0]&SD_8TO15BITS)<<8)|((tempscr[0]&SD_16TO23BITS)>>8)|((tempscr[0]&SD_24TO31BITS)>>24);
	*(pscr)=((tempscr[1]&SD_0TO7BITS)<<24)|((tempscr[1]&SD_8TO15BITS)<<8)|((tempscr[1]&SD_16TO23BITS)>>8)|((tempscr[1]&SD_24TO31BITS)>>24);
 	return errorstatus;
}

u8 GRSdio::convert_from_bytes_to_power_of_two(u16 NumberOfBytes)
{
	u8 count=0;
	while(NumberOfBytes!=1)
	{
		NumberOfBytes>>=1;
		count++;
	}
	return count;
}

void GRSdio::sd_dma_config(u32*mbuf,u32 bufsize,u8 dir)
{
 	DMA2->IFCR|=(0XF<<12);				//清除DMA2通道4的各种标记
 	DMA2_Channel4->CCR&=~(1<<0);		//关闭DMA 通道4
  DMA2_Channel4->CCR&=~(0X7FF<<4);	//清除之前的设置,DIR,CIRC,PINC,MINC,PSIZE,MSIZE,PL,MEM2MEM
 	DMA2_Channel4->CCR|=dir<<4;  		//从存储器读   
	DMA2_Channel4->CCR|=0<<5;  			//普通模式
	DMA2_Channel4->CCR|=0<<6; 			//外设地址非增量模式
	DMA2_Channel4->CCR|=1<<7;  			//存储器增量模式
	DMA2_Channel4->CCR|=2<<8;  			//外设数据宽度为32位
	DMA2_Channel4->CCR|=2<<10; 			//存储器数据宽度32位
	DMA2_Channel4->CCR|=2<<12; 			//高优先级	  
  DMA2_Channel4->CNDTR=bufsize/4;   	//DMA2,传输数据量	  
 	DMA2_Channel4->CPAR=(u32)&SDIO->FIFO;//DMA2 外设地址 
	DMA2_Channel4->CMAR=(u32)mbuf; 		//DMA2,存储器地址
 	DMA2_Channel4->CCR|=1<<0; 			//开启DMA通道
}

//SDIO中断服务函数		  
void SDIO_IRQHandler(void) 
{											
	if(GRCore::CIRQPtr(GRCore::Sdio))
		((GRSdio*)GRCore::CIRQPtr(GRCore::Sdio))->CIRQ();
	return;
}	 		

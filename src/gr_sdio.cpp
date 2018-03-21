#include "gr_sdio.h"
#include "string.h"	 

//typedef unsigned int uint32_t;

//SDIO��ر�־λ
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
//SDIO ָ�
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
  *        SDIO_APP_CMD ��CMD55 should be sent before sending these commands. 
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
  			   
//֧�ֵ�SD������
#define SDIO_STD_CAPACITY_SD_CARD_V1_1             ((u32)0x00000000)
#define SDIO_STD_CAPACITY_SD_CARD_V2_0             ((u32)0x00000001)
#define SDIO_HIGH_CAPACITY_SD_CARD                 ((u32)0x00000002)
#define SDIO_MULTIMEDIA_CARD                       ((u32)0x00000003)
#define SDIO_SECURE_DIGITAL_IO_CARD                ((u32)0x00000004)
#define SDIO_HIGH_SPEED_MULTIMEDIA_CARD            ((u32)0x00000005)
#define SDIO_SECURE_DIGITAL_IO_COMBO_CARD          ((u32)0x00000006)
#define SDIO_HIGH_CAPACITY_MMC_CARD                ((u32)0x00000007)

//SDIO��ز�������
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
																	 
//CMD8ָ��
#define SDIO_SEND_IF_COND               ((u32)0x00000008)


//�û�������			  
//SDIOʱ�Ӽ��㹫ʽ:SDIO_CKʱ��=SDIOCLK/[clkdiv+2];����,SDIOCLKһ��Ϊ72Mhz
//ʹ��DMAģʽ��ʱ��,�������ʿ��Ե�24Mhz,���������Ŀ����Ǹ��ٿ�,����Ҳ�����
//������뽵��ʱ��,ʹ�ò�ѯģʽ�Ļ�,�Ƽ�SDIO_TRANSFER_CLK_DIV����Ϊ3���߸���
#define SDIO_INIT_CLK_DIV        0xB2 		//SDIO��ʼ��Ƶ�ʣ����400Kh  
#define SDIO_TRANSFER_CLK_DIV    0x04		//SDIO����Ƶ��,��ֵ̫С���ܻᵼ�¶�д�ļ����� 
//SDIO����ģʽ����,ͨ��SD_SetDeviceMode��������.
#define SD_POLLING_MODE    	0  	//��ѯģʽ,��ģʽ��,�����д������,��������SDIO_TRANSFER_CLK_DIV������.
#define SD_DMA_MODE    		1	//DMAģʽ,��ģʽ��,�����д������,��������SDIO_TRANSFER_CLK_DIV������.   

static u8 CardType=SDIO_STD_CAPACITY_SD_CARD_V1_1;		//SD�����ͣ�Ĭ��Ϊ1.x����
static u32 CSD_Tab[4],CID_Tab[4],RCA=0;					//SD��CSD,CID�Լ���Ե�ַ(RCA)����
static u8 DeviceMode=SD_DMA_MODE;		   				//����ģʽ,ע��,����ģʽ����ͨ��SD_SetDeviceMode,�������.����ֻ�Ƕ���һ��Ĭ�ϵ�ģʽ(SD_DMA_MODE)
static u8 StopCondition=0; 								//�Ƿ���ֹͣ�����־λ,DMA����д��ʱ���õ�  
volatile GRSdio::SdErr TransferError=GRSdio::SD_OK;					//���ݴ�������־,DMA��дʱʹ��	    
volatile u8 TransferEnd=0;								//���������־,DMA��дʱʹ��

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
	//SDIO IO�ڳ�ʼ��

  RCC->AHBENR|=1<<10;    	//ʹ��SDIOʱ��	   	 
 	RCC->AHBENR|=1<<1;    	//ʹ��DMA2ʱ��

	RCC->APB2ENR|=1<<4;    	//ʹ��PORTCʱ��	   	 
	RCC->APB2ENR|=1<<5;    	//ʹ��PORTDʱ��
	GPIOC->CRH&=0XFFF00000; 
	GPIOC->CRH|=0X000BBBBB;	//PC.8~12 �������

	GPIOD->CRL&=0XFFFFF0FF; 
	GPIOD->CRL|=0X00000B00;	//PD2�������,PD7 ��������
 	//SDIO����Ĵ�������ΪĬ��ֵ 			   
	SDIO->POWER=0x00000000;
	SDIO->CLKCR=0x00000000;
	SDIO->ARG=0x00000000;
	SDIO->CMD=0x00000000;
	SDIO->DTIMER=0x00000000;
	SDIO->DLEN=0x00000000;
	SDIO->DCTRL=0x00000000;
	SDIO->ICR=0x00C007FF;
	SDIO->MASK=0x00000000;	  
 	GRCore::NvicInit(0,0,SDIO_IRQn,2);		//SDIO�ж�����
   	errorstatus=power_on();			//SD���ϵ�
 	if(errorstatus==SD_OK)errorstatus=init_card();			//��ʼ��SD��														  
  	if(errorstatus==SD_OK)errorstatus=get_card_info(&cardInfo);	//��ȡ����Ϣ
 	if(errorstatus==SD_OK)errorstatus=select_deselect((u32)(cardInfo.RCA<<16));//ѡ��SD��   
   	if(errorstatus==SD_OK)errorstatus=enable_wide_bus_operation(1);	//4λ���,�����MMC��,������4λģʽ 
  	if((errorstatus==SD_OK)||(SDIO_MULTIMEDIA_CARD==CardType))
	{  		    
		if(cardInfo.CardType==SDIO_STD_CAPACITY_SD_CARD_V1_1||cardInfo.CardType==SDIO_STD_CAPACITY_SD_CARD_V2_0)
		{
			clkdiv=SDIO_TRANSFER_CLK_DIV+6;	//V1.1/V2.0�����������72/12=6Mhz
		}else clkdiv=SDIO_TRANSFER_CLK_DIV;	//SDHC�����������������72/6=12Mhz
		clk_set(clkdiv);				//����ʱ��Ƶ��,SDIOʱ�Ӽ��㹫ʽ:SDIO_CKʱ��=SDIOCLK/[clkdiv+2];����,SDIOCLK�̶�Ϊ48Mhz 
		//errorstatus=SD_SetDeviceMode(SD_DMA_MODE);	//����ΪDMAģʽ
		errorstatus=set_device_mode(SD_POLLING_MODE);	//����Ϊ��ѯģʽ
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
		 	sta=read_block(SDIO_DATA_BUFFER,lsector+512*n,512);//����sector�Ķ�����
			memcpy(buf,SDIO_DATA_BUFFER,512);
			buf+=512;
		} 
	}else
	{
		if(cnt==1)sta=read_block(buf,lsector,512);    	//����sector�Ķ�����
		else sta=read_multi_block(buf,lsector,512,cnt);//���sector  
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
		 	sta=write_block(SDIO_DATA_BUFFER,lsector+512*n,512);//����sector��д����
			buf+=512;
		} 
	}else
	{
		if(cnt==1)sta=write_block(buf,lsector,512);    	//����sector��д����
		else sta=write_multi_block(buf,lsector,512,cnt);	//���sector
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
	tmpreg&=0XFFFFF800;		//���index��waitrsp
	tmpreg|=cmdindex&0X3F;	//�����µ�index			 
	tmpreg|=waitrsp<<6;		//�����µ�wait rsp 
	tmpreg|=0<<8;			//�޵ȴ�
  tmpreg|=1<<10;			//����ͨ��״̬��ʹ��
	SDIO->CMD=tmpreg;
}

void GRSdio::send_data_cfg(u32 datatimeout,u32 datalen,u8 blksize,u8 dir)
{
	u32 tmpreg;
	SDIO->DTIMER=datatimeout;
  SDIO->DLEN=datalen&0X1FFFFFF;	//��25λ��Ч
	tmpreg=SDIO->DCTRL; 
	tmpreg&=0xFFFFFF08;		//���֮ǰ������.
	tmpreg|=blksize<<4;		//���ÿ��С
	tmpreg|=0<<2;			//�����ݴ���
	tmpreg|=(dir&0X01)<<1;	//�������
	tmpreg|=1<<0;			//���ݴ���ʹ��,DPSM״̬��
	SDIO->DCTRL=tmpreg;		
}

GRSdio::SdErr GRSdio::power_on()
{
 	u8 i=0;
	SdErr errorstatus=SD_OK;
	u32 response=0,count=0,validvoltage=0;
	u32 SDType=SD_STD_CAPACITY;
	//����CLKCR�Ĵ��� 
	SDIO->CLKCR=0;				//���CLKCR֮ǰ������
	SDIO->CLKCR|=0<<9;			//��ʡ��ģʽ
	SDIO->CLKCR|=0<<10;			//�ر���·,CK���ݷ�Ƶ�������
	SDIO->CLKCR|=0<<11;			//1λ���ݿ��
	SDIO->CLKCR|=0<<13;			//SDIOCLK�����ز���SDIOCK
	SDIO->CLKCR|=0<<14;			//�ر�Ӳ��������    
	clk_set(SDIO_INIT_CLK_DIV);//����ʱ��Ƶ��(��ʼ����ʱ��,���ܳ���400Khz)			 
 	SDIO->POWER=0X03;			//�ϵ�״̬,������ʱ��    
  	SDIO->CLKCR|=1<<8;			//SDIOCKʹ��   
   	for(i=0;i<74;i++)
	{
		send_cmd(SD_CMD_GO_IDLE_STATE,0,0);//����CMD0����IDLE STAGEģʽ����.												  
		errorstatus=cmd_error();
		if(errorstatus==SD_OK)break;
 	}
 	if(errorstatus)return errorstatus;//���ش���״̬
	send_cmd(SDIO_SEND_IF_COND,1,SD_CHECK_PATTERN);//����CMD8,����Ӧ,���SD���ӿ�����.
 														//arg[11:8]:01,֧�ֵ�ѹ��Χ,2.7~3.6V
														//arg[7:0]:Ĭ��0XAA
														//������Ӧ7
  	errorstatus=cmd_res7_error();						//�ȴ�R7��Ӧ
 	if(errorstatus==SD_OK) 								//R7��Ӧ����
	{
		CardType=SDIO_STD_CAPACITY_SD_CARD_V2_0;		//SD 2.0��
		SDType=SD_HIGH_CAPACITY;			   			//��������
	}
	send_cmd(SD_CMD_APP_CMD,1,0);					//����CMD55,����Ӧ	 
	errorstatus=cmd_res1_error(SD_CMD_APP_CMD); 		 	//�ȴ�R1��Ӧ   
	if(errorstatus==SD_OK)//SD2.0/SD 1.1,����ΪMMC��
	{																  
		//SD��,����ACMD41 SD_APP_OP_COND,����Ϊ:0x80100000 
		while((!validvoltage)&&(count<SD_MAX_VOLT_TRIAL))
		{	   										   
			send_cmd(SD_CMD_APP_CMD,1,0);				//����CMD55,����Ӧ	 
			errorstatus=cmd_res1_error(SD_CMD_APP_CMD); 	 	//�ȴ�R1��Ӧ   
 			if(errorstatus!=SD_OK)return errorstatus;   	//��Ӧ����
			send_cmd(SD_CMD_SD_APP_OP_COND,1,SD_VOLTAGE_WINDOW_SD|SDType);//����ACMD41,����Ӧ	 
			errorstatus=cmd_res3_error(); 					//�ȴ�R3��Ӧ   
 			if(errorstatus!=SD_OK)return errorstatus;   	//��Ӧ����  
			response=SDIO->RESP1;;			   				//�õ���Ӧ
			validvoltage=(((response>>31)==1)?1:0);			//�ж�SD���ϵ��Ƿ����
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
 	}else//MMC��
	{
		//MMC��,����CMD1 SDIO_SEND_OP_COND,����Ϊ:0x80FF8000 
		while((!validvoltage)&&(count<SD_MAX_VOLT_TRIAL))
		{	   										   				   
			send_cmd(SD_CMD_SEND_OP_COND,1,SD_VOLTAGE_WINDOW_MMC);//����CMD1,����Ӧ	 
			errorstatus=cmd_res3_error(); 					//�ȴ�R3��Ӧ   
 			if(errorstatus!=SD_OK)return errorstatus;   	//��Ӧ����  
			response=SDIO->RESP1;;			   				//�õ���Ӧ
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
  SDIO->POWER&=~(3<<0);//SDIO��Դ�ر�,ʱ��ֹͣ	
	return SD_OK;		 
}

GRSdio::SdErr GRSdio::init_card()
{
 	SdErr errorstatus=SD_OK;
	u16 rca = 0x01;
 	if((SDIO->POWER&0X03)==0)return SD_REQUEST_NOT_APPLICABLE;//����Դ״̬,ȷ��Ϊ�ϵ�״̬
 	if(SDIO_SECURE_DIGITAL_IO_CARD!=CardType)			//��SECURE_DIGITAL_IO_CARD
	{
		send_cmd(SD_CMD_ALL_SEND_CID,3,0);			//����CMD2,ȡ��CID,����Ӧ	 
		errorstatus=cmd_res2_error(); 					//�ȴ�R2��Ӧ   
		if(errorstatus!=SD_OK)return errorstatus;   	//��Ӧ����		    
 		CID_Tab[0]=SDIO->RESP1;
		CID_Tab[1]=SDIO->RESP2;
		CID_Tab[2]=SDIO->RESP3;
		CID_Tab[3]=SDIO->RESP4;
	}
	if((SDIO_STD_CAPACITY_SD_CARD_V1_1==CardType)||(SDIO_STD_CAPACITY_SD_CARD_V2_0==CardType)||(SDIO_SECURE_DIGITAL_IO_COMBO_CARD==CardType)||(SDIO_HIGH_CAPACITY_SD_CARD==CardType))//�жϿ�����
	{
		send_cmd(SD_CMD_SET_REL_ADDR,1,0);			//����CMD3,����Ӧ 
		errorstatus=cmd_res6_error(SD_CMD_SET_REL_ADDR,&rca);//�ȴ�R6��Ӧ 
		if(errorstatus!=SD_OK)return errorstatus;   	//��Ӧ����		    
	}   
    if (SDIO_MULTIMEDIA_CARD==CardType)
    {
 		send_cmd(SD_CMD_SET_REL_ADDR,1,(u32)(rca<<16));//����CMD3,����Ӧ 	   
		errorstatus=cmd_res2_error(); 					//�ȴ�R2��Ӧ   
		if(errorstatus!=SD_OK)return errorstatus;   	//��Ӧ����	 
    }
	if (SDIO_SECURE_DIGITAL_IO_CARD!=CardType)			//��SECURE_DIGITAL_IO_CARD
	{
		RCA = rca;
		send_cmd(SD_CMD_SEND_CSD,3,(u32)(rca<<16));//����CMD9+��RCA,ȡ��CSD,����Ӧ 	   
		errorstatus=cmd_res2_error(); 					//�ȴ�R2��Ӧ   
		if(errorstatus!=SD_OK)return errorstatus;   	//��Ӧ����		    
  		CSD_Tab[0]=SDIO->RESP1;
		CSD_Tab[1]=SDIO->RESP2;
		CSD_Tab[2]=SDIO->RESP3;						
		CSD_Tab[3]=SDIO->RESP4;					    
	}
	return SD_OK;//����ʼ���ɹ�
}

GRSdio::SdErr GRSdio::get_card_info(GRSdio::SdInfo* cardinfo)
{
 	SdErr errorstatus=SD_OK;
	u8 tmp=0;	   
	cardinfo->CardType=(u8)CardType; 				//������
	cardinfo->RCA=(u16)RCA;							//��RCAֵ
	tmp=(u8)((CSD_Tab[0]&0xFF000000)>>24);
	cardinfo->SD_csd.CSDStruct=(tmp&0xC0)>>6;		//CSD�ṹ
	cardinfo->SD_csd.SysSpecVersion=(tmp&0x3C)>>2;	//2.0Э�黹û�����ⲿ��(Ϊ����),Ӧ���Ǻ���Э�鶨���
	cardinfo->SD_csd.Reserved1=tmp&0x03;			//2������λ  
	tmp=(u8)((CSD_Tab[0]&0x00FF0000)>>16);			//��1���ֽ�
	cardinfo->SD_csd.TAAC=tmp;				   		//���ݶ�ʱ��1
	tmp=(u8)((CSD_Tab[0]&0x0000FF00)>>8);	  		//��2���ֽ�
	cardinfo->SD_csd.NSAC=tmp;		  				//���ݶ�ʱ��2
	tmp=(u8)(CSD_Tab[0]&0x000000FF);				//��3���ֽ�
	cardinfo->SD_csd.MaxBusClkFrec=tmp;		  		//�����ٶ�	   
	tmp=(u8)((CSD_Tab[1]&0xFF000000)>>24);			//��4���ֽ�
	cardinfo->SD_csd.CardComdClasses=tmp<<4;    	//��ָ�������λ
	tmp=(u8)((CSD_Tab[1]&0x00FF0000)>>16);	 		//��5���ֽ�
	cardinfo->SD_csd.CardComdClasses|=(tmp&0xF0)>>4;//��ָ�������λ
	cardinfo->SD_csd.RdBlockLen=tmp&0x0F;	    	//����ȡ���ݳ���
	tmp=(u8)((CSD_Tab[1]&0x0000FF00)>>8);			//��6���ֽ�
	cardinfo->SD_csd.PartBlockRead=(tmp&0x80)>>7;	//����ֿ��
	cardinfo->SD_csd.WrBlockMisalign=(tmp&0x40)>>6;	//д���λ
	cardinfo->SD_csd.RdBlockMisalign=(tmp&0x20)>>5;	//�����λ
	cardinfo->SD_csd.DSRImpl=(tmp&0x10)>>4;
	cardinfo->SD_csd.Reserved2=0; 					//����
 	if((CardType==SDIO_STD_CAPACITY_SD_CARD_V1_1)||(CardType==SDIO_STD_CAPACITY_SD_CARD_V2_0)||(SDIO_MULTIMEDIA_CARD==CardType))//��׼1.1/2.0��/MMC��
	{
		cardinfo->SD_csd.DeviceSize=(tmp&0x03)<<10;	//C_SIZE(12λ)
	 	tmp=(u8)(CSD_Tab[1]&0x000000FF); 			//��7���ֽ�	
		cardinfo->SD_csd.DeviceSize|=(tmp)<<2;
 		tmp=(u8)((CSD_Tab[2]&0xFF000000)>>24);		//��8���ֽ�	
		cardinfo->SD_csd.DeviceSize|=(tmp&0xC0)>>6;
 		cardinfo->SD_csd.MaxRdCurrentVDDMin=(tmp&0x38)>>3;
		cardinfo->SD_csd.MaxRdCurrentVDDMax=(tmp&0x07);
 		tmp=(u8)((CSD_Tab[2]&0x00FF0000)>>16);		//��9���ֽ�	
		cardinfo->SD_csd.MaxWrCurrentVDDMin=(tmp&0xE0)>>5;
		cardinfo->SD_csd.MaxWrCurrentVDDMax=(tmp&0x1C)>>2;
		cardinfo->SD_csd.DeviceSizeMul=(tmp&0x03)<<1;//C_SIZE_MULT
 		tmp=(u8)((CSD_Tab[2]&0x0000FF00)>>8);	  	//��10���ֽ�	
		cardinfo->SD_csd.DeviceSizeMul|=(tmp&0x80)>>7;
 		cardinfo->CardCapacity=(cardinfo->SD_csd.DeviceSize+1);//���㿨����
		cardinfo->CardCapacity*=(1<<(cardinfo->SD_csd.DeviceSizeMul+2));
		cardinfo->CardBlockSize=1<<(cardinfo->SD_csd.RdBlockLen);//���С
		cardinfo->CardCapacity*=cardinfo->CardBlockSize;
	}else if(CardType==SDIO_HIGH_CAPACITY_SD_CARD)	//��������
	{
 		tmp=(u8)(CSD_Tab[1]&0x000000FF); 		//��7���ֽ�	
		cardinfo->SD_csd.DeviceSize=(tmp&0x3F)<<16;//C_SIZE
 		tmp=(u8)((CSD_Tab[2]&0xFF000000)>>24); 	//��8���ֽ�	
 		cardinfo->SD_csd.DeviceSize|=(tmp<<8);
 		tmp=(u8)((CSD_Tab[2]&0x00FF0000)>>16);	//��9���ֽ�	
 		cardinfo->SD_csd.DeviceSize|=(tmp);
 		tmp=(u8)((CSD_Tab[2]&0x0000FF00)>>8); 	//��10���ֽ�	
 		cardinfo->CardCapacity=(long long)(cardinfo->SD_csd.DeviceSize+1)*512*1024;//���㿨����
		cardinfo->CardBlockSize=512; 			//���С�̶�Ϊ512�ֽ�
	}	  
	cardinfo->SD_csd.EraseGrSize=(tmp&0x40)>>6;
	cardinfo->SD_csd.EraseGrMul=(tmp&0x3F)<<1;	   
	tmp=(u8)(CSD_Tab[2]&0x000000FF);			//��11���ֽ�	
	cardinfo->SD_csd.EraseGrMul|=(tmp&0x80)>>7;
	cardinfo->SD_csd.WrProtectGrSize=(tmp&0x7F);
 	tmp=(u8)((CSD_Tab[3]&0xFF000000)>>24);		//��12���ֽ�	
	cardinfo->SD_csd.WrProtectGrEnable=(tmp&0x80)>>7;
	cardinfo->SD_csd.ManDeflECC=(tmp&0x60)>>5;
	cardinfo->SD_csd.WrSpeedFact=(tmp&0x1C)>>2;
	cardinfo->SD_csd.MaxWrBlockLen=(tmp&0x03)<<2;	 
	tmp=(u8)((CSD_Tab[3]&0x00FF0000)>>16);		//��13���ֽ�
	cardinfo->SD_csd.MaxWrBlockLen|=(tmp&0xC0)>>6;
	cardinfo->SD_csd.WriteBlockPaPartial=(tmp&0x20)>>5;
	cardinfo->SD_csd.Reserved3=0;
	cardinfo->SD_csd.ContentProtectAppli=(tmp&0x01);  
	tmp=(u8)((CSD_Tab[3]&0x0000FF00)>>8);		//��14���ֽ�
	cardinfo->SD_csd.FileFormatGrouop=(tmp&0x80)>>7;
	cardinfo->SD_csd.CopyFlag=(tmp&0x40)>>6;
	cardinfo->SD_csd.PermWrProtect=(tmp&0x20)>>5;
	cardinfo->SD_csd.TempWrProtect=(tmp&0x10)>>4;
	cardinfo->SD_csd.FileFormat=(tmp&0x0C)>>2;
	cardinfo->SD_csd.ECC=(tmp&0x03);  
	tmp=(u8)(CSD_Tab[3]&0x000000FF);			//��15���ֽ�
	cardinfo->SD_csd.CSD_CRC=(tmp&0xFE)>>1;
	cardinfo->SD_csd.Reserved4=1;		 
	tmp=(u8)((CID_Tab[0]&0xFF000000)>>24);		//��0���ֽ�
	cardinfo->SD_cid.ManufacturerID=tmp;		    
	tmp=(u8)((CID_Tab[0]&0x00FF0000)>>16);		//��1���ֽ�
	cardinfo->SD_cid.OEM_AppliID=tmp<<8;	  
	tmp=(u8)((CID_Tab[0]&0x000000FF00)>>8);		//��2���ֽ�
	cardinfo->SD_cid.OEM_AppliID|=tmp;	    
	tmp=(u8)(CID_Tab[0]&0x000000FF);			//��3���ֽ�	
	cardinfo->SD_cid.ProdName1=tmp<<24;				  
	tmp=(u8)((CID_Tab[1]&0xFF000000)>>24); 		//��4���ֽ�
	cardinfo->SD_cid.ProdName1|=tmp<<16;	  
	tmp=(u8)((CID_Tab[1]&0x00FF0000)>>16);	   	//��5���ֽ�
	cardinfo->SD_cid.ProdName1|=tmp<<8;		 
	tmp=(u8)((CID_Tab[1]&0x0000FF00)>>8);		//��6���ֽ�
	cardinfo->SD_cid.ProdName1|=tmp;		   
	tmp=(u8)(CID_Tab[1]&0x000000FF);	  		//��7���ֽ�
	cardinfo->SD_cid.ProdName2=tmp;			  
	tmp=(u8)((CID_Tab[2]&0xFF000000)>>24); 		//��8���ֽ�
	cardinfo->SD_cid.ProdRev=tmp;		 
	tmp=(u8)((CID_Tab[2]&0x00FF0000)>>16);		//��9���ֽ�
	cardinfo->SD_cid.ProdSN=tmp<<24;	   
	tmp=(u8)((CID_Tab[2]&0x0000FF00)>>8); 		//��10���ֽ�
	cardinfo->SD_cid.ProdSN|=tmp<<16;	   
	tmp=(u8)(CID_Tab[2]&0x000000FF);   			//��11���ֽ�
	cardinfo->SD_cid.ProdSN|=tmp<<8;		   
	tmp=(u8)((CID_Tab[3]&0xFF000000)>>24); 		//��12���ֽ�
	cardinfo->SD_cid.ProdSN|=tmp;			     
	tmp=(u8)((CID_Tab[3]&0x00FF0000)>>16);	 	//��13���ֽ�
	cardinfo->SD_cid.Reserved1|=(tmp&0xF0)>>4;
	cardinfo->SD_cid.ManufactDate=(tmp&0x0F)<<8;    
	tmp=(u8)((CID_Tab[3]&0x0000FF00)>>8);		//��14���ֽ�
	cardinfo->SD_cid.ManufactDate|=tmp;		 	  
	tmp=(u8)(CID_Tab[3]&0x000000FF);			//��15���ֽ�
	cardinfo->SD_cid.CID_CRC=(tmp&0xFE)>>1;
	cardinfo->SD_cid.Reserved2=1;	 
	return errorstatus;
}

GRSdio::SdErr GRSdio::enable_wide_bus_operation(u32 wmode)
{
  SdErr errorstatus=SD_OK;
 	if(SDIO_MULTIMEDIA_CARD==CardType)return SD_UNSUPPORTED_FEATURE;//MMC����֧��
 	else if((SDIO_STD_CAPACITY_SD_CARD_V1_1==CardType)||(SDIO_STD_CAPACITY_SD_CARD_V2_0==CardType)||(SDIO_HIGH_CAPACITY_SD_CARD==CardType))
	{
		if(wmode>=2)return SD_UNSUPPORTED_FEATURE;//��֧��8λģʽ
 		else   
		{
			errorstatus=en_wide_bus(wmode);
 			if(SD_OK==errorstatus)
			{
				SDIO->CLKCR&=~(3<<11);		//���֮ǰ��λ������    
				SDIO->CLKCR|=(u16)wmode<<11;//1λ/4λ���߿�� 
				SDIO->CLKCR|=0<<14;			//������Ӳ��������
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
 	send_cmd(SD_CMD_SEL_DESEL_CARD,1,addr);	//����CMD7,ѡ��,����Ӧ	 	   
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
 	send_cmd(SD_CMD_SEND_STATUS,1,RCA<<16);	//����CMD13,����Ӧ		 
	errorstatus=cmd_res1_error(SD_CMD_SEND_STATUS);	//��ѯ��Ӧ״̬ 
	if(errorstatus!=SD_OK)return errorstatus;
	*pcardstatus=SDIO->RESP1;//��ȡ��Ӧֵ
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
  u32 count=0,*tempbuff=(u32*)buf;//ת��Ϊu32ָ�� 
	u32 timeout=SDIO_DATATIMEOUT;   
   	if(NULL==buf)return SD_INVALID_PARAMETER; 
   	SDIO->DCTRL=0x0;	//���ݿ��ƼĴ�������(��DMA)   
	if(CardType==SDIO_HIGH_CAPACITY_SD_CARD)//��������
	{
		blksize=512;
		addr>>=9;
	}   
  	send_data_cfg(SD_DATATIMEOUT,0,0,0);	//���DPSM״̬������
	if(SDIO->RESP1&SD_CARD_LOCKED)return SD_LOCK_UNLOCK_FAILED;//������
	if((blksize>0)&&(blksize<=2048)&&((blksize&(blksize-1))==0))
	{
		power=convert_from_bytes_to_power_of_two(blksize);	    	   
		send_cmd(SD_CMD_SET_BLOCKLEN,1,blksize);	//����CMD16+�������ݳ���Ϊblksize,����Ӧ 	   
		errorstatus=cmd_res1_error(SD_CMD_SET_BLOCKLEN);	//�ȴ�R1��Ӧ   
		if(errorstatus!=SD_OK)return errorstatus;   	//��Ӧ����	 
	}else return SD_INVALID_PARAMETER;	  	  									    
  	send_data_cfg(SD_DATATIMEOUT,blksize,power,1);	//blksize,����������	  
   	send_cmd(SD_CMD_READ_SINGLE_BLOCK,1,addr);		//����CMD17+��addr��ַ����ȡ����,����Ӧ 	   
	errorstatus=cmd_res1_error(SD_CMD_READ_SINGLE_BLOCK);//�ȴ�R1��Ӧ   
	if(errorstatus!=SD_OK)return errorstatus;   		//��Ӧ����	 
 	if(DeviceMode==SD_POLLING_MODE)						//��ѯģʽ,��ѯ����	 
	{
 		GRCore::ASM_INTX_DISABLE();//�ر����ж�(POLLINGģʽ,�Ͻ��жϴ��SDIO��д����!!!)
		while(!(SDIO->STA&((1<<5)|(1<<1)|(1<<3)|(1<<10)|(1<<9))))//������/CRC/��ʱ/���(��־)/��ʼλ����
		{
			if(SDIO->STA&(1<<15))						//����������,��ʾ���ٴ���8����
			{
				for(count=0;count<8;count++)			//ѭ����ȡ����
				{
					*(tempbuff+count)=SDIO->FIFO;
				}
				tempbuff+=8;	 
				timeout=0X7FFFFF; 	//���������ʱ��
			}else 	//����ʱ
			{
				if(timeout==0)return SD_DATA_TIMEOUT;
				timeout--;
			}
		} 
		if(SDIO->STA&(1<<3))		//���ݳ�ʱ����
		{										   
	 		SDIO->ICR|=1<<3; 		//������־
			return SD_DATA_TIMEOUT;
	 	}else if(SDIO->STA&(1<<1))	//���ݿ�CRC����
		{
	 		SDIO->ICR|=1<<1; 		//������־
			return SD_DATA_CRC_FAIL;		   
		}else if(SDIO->STA&(1<<5)) 	//����fifo�������
		{
	 		SDIO->ICR|=1<<5; 		//������־
			return SD_RX_OVERRUN;		 
		}else if(SDIO->STA&(1<<9)) 	//������ʼλ����
		{
	 		SDIO->ICR|=1<<9; 		//������־
			return SD_START_BIT_ERR;		 
		}   
		while(SDIO->STA&(1<<21))	//FIFO����,�����ڿ�������
		{
			*tempbuff=SDIO->FIFO;	//ѭ����ȡ����
			tempbuff++;
		}
		GRCore::ASM_INTX_ENABLE();//�������ж�
		SDIO->ICR=0X5FF;	 		//������б��
	}else if(DeviceMode==SD_DMA_MODE)
	{
  	    sd_dma_config((u32*)buf,blksize,0); 
		TransferError=SD_OK;
		StopCondition=0;			//�����,����Ҫ����ֹͣ����ָ��
		TransferEnd=0;				//�����������λ�����жϷ�����1
		SDIO->MASK|=(1<<1)|(1<<3)|(1<<8)|(1<<5)|(1<<9);	//������Ҫ���ж� 
	 	SDIO->DCTRL|=1<<3;		 	//SDIO DMAʹ�� 
 		while(((DMA2->ISR&0X2000)==RESET)&&(TransferEnd==0)&&(TransferError==SD_OK)&&timeout)timeout--;//�ȴ�������� 
		if(timeout==0)return SD_DATA_TIMEOUT;//��ʱ
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
	tempbuff=(u32*)buf;//ת��Ϊu32ָ��
	
    SDIO->DCTRL=0x0;		//���ݿ��ƼĴ�������(��DMA)   
	if(CardType==SDIO_HIGH_CAPACITY_SD_CARD)//��������
	{
		blksize=512;
		addr>>=9;
	}  
   	send_data_cfg(SD_DATATIMEOUT,0,0,0);	//���DPSM״̬������
	if(SDIO->RESP1&SD_CARD_LOCKED)return SD_LOCK_UNLOCK_FAILED;//������
	if((blksize>0)&&(blksize<=2048)&&((blksize&(blksize-1))==0))
	{
		power=convert_from_bytes_to_power_of_two(blksize);	    
		send_cmd(SD_CMD_SET_BLOCKLEN,1,blksize);	//����CMD16+�������ݳ���Ϊblksize,����Ӧ 	   
		errorstatus=cmd_res1_error(SD_CMD_SET_BLOCKLEN);	//�ȴ�R1��Ӧ   
		if(errorstatus!=SD_OK)return errorstatus;   	//��Ӧ����	 
	}else return SD_INVALID_PARAMETER;	  
	if(nblks>1)											//����  
	{									    
 	  	if(nblks*blksize>SD_MAX_DATA_LENGTH)return SD_INVALID_PARAMETER;//�ж��Ƿ񳬹������ճ���
		send_data_cfg(SD_DATATIMEOUT,nblks*blksize,power,1);//nblks*blksize,512���С,����������	  
	  	send_cmd(SD_CMD_READ_MULT_BLOCK,1,addr);	//����CMD18+��addr��ַ����ȡ����,����Ӧ 	   
		errorstatus=cmd_res1_error(SD_CMD_READ_MULT_BLOCK);//�ȴ�R1��Ӧ   
		if(errorstatus!=SD_OK)return errorstatus;   	//��Ӧ����	  
 		if(DeviceMode==SD_POLLING_MODE)
		{
			GRCore::ASM_INTX_DISABLE();//�ر����ж�(POLLINGģʽ,�Ͻ��жϴ��SDIO��д����!!!)
			while(!(SDIO->STA&((1<<5)|(1<<1)|(1<<3)|(1<<8)|(1<<9))))//������/CRC/��ʱ/���(��־)/��ʼλ����
			{
				if(SDIO->STA&(1<<15))						//����������,��ʾ���ٴ���8����
				{
					for(count=0;count<8;count++)			//ѭ����ȡ����
					{
						*(tempbuff+count)=SDIO->FIFO;
					}
					tempbuff+=8;	 
					timeout=0X7FFFFF; 	//���������ʱ��
				}else 	//����ʱ
				{
					if(timeout==0)return SD_DATA_TIMEOUT;
					timeout--;
				}
			}  
			if(SDIO->STA&(1<<3))		//���ݳ�ʱ����
			{										   
		 		SDIO->ICR|=1<<3; 		//������־
				return SD_DATA_TIMEOUT;
		 	}else if(SDIO->STA&(1<<1))	//���ݿ�CRC����
			{
		 		SDIO->ICR|=1<<1; 		//������־
				return SD_DATA_CRC_FAIL;		   
			}else if(SDIO->STA&(1<<5)) 	//����fifo�������
			{
		 		SDIO->ICR|=1<<5; 		//������־
				return SD_RX_OVERRUN;		 
			}else if(SDIO->STA&(1<<9)) 	//������ʼλ����
			{
		 		SDIO->ICR|=1<<9; 		//������־
				return SD_START_BIT_ERR;		 
			}   
			while(SDIO->STA&(1<<21))	//FIFO����,�����ڿ�������
			{
				*tempbuff=SDIO->FIFO;	//ѭ����ȡ����
				tempbuff++;
			}
	 		if(SDIO->STA&(1<<8))		//���ս���
			{
				if((SDIO_STD_CAPACITY_SD_CARD_V1_1==CardType)||(SDIO_STD_CAPACITY_SD_CARD_V2_0==CardType)||(SDIO_HIGH_CAPACITY_SD_CARD==CardType))
				{
					send_cmd(SD_CMD_STOP_TRANSMISSION,1,0);		//����CMD12+�������� 	   
					errorstatus=cmd_res1_error(SD_CMD_STOP_TRANSMISSION);//�ȴ�R1��Ӧ   
					if(errorstatus!=SD_OK)return errorstatus;	 
				}
 			}
			GRCore::ASM_INTX_ENABLE();//�������ж�
	 		SDIO->ICR=0X5FF;	 		//������б�� 
 		}else if(DeviceMode==SD_DMA_MODE)
		{
	 	    sd_dma_config((u32*)buf,nblks*blksize,0); 
	   		TransferError=SD_OK;
			StopCondition=1;			//����,��Ҫ����ֹͣ����ָ�� 
			TransferEnd=0;				//�����������λ�����жϷ�����1
			SDIO->MASK|=(1<<1)|(1<<3)|(1<<8)|(1<<5)|(1<<9);	//������Ҫ���ж� 
		 	SDIO->DCTRL|=1<<3;		 						//SDIO DMAʹ�� 
	 		while(((DMA2->ISR&0X2000)==RESET)&&timeout)timeout--;//�ȴ�������� 
			if(timeout==0)return SD_DATA_TIMEOUT;//��ʱ
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
	u32	tlen=blksize;						//�ܳ���(�ֽ�)
	u32*tempbuff=(u32*)buf;								 
 	if(buf==NULL)return SD_INVALID_PARAMETER;//��������   
  	SDIO->DCTRL=0x0;							//���ݿ��ƼĴ�������(��DMA)   
  	send_data_cfg(SD_DATATIMEOUT,0,0,0);	//���DPSM״̬������
	if(SDIO->RESP1&SD_CARD_LOCKED)return SD_LOCK_UNLOCK_FAILED;//������
 	if(CardType==SDIO_HIGH_CAPACITY_SD_CARD)	//��������
	{
		blksize=512;
		addr>>=9;
	}    
	if((blksize>0)&&(blksize<=2048)&&((blksize&(blksize-1))==0))
	{
		power=convert_from_bytes_to_power_of_two(blksize);	    
		send_cmd(SD_CMD_SET_BLOCKLEN,1,blksize);	//����CMD16+�������ݳ���Ϊblksize,����Ӧ 	   
		errorstatus=cmd_res1_error(SD_CMD_SET_BLOCKLEN);	//�ȴ�R1��Ӧ   
		if(errorstatus!=SD_OK)return errorstatus;   	//��Ӧ����	 
	}else return SD_INVALID_PARAMETER;	 
   	send_cmd(SD_CMD_SEND_STATUS,1,(u32)RCA<<16);	//����CMD13,��ѯ����״̬,����Ӧ 	   
	errorstatus=cmd_res1_error(SD_CMD_SEND_STATUS);		//�ȴ�R1��Ӧ   		   
	if(errorstatus!=SD_OK)return errorstatus;
	cardstatus=SDIO->RESP1;													  
	timeout=SD_DATATIMEOUT;
   	while(((cardstatus&0x00000100)==0)&&(timeout>0)) 	//���READY_FOR_DATAλ�Ƿ���λ
	{
		timeout--;
	   	send_cmd(SD_CMD_SEND_STATUS,1,(u32)RCA<<16);//����CMD13,��ѯ����״̬,����Ӧ 	   
		errorstatus=cmd_res1_error(SD_CMD_SEND_STATUS);	//�ȴ�R1��Ӧ   		   
		if(errorstatus!=SD_OK)return errorstatus;				    
		cardstatus=SDIO->RESP1;													  
	}
	if(timeout==0)return SD_ERROR;
   	send_cmd(SD_CMD_WRITE_SINGLE_BLOCK,1,addr);	//����CMD24,д����ָ��,����Ӧ 	   
	errorstatus=cmd_res1_error(SD_CMD_WRITE_SINGLE_BLOCK);//�ȴ�R1��Ӧ   		   
	if(errorstatus!=SD_OK)return errorstatus;   	  
	StopCondition=0;									//����д,����Ҫ����ֹͣ����ָ�� 
 	send_data_cfg(SD_DATATIMEOUT,blksize,power,0);	//blksize, ����������	  
	timeout=SDIO_DATATIMEOUT;
	if (DeviceMode == SD_POLLING_MODE)
	{
		GRCore::ASM_INTX_DISABLE();//�ر����ж�(POLLINGģʽ,�Ͻ��жϴ��SDIO��д����!!!)
		while(!(SDIO->STA&((1<<10)|(1<<4)|(1<<1)|(1<<3)|(1<<9))))//���ݿ鷢�ͳɹ�/����/CRC/��ʱ/��ʼλ����
		{
			if(SDIO->STA&(1<<14))							//���������,��ʾ���ٴ���8����
			{
				if((tlen-bytestransferred)<SD_HALFFIFOBYTES)//����32�ֽ���
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
				timeout=0X3FFFFFFF;	//д�������ʱ��
			}else
			{
				if(timeout==0)return SD_DATA_TIMEOUT;
				timeout--;
			}
		} 
		if(SDIO->STA&(1<<3))		//���ݳ�ʱ����
		{										   
	 		SDIO->ICR|=1<<3; 		//������־
			return SD_DATA_TIMEOUT;
	 	}else if(SDIO->STA&(1<<1))	//���ݿ�CRC����
		{
	 		SDIO->ICR|=1<<1; 		//������־
			return SD_DATA_CRC_FAIL;		   
		}else if(SDIO->STA&(1<<4)) 	//����fifo�������
		{
	 		SDIO->ICR|=1<<4; 		//������־
			return SD_TX_UNDERRUN;		 
		}else if(SDIO->STA&(1<<9)) 	//������ʼλ����
		{
	 		SDIO->ICR|=1<<9; 		//������־
			return SD_START_BIT_ERR;		 
		}   
		GRCore::ASM_INTX_ENABLE();//�������ж�
		SDIO->ICR=0X5FF;	 		//������б��	  
	}else if(DeviceMode==SD_DMA_MODE)
	{
		sd_dma_config((u32*)buf,blksize,1);//SDIO DMA����
   		TransferError=SD_OK;
		StopCondition=0;			//����д,����Ҫ����ֹͣ����ָ�� 
		TransferEnd=0;				//�����������λ�����жϷ�����1
		SDIO->MASK|=(1<<1)|(1<<3)|(1<<8)|(1<<4)|(1<<9);	//���ò������ݽ�������ж�
 	 	SDIO->DCTRL|=1<<3;								//SDIO DMAʹ��.  
 		while(((DMA2->ISR&0X2000)==RESET)&&timeout)timeout--;//�ȴ�������� 
		if(timeout==0)
		{
  			init();	 					//���³�ʼ��SD��,���Խ��д������������
			return SD_DATA_TIMEOUT;			//��ʱ	 
 		}
		timeout=SDIO_DATATIMEOUT;
		while((TransferEnd==0)&&(TransferError==SD_OK)&&timeout)timeout--;
 		if(timeout==0)return SD_DATA_TIMEOUT;			//��ʱ	 
  		if(TransferError!=SD_OK)return TransferError;
 	}  
 	SDIO->ICR=0X5FF;	 		//������б��
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
	u32 tlen=nblks*blksize;				//�ܳ���(�ֽ�)
	u32 *tempbuff = (u32*)buf;  
  	if(buf==NULL)return SD_INVALID_PARAMETER; //��������  
  	SDIO->DCTRL=0x0;							//���ݿ��ƼĴ�������(��DMA)   
  	send_data_cfg(SD_DATATIMEOUT,0,0,0);	//���DPSM״̬������
	if(SDIO->RESP1&SD_CARD_LOCKED)return SD_LOCK_UNLOCK_FAILED;//������
 	if(CardType==SDIO_HIGH_CAPACITY_SD_CARD)//��������
	{
		blksize=512;
		addr>>=9;
	}    
	if((blksize>0)&&(blksize<=2048)&&((blksize&(blksize-1))==0))
	{
		power=convert_from_bytes_to_power_of_two(blksize);	    
		send_cmd(SD_CMD_SET_BLOCKLEN,1,blksize);	//����CMD16+�������ݳ���Ϊblksize,����Ӧ 	   
		errorstatus=cmd_res1_error(SD_CMD_SET_BLOCKLEN);	//�ȴ�R1��Ӧ   
		if(errorstatus!=SD_OK)return errorstatus;   	//��Ӧ����	 
	}else return SD_INVALID_PARAMETER;	 
	if(nblks>1)
	{					  
		if(nblks*blksize>SD_MAX_DATA_LENGTH)return SD_INVALID_PARAMETER;   
     	if((SDIO_STD_CAPACITY_SD_CARD_V1_1==CardType)||(SDIO_STD_CAPACITY_SD_CARD_V2_0==CardType)||(SDIO_HIGH_CAPACITY_SD_CARD==CardType))
    	{
			//�������
	 	   	send_cmd(SD_CMD_APP_CMD,1,(u32)RCA<<16);	//����ACMD55,����Ӧ 	   
			errorstatus=cmd_res1_error(SD_CMD_APP_CMD);		//�ȴ�R1��Ӧ   		   
			if(errorstatus!=SD_OK)return errorstatus;				    
	 	   	send_cmd(SD_CMD_SET_BLOCK_COUNT,1,nblks);	//����CMD23,���ÿ�����,����Ӧ 	   
			errorstatus=cmd_res1_error(SD_CMD_SET_BLOCK_COUNT);//�ȴ�R1��Ӧ   		   
			if(errorstatus!=SD_OK)return errorstatus;				    
		} 
		send_cmd(SD_CMD_WRITE_MULT_BLOCK,1,addr);		//����CMD25,���дָ��,����Ӧ 	   
		errorstatus=cmd_res1_error(SD_CMD_WRITE_MULT_BLOCK);	//�ȴ�R1��Ӧ   		   
		if(errorstatus!=SD_OK)return errorstatus;
 	 	send_data_cfg(SD_DATATIMEOUT,nblks*blksize,power,0);//blksize, ����������	
		if(DeviceMode==SD_POLLING_MODE)
	    {
			timeout=SDIO_DATATIMEOUT;
				GRCore::ASM_INTX_DISABLE();//�ر����ж�(POLLINGģʽ,�Ͻ��жϴ��SDIO��д����!!!)
			while(!(SDIO->STA&((1<<4)|(1<<1)|(1<<8)|(1<<3)|(1<<9))))//����/CRC/���ݽ���/��ʱ/��ʼλ����
			{
				if(SDIO->STA&(1<<14))							//���������,��ʾ���ٴ���8��(32�ֽ�)
				{	  
					if((tlen-bytestransferred)<SD_HALFFIFOBYTES)//����32�ֽ���
					{
						restwords=((tlen-bytestransferred)%4==0)?((tlen-bytestransferred)/4):((tlen-bytestransferred)/4+1);
						for(count=0;count<restwords;count++,tempbuff++,bytestransferred+=4)
						{
							SDIO->FIFO=*tempbuff;
						}
					}else 										//���������,���Է�������8��(32�ֽ�)����
					{
						for(count=0;count<SD_HALFFIFO;count++)
						{
							SDIO->FIFO=*(tempbuff+count);
						}
						tempbuff+=SD_HALFFIFO;
						bytestransferred+=SD_HALFFIFOBYTES;
					}
					timeout=0X3FFFFFFF;	//д�������ʱ��
				}else
				{
					if(timeout==0)return SD_DATA_TIMEOUT; 
					timeout--;
				}
			} 
			if(SDIO->STA&(1<<3))		//���ݳ�ʱ����
			{										   
		 		SDIO->ICR|=1<<3; 		//������־
				return SD_DATA_TIMEOUT;
		 	}else if(SDIO->STA&(1<<1))	//���ݿ�CRC����
			{
		 		SDIO->ICR|=1<<1; 		//������־
				return SD_DATA_CRC_FAIL;		   
			}else if(SDIO->STA&(1<<4)) 	//����fifo�������
			{
		 		SDIO->ICR|=1<<4; 		//������־
				return SD_TX_UNDERRUN;		 
			}else if(SDIO->STA&(1<<9)) 	//������ʼλ����
			{
		 		SDIO->ICR|=1<<9; 		//������־
				return SD_START_BIT_ERR;		 
			}   										   
			if(SDIO->STA&(1<<8))		//���ͽ���
			{															 
				if((SDIO_STD_CAPACITY_SD_CARD_V1_1==CardType)||(SDIO_STD_CAPACITY_SD_CARD_V2_0==CardType)||(SDIO_HIGH_CAPACITY_SD_CARD==CardType))
				{
					send_cmd(SD_CMD_STOP_TRANSMISSION,1,0);		//����CMD12+�������� 	   
					errorstatus=cmd_res1_error(SD_CMD_STOP_TRANSMISSION);//�ȴ�R1��Ӧ   
					if(errorstatus!=SD_OK)return errorstatus;	 
				}
			}
			GRCore::ASM_INTX_ENABLE();//�������ж�
	 		SDIO->ICR=0X5FF;	 		//������б�� 
	    }else if(DeviceMode==SD_DMA_MODE)
		{
			sd_dma_config((u32*)buf,nblks*blksize,1);//SDIO DMA����
	   		TransferError=SD_OK;
			StopCondition=1;			//���д,��Ҫ����ֹͣ����ָ�� 
			TransferEnd=0;				//�����������λ�����жϷ�����1
			SDIO->MASK|=(1<<1)|(1<<3)|(1<<8)|(1<<4)|(1<<9);	//���ò������ݽ�������ж�
	 	 	SDIO->DCTRL|=1<<3;								//SDIO DMAʹ��. 
			timeout=SDIO_DATATIMEOUT;
	 		while(((DMA2->ISR&0X2000)==RESET)&&timeout)timeout--;//�ȴ�������� 
			if(timeout==0)	 								//��ʱ
			{									  
  				init();	 					//���³�ʼ��SD��,���Խ��д������������
	 			return SD_DATA_TIMEOUT;			//��ʱ	 
	 		}
			timeout=SDIO_DATATIMEOUT;
			while((TransferEnd==0)&&(TransferError==SD_OK)&&timeout)timeout--;
	 		if(timeout==0)return SD_DATA_TIMEOUT;			//��ʱ	 
	 		if(TransferError!=SD_OK)return TransferError;	 
		}
  	}
 	SDIO->ICR=0X5FF;	 		//������б��
 	errorstatus=is_card_programming(&cardstate);
 	while((errorstatus==SD_OK)&&((cardstate==SD_CARD_PROGRAMMING)||(cardstate==SD_CARD_RECEIVING)))
	{
		errorstatus=is_card_programming(&cardstate);
	}   
	return errorstatus;	  
}

void GRSdio::CIRQ()
{
	if(SDIO->STA&(1<<8))//��������ж�
	{	 
		if (StopCondition==1)
		{
			send_cmd(SD_CMD_STOP_TRANSMISSION,1,0);		//����CMD12,�������� 	   
			TransferError=cmd_res1_error(SD_CMD_STOP_TRANSMISSION);
		}else TransferError = SD_OK;	
 		SDIO->ICR|=1<<8;//�������жϱ��
		SDIO->MASK&=~((1<<1)|(1<<3)|(1<<8)|(1<<14)|(1<<15)|(1<<4)|(1<<5)|(1<<9));//�ر�����ж�
 		TransferEnd = 1;
		//return(TransferError);
	}
 	if(SDIO->STA&(1<<1))//����CRC����
	{
		SDIO->ICR|=1<<1;//����жϱ��
		SDIO->MASK&=~((1<<1)|(1<<3)|(1<<8)|(1<<14)|(1<<15)|(1<<4)|(1<<5)|(1<<9));//�ر�����ж�
	    TransferError = SD_DATA_CRC_FAIL;
	    //return(SD_DATA_CRC_FAIL);
	}
 	if(SDIO->STA&(1<<3))//���ݳ�ʱ����
	{
		SDIO->ICR|=1<<3;//����жϱ��
		SDIO->MASK&=~((1<<1)|(1<<3)|(1<<8)|(1<<14)|(1<<15)|(1<<4)|(1<<5)|(1<<9));//�ر�����ж�
	    TransferError = SD_DATA_TIMEOUT;
	    //return(SD_DATA_TIMEOUT);
	}
  	if(SDIO->STA&(1<<5))//FIFO�������
	{
		SDIO->ICR|=1<<5;//����жϱ��
		SDIO->MASK&=~((1<<1)|(1<<3)|(1<<8)|(1<<14)|(1<<15)|(1<<4)|(1<<5)|(1<<9));//�ر�����ж�
	    TransferError = SD_RX_OVERRUN;
	    //return(SD_RX_OVERRUN);
	}
   	if(SDIO->STA&(1<<4))//FIFO�������
	{
		SDIO->ICR|=1<<4;//����жϱ��
		SDIO->MASK&=~((1<<1)|(1<<3)|(1<<8)|(1<<14)|(1<<15)|(1<<4)|(1<<5)|(1<<9));//�ر�����ж�
	    TransferError = SD_TX_UNDERRUN;
	    //return(SD_TX_UNDERRUN);
	}
	if(SDIO->STA&(1<<9))//��ʼλ����
	{
		SDIO->ICR|=1<<9;//����жϱ��
		SDIO->MASK&=~((1<<1)|(1<<3)|(1<<8)|(1<<14)|(1<<15)|(1<<4)|(1<<5)|(1<<9));//�ر�����ж�
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
		if(SDIO->STA&(1<<7))break;	//�����ѷ���(������Ӧ)	 
	}	    
	if(timeout==0)return SD_CMD_RSP_TIMEOUT;  
	SDIO->ICR=0X5FF;				//������
	return errorstatus;
}

GRSdio::SdErr GRSdio::cmd_res1_error(u8 cmd)
{
  u32 status; 
	while(1)
	{
		status=SDIO->STA;
		if(status&((1<<0)|(1<<2)|(1<<6)))break;//CRC����/������Ӧ��ʱ/�Ѿ��յ���Ӧ(CRCУ��ɹ�)
	} 
	if(status&(1<<2))					//��Ӧ��ʱ
	{																				    
 		SDIO->ICR=1<<2;					//���������Ӧ��ʱ��־
		return SD_CMD_RSP_TIMEOUT;
	}	
 	if(status&(1<<0))					//CRC����
	{																				    
 		SDIO->ICR=1<<0;					//�����־
		return SD_CMD_CRC_FAIL;
	}		
	if(SDIO->RESPCMD!=cmd)return SD_ILLEGAL_CMD;//���ƥ�� 
  	SDIO->ICR=0X5FF;	 				//������
	return (SdErr)(SDIO->RESP1&SD_OCR_ERRORBITS);//���ؿ���Ӧ
}

GRSdio::SdErr GRSdio::cmd_res2_error()
{
	SdErr errorstatus=SD_OK;
	u32 status;
	u32 timeout=SDIO_CMD0TIMEOUT;
 	while(timeout--)
	{
		status=SDIO->STA;
		if(status&((1<<0)|(1<<2)|(1<<6)))break;//CRC����/������Ӧ��ʱ/�Ѿ��յ���Ӧ(CRCУ��ɹ�)	
	}
  	if((timeout==0)||(status&(1<<2)))	//��Ӧ��ʱ
	{																				    
		errorstatus=SD_CMD_RSP_TIMEOUT; 
		SDIO->ICR|=1<<2;				//���������Ӧ��ʱ��־
		return errorstatus;
	}	 
	if(status&1<<0)						//CRC����
	{								   
		errorstatus=SD_CMD_CRC_FAIL;
		SDIO->ICR|=1<<0;				//�����Ӧ��־
 	}
	SDIO->ICR=0X5FF;	 				//������
 	return errorstatus;		
}

GRSdio::SdErr GRSdio::cmd_res3_error()
{
	u32 status;						 
 	while(1)
	{
		status=SDIO->STA;
		if(status&((1<<0)|(1<<2)|(1<<6)))break;//CRC����/������Ӧ��ʱ/�Ѿ��յ���Ӧ(CRCУ��ɹ�)	
	}
 	if(status&(1<<2))					//��Ӧ��ʱ
	{											 
		SDIO->ICR|=1<<2;				//���������Ӧ��ʱ��־
		return SD_CMD_RSP_TIMEOUT;
	}	 
   	SDIO->ICR=0X5FF;	 				//������
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
		if(status&((1<<0)|(1<<2)|(1<<6)))break;//CRC����/������Ӧ��ʱ/�Ѿ��յ���Ӧ(CRCУ��ɹ�)	
	}
	if(status&(1<<2))					//��Ӧ��ʱ
	{																				    
 		SDIO->ICR|=1<<2;				//���������Ӧ��ʱ��־
		return SD_CMD_RSP_TIMEOUT;
	}	 	 
	if(status&1<<0)						//CRC����
	{								   
		SDIO->ICR|=1<<0;				//�����Ӧ��־
 		return SD_CMD_CRC_FAIL;
	}
	if(SDIO->RESPCMD!=cmd)				//�ж��Ƿ���Ӧcmd����
	{
 		return SD_ILLEGAL_CMD; 		
	}	    
	SDIO->ICR=0X5FF;	 				//������б��
	rspr1=SDIO->RESP1;					//�õ���Ӧ 	 
	if(SD_ALLZERO==(rspr1&(SD_R6_GENERAL_UNKNOWN_ERROR|SD_R6_ILLEGAL_CMD|SD_R6_COM_CRC_FAILED)))
	{
		*prca=(u16)(rspr1>>16);			//����16λ�õ�,rca
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
		if(status&((1<<0)|(1<<2)|(1<<6)))break;//CRC����/������Ӧ��ʱ/�Ѿ��յ���Ӧ(CRCУ��ɹ�)	
	}
 	if((timeout==0)||(status&(1<<2)))	//��Ӧ��ʱ
	{																				    
		errorstatus=SD_CMD_RSP_TIMEOUT;	//��ǰ������2.0���ݿ�,���߲�֧���趨�ĵ�ѹ��Χ
		SDIO->ICR|=1<<2;				//���������Ӧ��ʱ��־
		return errorstatus;
	}	 
	if(status&1<<6)						//�ɹ����յ���Ӧ
	{								   
		errorstatus=SD_OK;
		SDIO->ICR|=1<<6;				//�����Ӧ��־
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
 	if(SDIO->RESP1&SD_CARD_LOCKED)return SD_LOCK_UNLOCK_FAILED;//SD������LOCKED״̬		    
 	errorstatus=find_scr(RCA,scr);						//�õ�SCR�Ĵ�������
 	if(errorstatus!=SD_OK)return errorstatus;
	if((scr[1]&SD_WIDE_BUS_SUPPORT)!=SD_ALLZERO)		//֧�ֿ�����
	{
	 	send_cmd(SD_CMD_APP_CMD,1,(u32)RCA<<16);	//����CMD55+RCA,����Ӧ											  
	 	errorstatus=cmd_res1_error(SD_CMD_APP_CMD);
	 	if(errorstatus!=SD_OK)return errorstatus; 
	 	send_cmd(SD_CMD_APP_SD_SET_BUSWIDTH,1,arg);//����ACMD6,����Ӧ,����:10,4λ;00,1λ.											  
		errorstatus=cmd_res1_error(SD_CMD_APP_SD_SET_BUSWIDTH);
		return errorstatus;
	}else return SD_REQUEST_NOT_APPLICABLE;				//��֧�ֿ��������� 	 
}

GRSdio::SdErr GRSdio::is_card_programming(u8 *pstatus)
{
 	vu32 respR1 = 0, status = 0; 
  	send_cmd(SD_CMD_SEND_STATUS,1,(u32)RCA<<16);		//����CMD13 	   
  	status=SDIO->STA;
	while(!(status&((1<<0)|(1<<6)|(1<<2))))status=SDIO->STA;//�ȴ��������
   	if(status&(1<<0))			//CRC���ʧ��
	{
		SDIO->ICR|=1<<0;		//���������
		return SD_CMD_CRC_FAIL;
	}
   	if(status&(1<<2))			//���ʱ 
	{
		SDIO->ICR|=1<<2;		//���������
		return SD_CMD_RSP_TIMEOUT;
	}
 	if(SDIO->RESPCMD!=SD_CMD_SEND_STATUS)return SD_ILLEGAL_CMD;
	SDIO->ICR=0X5FF;	 		//������б��
	respR1=SDIO->RESP1;
	*pstatus=(u8)((respR1>>9)&0x0000000F);
	return SD_OK;
}

GRSdio::SdErr GRSdio::find_scr(u16 rca,u32 *pscr)
{
	u32 index = 0; 
	SdErr errorstatus = SD_OK;
	u32 tempscr[2]={0,0};  
 	send_cmd(SD_CMD_SET_BLOCKLEN,1,8);			//����CMD16,����Ӧ,����Block SizeΪ8�ֽ�											  
 	errorstatus=cmd_res1_error(SD_CMD_SET_BLOCKLEN);
 	if(errorstatus!=SD_OK)return errorstatus;	    
  	send_cmd(SD_CMD_APP_CMD,1,(u32)rca<<16);	//����CMD55,����Ӧ 									  
 	errorstatus=cmd_res1_error(SD_CMD_APP_CMD);
 	if(errorstatus!=SD_OK)return errorstatus;
	send_data_cfg(SD_DATATIMEOUT,8,3,1);		//8���ֽڳ���,blockΪ8�ֽ�,SD����SDIO.
   	send_cmd(SD_CMD_SD_APP_SEND_SCR,1,0);		//����ACMD51,����Ӧ,����Ϊ0											  
 	errorstatus=cmd_res1_error(SD_CMD_SD_APP_SEND_SCR);
 	if(errorstatus!=SD_OK)return errorstatus;							   
 	while(!(SDIO->STA&(SDIO_FLAG_RXOVERR|SDIO_FLAG_DCRCFAIL|SDIO_FLAG_DTIMEOUT|SDIO_FLAG_DBCKEND|SDIO_FLAG_STBITERR)))
	{ 
		if(SDIO->STA&(1<<21))//����FIFO���ݿ���
		{
			*(tempscr+index)=SDIO->FIFO;	//��ȡFIFO����
			index++;
			if(index>=2)break;
		}
	}
 	if(SDIO->STA&(1<<3))		//�������ݳ�ʱ
	{										 
 		SDIO->ICR|=1<<3;		//������
		return SD_DATA_TIMEOUT;
	}
	else if(SDIO->STA&(1<<1))	//�ѷ���/���յ����ݿ�CRCУ�����
	{
 		SDIO->ICR|=1<<1;		//������
		return SD_DATA_CRC_FAIL;   
	}
	else if(SDIO->STA&(1<<5))	//����FIFO���
	{
 		SDIO->ICR|=1<<5;		//������
		return SD_RX_OVERRUN;   	   
	}
	else if(SDIO->STA&(1<<9))	//��ʼλ������
	{
 		SDIO->ICR|=1<<9;		//������
		return SD_START_BIT_ERR;    
	}
   	SDIO->ICR=0X5FF;	 		//������	 
	//������˳��8λΪ��λ������.   	
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
 	DMA2->IFCR|=(0XF<<12);				//���DMA2ͨ��4�ĸ��ֱ��
 	DMA2_Channel4->CCR&=~(1<<0);		//�ر�DMA ͨ��4
  DMA2_Channel4->CCR&=~(0X7FF<<4);	//���֮ǰ������,DIR,CIRC,PINC,MINC,PSIZE,MSIZE,PL,MEM2MEM
 	DMA2_Channel4->CCR|=dir<<4;  		//�Ӵ洢����   
	DMA2_Channel4->CCR|=0<<5;  			//��ͨģʽ
	DMA2_Channel4->CCR|=0<<6; 			//�����ַ������ģʽ
	DMA2_Channel4->CCR|=1<<7;  			//�洢������ģʽ
	DMA2_Channel4->CCR|=2<<8;  			//�������ݿ��Ϊ32λ
	DMA2_Channel4->CCR|=2<<10; 			//�洢�����ݿ��32λ
	DMA2_Channel4->CCR|=2<<12; 			//�����ȼ�	  
  DMA2_Channel4->CNDTR=bufsize/4;   	//DMA2,����������	  
 	DMA2_Channel4->CPAR=(u32)&SDIO->FIFO;//DMA2 �����ַ 
	DMA2_Channel4->CMAR=(u32)mbuf; 		//DMA2,�洢����ַ
 	DMA2_Channel4->CCR|=1<<0; 			//����DMAͨ��
}

//SDIO�жϷ�����		  
void SDIO_IRQHandler(void) 
{											
	if(GRCore::CIRQPtr(GRCore::Sdio))
		((GRSdio*)GRCore::CIRQPtr(GRCore::Sdio))->CIRQ();
	return;
}	 		

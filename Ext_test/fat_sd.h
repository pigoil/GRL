#ifndef FAT_SD_H
#define FAT_SD_H

#include <GRSpi>
#include <GRFatFs>
#include <GRSdio>

class SDCard;
class FatSD;

class SDCard : public GRSpi
{
public:
	SDCard(SPI_TypeDef*);
	u8 init();
	u8 sync();

	u8 getCid(u8* cid_data);
	u8 getCsd(u8* csd_data);
	u32 getSectorCnt();
	
	u8 readDisk(u8*buf,u32 sector,u8 cnt);
	u8 writeDisk(u8*buf,u32 sector,u8 cnt);

private:
	u8 sd_type;

	u8 select();
	void disselect();

	u8 wait_ready();
	u8 get_response(u8 res);

	u8 recv_data(u8* buf,u16 len);
	u8 send_data(u8* buf,u8 cmd);
	u8 send_cmd(u8 cmd,u32 arg,u8 rcc);
};

class FatSD : public GRFatFs
{
public:
	//FatSD(SPI_TypeDef*);
	FatSD();
	~FatSD(){delete card;};
	virtual u8 	disk_initialize (u8 pdrv);
	virtual u8 	disk_status (u8 pdrv);
	virtual u8 	disk_ioctl (u8 pdrv, u8 cmd, void* buff);

	virtual u8 	disk_read (u8 pdrv, u8* buff, u32 sector, u32 count);
	virtual u8 	disk_write (u8 pdrv, const u8* buff, u32 sector, u32 count);

private:
	GRSdio* card;
};

#endif

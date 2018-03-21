#include "gr_fatfs.h"

GRFatFs* disk;

/*********************
****FS相关接口封装****
**********************/
GRFatFs::GRFatFs()
{
	disk=this;
	for(u8 i=0;i<(_VOLUMES);++i)
	{
		fs[i]=new FATFS;
	}
}

GRFatFs::~GRFatFs()
{
	for(u8 i=0;i<(_VOLUMES);++i)
	{
		delete fs[i];
	}
}

u8 GRFatFs::mount(u8 pdrv)
{
	char drv_id='0'+pdrv;
	return f_mount(fs[pdrv],&drv_id,1);
}

u32 GRFatFs::getFree(u8 pdrv)
{
	u32 fre_clust=0,fre_sect=0;
	char drv_id='0'+pdrv;
	FATFS *fs1;
	
	u8 res = f_getfree((const TCHAR*)&drv_id,(DWORD*)&fre_clust, &fs1);
	if(res==0)
	{
		fre_sect=fre_clust * fs1->csize;
	}
	return fre_sect;
}

u32 GRFatFs::getTotal(u8 pdrv)
{
	u32 fre_clust=0,tot_sect=0;
	char drv_id='0'+pdrv;
	FATFS *fs1;
	
	u8 res = f_getfree((const TCHAR*)&drv_id,(DWORD*)&fre_clust, &fs1);
	if(res==0)
	{
		tot_sect=(fs1->n_fatent-2) * fs1->csize;
	}
	return tot_sect;
}

/*********************
***FILE相关接口封装***
**********************/
GRFile::GRFile(const TCHAR* n,BYTE m):
		mode(m),full_name(n)
{
	//nothing to do
}

GRFile::~GRFile()
{
	close();
}

FRESULT GRFile::open()
{
	return f_open(&fil,full_name,mode);
}

FRESULT GRFile::close()
{
	return f_close(&fil);
}

FRESULT GRFile::flush()
{
	return f_sync(&fil);
}

u32 GRFile::tell()
{
	return (u32)f_tell(&fil);
}

FRESULT GRFile::seek(u32 pt)
{
	return f_lseek(&fil,pt);
}

u32 GRFile::size()
{
	return (u32)f_size(&fil);
}

FRESULT GRFile::write(u8* buf,u32 cnt)
{
	return f_write(&fil,buf,(UINT)cnt,(UINT*)&rw_flag);
}

FRESULT GRFile::read(u8* buf,u32 cnt)
{
	return f_read(&fil,buf,(UINT)cnt,(UINT*)&rw_flag);
}

#if USE_STDIO
char _printf_buf[GRMacro::PRINTF_BUF_SIZE + 1];

FRESULT GRFile::printf(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	
	int printed=vsnprintf(_printf_buf,(size_t)GRMacro::PRINTF_BUF_SIZE,fmt,args);//printed是字符数，不包括\0
	va_end(args);
	
	if(printed==-1)printed=GRMacro::PRINTF_BUF_SIZE;//如果超过最大数量则只发送最大数量的字符
	return write((u8*)_printf_buf,printed);//发送
}

#endif
/********************
***DIR相关接口封装***
*********************/



//FATFS硬件层接口
DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	return (DSTATUS)disk->disk_status(pdrv);
}

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	return (DSTATUS)disk->disk_initialize(pdrv);
}

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	return (DRESULT)disk->disk_read(pdrv,buff,sector,count);
}

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	return (DRESULT)disk->disk_write(pdrv,buff,sector,count);
}

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	return (DRESULT)disk->disk_ioctl(pdrv,cmd,buff);
}

DWORD get_fattime (void)
{				 
	return disk->get_fattime();
}			

//动态分配内存
void *ff_memalloc (UINT size)			
{
	void* p=new u8[size];
	return p;
}
//释放内存
void ff_memfree (void* mf)		 
{
	delete[] (u8*)mf;
}

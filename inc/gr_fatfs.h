#ifndef GR_FATFS_H
#define GR_FATFS_H

#include "gr_core.h"
#include "../third_party/FatFs/src/diskio.h"
#include "../third_party/FatFs/src/ff.h"

#if USE_STDIO
#include <stdarg.h>
#include <stdio.h>
#endif

/*****************************************************
**本库函数基于FatFs开发
**FatFs版本号：R0.12b
**英文页面：http://elm-chan.org/fsw/ff/00index_e.html
*****************************************************/

class GRFatFs;
class GRFile;
class GRFileInfo;
class GRDir;

class GRFatFs
{
public:
	GRFatFs();
	~GRFatFs();
	virtual u8 	disk_initialize (u8 pdrv)=0;
	virtual u8 	disk_status (u8 pdrv)=0;
	virtual u8 	disk_ioctl (u8 pdrv, u8 cmd, void* buff)=0;

	virtual u8 	disk_read (u8 pdrv, u8* buff, u32 sector, u32 count)=0;
	virtual u8 	disk_write (u8 pdrv, const u8* buff, u32 sector, u32 count){return 0;};
	virtual u32 get_fattime (void){return 0;};
	
	u8 mount(u8 pdrv);//0-9
	u32 getFree(u8 pdrv);//返回扇区数
	u32 getTotal(u8 pdrv);//同上
	
private:
	FATFS* fs[_VOLUMES];
};

class GRFile
{
public:
	GRFile(const TCHAR*,BYTE mode);
	~GRFile();
	FRESULT open();//打开文件
	FRESULT close();//关闭并保存文件
	FRESULT	flush();//刷新文件

	u32 tell();//获取当前读写指针
	FRESULT seek(u32 pt);//读写指针移动

	u32 size();//返回大小
	u32 rwFlag(){return rw_flag;};//读取读写变量值

	FRESULT write(u8* buf,u32 cnt);
	FRESULT read(u8* buf,u32 cnt);
	
	#if USE_STDIO
	FRESULT printf(const char *fmt, ...); 
	#endif
	
private:
	FIL fil;
	BYTE mode;
	u32 rw_flag;
	const TCHAR* full_name;
};

class GRDir
{
public:
	
private:
	DIR dir;
	const TCHAR* full_path;
};

#endif

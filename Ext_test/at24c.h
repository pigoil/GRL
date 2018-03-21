#include <GRI2c>

class AT24C;
class Setting;

class AT24C : public GRI2c
{
public:
	AT24C();

	bool 	check();
	void 	writeBuf(u8 address,u8* buff,u8 cnt);
	void 	readBuf(u8 address,u8* buff,u8 cnt);
	void 	clear(u8 address,u8 cnt);

private:
	void	write_eight(u8 address,u8* buff,u8 cnt);//写入八个字节
};

class Setting : public AT24C
{
public:
	enum Key{Kp,Ti,Td,Ur,Un,T,Report};//AT24C02最多64个设置项
	Setting():AT24C(){};
	
	void writeSetting(Key,double);
	void writeSetting(Key,int);
	void writeSetting(Key,bool);
	void readSetting(Key,float&);
	void readSetting(Key,s32&);
	void readSetting(Key,bool&);
};

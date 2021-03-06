#ifndef GR_RTC_H
#define GR_RTC_H

#include "gr_core.h"

class GRRtc;
class GRDateTime;

class GRRtc
{
public:
	GRRtc();
	GRRtc(GRDateTime);
	
	virtual ~GRRtc(){};

	bool setTimeOnce(GRDateTime);
	bool setTimeForce(GRDateTime);
	GRDateTime dateTime();
private:
	bool hw_init();
};

class GRDateTime
{
public:
	typedef u16 Years;
	enum Months{Jan = 1,Feb,Mar,Apr,May,Jun,Jul,Aug,Sept,Oct,Nov,Dec};
	typedef u8 Dates;
	enum Weeks{Sun = 0,Mon,Tues,Wed,Thur,Fri,Sat};
	typedef u8 Hours;
	typedef u8 Minutes;
	typedef u8 Seconds;

	GRDateTime(Years yy,Months MM,Dates dd,Hours hh,Minutes mm,Seconds ss);
	GRDateTime(Years yy,u8 MM,Dates dd,Hours hh,Minutes mm,Seconds ss);
	GRDateTime(u32 secs);
	
	virtual ~GRDateTime(){};

	void setDateTime(Years yy,Months MM,Dates dd,Hours hh,Minutes mm,Seconds ss);
	void setDateTime(u32 secs);
	
	void setYear(Years yy);
	void setMonth(Months MM);
	void setDate(Dates dd);
	void setHour(Hours hh);
	void setMinutes(Minutes mm);
	void setSeconds(Seconds ss);
	
	Years year(){return m_year;}
	Months month(){return m_month;}
	Dates date(){return m_date;}
	Weeks week(){return m_week;}
	Hours hour(){return m_hour;}
	Minutes minute(){return m_min;}
	Seconds second(){return m_sec;}
	u32 secsSinceEpoch(){return m_secs_since_epoch;}
	
	bool isLeapYear(Years yy);
	//const char* dateTimeInString(){return time_in_string;}
	
	bool operator==(GRDateTime&);
	bool operator!=(GRDateTime&);
	bool operator>(GRDateTime&);
	bool operator<(GRDateTime&);
	
private:

	Years	m_year;
	Months	m_month;
	Dates	m_date;
	Hours	m_hour;
	Minutes m_min;
	Seconds m_sec;
	Weeks	m_week;
	
	u32 m_secs_since_epoch;
	//char time_in_string[20];
	
	void sync_data(Years yy,Months MM,Dates dd,Hours hh,Minutes mm,Seconds ss);
	void sync_data(u32 secs);
	//void fill_srting();
	void normalize();
	Weeks week_calc(Years yy,Months MM,Dates dd);
};

#endif

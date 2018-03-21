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
	
	void setDateTime(Years yy,Months MM,Dates dd,Hours hh,Minutes mm,Seconds ss);
	void setDateTime(u32 secs);
	
	void setYear(Years yy);
	void setMonth(Months MM);
	void setDate(Dates dd);
	void setHour(Hours hh);
	void setMinutes(Minutes mm);
	void setSeconds(Seconds ss);
	
	Years year(){return o_year;}
	Months month(){return e_month;}
	Dates date(){return o_date;}
	Weeks week(){return e_week;}
	Hours hour(){return o_hour;}
	Minutes minute(){return o_min;}
	Seconds second(){return o_sec;}
	u32 secsSinceEpoch(){return secs_since_epoch;}
	
	bool isLeapYear(Years yy);
	//const char* dateTimeInString(){return time_in_string;}
	
	bool operator==(GRDateTime&);
	bool operator!=(GRDateTime&);
	bool operator>(GRDateTime&);
	bool operator<(GRDateTime&);
	
private:

	Years o_year;
	Months e_month;
	Dates o_date;
	Hours o_hour;
	Minutes o_min;
	Seconds o_sec;
	Weeks e_week;	
	
	u32 secs_since_epoch;
	//char time_in_string[20];
	
	void sync_data(Years yy,Months MM,Dates dd,Hours hh,Minutes mm,Seconds ss);
	void sync_data(u32 secs);
	//void fill_srting();
	void normalize();
	Weeks week_calc(Years yy,Months MM,Dates dd);
};

#endif

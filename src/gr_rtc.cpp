#include "gr_rtc.h"

GRRtc::GRRtc()
{
	GRDateTime tmp(0);
	if(BKP->DR1!=0X5050)
		setTimeOnce(tmp);
	else
	{
    	while(!(RTC->CRL&(1<<3)));//等待RTC寄存器同步  
    	RTC->CRH|=0X01;  		  //允许秒中断
    	while(!(RTC->CRL&(1<<5)));//等待RTC寄存器操作完成
	} 
}

GRRtc::GRRtc(GRDateTime dt)
{
	setTimeOnce(dt);
	while(!(RTC->CRL&(1<<3)));//等待RTC寄存器同步  
	RTC->CRH|=0X01;  		  //允许秒中断
	while(!(RTC->CRL&(1<<5)));//等待RTC寄存器操作完成
}

bool GRRtc::setTimeOnce(GRDateTime dt)
{
	if(BKP->DR1!=0X5050)//第一次配置
	{	 
	  setTimeForce(dt);
	}
	return true;
}

bool GRRtc::setTimeForce(GRDateTime dt)
{
		u8 temp = 0;
		RCC->APB1ENR|=1<<28;     //使能电源时钟	    
		RCC->APB1ENR|=1<<27;     //使能备份时钟	    
		PWR->CR|=1<<8;           //取消备份区写保护
		RCC->BDCR|=1<<16;        //备份区域软复位	   
		RCC->BDCR&=~(1<<16);     //备份区域软复位结束	  	 
	    RCC->BDCR|=1<<0;         //开启外部低速振荡器 
	  while((!(RCC->BDCR&0X02))&&temp<250)//等待外部时钟就绪	 
		{
			temp++;
			GRCore::SleepMs(10);
		};
		if(temp>=250)return false;//初始化时钟失败,晶振有问题	   
		RCC->BDCR|=1<<8; //LSI作为RTC时钟 	    
		RCC->BDCR|=1<<15;//RTC时钟使能	  
		while(!(RTC->CRL&(1<<5))){}//等待RTC寄存器操作完成	 
		while(!(RTC->CRL&(1<<3))){}//等待RTC寄存器同步  
//    	RTC->CRH|=0X01;  		  //允许秒中断
//    	RTC->CRH|=0X02;  		  //允许闹钟中断
//    	while(!(RTC->CRL&(1<<5)));//等待RTC寄存器操作完成	 
		RTC->CRL|=1<<4;           //允许配置	 
		RTC->PRLH=0X0000;
		RTC->PRLL=32767;          //时钟周期设置(有待观察,看是否跑慢了?)理论值：32767	
			//设置时钟
			
    RCC->APB1ENR|=1<<28;//使能电源时钟
    RCC->APB1ENR|=1<<27;//使能备份时钟
		PWR->CR|=1<<8;    //取消备份区写保护
		//上面三步是必须的!
		RTC->CRL|=1<<4;   //允许配置 
		RTC->CNTL=dt.secsSinceEpoch()&0xffff;
		RTC->CNTH=dt.secsSinceEpoch()>>16;
		RTC->CRL&=~(1<<4);//配置更新
		while(!(RTC->CRL&(1<<5)));//等待RTC寄存器操作完成  
			
		RTC->CRL&=~(1<<4);           //配置更新
		while(!(RTC->CRL&(1<<5)));   //等待RTC寄存器操作完成		 									  
		BKP->DR1=0X5050;  
		return true;
}

GRDateTime GRRtc::dateTime()
{
	u32 timecount=0; 
 	timecount=RTC->CNTH;//得到计数器中的值(秒钟数)
	timecount<<=16;
	timecount+=RTC->CNTL;		
	return GRDateTime(timecount);
}

/************日期时间类***************/
static const u8 months_tab[13] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
static const u8 table_week[12]={0,3,3,6,1,4,6,2,5,0,3,5}; //月修正数据表	  
static const u16 EPOCH_YEAR = 1970;//纪元年份
//static const u16 MAX_YEAR = 2100;//最大年份

GRDateTime::GRDateTime(Years yy,Months MM,Dates dd,Hours hh,Minutes mm,Seconds ss):
m_year(yy),
m_month(MM),
m_date(dd),
m_hour(hh),
m_min(mm),
m_sec(ss)
{
	normalize();
	sync_data(yy,MM,dd,hh,mm,ss);
	//fill_srting();
}

GRDateTime::GRDateTime(Years yy,u8 MM,Dates dd,Hours hh,Minutes mm,Seconds ss):
	m_year(yy),
	m_date(dd),
	m_hour(hh),
	m_min(mm),
	m_sec(ss)
{
	if(MM < 1 || MM > 12)m_month = Jan;
	else m_month = static_cast<Months>(MM);
	normalize();
	sync_data(yy,m_month,dd,hh,mm,ss);
	//fill_srting();
}

GRDateTime::GRDateTime(u32 secs):
	m_secs_since_epoch(secs)
{
	sync_data(secs);
	//fill_srting();
}

//根据日期计算星期，要求日期必须经过normalize
GRDateTime::Weeks GRDateTime::week_calc(Years yy,Months MM,Dates dd)
{
	u16 temp2;
	u8 yearH,yearL;
	
	yearH=yy/100;	yearL=yy%100; 
	// 如果为21世纪,年份数加100  
	if (yearH>19)yearL+=100;
	// 所过闰年数只算1900年之后的  
	temp2=yearL+yearL/4;
	temp2=temp2%7; 
	temp2=temp2+dd+table_week[MM-1];
	if (yearL%4==0 && MM<3)temp2--;
	return static_cast<Weeks>(temp2%7);
}

//根据年月日时分秒计算纪元时间和星期
//要求必须经过normalize
void GRDateTime::sync_data(Years yy,Months MM,Dates dd,Hours hh,Minutes mm,Seconds ss)
{
	u32 sec_cnt = 0;
	for(u16 t=1970;t<m_year;++t)
	{
		if(isLeapYear(t))sec_cnt += 31622400;
		else sec_cnt += 31536000;
	}
	for(u8 t=1 ; t<(static_cast<u8>(MM)) ; ++t)
	{
		sec_cnt+=(u32)months_tab[t] * 86400;
		if(isLeapYear(yy) && t==static_cast<u8>(Feb))sec_cnt+=86400;
	}
	sec_cnt+=(u32)(dd-1)*86400;
	sec_cnt+=(u32)hh*3600;
	sec_cnt+=(u32)mm*60;
	sec_cnt+=ss;
	
	m_secs_since_epoch = sec_cnt;
	m_week = week_calc(yy,MM,dd);
}

//根据纪元时间计算年月日时分秒和星期
void GRDateTime::sync_data(u32 secs)
{
	u32 days = secs/86400;
	u32 remain = secs%86400;
	
	//计算年份	
	u16 days_per_year = 365;//1970年是平年
	for(m_year=1970 ; days>days_per_year ; ++m_year)
	{
		days-=days_per_year;
		days_per_year = isLeapYear(m_year+1) ? 366 : 365;
	}
	//计算月份
	u8 days_per_month = 31;//一月31天
	u8 month = 1;
	for(month=1 ; days>days_per_month ; ++month)
	{
		days-=months_tab[month];
		if(isLeapYear(m_year) && month==2)days-=1;
		days_per_month = months_tab[month+1];
	}
	m_date = days+1;
	m_month = static_cast<Months>(month);
	//计算小时
	m_hour = remain/3600;
	m_min = (remain%3600)/60;
	m_sec = (remain%3600)%60;
	m_week = week_calc(m_year,m_month,m_date);
}

//返回当前年份是否是闰年
bool GRDateTime::isLeapYear(Years yy)
{
	if(yy%4 == 0)
	{
		if(yy%100==0)
		{
			if(yy%400==0)return true;
			else return false;
		}
		else return true;
	}
	return false;
}

//传进来的只有月份能保证是没问题的
//只管数值是否对，不管是否同步
void GRDateTime::normalize()
{
	//数值要求
	//年份区间限制
	if(m_year < EPOCH_YEAR)m_year = EPOCH_YEAR;
	//else if(o_year > MAX_YEAR)o_year = MAX_YEAR;
	//日期区间限制
	if(!m_date)m_date=1;
	else if(m_date > months_tab[m_month])//与平年月份表作比较
	{
		if(m_month == Feb)//如果是二月
		{
			if(isLeapYear(m_year))//如果是闰年
			{
				if(m_date > 29)m_date = 29;
			}
			else m_date = 28;
		}
		m_date = months_tab[m_month];
	}
	//秒区间限制
	if(m_sec > 59)m_sec = 0;		
	//分钟区间限制
	if(m_min > 59)m_min = 0;
	//小时区间限制
	if(m_hour > 23)m_hour = 0;
}

//void GRDateTime::fill_srting()
//{
//	
//	#define NUM2CHAR(var) (char)(var)+'0'
//		
//	time_in_string[0] = NUM2CHAR(o_year%10000/1000);
//	time_in_string[1] = NUM2CHAR(o_year%1000/100);
//	time_in_string[2] = NUM2CHAR(o_year%100/10);
//	time_in_string[3] = NUM2CHAR(o_year%10);
//	time_in_string[4] = '-';
//	time_in_string[5] = NUM2CHAR(e_month%100/10);
//	time_in_string[6] = NUM2CHAR(e_month%10);
//	time_in_string[7] = '-';
//	time_in_string[8] = NUM2CHAR(o_date%100/10);
//	time_in_string[9] = NUM2CHAR(o_date%10);
//	time_in_string[10] = ' ';
//	time_in_string[11] = NUM2CHAR(o_hour%100/10);
//	time_in_string[12] = NUM2CHAR(o_hour%10);
//	time_in_string[13] = ':';
//	time_in_string[14] = NUM2CHAR(o_min%100/10);
//	time_in_string[15] = NUM2CHAR(o_min%10);
//	time_in_string[16] = ':';
//	time_in_string[17] = NUM2CHAR(o_sec%100/10);
//	time_in_string[18] = NUM2CHAR(o_sec%10);
//	time_in_string[19] = '\0';
//}

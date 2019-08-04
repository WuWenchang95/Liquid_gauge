#ifndef _RTC_C_
#define _RTC_C_
/*********************************************************************************************************************/
#include "ca51f_config.h"
#if (IC_TYPE == IC_TYPE_CA51F2X)
#include "../../includes/ca51f2sfr.h"
#include "../../includes/ca51f2xsfr.h"
#include "../../includes/gpiodef_f2.h"
#elif (IC_TYPE == IC_TYPE_CA51F3X)
#include "../../includes/ca51f3sfr.h"
#include "../../includes/ca51f3xsfr.h"
#include "../../includes/gpiodef_f3.h"
#endif
#include "../../Library/includes/system_clock.h"
#include "../../includes/system.h"
#include "../../Library/includes/rtc.h"
#include "../../Library/includes/delay.h"
#include <intrins.h>
/*********************************************************************************************************************/
#define uchar unsigned char
#define uint  unsigned int
typedef unsigned char u8_t;
extern unsigned char SetMode;

code u8_t DAY_MAX[]={0,31,28,31,30,31,30,31,31,30,31,30,31};  //1-12每月的天数
//时间
char  last_hour=23;
char  hour=8;
char  minute=00;
char  second=00;
//日期
char  year=18;		//2000+year;
char  month=8;
char  day=7;
char  week;

uchar Flash_Half_S;
extern uchar Flag_0_5s;


uchar JudgeDayOfWeek(u8_t _year, u8_t _month, u8_t _day);

void EXINT8_ISR (void) interrupt 13 	 
{

//	if(RTCON & RTCAF)			//闹钟中断
//	{
//		RTCON |= RTCAF;
//		S0BUF = 0XFF;
//		while(!TI0);
//		TI0=0;
//		S0BUF = 0XFF;
//		while(!TI0);
//		TI0=0;
//		S0BUF = 0XFF;
//		while(!TI0);
//		TI0=0;
//	}
	if(RTCIF & RTC_HF)			//半秒中断
	{
		RTCIF |= RTC_HF;				

//		if(Flash_Half_S)	Flash_Half_S=0;
//		else Flash_Half_S=1;

//		Flag_0_5s=1;	
	}
	if(EPIF&BIT6) 					//External interrupt 	
	{
		EPIF|=BIT3;	
	}	

	if(RTCIF & RTC_AF) //闹钟中断
	{
		RTCIF |= RTC_AF;
		//闹钟中断服务程序
		RTM_Con(RTM_Off);
	}
	
}





/***********************************************************************************
函数名：		RTC_init
功能说明： 初始化RTC控制寄存器
输入参数： rtce		RTC功能模块使能控制
						mse		RTC毫秒中断使能控制
						hse		RTC半秒中断使能控制
返回值：	无
***********************************************************************************/																												
void RTC_init(RTCE_TypeDef rtce,MSE_TypeDef mse,HSE_TypeDef hse)
{	
	CKCON |=XLCKE;
	while(!(CKCON & XLSTA));
	
	RTCON = (RTCON & 0x1F) | RTCE(rtce) | MSE(mse) | HSE(hse);

	//INT8EN = 1;
	RTC_WriteHour(23);
	RTC_WriteMinute(59);
	RTC_WriteSecond(30);

	RTCS	=	30;
	RTCM	=	59;
	RTCH	=	23;	
	
	week=JudgeDayOfWeek(year,month,day);
	
}

//例如，设置闹钟时间为11:30:0，时、分、秒比较全使能，程序如下：
//----------------------------------------------------------------------------------------

void RTM_init(unsigned char _hour, unsigned char _sec, unsigned char _minute)
{
	RTAH = _hour; //设置闹钟小时
	RTAM = _sec; //设置闹钟分
	RTAS = _minute; //设置闹钟秒

}

void RTM_Con(RTM_TypeDef rtm)
{
	if(rtm != RTM_On)
	{
		RTCON |= SCE(SCE_Off)|MCE(MCE_Off)|HCE(HCE_Off); //时、分、秒比较使能
		INT8EN = 0;
	}	
	else
	{
		RTCON |= SCE(SCE_On)|MCE(MCE_On)|HCE(HCE_Off);	
		//RTCON |= SCE(SCE_On)|MCE(MCE_On)|HCE(HCE_On); //时、分、秒比较使能
		INT8EN = 1;		
	}	
}
/***********************************************************************************
函数名：		RTC_WriteSecond
功能说明： RTC写入秒值
输入参数： second		秒值
返回值：		无
***********************************************************************************/	
void RTC_WriteSecond(unsigned char second) //second = 0~59
{
	RTCON |= RTCWE(1);
	RTCS = second;
	Delay_50us(100);
	RTCON &= ~RTCWE(1);	 	
}

/***********************************************************************************
函数名：		RTC_WriteMinute
功能说明： RTC写入分值
输入参数： minute		分值
返回值：		无
***********************************************************************************/	
void RTC_WriteMinute(unsigned char minute) //minute = 0~59
{
	RTCON |= RTCWE(1);
	RTCM = minute;
	Delay_50us(100);
	RTCON &= ~RTCWE(1);
	
	RTCON |= RTCWE(1);
	RTCM = minute;
	Delay_50us(100);
	RTCON &= ~RTCWE(1);	
	
}


/***********************************************************************************
函数名：		RTC_WriteHour
功能说明： RTC写入小时
输入参数： hour		小时值
返回值：		无
***********************************************************************************/
void RTC_WriteHour(unsigned char hour)	//hour = 0~23
{
	RTCON |= RTCWE(1);
	RTCH = (RTCH&0xE0)|hour;
	Delay_50us(100);
	RTCON &= ~RTCWE(1);	 	
	
	RTCON |= RTCWE(1);
	RTCH = (RTCH&0xE0)|hour;
	Delay_50us(100);
	RTCON &= ~RTCWE(1);		

}



bit JudgeIsLeapYear(u8_t _year)				//判断是否闰年
{
   if((_year+2000)%4==0)	//2001--2099
   {
		return 1;
   }
   return 0;
}
uchar GetDayMax(u8_t _year,u8_t _month)	 	//获取每月的天数
{
	if((_month==2)&&(JudgeIsLeapYear(_year)))		//闰年2月份29天
	{
	   return ((DAY_MAX[_month])+1);
	}
  	return (DAY_MAX[_month]);
}
uchar JudgeDayOfWeek(u8_t _year, u8_t _month, u8_t _day)		//获取星期几
{  
	u8_t week_temp;
	u8_t c=20;
	if(_month<=2)
	{
		if((_year%100) == 0)
		  	c--;
		_year--;
		_month+=12;
	}
	week_temp=(_year+(u8_t)(_year/4)+(u8_t)(c/4)-(2*c)+(u8_t)((26*(_month+1))/10)+_day-1-1+2500)%7;
	if(week_temp)
	{
	   return week_temp;
	}
	return 7;
}

void _date_add(void)   						//每天累加计算日期（年，月，日，星期几）
{
	last_hour=hour;
	if(hour==0)
	{
		day++;
		if(day>GetDayMax(year,month))
		{
		   month++;
		   day=1;
		   if(month>12)
		   {
		   	  month=1;
			  year++;
			  if(year>99) year=00;
		   }
		}
		week=JudgeDayOfWeek(year,month,day);
	}
}

void RTC_ReadTime(void)						//读取最终的当前时间结果
{	
	second = RTCS;

	minute = RTCM;

	hour = RTCH&0x1f;

	if(last_hour!=hour)	_date_add();
}

///***********************************************************************************
//函数名：		RTC_WriteWeek
//功能说明： RTC写入星期
//输入参数： week		星期值
//返回值：		无
//***********************************************************************************/
//void RTC_WriteWeek(unsigned char week) //hour = 1~7代表星期一~星期天
//{
//	RTCON |= RTCWE(1);
//	RTCH = (RTCH&0x1F)|(week<<5);
//	Delay_50us(1);
//	RTCON &= ~RTCWE(1);	 	
//}


///***********************************************************************************
//函数名：		RTC_AlarmCfg
//功能说明： 配置RTC ALARM功能寄存器位
//输入参数： hce		小时比较使能控制
//						mce		分钟比较使能控制
//						sce		秒比较使能控制
//返回值：		无
//***********************************************************************************/
//void RTC_AlarmCfg(HCE_TypeDef hce,MCE_TypeDef mce,SCE_TypeDef sce)
//{	
//	RTCON = (RTCON & 0xE3) | HCE(hce) | MCE(mce) | SCE(sce);
//}

///***********************************************************************************
//函数名：		RTC_SetAlaramTime
//功能说明：  设置RTC ALARM时间
//输入参数：  al_hr		闹钟小时值
//						al_min		闹钟分钟值
//						al_sec		闹钟秒值
//返回值：		无
//***********************************************************************************/
//void RTC_SetAlaramTime(unsigned char al_hr,unsigned char al_min,unsigned char al_sec)
//{
//	RTAH	=	al_hr;
//	RTAM	=	al_min;
//	RTAS	=	al_sec;
//}

///***********************************************************************************
//函数名：		RTC_SetRTMSS
//功能说明：  设置毫秒中断间隔
//输入参数：  rtmss		
//返回值：		无
//***********************************************************************************/
//void RTC_SetRTMSS(unsigned char rtmss)
//{
//	RTMSS = rtmss;
//}

///*********************************************************************************************************************/
#endif
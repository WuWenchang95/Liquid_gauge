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

code u8_t DAY_MAX[]={0,31,28,31,30,31,30,31,31,30,31,30,31};  //1-12ÿ�µ�����
//ʱ��
char  last_hour=23;
char  hour=8;
char  minute=00;
char  second=00;
//����
char  year=18;		//2000+year;
char  month=8;
char  day=7;
char  week;

uchar Flash_Half_S;
extern uchar Flag_0_5s;


uchar JudgeDayOfWeek(u8_t _year, u8_t _month, u8_t _day);

void EXINT8_ISR (void) interrupt 13 	 
{

//	if(RTCON & RTCAF)			//�����ж�
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
	if(RTCIF & RTC_HF)			//�����ж�
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

	if(RTCIF & RTC_AF) //�����ж�
	{
		RTCIF |= RTC_AF;
		//�����жϷ������
		RTM_Con(RTM_Off);
	}
	
}





/***********************************************************************************
��������		RTC_init
����˵���� ��ʼ��RTC���ƼĴ���
��������� rtce		RTC����ģ��ʹ�ܿ���
						mse		RTC�����ж�ʹ�ܿ���
						hse		RTC�����ж�ʹ�ܿ���
����ֵ��	��
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

//���磬��������ʱ��Ϊ11:30:0��ʱ���֡���Ƚ�ȫʹ�ܣ��������£�
//----------------------------------------------------------------------------------------

void RTM_init(unsigned char _hour, unsigned char _sec, unsigned char _minute)
{
	RTAH = _hour; //��������Сʱ
	RTAM = _sec; //�������ӷ�
	RTAS = _minute; //����������

}

void RTM_Con(RTM_TypeDef rtm)
{
	if(rtm != RTM_On)
	{
		RTCON |= SCE(SCE_Off)|MCE(MCE_Off)|HCE(HCE_Off); //ʱ���֡���Ƚ�ʹ��
		INT8EN = 0;
	}	
	else
	{
		RTCON |= SCE(SCE_On)|MCE(MCE_On)|HCE(HCE_Off);	
		//RTCON |= SCE(SCE_On)|MCE(MCE_On)|HCE(HCE_On); //ʱ���֡���Ƚ�ʹ��
		INT8EN = 1;		
	}	
}
/***********************************************************************************
��������		RTC_WriteSecond
����˵���� RTCд����ֵ
��������� second		��ֵ
����ֵ��		��
***********************************************************************************/	
void RTC_WriteSecond(unsigned char second) //second = 0~59
{
	RTCON |= RTCWE(1);
	RTCS = second;
	Delay_50us(100);
	RTCON &= ~RTCWE(1);	 	
}

/***********************************************************************************
��������		RTC_WriteMinute
����˵���� RTCд���ֵ
��������� minute		��ֵ
����ֵ��		��
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
��������		RTC_WriteHour
����˵���� RTCд��Сʱ
��������� hour		Сʱֵ
����ֵ��		��
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



bit JudgeIsLeapYear(u8_t _year)				//�ж��Ƿ�����
{
   if((_year+2000)%4==0)	//2001--2099
   {
		return 1;
   }
   return 0;
}
uchar GetDayMax(u8_t _year,u8_t _month)	 	//��ȡÿ�µ�����
{
	if((_month==2)&&(JudgeIsLeapYear(_year)))		//����2�·�29��
	{
	   return ((DAY_MAX[_month])+1);
	}
  	return (DAY_MAX[_month]);
}
uchar JudgeDayOfWeek(u8_t _year, u8_t _month, u8_t _day)		//��ȡ���ڼ�
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

void _date_add(void)   						//ÿ���ۼӼ������ڣ��꣬�£��գ����ڼ���
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

void RTC_ReadTime(void)						//��ȡ���յĵ�ǰʱ����
{	
	second = RTCS;

	minute = RTCM;

	hour = RTCH&0x1f;

	if(last_hour!=hour)	_date_add();
}

///***********************************************************************************
//��������		RTC_WriteWeek
//����˵���� RTCд������
//��������� week		����ֵ
//����ֵ��		��
//***********************************************************************************/
//void RTC_WriteWeek(unsigned char week) //hour = 1~7��������һ~������
//{
//	RTCON |= RTCWE(1);
//	RTCH = (RTCH&0x1F)|(week<<5);
//	Delay_50us(1);
//	RTCON &= ~RTCWE(1);	 	
//}


///***********************************************************************************
//��������		RTC_AlarmCfg
//����˵���� ����RTC ALARM���ܼĴ���λ
//��������� hce		Сʱ�Ƚ�ʹ�ܿ���
//						mce		���ӱȽ�ʹ�ܿ���
//						sce		��Ƚ�ʹ�ܿ���
//����ֵ��		��
//***********************************************************************************/
//void RTC_AlarmCfg(HCE_TypeDef hce,MCE_TypeDef mce,SCE_TypeDef sce)
//{	
//	RTCON = (RTCON & 0xE3) | HCE(hce) | MCE(mce) | SCE(sce);
//}

///***********************************************************************************
//��������		RTC_SetAlaramTime
//����˵����  ����RTC ALARMʱ��
//���������  al_hr		����Сʱֵ
//						al_min		���ӷ���ֵ
//						al_sec		������ֵ
//����ֵ��		��
//***********************************************************************************/
//void RTC_SetAlaramTime(unsigned char al_hr,unsigned char al_min,unsigned char al_sec)
//{
//	RTAH	=	al_hr;
//	RTAM	=	al_min;
//	RTAS	=	al_sec;
//}

///***********************************************************************************
//��������		RTC_SetRTMSS
//����˵����  ���ú����жϼ��
//���������  rtmss		
//����ֵ��		��
//***********************************************************************************/
//void RTC_SetRTMSS(unsigned char rtmss)
//{
//	RTMSS = rtmss;
//}

///*********************************************************************************************************************/
#endif
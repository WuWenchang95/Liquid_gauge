#ifndef _MAIN_C_
#define _MAIN_C_

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
#include "../../includes/system.h"
#include "../../Library/includes/system_clock.h"

#include "../../Library/includes/uart.h"
#include "TM1640.h"
#include "flowfunction.h"
#include "key.h"
#include "buzzer.h"

#include <intrins.h>

uint8_t xdata Time_10ms;

void Timer1Init(void)			//T1��ʼ��	3.6864 MHZ/12*4       1������0.814us
{
 	TMOD = (TMOD&0xFF)|0x10;     //��ʱ��1 ģʽ1 16������  
	TH1=0xD0;	 	//     10ms     10,000 / 0.814 = 12285     65536-12285 = 53251
	TL1=0x03;	 	

	ET1=1;
	TR1=1;

}


void Timer1Zd(void) interrupt 3	   	//��ʱ��0�ж�
{

	TH0=0xD0;	 	
	TL0=0x03;	 	//10ms�ж�һ��
	Time_10ms++;
	if(Time_10ms == 0xff)
		Time_10ms = 0;
}


void main(void)
{
	Sys_Clk_Set_PLL(PLL_Multiple);	//ʱ�ӳ�ʼ��
	Timer1Init();					//timer1��ʼ��
	BuzzerGPIOInit();				//��������ʼ��
	KeyInit(KeyProcess);			//������ʼ��		
	TM1640Init();					//1640��ʼ��
	PoweroffMemoryInit(); 			//�����ݿռ�
	
	FlowProcess();
	return;
}	


#endif

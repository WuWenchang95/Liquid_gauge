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


void T0_init(void)			//T0��ʼ��	3.6864 MHZ/12*4    1������0.813us
{
	TMOD=0x11;	
 	
	TH0=0xcf;	 	//f3      10ms
	TL0=0xff;	 	//ff

	ET0=1;
	TR0=1;
	
//	TH1=256-16;	 	
//	TL1=256-16;	 	

//	ET1=1;
//	TR1=1;	
		
}


void T0_INT(void) interrupt 1	   	//��ʱ��0�ж�
{
//	cord++;		 //10ms  �ж�һ��
	TH0=0xcf;	 	
	TL0=0xff;	 	//10ms�ж�һ��
	Time_10ms++;
	if(Time_10ms == 0xff)
		Time_10ms = 0;
}



void main(void)
{
	Sys_Clk_Set_PLL(PLL_Multiple);	//ʱ�ӳ�ʼ��
	T0_init();						//timer0��ʼ��
	BuzzerGPIOInit();				//��������ʼ��
	KeyInit(KeyProcess);			//������ʼ��		
	TM1640Init();					//1640��ʼ��
	PoweroffMemory_init(); 			//�����ݿռ�
	
	FlowProcess();
	return;
}	


#endif

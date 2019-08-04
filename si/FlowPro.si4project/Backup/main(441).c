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


void T0_init(void)			//T0初始化	3.6864 MHZ/12*4    1个计数0.813us
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


void T0_INT(void) interrupt 1	   	//定时器0中断
{
//	cord++;		 //10ms  中断一次
	TH0=0xcf;	 	
	TL0=0xff;	 	//10ms中断一次
	Time_10ms++;
	if(Time_10ms == 0xff)
		Time_10ms = 0;
}



void main(void)
{
	Sys_Clk_Set_PLL(PLL_Multiple);	//时钟初始化
	T0_init();						//timer0初始化
	BuzzerGPIOInit();				//蜂鸣器初始化
	KeyInit(KeyProcess);			//按键初始化		
	TM1640Init();					//1640初始化
	PoweroffMemory_init(); 			//读数据空间
	
	FlowProcess();
	return;
}	


#endif

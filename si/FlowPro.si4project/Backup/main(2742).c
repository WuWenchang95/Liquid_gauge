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

void Timer1Init(void)			//T1初始化	3.6864 MHZ/12*4       1个计数0.814us
{
 	TMOD = (TMOD&0xFF)|0x10;     //定时器1 模式1 16计数器  
	TH1=0xD0;	 	//     10ms     10,000 / 0.814 = 12285     65536-12285 = 53251
	TL1=0x03;	 	

	ET1=1;
	TR1=1;

}


void Timer1Zd(void) interrupt 3	   	//定时器0中断
{

	TH0=0xD0;	 	
	TL0=0x03;	 	//10ms中断一次
	Time_10ms++;
	if(Time_10ms == 0xff)
		Time_10ms = 0;
}


void main(void)
{
	Sys_Clk_Set_PLL(PLL_Multiple);	//时钟初始化
	Timer1Init();					//timer1初始化
	BuzzerGPIOInit();				//蜂鸣器初始化
	KeyInit(KeyProcess);			//按键初始化		
	TM1640Init();					//1640初始化
	PoweroffMemoryInit(); 			//读数据空间
	
	FlowProcess();
	return;
}	


#endif

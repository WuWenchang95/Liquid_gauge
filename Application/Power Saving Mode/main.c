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
#include "../../Library/includes/wdt.h"
#include "../../Library/includes/uart.h"
#include "../../Library/includes/delay.h"

#include "TM1640.h"
#include "flowfunction.h"
#include "key.h"
#include "buzzer.h"

#include <intrins.h>

//uint8_t xdata Time_10ms;
uint8_t xdata Time_ms;

static uint16_t timets = 0;
static uint8_t  time_mst;
extern uint8_t tfru_zd;
extern uint8_t tfru_zdcnt;
extern uint8_t tfru_zdfg;


void Timer1Init(void)			//T1初始化	3.6864 MHZ/12*4       1个计数0.814us
{
 	TMOD = (TMOD&0xFF)|0x10;     //定时器1 模式1 16计数器  
	TH1=0xE8;TL1=0x02;	 	//     5ms     5,000 / 0.814 = 6142    65536-12285 = 59394
	//TH1=0xD0;TL1=0x03;	//10ms 	
	
	ET1=1;
	TR1=1;
	
}


void Timer1Zd(void) interrupt 3	   	//定时器0中断
{
	TH1=0xE8;TL1=0x02;			//5ms中断一次	

	//TH0=0xD0;	 TL0=0x03;	 	//10ms中断一次	
	
	Time_ms++;
	if(Time_ms == 0xff)
		Time_ms = 0;

}



void main(void)
{
//	uint8_t i;

	Sys_Clk_Set_PLL(PLL_Multiple);	//时钟初始化

	Timer1Init();					//timer1初始化
	BuzzerGPIOInit();				//蜂鸣器初始化
	KeyInit(KeyProcess);			//按键初始化		
	Uart0_Initial(UART0_BAUTRATE);          //串口0初始化
	TM1640Init();					//1640初始化
	
	GPIO_Init(P35F,INPUT); 	    //pwm
	GPIO_Init(P02F,OUTPUT);     //led
	GPIO_Init(P36F,OUTPUT);     //bump
	PUMP_OUT = PUMP_OFF;

	PoweroffMemoryInit();		//读数据空间
#ifdef PRINT_EN	
	uart_printf("main  test!\r\n");
#endif	
	//Delay_500ms(2);
	P02 = 0;
	//PUMP_OUT = PUMP_ON;
	WDT_init(WDTS_IRCH,WDRE_reset,0x708);      //设置看门狗时间为 1 秒
	EA = 1;
	while(1)
	{
		FlowProcess();

		WDT_FeedDog();
	}
	
	return;
}	


#endif

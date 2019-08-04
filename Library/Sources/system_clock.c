#ifndef _SYSTEM_CLOCK_C_
#define _SYSTEM_CLOCK_C_
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
#include <intrins.h>
/*********************************************************************************************************************/
#if 0
void Sys_Clk_Set_IRCH(void)
{
	CKCON |= IHCKE;
	CKSEL = (CKSEL&0xF8) | CKSEL_IRCH;		
}

void Sys_Clk_Set_IRCL(void)
{
	CKCON |= ILCKE;
	CKSEL = (CKSEL&0xF8) | CKSEL_IRCL;		
}
#endif

//void Sys_Clk_Set_XOSCL(void)
//{
//#if (IC_TYPE == IC_TYPE_CA51F3X)
//	GPIO_Init(P32F,P32_XOSCL_IN_SETTING);
//	GPIO_Init(P33F,P33_XOSCL_OUT_SETTING);
//#elif  (IC_TYPE == IC_TYPE_CA51F2X)
//	GPIO_Init(P71F,P71_XOSCL_OUT_SETTING);
//	GPIO_Init(P72F,P72_XOSCL_IN_SETTING);
//#endif
//	CKCON |= XLCKE;
//	while(!(CKCON & XLSTA));
//	CKSEL = (CKSEL&0xF8) | CKSEL_XOSCL;		
//}
#if 1
void Sys_Clk_Set_PLL(unsigned char Multiple)
{
	static bit NeedTrim = 1;
	unsigned char ck_bak;
	if(Multiple < 2 || Multiple > 8) return;
	if(NeedTrim)
	{
		ck_bak = CKCON;
		CKCON |= ILCKE|IHCKE|TFCKE;
		VCKDH = 1600/256;
		VCKDL = 1600%256;
		RCCON = MODE(2) | MSEX(0) | CKSS(8);
		while(RCCON&0xC0);	
		
		RCTAGH = RCMSLH;
		RCTAGL = RCMSLL;
		
		CKSEL = (CKSEL&0xF8) | CKSEL_TFRC;			
		RCCON = MODE(3) | MSEX(0) | CKSS(8);
		while(RCCON&0xC0);		
	}

	PLLCON = PLLON(1) | MULFT(Multiple-2);
	while(!(PLLCON & PLSTA));
	CKSEL = (CKSEL&0xF8) | CKSEL_PLL;		
	if(NeedTrim)
	{
		CKCON = ck_bak | IHCKE;
		NeedTrim = 0;
	}
}
#endif
//#if (IC_TYPE == IC_TYPE_CA51F2X)
//void Sys_Clk_Set_XOSCH(void)
//{
//	GPIO_Init(P74F,P74_XOSCH_IN_SETTING);
//	GPIO_Init(P73F,P73_XOSCH_OUT_SETTING);
//	CKCON |= XHCKE;
//	while(!(CKCON & XHSTA));
//	CKSEL = (CKSEL&0xF8) | CKSEL_XOSCH;		
//}
//#endif
//void Sys_Clk_Set_TFRC(void)
//{
//	CKCON |= TFCKE;
//	CKSEL = (CKSEL&0xF8) | CKSEL_TFRC;		
//}

/*********************************************************************************************************************/
#endif

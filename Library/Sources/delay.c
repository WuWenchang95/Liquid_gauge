#ifndef _Delay_C_
#define _Delay_C_
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
/*********************************************************************************************************************/
void Delay_50us(unsigned int n)	   
{
	unsigned char i;
#if (SYSCLK_SRC == PLL)
	n *= PLL_Multiple;
#endif 
	
	while(n--)
	{
		for(i=0;i<15;i++);
	}
}
void Delay_ms(unsigned int n)
{
	while(n--)
	{
		Delay_50us(20 * 4);
	}
	WDFLG = 0xA5;//Ë¢ÐÂ¿´ÃÅ¹·
}


void Delay_500ms(unsigned int n)
{
	while(n--)
	{
		Delay_ms(500);
		
	}
}


/*********************************************************************************************************************/
#endif
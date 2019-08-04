#ifndef _BUZZER_C_
#define _BUZZER_C_

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
#include "main.h"
#include <intrins.h>
#include "buzzer.h"


#define BUZZER_IO		P34F
#define BUZZER		    P34


#define BUZZER_ON()     (BUZZER = 1)
#define BUZZER_OFF()   	(BUZZER = 0)


BUZZER_MODE BuzzerFlag = NOT_RING;


void BuzzerGPIOInit(void)
{
	GPIO_Init(BUZZER_IO,OUTPUT|PU_EN);
	//BUZZER_ON();
}



void BuzzerCrl(void)
{
	static uint8_t time_count;
	static uint8_t buzzer_5ms;
	

	if((uint8_t)Time_ms != buzzer_5ms)
	{
		buzzer_5ms = (uint8_t)Time_ms;
		time_count++;
		switch(BuzzerFlag)
		{
			case NOT_RING:
				time_count = 0;
				BuzzerFlag = NOT_RING;
				BUZZER_OFF();		
				break;
			case RING_ONCE:
				if(time_count < 15)
				{
					BUZZER_ON();
				}
				else
				{
					time_count = 0;
					BuzzerFlag = NOT_RING;

					BUZZER_OFF();		

				}
				break;
			case RING_ONCE_LONG:
				if(time_count < 40)
				{

					BUZZER_ON();
				}
				else
				{
					time_count = 0;
					BuzzerFlag = NOT_RING;
					BUZZER_OFF();		

				}
				break;				
			case RING_TWICE:			//响两下
				if(time_count < 15)
				{
					BUZZER_ON();
				}
				else if(time_count < 30)
				{
					BUZZER_OFF();		
				}
				else if(time_count < 45)
				{
					BUZZER_ON();
				}
				else
				{
					time_count = 0;
					BuzzerFlag = NOT_RING;	
					BUZZER_OFF();		
				}
				
				break;	
				case RING_CONTINUOUS:			//响两下
					BUZZER_ON();

				break;	
			default:
				time_count = 0;
				BuzzerFlag = NOT_RING;
				BUZZER_OFF();		
				break;
		}

	}

}

void SetBuzzer(BUZZER_MODE buzzermode)
{
	BuzzerFlag = buzzermode;
}

#endif


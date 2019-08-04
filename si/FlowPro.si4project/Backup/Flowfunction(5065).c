#ifndef FLOW_FUNCTION_C
#define FLOW_FUNCTION_C

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
#include "flowfunction.h"
#include "key.h"
//#include "../../User/buzzer.h"
#include "TM1640.h"

#define  MAX_RATION_VALUE      999
#define  MIN_RATION_VALUE		0
#define  UNIT_DISCHARGE         2


FlowInfo_t idata FlowSt;
uint8_t idata DispalyDigitalBuf[5];
#if 1
void KeyProcess(void)
{
	uint8_t keynum;

	for(keynum = 0; keynum < 8; keynum++)
	{
	    if(CheckKey(keynum) == SHORT_PRESS)
        {
  //      	ResetKey(keynum);
			if(!FlowSt.setMode)
			{
				if(FlowSt.stepRevolve == PUMP_STOP)
				{
					FlowSt.dRationNum = keynum+1;
					FlowSt.dRationValue = FlowSt.rationValue[keynum];
					FlowSt.t_dRationValue = FlowSt.dRationValue; 
					FlowSt.stepRevolve = PUMP_READY;
					FlowSt.readyCountDown = 50;
				}
				else
				{
					FlowSt.stepRevolve = PUMP_STOP;
				}
			}
			else
			{	//保存
				if(FlowSt.dRationNum == keynum+1)
				{
					FlowSt.rationValue[keynum]=FlowSt.dRationValue;
				}
			}

        }   
		else if(CheckKey(keynum) == TWICE_PRESS)
		{
	//		ResetKey(keynum);
			FlowSt.dRationNum = keynum+1;
			FlowSt.dRationValue = FlowSt.rationValue[keynum] * 2;
			FlowSt.t_dRationValue = FlowSt.dRationValue; 
			FlowSt.stepRevolve = PUMP_READY;
		}
		else if(CheckKey(keynum) == LONG_PRESS)
		{
	//		ResetKey(keynum);
			FlowSt.dRationNum = keynum+1;
			FlowSt.dRationValue = FlowSt.rationValue[keynum];
			FlowSt.t_dRationValue = FlowSt.dRationValue; 
			//FlowSt.stepRevolve = PUMP_READY;
			FlowSt.setMode = 1;
		}
	}

	if(FlowSt.setMode)
	{
		if(CheckKey(KEY_UP) == SHORT_PRESS)
		{
	//		ResetKey(KEY_UP);
			FlowSt.dRationValue++; 
			if(FlowSt.dRationValue > MAX_RATION_VALUE)
			{
				FlowSt.dRationValue = MIN_RATION_VALUE;
			}
		}
		else if(CheckKey(KEY_UP) == TWICE_PRESS)
		{
		//	ResetKey(KEY_UP);
			FlowSt.dRationValue = FlowSt.dRationValue+2;
			if(FlowSt.dRationValue > MAX_RATION_VALUE)
			{
				FlowSt.dRationValue = MIN_RATION_VALUE;
			}	
		}
		
		if(CheckKey(KEY_DOWN) == SHORT_PRESS)
		{
	
		//	ResetKey(KEY_DOWN);
			FlowSt.dRationValue--;
			if(FlowSt.dRationValue < MIN_RATION_VALUE)
			{
				FlowSt.dRationValue = MAX_RATION_VALUE;
			}	
		}
		else if(CheckKey(KEY_DOWN) == TWICE_PRESS)
		{
		//	ResetKey(KEY_UP);
			FlowSt.dRationValue = FlowSt.dRationValue-2;
			if(FlowSt.dRationValue < MIN_RATION_VALUE)
			{
				FlowSt.dRationValue = MAX_RATION_VALUE;
			}	
		}
	}
	
}
#endif	
void DispalyDigital(void)
{
	static uint8_t digitalFlushTime;
	static uint8_t digitalFlushTime_t;
	
	if(digitalFlushTime_t != (uint8_t)Time_10ms)	
	{
		digitalFlushTime_t = (uint8_t)Time_10ms;
		digitalFlushTime++;
		if(digitalFlushTime >= 5)
		{
			digitalFlushTime = 0;
			DispalyDigitalBuf[0] = table_YIN[FlowSt.dRationValue/1000];
			DispalyDigitalBuf[1] = table_YIN[FlowSt.dRationValue%1000/100];
			DispalyDigitalBuf[2] = table_YIN[FlowSt.dRationValue%100/10];
			DispalyDigitalBuf[3] = table_YIN[FlowSt.dRationValue%10];
			DispalyDigitalBuf[4] = table_YIN[FlowSt.dRationNum];
			TM1640_SendData(0, DispalyDigitalBuf, 5);
		}
			
	}
	
}

void RevolveProcess(void)
{
	static uint8_t ReadyCount_t;
	switch(FlowSt.stepRevolve)
	{
		case PUMP_STOP:
				PUMP_OUT = PUMP_OFF;
			break;
		case PUMP_READY:
			if(ReadyCount_t != (uint8_t)Time_10ms)
			{
				ReadyCount_t = (uint8_t)Time_10ms;
				FlowSt.readyCountDown--;
				if(FlowSt.readyCountDown == 0)
				{
					FlowSt.stepRevolve = PUMP_OPEN;
					
					//启动定时器 定时器计数。每记一次水泵的流量一定，总流量除以单位流量等于总时间。计时记到总时间水泵停止。
				}
			}
			break;
		case PUMP_OPEN:
			{
				uint16_t FlowValue_t;
				
				FlowValue_t = FlowSt.timeCount*UNIT_DISCHARGE;			//总流量与时间计数的换算
				
				FlowSt.dRationValue = FlowSt.t_dRationValue - FlowSt.timeCount*UNIT_DISCHARGE;

				
				if(FlowSt.timeCount == FlowSt.targetTimeCount - 2)
				{
					//停止定时器计数 
					FlowSt.stepRevolve = PUMP_STOP;
					FlowSt.dRationValue = 0;
				}

				
			}

			break;

	}
}


void FlowProcess(void)
{
	
	AllKeyScan();
	RevolveProcess();
	DispalyDigital();
	BuzzerCrl();

}

#endif


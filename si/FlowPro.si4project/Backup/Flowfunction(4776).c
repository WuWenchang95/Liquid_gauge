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
#include "../../Library/includes/flash.h"
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

void WriteRationValueToMemory(void)
{
	uint16_t DataAreaBuf[10] = {0};
	
	DataAreaBuf[0] = 0xAA55;
	
	DataAreaBuf[1] = FlowSt.rationValue[0];
	DataAreaBuf[2] = FlowSt.rationValue[1];
	DataAreaBuf[3] = FlowSt.rationValue[2];
	DataAreaBuf[4] = FlowSt.rationValue[3];
	DataAreaBuf[5] = FlowSt.rationValue[4];
	DataAreaBuf[6] = FlowSt.rationValue[5];
	DataAreaBuf[7] = FlowSt.rationValue[6];
	DataAreaBuf[8] = FlowSt.rationValue[7];
	DataAreaBuf[9] = (uint16_t)FlowSt.dRationNum;

	Data_Area_Sector_Erase(0);				//擦除0号扇区
	Data_Area_Mass_Write(0,DataAreaBuf,20);	//写入数据
	
}

void ReadRationValueToMemory(void)
{
	uint16_t  DataAreaBuf[10];

	Data_Area_Mass_Read(0,DataAreaBuf,20);	//读出保存的数据
	FlowSt.rationValue[0] = DataAreaBuf[2];			//装载数据
	FlowSt.rationValue[1] = DataAreaBuf[3];
	FlowSt.rationValue[2] = DataAreaBuf[4];
	FlowSt.rationValue[3] = DataAreaBuf[5];
	FlowSt.rationValue[4] = DataAreaBuf[6];
	FlowSt.rationValue[5] = DataAreaBuf[7];
	FlowSt.rationValue[6] = DataAreaBuf[8];
	FlowSt.rationValue[7] = DataAreaBuf[9];
	FlowSt.rationValue[8] = DataAreaBuf[10];
	FlowSt.rationValue[9] = DataAreaBuf[11];
}

void PoweroffMemory_init(void)
{
	unsigned char  DataAreaBuf[2];
	PADRD = FLASH_SIZE*4 - 1;								//划分数据区，将flash的最后一块（256字节）为数据空间
	Data_Area_Mass_Read(0,DataAreaBuf,2);		//读标志位
	if((DataAreaBuf[0] != 0xAA)||(DataAreaBuf[1] != 0x55))//如果不是x0AA55为初次上电，需要初始化变量存入数据空间
	{
		FlowSt.rationValue[0] = 5; 		//装载数据
		FlowSt.rationValue[1] = 10;
		FlowSt.rationValue[2] = 15;
		FlowSt.rationValue[3] = 20;
		FlowSt.rationValue[4] = 25;
		FlowSt.rationValue[5] = 30;
		FlowSt.rationValue[6] = 35;
		FlowSt.rationValue[7] = 40;
		FlowSt.dRationNum = 1;

		WriteRationValueToMemory();				//写入状态字和变量到数据空间
	}
	else
	{
		ReadRationValueToMemory();				//不是初次上电，读出在数据空间的值
	}
}



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
				WriteRationValueToMemory();
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


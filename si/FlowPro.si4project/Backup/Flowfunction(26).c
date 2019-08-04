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
#include "buzzer.h"
#include "TM1640.h"

#define  MAX_RATION_VALUE      999
#define  MIN_RATION_VALUE		0
#define  UNIT_FLOWPUMP       	2			  //水泵单位时间的流量
#define  UNIT_FLOWMETER         2			  //转子单位计数的流量


FlowInfo_t idata FlowSt;						
uint8_t idata DispalyDigitalBuf[5] = {0,0,0,0,0};


/**************************************************************
**函数名称：WriteRationValueToMemory()
**函数描述：向数据区写入值
**入口参数：无
**出口参数：无
***************************************************************/
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

/**************************************************************
**函数名称：ReadRationValueToMemory()
**函数描述：向数据区读出保存的数据
**入口参数：无
**出口参数：无
***************************************************************/
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

/**************************************************************
**函数名称：PoweroffMemoryInit()
**函数描述：掉电保存数据初始化
**入口参数：无
**出口参数：无
***************************************************************/
void PoweroffMemoryInit(void)
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


/**************************************************************
**函数名称：DispalyDigital()
**函数描述：数码管显示过程
**入口参数：无
**出口参数：无
***************************************************************/
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

/**************************************************************
**函数名称：Timer0Init(uint8_t      isStart)
**函数描述：定时器0初始化，100us中断   
**入口参数：isStart      是否开启定时器中断   1开启   0关闭
**出口参数：无
***************************************************************/
void Timer0Init(uint8_t      isStart)			//T0初始化	3.6864 MHZ/12*4        1个计数0.8138us
{
	//TMOD=0x11;	
 	TMOD = (TMOD&0xFF)|0x03;   //定时器0 模式3   8计数器  
	TH0=0;	 	      //  100us        100/0.8138  = 122.88 
	TL0=0x85;			  //256-123;	 	
	if(isStart)
	{
		ET0=1;			//允许定时器/计数器0 中断
		TR0=1;			//启动定时器/计数器0 中断
	}
	else
	{
		ET0=0;			//不允许定时器/计数器0 中断
		TR0=0;			//停止时器/计数器0 中断
	}

}

/**************************************************************
**函数名称：Timer0Zd
**函数描述：定时器0中断   
**入口参数：无
**出口参数：无
***************************************************************/
void Timer0Zd(void) interrupt 1	   	//定时器0中断
{
	FlowSt.pumpTimeCount++;
}

/**************************************************************
**函数名称：INT0Init
**函数描述：外部0中初始化   
**入口参数：isStart      是否开启中断          1开启   0关闭
**出口参数：无
***************************************************************/
void INT0Init(uint8_t     isStart)	//开机
{
//	P37F = 1; //设置P37为输入引脚
	IT0CON = 31;

	if(isStart)
	{
		EX0 =1; 	//INT0中断使能
		IE0 =1; 	//外部中断使能
		
	}
	else
	{
		EX0 =0; 	//INT0中断失能
		IE0 =0; 	//外部中断失能
	}

	IT0 =1;
	PX0 =1;	

}

/**************************************************************
**函数名称：INT0Zd
**函数描述：外部0中断  转子的外部中断计数 
**入口参数：无
**出口参数：无
***************************************************************/
void INT0Zd(void) interrupt 0
{
	WDFLG = 0xA5;//刷新看门狗
	//中断  转子流量计计数
	FlowSt.flowCount++;
}	

#if 1

/**************************************************************
**函数名称：KeyProcess
**函数描述：按键处理过程   
**入口参数：无
**出口参数：无
***************************************************************/
void KeyProcess(void)
{
	uint8_t keynum;

	for(keynum = 0; keynum < 8; keynum++)
	{
	    if(CheckKey(keynum) == SHORT_PRESS)
        {
  //      	ResetKey(keynum);
			if(!FlowSt.setMode)      //不是设置模式
			{
				if(FlowSt.stepRevolve == PUMP_STOP)     //开始抽水
				{
					FlowSt.dRationNum = keynum+1;
					FlowSt.dRationValue = FlowSt.rationValue[keynum];
					FlowSt.t_dRationValue = FlowSt.dRationValue; 

					FlowSt.tpumpTimeCount = FlowSt.t_dRationValue/UNIT_FLOWPUMP; //计算水泵开始的目标时间
					FlowSt.tFlowCount = FlowSt.t_dRationValue/UNIT_FLOWMETER;	 //计算流量计的目标计数值
	
					FlowSt.stepRevolve = PUMP_READY;
					FlowSt.readyCountDown = 50;
				}
				else					//设置模式
				{
					FlowSt.stepRevolve = PUMP_STOP;   //停止抽水
				}
			}
			else					//设置模式
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
					FlowSt.pumpTimeCount = 0;
					FlowSt.flowCount = 0;

					Timer0Init(1);  //开启定时器0中断
					INT0Init(1);	//开启外部中断0中断

				}
			}
			break;
		case PUMP_OPEN:
			{
				uint16_t FlowValue_t;
				
				FlowValue_t = FlowSt.pumpTimeCount*UNIT_FLOWPUMP;			//总流量与时间计数的换算
				
				FlowSt.dRationValue = FlowSt.t_dRationValue - FlowSt.pumpTimeCount*UNIT_FLOWPUMP;

				
				if(FlowSt.pumpTimeCount == FlowSt.tpumpTimeCount - 2)
				{
					//停止定时器计数 
					
					FlowSt.stepRevolve = PUMP_STOP;
					FlowSt.dRationValue = 0;
					Timer0Init(0);
					INT0Init(0);
				}

				
			}

			break;

	}
}


void FlowProcess(void)
{
	
	AllKeyScan();
	KeyProcess();
	RevolveProcess();
	DispalyDigital();
	BuzzerCrl();
	SetBuzzer(RING_ONCE);
}

#endif


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
#include "../../Library/includes/uart.h"
#include "main.h"
#include <intrins.h>
#include "flowfunction.h"
#include "key.h"
#include "buzzer.h"
#include "TM1640.h"

//100ml  1872,
//水泵555   5s,270ml.  10s,510ml.   15s,800ml.  20s, 1010ml    //水泵的时间    0.0534ml/ms
//水泵545   5s,195ml.  10s,380ml.   15s,540ml.  20s, 720ml    //水泵的时间    0.0370ml/ms

//100ml，218  200ml，240    300ml，613  400ml ，481  500ml，602
//2  100ml，208/204/278/275/278/268     200ml，/539/545/538/534  290,766/790/771/774  300ml，804/804  304,804/821   400ml ，1011  500ml，602

//micro   100,652/670  200,1130/938  300,1581/1707/1936/1819/1937/1949/1908/1907/1774   600,3302
//水泵555 2200ml每分钟，2200ml/60000ms = 0.036667ml/ms  以1ms为计数单位


#define  MAX_RATION_VALUE       (uint32_t)9999
#define  MIN_RATION_VALUE		(uint32_t)0
#define  UNIT_FLOWPUMP       	(double)0.0534  //0.0370   //0.037			  //水泵单位时间的流量
#define  UNIT_FLOWMETER1        (double)3.285	//3.280  //3.66 //2.900  //6.5    //0.383  ///0.83		 			 //转子单位计数的流量
#define  UNIT_FLOWMETER2        (double)3.250   //3.335  //3.66 //2.900  //6.5    //0.383  ///0.83		 			 //转子单位计数的流量

#define  RATION_STOP_CNT        100	 

FlowInfo_t  FlowSt = {0};						
uint8_t idata DispalyDigitalBuf[5] = {0,0,0,0,0};
uint8_t rationStopTimeCnt = 0;

double  unitFlow_t;


uint8_t tfru_zd;
uint8_t tfru_zdcnt;
uint8_t tfru_zdfg = 0;


typedef struct 
{
	uint8_t higtCnt;
	uint8_t lowCnt;
	uint8_t ttl;
	uint8_t curTtl;
	uint8_t perTtl;
	uint8_t flipFg;
}sPulse;

sPulse idata tPulse = {0};

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
	Data_Area_Mass_Write(0,(unsigned char *)DataAreaBuf,20);	//写入数据	
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

	Data_Area_Mass_Read(0,(unsigned char *)DataAreaBuf,20);	//读出保存的数据
	FlowSt.rationValue[0] = DataAreaBuf[1];			//装载数据
	FlowSt.rationValue[1] = DataAreaBuf[2];
	FlowSt.rationValue[2] = DataAreaBuf[3];
	FlowSt.rationValue[3] = DataAreaBuf[4];
	FlowSt.rationValue[4] = DataAreaBuf[5];
	FlowSt.rationValue[5] = DataAreaBuf[6];
	FlowSt.rationValue[6] = DataAreaBuf[7];
	FlowSt.rationValue[7] = DataAreaBuf[8];
	
	FlowSt.dRationNum = DataAreaBuf[9]&0xff;

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
#if 1
		FlowSt.rationValue[0] = 5; 		//装载数据
		FlowSt.rationValue[1] = 15;
		FlowSt.rationValue[2] = 20;
		FlowSt.rationValue[3] = 25;
		FlowSt.rationValue[4] = 100;
		FlowSt.rationValue[5] = 300;
		FlowSt.rationValue[6] = 500;
		FlowSt.rationValue[7] = 1000;
#else
		FlowSt.rationValue[0] = 5; 		//装载数据
		FlowSt.rationValue[1] = 10;
		FlowSt.rationValue[2] = 15;
		FlowSt.rationValue[3] = 20;
		FlowSt.rationValue[4] = 25;
		FlowSt.rationValue[5] = 30;
		FlowSt.rationValue[6] = 35;
		FlowSt.rationValue[7] = 40;
#endif
		FlowSt.dRationNum = 1;

		WriteRationValueToMemory();				//写入状态字和变量到数据空间
	}
	else
	{
		ReadRationValueToMemory();				//不是初次上电，读出在数据空间的值
	}

	FlowSt.dRationValue = FlowSt.rationValue[FlowSt.dRationNum-1];
}




/**************************************************************
**函数名称：Timer0Init(uint8_t      isStart)
**函数描述：定时器0初始化，100us中断   
**入口参数：isStart      是否开启定时器中断   1开启   0关闭
**出口参数：无
***************************************************************/
void Timer0Init(uint8_t      isStart)			//T0初始化	3.6864 MHZ/12*4        1个计数0.8138us
{
	if(isStart)
	{
		//TMOD=0x11;
	 	//TMOD = (TMOD&0xFF)|0x03;   //定时器0 模式3   8计数器  
		//TH0=0;	 	             //  100us        100/0.8138  = 122.88 
		//TL0=0x85;			  //256-123;		


		TMOD = (TMOD&0xFF)|0x01;   //定时器0 模式1   16计数器  
		//TH0=0xF6;	 	           // 2ms        2000/0.8138  = 2458
		//TL0=0x66;			  	   //65536 - 2458 = 63078;	
		TH0=0xFB;	 	           // 1ms        1000/0.8138  = 1228.8
		TL0=0x33;			       //65536 - 1229 = 64307;	

//		tfru_zdcnt = 0;
//		tfru_zdfg = 0;
		
		ET0=1;			//允许定时器/计数器0 中断
		TR0=1;			//启动定时器/计数器0 中断
	}
	else
	{
		ET0=0;			//不允许定时器/计数器0 中断
		TR0=0;			//停止时器/计数器0 中断
	}
	
}

#define FRUECNT  50

/**************************************************************
**函数名称：Timer0Zd
**函数描述：定时器0中断   
**入口参数：无
**出口参数：无
***************************************************************/
void Timer0Zd(void) interrupt 1	   	//定时器0中断
{

	FlowSt.pumpTimeCount++;
	//TH0=0xF6;					 // 2ms 	   2000/0.8138	= 2458
	//TL0=0x66;			  //65536 - 2458 = 63078;	
	TH0=0xFB;				   // 1ms		 1000/0.8138  = 1228.8
	TL0=0x33;				   //65536 - 1229 = 64307;	

#if 1
	if(P35 == 1)
	{
		if((tPulse.perTtl == 0)&&(tPulse.ttl == 1))
		{
			if(tPulse.lowCnt < 3)
			{
				tPulse.lowCnt = 0;
			}
		}
		
		if(tPulse.ttl == 0) 
		{
			tPulse.higtCnt++;
			if(tPulse.higtCnt >= 30)
			{
				tPulse.higtCnt = 0;
				tPulse.ttl = 1;
				FlowSt.flowCount++;
			}
		}
		
		tPulse.perTtl = 1;
	}
	else
	{
		if((tPulse.perTtl == 1)&&(tPulse.ttl == 0))
		{
			if(tPulse.higtCnt < 3)
			{
				tPulse.higtCnt = 0;
			}
		}

		if(tPulse.ttl == 1)	
		{		
			tPulse.lowCnt++;
			if(tPulse.lowCnt >= 30)
			{
				tPulse.lowCnt = 0;
				tPulse.ttl = 0;
				FlowSt.flowCount++;
			}
		}

		tPulse.perTtl = 0;
	}
#endif	
}

/**************************************************************
**函数名称：INT0Init
**函数描述：外部0中初始化   
**入口参数：isStart      是否开启中断          1开启   0关闭
**出口参数：无
***************************************************************/
void INT0Init(uint8_t     isStart)	//开机
{

	if(isStart)
	{
		P35F = 1; //设置P35为输入引脚
		IT0CON = 29;

		EX0 =1; 	//INT0中断使能
		IE0 =1; 	//外部中断使能
		IT0 = 0; //设置为下降沿中断
		//PX0 = 1; //设置 INT0 为高优先级
		tfru_zdcnt = 0;	
		tfru_zdfg = 0;
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
	//中断  转子流量计计数
#if 0
	//if(tfru_zd)
	{
	
		EX0 =0; 	//INT0中断失能
		IE0 =0; 	//外部中断失能
		
//		tfru_zd = 0;
//		tfru_zdcnt = 0;
		FlowSt.flowCount++;

		if(IT0)
		{
			IT0= 0; //设置为下降沿中断
			tfru_zdfg = 0;
		} 
		else
		{
			IT0= 1;
			tfru_zdfg = 1;
		}
			

		EX0 =1; 	//INT0中断使能
		IE0 =1; 	//外部中断使能

	}
#endif
}	

void UnitFlowCalculate(void)
{
	if(FlowSt.dRationValue <= 300)
	{
		unitFlow_t = UNIT_FLOWMETER1;
		//FlowSt.tFlowCount = FlowSt.tFlowCount+1;
	}		
	else
	{
		unitFlow_t = UNIT_FLOWMETER2;
		
	}
		
}
void tFlowCalculate(void)
{
	if(FlowSt.dRationValue <= 50)
	{
		FlowSt.tFlowCount = FlowSt.tFlowCount-1;
	}
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
	static uint8_t tkeycnt = 0;
	
	for(keynum = 0; keynum < 8; keynum++)
	{
	    if(CheckKey(keynum) == SHORT_PRESS)
        {
#ifdef PRINT_EN	
			uart_printf("KEY %d short press! \r\n",keynum+1);
#endif		
  //      	ResetKey(keynum);
			if(!FlowSt.setMode)      //不是设置模式
			{
				if(FlowSt.stepRevolve == PUMP_STOP)     //开始抽水
				{
					FlowSt.dRationNum = keynum+1;
					FlowSt.dRationValue = FlowSt.rationValue[keynum];
					//UnitFlowCalculate();
					FlowSt.t_dRationValue = (uint32_t)(FlowSt.dRationValue); 
					Timer0Init(0);
					INT0Init(0);  
					UnitFlowCalculate();
					//FlowSt.tpumpTimeCount = (uint32_t)(((double)FlowSt.t_dRationValue/UNIT_FLOWPUMP)+0.500); //计算水泵开始的目标时间
					FlowSt.tFlowCount = (uint32_t)(((double)((double)FlowSt.t_dRationValue)/unitFlow_t)+0.500);	 //计算流量计的目标计数值
					tFlowCalculate();
					FlowSt.stepRevolve = PUMP_READY;
					FlowSt.readyCountDown = 100;

					WriteRationValueToMemory();
					
#ifdef PRINT_EN
					uart_printf("Num:%u,RationValue:%lu,zCnt:%lu \r\n",FlowSt.dRationNum,FlowSt.dRationValue,FlowSt.tFlowCount);
#endif
				}
				else
				{
					FlowSt.stepRevolve = PUMP_STOP;   //停止抽水
					rationStopTimeCnt = RATION_STOP_CNT;
				}
			}
			else					//设置模式
			{	//保存
				if(FlowSt.dRationNum == keynum+1)
				{
					FlowSt.rationValue[keynum]=FlowSt.dRationValue;
				}
				WriteRationValueToMemory();
				FlowSt.setMode = 0;
			}

        }   
		else if(CheckKey(keynum) == TWICE_PRESS)
		{
	//		ResetKey(keynum);
#ifdef PRINT_EN		
			uart_printf("KEY %d twice press! \r\n",keynum+1);
#endif
			if(!FlowSt.setMode) 	 //不是设置模式
			{
				if(FlowSt.stepRevolve == PUMP_STOP) 	//开始抽水
				{
					FlowSt.dRationNum = keynum+1;
					FlowSt.dRationValue = FlowSt.rationValue[keynum]*2;
					//UnitFlowCalculate();
					FlowSt.t_dRationValue = (uint32_t)(FlowSt.dRationValue); 
					Timer0Init(0);
					INT0Init(0);
					UnitFlowCalculate();
					//FlowSt.tpumpTimeCount = (uint32_t)(((double)FlowSt.t_dRationValue/UNIT_FLOWPUMP)+0.5); //计算水泵开始的目标时间
					FlowSt.tFlowCount = (uint32_t)(((double)FlowSt.t_dRationValue/unitFlow_t)+0.5);	 //计算流量计的目标计数值
					tFlowCalculate();
					FlowSt.stepRevolve = PUMP_READY;
					FlowSt.readyCountDown = 100;
					
					WriteRationValueToMemory();
#ifdef PRINT_EN				
					uart_printf("Num: %u,RationValue: %lu, zCnt: %lu \r\n",FlowSt.dRationNum,FlowSt.dRationValue,FlowSt.tFlowCount);
#endif
				}
				else					
				{
					FlowSt.stepRevolve = PUMP_STOP;   //停止抽水
					rationStopTimeCnt = RATION_STOP_CNT;
				}
			}

		}
		
		else if(CheckKey(keynum) == LONG_PRESS)
		{
	//		ResetKey(keynum);
#ifdef PRINT_EN			
			uart_printf("KEY %d long press! \r\n",keynum+1);
#endif
			if(FlowSt.stepRevolve == PUMP_STOP)     //进入设置模式
			{
				FlowSt.dRationNum = keynum+1;
				FlowSt.dRationValue = FlowSt.rationValue[keynum];
				FlowSt.t_dRationValue = FlowSt.dRationValue; 
				FlowSt.stepRevolve = PUMP_STOP;
				FlowSt.setMode = 1;
			}
			else
			{
				FlowSt.stepRevolve = PUMP_STOP;
				rationStopTimeCnt = RATION_STOP_CNT;
			}
		}
	}

	if(FlowSt.setMode)
	{
		if(CheckKey(KEY_UP) == SHORT_PRESS)
		{
#ifdef PRINT_EN		
			uart_printf("KEY_UP short press! \r\n");
#endif
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
		else if(CheckKey(KEY_UP) == THREE_PRESS)
		{
		//	ResetKey(KEY_UP);
			FlowSt.dRationValue = FlowSt.dRationValue+3;
			if(FlowSt.dRationValue > MAX_RATION_VALUE)
			{
				FlowSt.dRationValue = MIN_RATION_VALUE;
			}	
		}		
		else if(IsLongPress(KEY_UP))
		{
			tkeycnt++;
			if(tkeycnt > 20)
			{
				tkeycnt = 0;
#ifdef PRINT_EN						
                uart_printf("KEY_UP Long press! \r\n");
#endif
        //      ResetKey(KEY_UP);
                FlowSt.dRationValue++; 
                if(FlowSt.dRationValue > MAX_RATION_VALUE)
                {
                    FlowSt.dRationValue = MIN_RATION_VALUE;
                }   

			}
		
		}
		
		if(CheckKey(KEY_DOWN) == SHORT_PRESS)
		{
#ifdef PRINT_EN		
			uart_printf("KEY_DOWN short press! \r\n");
#endif
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
		else if(CheckKey(KEY_DOWN) == THREE_PRESS)
		{
		//	ResetKey(KEY_UP);
			FlowSt.dRationValue = FlowSt.dRationValue-3;
			if(FlowSt.dRationValue < MIN_RATION_VALUE)
			{
				FlowSt.dRationValue = MAX_RATION_VALUE;
			}	
		}		
		else if(IsLongPress(KEY_DOWN))
		{
			tkeycnt++;
			if(tkeycnt > 20)
			{
				tkeycnt = 0;
#ifdef PRINT_EN					
                uart_printf("KEY_UP Long press! \r\n");
#endif
        //      ResetKey(KEY_UP);
                FlowSt.dRationValue--; 
                if(FlowSt.dRationValue > MAX_RATION_VALUE)
                {
                    FlowSt.dRationValue = MIN_RATION_VALUE;
                }   

			}
		
		}		
	
	}
	else
	{
        tkeycnt = 0;
	}
	
}
#endif	

void ResetPulse(void)
{
	tPulse.curTtl = 0;
	tPulse.flipFg = 0;
	tPulse.higtCnt = 0;
	tPulse.lowCnt = 0;
	tPulse.perTtl = 0;
	tPulse.ttl = 0;
}


void RevolveProcess(void)
{
	static uint8_t ReadyCount_t;
	switch(FlowSt.stepRevolve)
	{
		case PUMP_STOP:
				PUMP_OUT = PUMP_OFF;
				Timer0Init(0);
				INT0Init(0);
			break;
		case PUMP_READY:
			if(ReadyCount_t != (uint8_t)Time_ms)
			{
				ReadyCount_t = (uint8_t)Time_ms;
				FlowSt.readyCountDown--;
				if(FlowSt.readyCountDown == 0)
				{
					FlowSt.stepRevolve = PUMP_OPEN;
					
					//启动定时器 定时器计数。每记一次水泵的流量一定，总流量除以单位流量等于总时间。计时记到总时间水泵停止。
					FlowSt.pumpTimeCount = 0;
					FlowSt.flowCount = 0;
					Timer0Init(1);  //开启定时器0中断
					INT0Init(0);	//开启外部中断0中断	
					ResetPulse();
					PUMP_OUT = PUMP_ON;
#ifdef PRINT_EN						
					uart_printf("PUMP_OPEN!ziCnt:%lu\r\n",FlowSt.flowCount);
#endif
				}
			}
			break;
		case PUMP_OPEN:
			{
				uint32_t FlowValue_t;

				FlowValue_t = (uint32_t)((double)FlowSt.flowCount*unitFlow_t);    //总流量与流量计计数的换算
				//FlowValue_t = (uint32_t)((double)FlowSt.pumpTimeCount*UNIT_FLOWPUMP);	  //总流量与时间计数的换算

				if(FlowValue_t > FlowSt.t_dRationValue) FlowValue_t = FlowSt.t_dRationValue;

				FlowSt.dRationValue = (uint16_t)(FlowSt.t_dRationValue - FlowValue_t);

				//if((FlowSt.pumpTimeCount >= (FlowSt.tpumpTimeCount)) &&(FlowSt.flowCount >= FlowSt.tFlowCount))
				//if(FlowSt.pumpTimeCount >= (FlowSt.tpumpTimeCount))
				
					
				if((FlowSt.flowCount >= (FlowSt.tFlowCount)))
				{
					//停止定时器计数 
					PUMP_OUT = PUMP_OFF;
					Timer0Init(0);
					
					INT0Init(0);					
					FlowSt.stepRevolve = PUMP_STOP;
					FlowSt.dRationValue = 0;
#ifdef PRINT_EN						
					uart_printf("ST!zCnt:%lu,pTC:%lu,pCnt:%lu\r\n",FlowSt.flowCount,FlowSt.pumpTimeCount,FlowSt.tpumpTimeCount);
#endif
					rationStopTimeCnt = RATION_STOP_CNT;
				}
				
			}

			break;
		default:
			break;
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
	static uint8_t setModeDigitalFlushTime_t;
	static uint8_t destroyfg = 0;
	
	if(digitalFlushTime_t != (uint8_t)Time_ms)	
	{
		digitalFlushTime_t = (uint8_t)Time_ms;
		digitalFlushTime++;
		
		if(rationStopTimeCnt > 0)
		{
			rationStopTimeCnt--;
		}
		
		if(digitalFlushTime >= 20)
		{
			digitalFlushTime = 0;
			if(FlowSt.setMode)					// 在设置模式下，定量号要闪烁
			{
                setModeDigitalFlushTime_t++;
                if(setModeDigitalFlushTime_t > 5)
                {
                    setModeDigitalFlushTime_t = 0;
                    if(destroyfg)
                    {
                        destroyfg = 0;
                    }
                    else
                    {
                        destroyfg = 1;
                    }
                }
			}
			else
			{
                destroyfg = 0;
				if((rationStopTimeCnt == 0) && (	FlowSt.stepRevolve == PUMP_STOP))
				{
					FlowSt.dRationValue = FlowSt.rationValue[FlowSt.dRationNum-1];
				}				
			}

			DispalyDigitalBuf[1] = table_YIN[FlowSt.dRationValue/1000];
			DispalyDigitalBuf[2] = table_YIN[FlowSt.dRationValue%1000/100];
			DispalyDigitalBuf[3] = table_YIN[FlowSt.dRationValue%100/10];
			DispalyDigitalBuf[0] = table_YIN[FlowSt.dRationValue%10];
			if(destroyfg)
			{
                DispalyDigitalBuf[4] = table_YIN[10];
			}
			else
			{
                DispalyDigitalBuf[4] = table_YIN[FlowSt.dRationNum];
			}
			TM1640_SendData(0, DispalyDigitalBuf, 5);
		}
			
	}
	
}

void FlowProcess(void)
{

	AllKeyScan();
	//KeyProcess();
	RevolveProcess();
	DispalyDigital();
	BuzzerCrl();

}

#endif


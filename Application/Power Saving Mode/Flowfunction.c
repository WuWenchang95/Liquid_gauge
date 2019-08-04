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
//ˮ��555   5s,270ml.  10s,510ml.   15s,800ml.  20s, 1010ml    //ˮ�õ�ʱ��    0.0534ml/ms
//ˮ��545   5s,195ml.  10s,380ml.   15s,540ml.  20s, 720ml    //ˮ�õ�ʱ��    0.0370ml/ms

//100ml��218  200ml��240    300ml��613  400ml ��481  500ml��602
//2  100ml��208/204/278/275/278/268     200ml��/539/545/538/534  290,766/790/771/774  300ml��804/804  304,804/821   400ml ��1011  500ml��602

//micro   100,652/670  200,1130/938  300,1581/1707/1936/1819/1937/1949/1908/1907/1774   600,3302
//ˮ��555 2200mlÿ���ӣ�2200ml/60000ms = 0.036667ml/ms  ��1msΪ������λ


#define  MAX_RATION_VALUE       (uint32_t)9999
#define  MIN_RATION_VALUE		(uint32_t)0
#define  UNIT_FLOWPUMP       	(double)0.0534  //0.0370   //0.037			  //ˮ�õ�λʱ�������
#define  UNIT_FLOWMETER1        (double)3.285	//3.280  //3.66 //2.900  //6.5    //0.383  ///0.83		 			 //ת�ӵ�λ����������
#define  UNIT_FLOWMETER2        (double)3.250   //3.335  //3.66 //2.900  //6.5    //0.383  ///0.83		 			 //ת�ӵ�λ����������

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
**�������ƣ�WriteRationValueToMemory()
**������������������д��ֵ
**��ڲ�������
**���ڲ�������
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

	Data_Area_Sector_Erase(0);				//����0������
	Data_Area_Mass_Write(0,(unsigned char *)DataAreaBuf,20);	//д������	
}

/**************************************************************
**�������ƣ�ReadRationValueToMemory()
**�������������������������������
**��ڲ�������
**���ڲ�������
***************************************************************/
void ReadRationValueToMemory(void)
{
	uint16_t  DataAreaBuf[10];

	Data_Area_Mass_Read(0,(unsigned char *)DataAreaBuf,20);	//�������������
	FlowSt.rationValue[0] = DataAreaBuf[1];			//װ������
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
**�������ƣ�PoweroffMemoryInit()
**�������������籣�����ݳ�ʼ��
**��ڲ�������
**���ڲ�������
***************************************************************/
void PoweroffMemoryInit(void)
{
	unsigned char  DataAreaBuf[2];
	PADRD = FLASH_SIZE*4 - 1;								//��������������flash�����һ�飨256�ֽڣ�Ϊ���ݿռ�
	Data_Area_Mass_Read(0,DataAreaBuf,2);		//����־λ
	if((DataAreaBuf[0] != 0xAA)||(DataAreaBuf[1] != 0x55))//�������x0AA55Ϊ�����ϵ磬��Ҫ��ʼ�������������ݿռ�
	{
#if 1
		FlowSt.rationValue[0] = 5; 		//װ������
		FlowSt.rationValue[1] = 15;
		FlowSt.rationValue[2] = 20;
		FlowSt.rationValue[3] = 25;
		FlowSt.rationValue[4] = 100;
		FlowSt.rationValue[5] = 300;
		FlowSt.rationValue[6] = 500;
		FlowSt.rationValue[7] = 1000;
#else
		FlowSt.rationValue[0] = 5; 		//װ������
		FlowSt.rationValue[1] = 10;
		FlowSt.rationValue[2] = 15;
		FlowSt.rationValue[3] = 20;
		FlowSt.rationValue[4] = 25;
		FlowSt.rationValue[5] = 30;
		FlowSt.rationValue[6] = 35;
		FlowSt.rationValue[7] = 40;
#endif
		FlowSt.dRationNum = 1;

		WriteRationValueToMemory();				//д��״̬�ֺͱ��������ݿռ�
	}
	else
	{
		ReadRationValueToMemory();				//���ǳ����ϵ磬���������ݿռ��ֵ
	}

	FlowSt.dRationValue = FlowSt.rationValue[FlowSt.dRationNum-1];
}




/**************************************************************
**�������ƣ�Timer0Init(uint8_t      isStart)
**������������ʱ��0��ʼ����100us�ж�   
**��ڲ�����isStart      �Ƿ�����ʱ���ж�   1����   0�ر�
**���ڲ�������
***************************************************************/
void Timer0Init(uint8_t      isStart)			//T0��ʼ��	3.6864 MHZ/12*4        1������0.8138us
{
	if(isStart)
	{
		//TMOD=0x11;
	 	//TMOD = (TMOD&0xFF)|0x03;   //��ʱ��0 ģʽ3   8������  
		//TH0=0;	 	             //  100us        100/0.8138  = 122.88 
		//TL0=0x85;			  //256-123;		


		TMOD = (TMOD&0xFF)|0x01;   //��ʱ��0 ģʽ1   16������  
		//TH0=0xF6;	 	           // 2ms        2000/0.8138  = 2458
		//TL0=0x66;			  	   //65536 - 2458 = 63078;	
		TH0=0xFB;	 	           // 1ms        1000/0.8138  = 1228.8
		TL0=0x33;			       //65536 - 1229 = 64307;	

//		tfru_zdcnt = 0;
//		tfru_zdfg = 0;
		
		ET0=1;			//����ʱ��/������0 �ж�
		TR0=1;			//������ʱ��/������0 �ж�
	}
	else
	{
		ET0=0;			//������ʱ��/������0 �ж�
		TR0=0;			//ֹͣʱ��/������0 �ж�
	}
	
}

#define FRUECNT  50

/**************************************************************
**�������ƣ�Timer0Zd
**������������ʱ��0�ж�   
**��ڲ�������
**���ڲ�������
***************************************************************/
void Timer0Zd(void) interrupt 1	   	//��ʱ��0�ж�
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
**�������ƣ�INT0Init
**�����������ⲿ0�г�ʼ��   
**��ڲ�����isStart      �Ƿ����ж�          1����   0�ر�
**���ڲ�������
***************************************************************/
void INT0Init(uint8_t     isStart)	//����
{

	if(isStart)
	{
		P35F = 1; //����P35Ϊ��������
		IT0CON = 29;

		EX0 =1; 	//INT0�ж�ʹ��
		IE0 =1; 	//�ⲿ�ж�ʹ��
		IT0 = 0; //����Ϊ�½����ж�
		//PX0 = 1; //���� INT0 Ϊ�����ȼ�
		tfru_zdcnt = 0;	
		tfru_zdfg = 0;
	}
	else
	{
		EX0 =0; 	//INT0�ж�ʧ��
		IE0 =0; 	//�ⲿ�ж�ʧ��
	}

	IT0 =1;
	PX0 =1;	

}

/**************************************************************
**�������ƣ�INT0Zd
**�����������ⲿ0�ж�  ת�ӵ��ⲿ�жϼ��� 
**��ڲ�������
**���ڲ�������
***************************************************************/
void INT0Zd(void) interrupt 0
{
	//�ж�  ת�������Ƽ���
#if 0
	//if(tfru_zd)
	{
	
		EX0 =0; 	//INT0�ж�ʧ��
		IE0 =0; 	//�ⲿ�ж�ʧ��
		
//		tfru_zd = 0;
//		tfru_zdcnt = 0;
		FlowSt.flowCount++;

		if(IT0)
		{
			IT0= 0; //����Ϊ�½����ж�
			tfru_zdfg = 0;
		} 
		else
		{
			IT0= 1;
			tfru_zdfg = 1;
		}
			

		EX0 =1; 	//INT0�ж�ʹ��
		IE0 =1; 	//�ⲿ�ж�ʹ��

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
**�������ƣ�KeyProcess
**���������������������   
**��ڲ�������
**���ڲ�������
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
			if(!FlowSt.setMode)      //��������ģʽ
			{
				if(FlowSt.stepRevolve == PUMP_STOP)     //��ʼ��ˮ
				{
					FlowSt.dRationNum = keynum+1;
					FlowSt.dRationValue = FlowSt.rationValue[keynum];
					//UnitFlowCalculate();
					FlowSt.t_dRationValue = (uint32_t)(FlowSt.dRationValue); 
					Timer0Init(0);
					INT0Init(0);  
					UnitFlowCalculate();
					//FlowSt.tpumpTimeCount = (uint32_t)(((double)FlowSt.t_dRationValue/UNIT_FLOWPUMP)+0.500); //����ˮ�ÿ�ʼ��Ŀ��ʱ��
					FlowSt.tFlowCount = (uint32_t)(((double)((double)FlowSt.t_dRationValue)/unitFlow_t)+0.500);	 //���������Ƶ�Ŀ�����ֵ
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
					FlowSt.stepRevolve = PUMP_STOP;   //ֹͣ��ˮ
					rationStopTimeCnt = RATION_STOP_CNT;
				}
			}
			else					//����ģʽ
			{	//����
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
			if(!FlowSt.setMode) 	 //��������ģʽ
			{
				if(FlowSt.stepRevolve == PUMP_STOP) 	//��ʼ��ˮ
				{
					FlowSt.dRationNum = keynum+1;
					FlowSt.dRationValue = FlowSt.rationValue[keynum]*2;
					//UnitFlowCalculate();
					FlowSt.t_dRationValue = (uint32_t)(FlowSt.dRationValue); 
					Timer0Init(0);
					INT0Init(0);
					UnitFlowCalculate();
					//FlowSt.tpumpTimeCount = (uint32_t)(((double)FlowSt.t_dRationValue/UNIT_FLOWPUMP)+0.5); //����ˮ�ÿ�ʼ��Ŀ��ʱ��
					FlowSt.tFlowCount = (uint32_t)(((double)FlowSt.t_dRationValue/unitFlow_t)+0.5);	 //���������Ƶ�Ŀ�����ֵ
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
					FlowSt.stepRevolve = PUMP_STOP;   //ֹͣ��ˮ
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
			if(FlowSt.stepRevolve == PUMP_STOP)     //��������ģʽ
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
					
					//������ʱ�� ��ʱ��������ÿ��һ��ˮ�õ�����һ�������������Ե�λ����������ʱ�䡣��ʱ�ǵ���ʱ��ˮ��ֹͣ��
					FlowSt.pumpTimeCount = 0;
					FlowSt.flowCount = 0;
					Timer0Init(1);  //������ʱ��0�ж�
					INT0Init(0);	//�����ⲿ�ж�0�ж�	
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

				FlowValue_t = (uint32_t)((double)FlowSt.flowCount*unitFlow_t);    //�������������Ƽ����Ļ���
				//FlowValue_t = (uint32_t)((double)FlowSt.pumpTimeCount*UNIT_FLOWPUMP);	  //��������ʱ������Ļ���

				if(FlowValue_t > FlowSt.t_dRationValue) FlowValue_t = FlowSt.t_dRationValue;

				FlowSt.dRationValue = (uint16_t)(FlowSt.t_dRationValue - FlowValue_t);

				//if((FlowSt.pumpTimeCount >= (FlowSt.tpumpTimeCount)) &&(FlowSt.flowCount >= FlowSt.tFlowCount))
				//if(FlowSt.pumpTimeCount >= (FlowSt.tpumpTimeCount))
				
					
				if((FlowSt.flowCount >= (FlowSt.tFlowCount)))
				{
					//ֹͣ��ʱ������ 
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
**�������ƣ�DispalyDigital()
**�����������������ʾ����
**��ڲ�������
**���ڲ�������
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
			if(FlowSt.setMode)					// ������ģʽ�£�������Ҫ��˸
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


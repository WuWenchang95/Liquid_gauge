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
#define  UNIT_FLOWPUMP       	2			  //ˮ�õ�λʱ�������
#define  UNIT_FLOWMETER         2			  //ת�ӵ�λ����������


FlowInfo_t idata FlowSt;						
uint8_t idata DispalyDigitalBuf[5] = {0,0,0,0,0};


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
	Data_Area_Mass_Write(0,DataAreaBuf,20);	//д������
	
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

	Data_Area_Mass_Read(0,DataAreaBuf,20);	//�������������
	FlowSt.rationValue[0] = DataAreaBuf[2];			//װ������
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
		FlowSt.rationValue[0] = 5; 		//װ������
		FlowSt.rationValue[1] = 10;
		FlowSt.rationValue[2] = 15;
		FlowSt.rationValue[3] = 20;
		FlowSt.rationValue[4] = 25;
		FlowSt.rationValue[5] = 30;
		FlowSt.rationValue[6] = 35;
		FlowSt.rationValue[7] = 40;
		FlowSt.dRationNum = 1;

		WriteRationValueToMemory();				//д��״̬�ֺͱ��������ݿռ�
	}
	else
	{
		ReadRationValueToMemory();				//���ǳ����ϵ磬���������ݿռ��ֵ
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
**�������ƣ�Timer0Init(uint8_t      isStart)
**������������ʱ��0��ʼ����100us�ж�   
**��ڲ�����isStart      �Ƿ�����ʱ���ж�   1����   0�ر�
**���ڲ�������
***************************************************************/
void Timer0Init(uint8_t      isStart)			//T0��ʼ��	3.6864 MHZ/12*4        1������0.8138us
{
	//TMOD=0x11;	
 	TMOD = (TMOD&0xFF)|0x03;   //��ʱ��0 ģʽ3   8������  
	TH0=0;	 	      //  100us        100/0.8138  = 122.88 
	TL0=0x85;			  //256-123;	 	
	if(isStart)
	{
		ET0=1;			//����ʱ��/������0 �ж�
		TR0=1;			//������ʱ��/������0 �ж�
	}
	else
	{
		ET0=0;			//������ʱ��/������0 �ж�
		TR0=0;			//ֹͣʱ��/������0 �ж�
	}

}

/**************************************************************
**�������ƣ�Timer0Zd
**������������ʱ��0�ж�   
**��ڲ�������
**���ڲ�������
***************************************************************/
void Timer0Zd(void) interrupt 1	   	//��ʱ��0�ж�
{
	FlowSt.pumpTimeCount++;
}

/**************************************************************
**�������ƣ�INT0Init
**�����������ⲿ0�г�ʼ��   
**��ڲ�����isStart      �Ƿ����ж�          1����   0�ر�
**���ڲ�������
***************************************************************/
void INT0Init(uint8_t     isStart)	//����
{
//	P37F = 1; //����P37Ϊ��������
	IT0CON = 31;

	if(isStart)
	{
		EX0 =1; 	//INT0�ж�ʹ��
		IE0 =1; 	//�ⲿ�ж�ʹ��
		
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
	WDFLG = 0xA5;//ˢ�¿��Ź�
	//�ж�  ת�������Ƽ���
	FlowSt.flowCount++;
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

	for(keynum = 0; keynum < 8; keynum++)
	{
	    if(CheckKey(keynum) == SHORT_PRESS)
        {
  //      	ResetKey(keynum);
			if(!FlowSt.setMode)      //��������ģʽ
			{
				if(FlowSt.stepRevolve == PUMP_STOP)     //��ʼ��ˮ
				{
					FlowSt.dRationNum = keynum+1;
					FlowSt.dRationValue = FlowSt.rationValue[keynum];
					FlowSt.t_dRationValue = FlowSt.dRationValue; 

					FlowSt.tpumpTimeCount = FlowSt.t_dRationValue/UNIT_FLOWPUMP; //����ˮ�ÿ�ʼ��Ŀ��ʱ��
					FlowSt.tFlowCount = FlowSt.t_dRationValue/UNIT_FLOWMETER;	 //���������Ƶ�Ŀ�����ֵ
	
					FlowSt.stepRevolve = PUMP_READY;
					FlowSt.readyCountDown = 50;
				}
				else					//����ģʽ
				{
					FlowSt.stepRevolve = PUMP_STOP;   //ֹͣ��ˮ
				}
			}
			else					//����ģʽ
			{	//����
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
					
					//������ʱ�� ��ʱ��������ÿ��һ��ˮ�õ�����һ�������������Ե�λ����������ʱ�䡣��ʱ�ǵ���ʱ��ˮ��ֹͣ��
					FlowSt.pumpTimeCount = 0;
					FlowSt.flowCount = 0;

					Timer0Init(1);  //������ʱ��0�ж�
					INT0Init(1);	//�����ⲿ�ж�0�ж�

				}
			}
			break;
		case PUMP_OPEN:
			{
				uint16_t FlowValue_t;
				
				FlowValue_t = FlowSt.pumpTimeCount*UNIT_FLOWPUMP;			//��������ʱ������Ļ���
				
				FlowSt.dRationValue = FlowSt.t_dRationValue - FlowSt.pumpTimeCount*UNIT_FLOWPUMP;

				
				if(FlowSt.pumpTimeCount == FlowSt.tpumpTimeCount - 2)
				{
					//ֹͣ��ʱ������ 
					
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


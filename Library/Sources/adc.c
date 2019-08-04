#ifndef _ADC_C_
#define _ADC_C_

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
//#include "../../Library/includes/uart.h"

#include "../../includes/system.h"
#include "../../Library/includes/adc.h"
#include "../../Library/includes/My_header.h"
#include "../../Library/includes/pwm.h"
#include "../../Library/includes/mydefine.h"
#include "../../Library/includes/delay.h"
#include <intrins.h>
/*********************************************************************************************************************/

unsigned int ADCTempData = 88;


unsigned int ADCTempDataTOT=0;
unsigned char ADC_cnt =0;
unsigned char ADC_cnt1 =0;

extern int T_data;
extern int T_dataErr;
extern int T_dataPre;

extern unsigned int ShiDu_data;

extern unsigned char ADC_DY_Ready;


unsigned int ADC_DY;
unsigned int ADC_SD;

unsigned int HIUM_PRE;
int HIUM_T_H;


extern unsigned char ADStep;

/***********************************************************************************
������:		ADC_init										 	
������������ʼ��ADC����
���������adc_ref 	ADC�ο���ѹ����
					adc_int		ADC�ж�����
					amp_sw		�Ƿ����˷Ź���
					gain			�˷�����
����ֵ��  ��
***********************************************************************************/
void ADC_init(ADC_RefVolt_TypeDef adc_ref,InterruptSW_TypeDef adc_int,AmpSW_TypeDef amp_sw,AMP_Gain_TypeDef gain)
{
	ADCON = AST(0) | ADIE(adc_int) | HTME(7) | VSEL(adc_ref);	
	ADCFGL = ACKD(7) | ADCALE(1) | ADCHS(0);	
#if (IC_TYPE == IC_TYPE_CA51F2X)
	ADCFGH = AOVE(0) | VTRIM(35);
	if(amp_sw == ENABLE)
	{
		OPACON = OPAM(2) | OPAS(gain);			
	}
#elif (IC_TYPE == IC_TYPE_CA51F3X)
	if(amp_sw == ENABLE)
	{
		ADCFGH = AOPS(gain) | VTRIM(35);
	}
	else
	{
		ADCFGH = AOPS(0) | VTRIM(35);
	}
#endif
	if(adc_int == ENABLE)
	{
		INT2EN = 1; 
	}
	ADCON |= ADIF;
}
/***********************************************************************************
������:		ADC_ConfigChannel										 	
��������������ADC����ͨ��
���������channel 	ADCͨ��
����ֵ��  ��
***********************************************************************************/
void ADC_ConfigChannel(ADC_Channel_TypeDef channel)
{
	ADCFGL = (ADCFGL&0xF0) | ADCHS(channel);
}
/***********************************************************************************
������:		ADC_StartConversion										 	
��������������ADCת��
�����������
����ֵ��  ��
***********************************************************************************/
void ADC_StartConversion(void)
{
  ADCON |= AST(1);
}

/***********************************************************************************
������:		GetAdcValue										 	
������������ȡADCת����ֵ
�����������
����ֵ��  ADCֵ
***********************************************************************************/
unsigned int GetAdcValue(void)
{
	unsigned int ADC_Value;
	while(!(ADCON & ADIF));
	ADCON |= ADIF;
	ADC_Value = ADCDH*256 + ADCDL;
	ADC_Value >>= 4;
	return ADC_Value;
}




void wendu_caiji(void)       //�¶Ȳɼ�
{
	unsigned char i;     //��ѯ�¶ȱ���
	i=0;

	
	while(i<=130)         //����¶�ֵ
	{

	  if(ADCTempData>=TempTable[i])
	  {

		  	T_data=i-30;	
		    T_data = T_data+T_dataErr;
			
			if((T_data<=(-30)) || (T_data>99))
					T_data = T_dataPre;
			T_dataPre = T_data;
			
/*			
			if((T_data > (T_dataPre+5)))
			{
				T_dataPre = T_dataPre+1;
				T_data = T_dataPre;
			}	
			else if(T_data < (T_dataPre-5))
			{
				T_dataPre = T_dataPre-1;
				T_data = T_dataPre;
			}		
			else
				T_dataPre = T_data;
*/		    
		    break;
	  }
	  i++;
	}
}



void ADC_Wendu(void)
{

	unsigned int Temp;
	ADC_ConfigChannel(ADC_CH1);
	Delay_ms(1);
	ADC_StartConversion();
	
	Temp=GetAdcValue();
	

	ADCTempDataTOT += Temp;

	ADC_cnt++;
	if(ADC_cnt>=16)
	{
		ADCTempData = (ADCTempDataTOT>>4);		
		ADC_cnt=0;
		ADCTempDataTOT=0;
		ADC_cnt1++;
	}
	if(ADC_cnt1>2)
	{
		ADC_cnt1=0;
		wendu_caiji();
		ADStep=1;
		ADC_init(ADC_REF_INNER,DISABLE,DISABLE,NO_AMP);	//��ѹ�ɼ�
		
	}
}


void ADC_Dianya(void)
{
	unsigned int Temp;
	ADC_ConfigChannel(ADC_CH4);
	ADC_StartConversion();
	
	Temp=GetAdcValue();

	ADCTempDataTOT += Temp;

	ADC_cnt++;
	if(ADC_cnt>=16)
	{
		ADCTempData = (ADCTempDataTOT>>4);		
		ADC_cnt=0;
		ADCTempDataTOT=0;
		ADC_cnt1++;
	}
	if(ADC_cnt1>2)
	{
		ADC_cnt1=0;
//		wendu_caiji();
		ADC_DY=ADCTempData;
		
		ADC_DY_Ready =1;
		
		if(BEIGUANG)	
		{	
			ADStep=2;
			
			PWMEN |= (1<<PWM_CH4)|(1<<PWM_CH5);
			
			ADC_init(ADC_REF_VDD,DISABLE,DISABLE,NO_AMP);									
		}	
		else ADStep=0;
	}
}




///*****************************************
////31Kʪ�������ȡʪ��ֵ
//*****************************************/
unsigned int get_31HUMI_vule(unsigned int R_Humideal_int,unsigned char T_temp_char)
{

	unsigned int HUMI_vule;
	
	if(R_Humideal_int>=5000)//20
	{
		HUMI_vule = 200;
	}	
	else if(R_Humideal_int>=2600) //20~25
	{
		HUMI_vule = 304 - R_Humideal_int/48 ;	
	}
	else if(R_Humideal_int>=1300)  //25~30
	{
		HUMI_vule = 350 - R_Humideal_int/26 ;	
	}	
	else if(R_Humideal_int>=630)  //30~35
	{
		HUMI_vule = 397 - (R_Humideal_int*10)/134 ;	
	}	
	else if(R_Humideal_int>=310)  //35~40
	{
		HUMI_vule = 448 - (R_Humideal_int*10)/64 ;	
	}	
	else if(R_Humideal_int>=160)  //40~45
	{
		HUMI_vule = 503 - R_Humideal_int/3 ;	
	}
	else if(R_Humideal_int>=87)  //45~50
	{
		HUMI_vule = 560 - (R_Humideal_int*100)/146 ;	
	}
	else if(R_Humideal_int>=49)  //50~55
	{
		HUMI_vule = 614 - (R_Humideal_int*100)/76 ;	
	}
	else if(R_Humideal_int>=31)  //55~60
	{
		HUMI_vule = 686 - (R_Humideal_int*100)/36 ;	
	}
	
	else if(R_Humideal_int>=20)  //60~65
	{
		HUMI_vule = 741 - (R_Humideal_int*100)/22 ;	
	}	
	else if(R_Humideal_int>=13)  //65~70
	{
		HUMI_vule = 793 - (R_Humideal_int*100)/14 ;	
	}		
	else if(R_Humideal_int>=8)  //70~75
	{
		HUMI_vule = 841 - (R_Humideal_int*1000)/92 ;	
	}	
	else if(R_Humideal_int>=6)  //75~80
	{
		HUMI_vule = 906 - (R_Humideal_int*1000)/54 ;	
	}	
	
	else
	{
		HUMI_vule = 970 - (R_Humideal_int*100)/4 ;	
	}	

  //�¶Ȳ���0-60��
  if(T_temp_char<=40)  			HUMI_vule= HUMI_vule + 50-T_temp_char*2;   //С��25һ�ȼ�0.2
  if(ShiDu_data > 200)
  {
	  HUMI_vule += ShiDu_data;
	  HUMI_vule /=2; 

  }
  
  if(HUMI_vule >= 950 && HUMI_vule < 999)
  {
	  HUMI_vule += 975;
	  HUMI_vule /=2; 
  }
  else if(HUMI_vule >= 900 && HUMI_vule < 949)
  {
	  HUMI_vule += 925;
	  HUMI_vule /=2; 
  }
  else if(HUMI_vule >= 850 && HUMI_vule < 899)
  {
	  HUMI_vule += 875;
	  HUMI_vule /=2;
  }
  else if(HUMI_vule >= 800 && HUMI_vule < 849)
  {
	  HUMI_vule += 825;
	  HUMI_vule /=2;
  }		
  
  HUMI_vule+=5;
  if(HUMI_vule>=999)  HUMI_vule=999;  //����99 
  if(HIUM_PRE>HUMI_vule)
  	HIUM_T_H = -5;
  else if(HIUM_PRE<HUMI_vule)
  	HIUM_T_H = 7;
  else
  	HIUM_T_H = 0;
  
  //HIUM_T_H = 0;
  HIUM_PRE = HUMI_vule;
  return HUMI_vule;
  
}


void ADC_ShiDu(void)
{	
	unsigned int Temp;
	unsigned int Rtemp;	
	unsigned int T_Temp;

//	ADC_ConfigChannel(ADC_CH0);
	ADC_StartConversion();
	
	Temp=GetAdcValue();

	ADCTempDataTOT += Temp;

	ADC_cnt++;
	if(ADC_cnt>=16)
	{
		ADCTempData = (ADCTempDataTOT>>4);		
		ADC_cnt=0;
		ADCTempDataTOT=0;
		ADC_cnt1++;
	}
	if(ADC_cnt1>2)
	{
		ADC_cnt1=0;
//		wendu_caiji();
		ADC_SD=ADCTempData;
//		uart_printf("ADC:%d\r\n",ADC_SD);

		//Rtemp = ((unsigned long)ADC_SD * 47)/(4096-ADC_SD);
		Rtemp = ((unsigned long)(4096-ADC_SD)*470)/ADC_SD;
		//Rtemp = ((unsigned long)(4096-ADC_SD)*100)/ADC_SD;

		Rtemp+=5;
		Rtemp/=10;
//		uart_printf("Res:%d\r\n",Rtemp);

		if(T_data<0)	T_Temp=0;
		else T_Temp = T_data;

		ShiDu_data = get_31HUMI_vule(Rtemp,T_Temp);
		
//		uart_printf("humi:%d\r\n",ShiDu_data);

		//PWM_EnContrl(0);
		
		PWMEN &= (~((1<<PWM_CH4)|(1<<PWM_CH5)));
		
//		ADC_init(ADC_REF_VDD,DISABLE,DISABLE,NO_AMP);
		ADStep=0;
	}

}


void T1_INT(void) interrupt 3	   	//��ʱ��1�ж� 37.9KHz
{
	ET1=0;
	TR1=0;
	ADC_ShiDu();
}

void PWM_ISR(void) interrupt 14
{
/*
	if(PWMCIF & TIF0)
	{
		PWMCIF = TIF0;
				
		if(ADStep==2)
		{
			Delay_50us(3);
			ADC_ShiDu();			
		}				
	}	
*/
	if(PWMCIF & PIF0)
	{
		PWMCIF = PIF0;
		//�������жϷ������
		if(ADStep==2)
		{
			ADC_ConfigChannel(ADC_CH0);
	/*
			
				
			if(ShiDu_data > 880)
			{
				TH1=(65536-27)/256; 	
				TL1=(65536-27)%256;

			}
			else 
				
		
			if(ShiDu_data > 910)
			{
				TH1=(65536-(20+HIUM_T_H))/256; 	
				TL1=(65536-(20+HIUM_T_H))%256;

			}				
			else if(ShiDu_data > 900)
			{
				TH1=(65536-(22+HIUM_T_H))/256; 	
				TL1=(65536-(22+HIUM_T_H))%256;

			}			
			else if(ShiDu_data > 890)
			{
				TH1=(65536-(24+HIUM_T_H))/256; 	
				TL1=(65536-(24+HIUM_T_H))%256;

			}		
			else if(ShiDu_data > 880)
			{
				TH1=(65536-(26+HIUM_T_H))/256; 	
				TL1=(65536-(26+HIUM_T_H))%256;

			}				
			else if(ShiDu_data > 870)
			{
				TH1=(65536-(28+HIUM_T_H))/256; 	
				TL1=(65536-(28+HIUM_T_H))%256;

			}
			else if(ShiDu_data > 860)
			{
				TH1=(65536-(30+HIUM_T_H))/256; 	
				TL1=(65536-(30+HIUM_T_H))%256;

			}			
			else if(ShiDu_data > 850)
			{
				TH1=(65536-(33+HIUM_T_H))/256; 	
				TL1=(65536-(33+HIUM_T_H))%256;

			}
			else if(ShiDu_data > 840)
			{
				TH1=(65536-(36+HIUM_T_H))/256; 	
				TL1=(65536-(36+HIUM_T_H))%256;

			}
			else if(ShiDu_data > 830)
			{
				TH1=(65536-(38+HIUM_T_H))/256; 	
				TL1=(65536-(38+HIUM_T_H))%256;

			}
			else if(ShiDu_data > 820)
			{
				TH1=(65536-(40+HIUM_T_H))/256; 	
				TL1=(65536-(40+HIUM_T_H))%256;

			}
			else if(ShiDu_data > 810)
			{
				TH1=(65536-(44+HIUM_T_H))/256; 	
				TL1=(65536-(44+HIUM_T_H))%256;
			}
			else if(ShiDu_data > 800)
			{
				TH1=(65536-(50+HIUM_T_H))/256; 	
				TL1=(65536-(50+HIUM_T_H))%256;
			}			
			else if(ShiDu_data > 750)
			{
				TH1=(65536-(75+HIUM_T_H))/256; 	
				TL1=(65536-(75+HIUM_T_H))%256; 
			}
			else if(ShiDu_data > 700)
			{
				TH1=(65536-(100+HIUM_T_H))/256; 	
				TL1=(65536-(100+HIUM_T_H))%256; 
			}
			else if(ShiDu_data > 650)
			{
				TH1=(65536-(150+HIUM_T_H))/256; 	
				TL1=(65536-(150+HIUM_T_H))%256; 
			}
			else
			{
				TH1=(65536-250)/256; 	
				TL1=(65536-250)%256; 				
			}

*/				

			TH1=(65536-25)/256; 	
			TL1=(65536-25)%256;

			ET1=1;
			TR1=1;

			//ADC_ShiDu();

		}	
	}
}	











/*********************************************************************************************************************/
#endif

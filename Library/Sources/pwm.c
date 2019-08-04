#ifndef _PWM_C_
#define _PWM_C_

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
#include "../../Library/includes/uart.h"
#include "../../Library/includes/pwm.h"
#include <intrins.h>
/*********************************************************************************************************************/
/******************************************************************************
函数名：PWM_init
功能描述：PWM功能初始化
输入参数: pwm_ch		PWM通道选择
					AlignMod	对齐模式设置
					cks				PWM时钟源设置（只有PWM0,2,4,6才有此项设置）
					toggle		PWM是否反相输出设置
					Prescal		PWM时钟分频设置
					Comp			PWM互补输出设置（只有PWM1,3,5,7才有此项设置）
返回值： 无					

******************************************************************************/
void PWM_init(PWM_Channel_TypeDef pwm_ch,
							PWM_AlignMod_TypeDef AlignMod,
							PWM_CKS_TypeDef cks,
							PWM_Toggle_TypeDef toggle,
							unsigned char Prescal,
							PWM_Complementary_TypeDef Comp)
{
	INDEX = pwm_ch;
	if(pwm_ch & 0x01)	//PWM1,PWM3,PWM5,PWM7
	{
		PWMCON = (PWMCON&0xF0) | MS(AlignMod) | MOD(Comp);
		PWMCFG = TOG(toggle) | Prescal;		
	}
	else							//PWM0,PWM2,PWM4,PWM6
	{
		PWMCON = (PWMCON&0xF0) | MS(AlignMod) | CKS(cks);
		PWMCFG = TOG(toggle) | Prescal;		
	}
}



/******************************************************************************
函数名：	PWM_CfgDivDuty
功能描述：设置PWM周期和占空比
输入参数: pwm_ch		PWM通道选择
					DIV				PWM周期设置
					DUT				PWM占空比设置
返回值：  无					
******************************************************************************/
void PWM_CfgDivDuty(PWM_Channel_TypeDef pwm_ch,unsigned int DIV,unsigned int DUT)
{
	INDEX = pwm_ch;
	PWMDIVH	= (unsigned char)(DIV>>8);
	PWMDIVL	= (unsigned char)(DIV);
	PWMDUTH	= (unsigned char)(DUT>>8);
	PWMDUTL	= (unsigned char)(DUT);	
}

/******************************************************************************
函数名：	PWM_Update
功能描述：PWM寄存器更新
输入参数: PWMCH_Bits	
返回值：  无					
******************************************************************************/
void PWM_Update(unsigned char PWMCH_Bits)
{
	PWMUPD = PWMCH_Bits;
}

/******************************************************************************
函数名：	WaitPWMUpdateComplete
功能描述：等待PWM配置值更新到实际寄存器
输入参数: 无	
返回值：  无					
******************************************************************************/
void WaitPWMUpdateComplete(void)
{
	while(PWMUPD);
}

/******************************************************************************
函数名：	PWM_EnContrl
功能描述：PWM使能
输入参数: PWMEn_Bits	，每一位对应PWM的一个通道
返回值：  无					
******************************************************************************/
//void PWM_EnContrl(unsigned char PWMEn_Bits)
//{
//	PWMEN = PWMEn_Bits;
//}


/******************************************************************************
函数名：	PWM_InterruptCfg
功能描述：设置PWM周期和占空比
输入参数: pwm_ch		PWM通道选择
					tie				PWM顶点中断设置
					zie				PWM零点中断设置
					pie				PWM上升沿中断
					nie				PWM下升沿中断
					pwmcmax		寄存器PWMCMAX设置值，表示多少个PWM周期产生一次PWM中断
返回值：  无					
******************************************************************************/
void PWM_InterruptCfg(PWM_Channel_TypeDef pwm_ch,
											PWM_TIE_TypeDef tie,
											PWM_ZIE_TypeDef zie,
											PWM_PIE_TypeDef pie,
											PWM_NIE_TypeDef nie,
											unsigned char pwmcmax
											)
{
	INDEX = pwm_ch;
	PWMCON = (PWMCON&0x0F) | TIE(tie) | ZIE(zie) | PIE(pie) | NIE(nie);
	PWMCMAX = pwmcmax;
}




void PWM_init_Set(void)
{
	PWM_init(PWM_CH0,Center_Align,CKS_IH,Toggle_On,0,Complementary_Off);
	PWM_init(PWM_CH1,Center_Align,CKS_IH,Toggle_On,0,Complementary_On);

	PWM_init(PWM_CH4,Center_Align,CKS_IH,Toggle_On,0,Complementary_Off);
	PWM_init(PWM_CH5,Center_Align,CKS_IH,Toggle_On,0,Complementary_On);	
	
	
	
	PWM_CfgDivDuty(PWM_CH0,600,300);
	PWM_Update((1<<PWM_CH0)|(1<<PWM_CH1));
	WaitPWMUpdateComplete();
//	PWMEN |= ((1<<PWM_CH0)|(1<<PWM_CH1));


	PWM_CfgDivDuty(PWM_CH4,1843,922);
	//PWM_CfgDivDuty(PWM_CH4,922,461);
	PWM_Update((1<<PWM_CH4)|(1<<PWM_CH5));
	WaitPWMUpdateComplete();
	PWMEN |= ((1<<PWM_CH4)|(1<<PWM_CH5));


	
	PWM_InterruptCfg(PWM_CH4,TIE_Off,ZIE_Off,PIE_On,NIE_Off,25);
	//PWM_InterruptCfg(PWM_CH4,TIE_On,ZIE_Off,PIE_Off,NIE_Off,25);
	INT9EN =1;

}	





#endif
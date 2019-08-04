#ifndef _STOP_IDLE_C_
#define _STOP_IDLE_C_
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
#include "../../Library/includes/stop_idle.h"
#include "../../Library/includes/system_clock.h"
#include <intrins.h>
/*********************************************************************************************************************/
/******************************************************************************************
函数名：StopEnter
功能说明：进入STOP模式
输入参数：无
返回值：  无
******************************************************************************************/
void StopEnter(void)
{
	PCON |= STOP;
	_nop_();
	_nop_();
	_nop_();	
}

/******************************************************************************************
函数名：CheckStopCondition
功能说明：检查进入STOP模式的条件（注：如果STPSTH、STPSTL不为0，即使执行STOP指令，芯片也不会进入STOP）
输入参数：无
返回值：  0：不能进入STOP； 1： 可以进入STOP
******************************************************************************************/
unsigned char CheckStopCondition(void)
{
	if(STPSTH || STPSTL) return 0;
	else return 1;
}

/******************************************************************************************
函数名：	IdleEnter
功能说明：进入IDLE模式
输入参数：无
返回值：  无
******************************************************************************************/
//void IdleEnter(void)
//{
//	PCON |= IDLE;
//	_nop_();
//	_nop_();
//}

/******************************************************************************************
函数名：CheckIdleCondition
功能说明：检查进入IDLE模式的条件（注：如果IDLSTH、IDLSTL不为0，即使执行IDLE指令，芯片也不会进入IDLE）
输入参数：无
返回值：  0：不能进入IDLE； 1： 可以进入IDLE
******************************************************************************************/
//unsigned char CheckIdleCondition(void)
//{
//	if(IDLSTH || IDLSTL) return 0;
//	else return 1;
//}
/*********************************************************************************************************************/






//void PowerSaving_Loop(void)
//{

//	bit IE_EA;
//	I2CCON = 0;						//关闭I2C，因为I2C上电默认是打开的，必须关闭，否则主时钟无法关闭
//	CKCON = 0;						//关闭所有时钟
//	MECON |= BIT6;				//设置FLASH进入深度睡眠模式
//	PWCON = (PWCON&0xF0) | 0x04;		//设置内部LDO为低功率模式
//	///////////////////////////////////////////////////////////////////////
//	INT9EN =0;
//	PLLCON = 0;
//		
//	while(!CheckStopCondition());
//	IE_EA = EA;				//保存当前总中断开关状态
//	EA = 0;					//关总中断
//	StopEnter();	//进入STOP模式
//	
//	PWCON = (PWCON&0xF0) | 0x0B;	//退出STOP模式后必须设置LDO为高功率模式
//	EA = IE_EA;				//恢复之前的总中断状态
//	
//	INT9EN =1;
//	
//	while(1)
//	{
//		
//		
//	}
//}	













#endif
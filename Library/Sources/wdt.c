#ifndef _WDT_C_
#define _WDT_C_
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
#include "../../Library/includes/wdt.h"
#include <intrins.h>
/*********************************************************************************************************************/
/*****************************************************************************
函数名：WDT_init
功能描述：初始化看门狗
输入参数：wdts		看门狗时钟源设置
					wdre		看门狗模式设置
					wdtch		看门狗时间设置
返回值： 无
******************************************************************************/
void WDT_init(WDTS_TypeDef wdts,WDRE_TypeDef wdre,unsigned int wdtvh)
{
	WDCON = WDTS(wdts) | WDRE(wdre);
	WDVTHH = (unsigned char)(wdtvh>>8);
	WDVTHL = (unsigned char)(wdtvh);	
}

/*****************************************************************************
函数名：WDT_FeedDog
功能描述：刷新看门狗
输入参数：无
返回值： 无
******************************************************************************/
void WDT_FeedDog(void)
{
	WDFLG = 0xA5;
}
/*********************************************************************************************************************/
#endif
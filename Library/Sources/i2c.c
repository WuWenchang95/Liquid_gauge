#ifndef _I2C_C_
#define _I2C_C_

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
#include "../../Library/includes/i2c.h"
#include <intrins.h>
/*********************************************************************************************************************/

void I2C_init(I2CE_TypeDef i2ce,I2CIE_TypeDef i2cie,STFE_TypeDef stfe,unsigned char i2cadr,unsigned char i2ccr)
{
	I2CCON = I2CE(i2ce) | I2CIE(i2cie) | STA(0) | STP(0)| CKHD(1) | AAK(1)| CBSE(0) | STFE(stfe);		
	I2CADR = GCE(0)|(i2cadr>>1);
	I2CCR = (i2ccr>>1);
}

void I2C_SelectComPin(I2CIOS_TypeDef i2cios)
{
	I2CIOS = i2cios;
#if (IC_TYPE == IC_TYPE_CA51F3X)
	if(i2cios == I2C_SEL_P30_P31)
	{
		GPIO_Init(P30F,P30_I2C_SDA_SETTING | PU_EN);
		GPIO_Init(P31F,P31_I2C_SCL_SETTING | PU_EN);		
	}
	else if(i2cios == I2C_SEL_P11_P12)
	{
		GPIO_Init(P11F,P11_I2C_SDA_SETTING | PU_EN);
		GPIO_Init(P12F,P12_I2C_SCL_SETTING | PU_EN);		
	}
#elif  (IC_TYPE == IC_TYPE_CA51F2X)
	if(i2cios == I2C_SEL_P36_P37)
	{
		GPIO_Init(P36F,P36_I2C_SDA_SETTING | PU_EN);
		GPIO_Init(P37F,P37_I2C_SCL_SETTING | PU_EN);
	}
	else if(i2cios == I2C_SEL_P30_P31)
	{
		GPIO_Init(P30F,P30_I2C_SDA_SETTING | PU_EN);
		GPIO_Init(P31F,P31_I2C_SCL_SETTING | PU_EN);	
	}
	else if(i2cios == I2C_SEL_P66_P67)
	{
		GPIO_Init(P66F,P66_I2C_SDA_SETTING | PU_EN);
		GPIO_Init(P67F,P67_I2C_SCL_SETTING | PU_EN);
	}
	else if(i2cios == I2C_SEL_P61_P60)
	{
		GPIO_Init(P61F,P61_I2C_SDA_SETTING | PU_EN);
		GPIO_Init(P60F,P60_I2C_SCL_SETTING | PU_EN);
	}
#endif	
}
#endif
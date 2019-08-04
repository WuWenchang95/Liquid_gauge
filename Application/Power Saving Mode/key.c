#ifndef _KEY_C_
#define _KEY_C_

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

#include "main.h"
#include <intrins.h>
#include "key.h"
#include "buzzer.h"

//key_msg xdata  key_dev[KEY_SUM];

stkey_msg data  tkey_dev;

/**************************************************************
**函数名称：KeyInit
**函数描述：按键初始化  
**入口参数：无
**出口参数：无
***************************************************************/
void KeyInit(void (*KeyProc)(void))
{
	GPIO_Init(P21F,INPUT | PD_EN);
	GPIO_Init(P10F,INPUT | PD_EN);
	GPIO_Init(P11F,INPUT | PD_EN);
	GPIO_Init(P12F,INPUT | PD_EN);
	GPIO_Init(P13F,INPUT | PD_EN);
	GPIO_Init(P14F,INPUT | PD_EN);
	GPIO_Init(P15F,INPUT | PD_EN);
	GPIO_Init(P16F,INPUT | PD_EN);
	GPIO_Init(P17F,INPUT | PD_EN);
	GPIO_Init(P37F,INPUT | PD_EN);

	
	tkey_dev.KeyPro = KeyProc;
	tkey_dev.oldkeyValue = KEY_NONO;
}

/**************************************************************
**函数名称：ReadKeyPin
**函数描述：读取按键IO,赋值到变量中  
**入口参数：无
**出口参数：无
***************************************************************/
uint16_t ReadKeyPin(void)	//按键对应IO口检测
{
	uint16_t tmp = 0;	//对应位有按下时1    按下高电平
	
	if(KEY1)  
		tmp |= (1<<0);
	if(KEY2) 
		tmp |= (1<<1);
	if(KEY3) 
		tmp |= (1<<2);
	if(KEY4)  
		tmp |= (1<<3);
	if(KEY5) 
		tmp |= (1<<4);
	if(KEY6) 
		tmp |= (1<<5);
	if(KEY7) 
		tmp |= (1<<6);
	if(KEY8)  
		tmp |= (1<<7);
	if(KEYUP) 
		tmp |= (1<<8);
	if(KEYDOWN) 
		tmp |= (1<<9);

	return tmp;
}

/**************************************************************
**函数名称：CheckKeyPressNum
**函数描述：检测当前按下的按键数量
**入口参数：无
**出口参数：无
***************************************************************/
uint8_t CheckKeyPressNum(uint16_t KeyValue)
{
	uint8_t i; 
	uint8_t sum = 0;
	
	for(i = 0; i < KEY_SUM; i++)
	{
		if(KeyValue & (1 << i))
		{
			sum++;
		}
	}

	return sum;
}

/**************************************************************
**函数名称：CheckKeyValue
**函数描述：检测当前按下的按键值
**入口参数：无
**出口参数：无
***************************************************************/
KeyCode CheckKeyValue(uint16_t KeyValue)

{
	KeyCode i; 
	uint8_t keyV = 0;
	
	for(i = KEY_1; i < KEY_SUM; i++)
	{
		if(KeyValue & (1 << i))
		{
			keyV = i; 
			break;
		}
	}

	return keyV;
}


void KeyScan(void)
{
	uint16_t key_data;
	
	key_data = ReadKeyPin();

	tkey_dev.cur = key_data;

	if(tkey_dev.old != tkey_dev.cur)
	{
		tkey_dev.old = tkey_dev.cur;
#ifdef PRINT_EN			
		uart_printf("key c ");
#endif
		//tkey_dev.count = 0;
		
		if(CheckKeyPressNum(tkey_dev.cur) == 1)
		{
#ifdef PRINT_EN	
			uart_printf("key_data  %x ",tkey_dev.cur);
#endif
			tkey_dev.key_on = 1;
			
			
		}
#if 0		
		else if(CheckKeyPressNum(tkey_dev.cur) == 0)
		{
			tkey_dev.key_on = 0;
			uart_printf("a");
		}
#endif		
		else
		{
			tkey_dev.key_on = 0;
#ifdef PRINT_EN				
			uart_printf("e");
#endif
		}
	}
	
	if(tkey_dev.key_on) //单次和长按都可判断
	{
		tkey_dev.curkeyValue = CheckKeyValue(tkey_dev.cur);
		tkey_dev.down_time = 0;
		if(tkey_dev.count < 3000)
			tkey_dev.count++;
		
	}

	if(tkey_dev.count == 300)
	{	
		tkey_dev.keyValue = tkey_dev.curkeyValue;
		tkey_dev.key_sta = LONG_PRESS;			//
		tkey_dev.press_cnt = 0;
		tkey_dev.oldkeyValue = KEY_NONO;
				
		tkey_dev.longpressfg = 1;
        tkey_dev.isRead = 1; 
        SetBuzzer(RING_ONCE);
	}

	if(!tkey_dev.key_on && tkey_dev.count > 0)
	{
		if(!tkey_dev.longpressfg)
		{
#if 0		
			if(tkey_dev.count >= 300)
			{
			
				tkey_dev.keyValue = tkey_dev.curkeyValue;
				tkey_dev.key_sta = LONG_PRESS;			//
				tkey_dev.press_cnt = 0;
				tkey_dev.oldkeyValue = KEY_NONO;
				tkey_dev.isRead = 1; 
#ifdef PRINT_EN			
				uart_printf("b");
#endif
			}
			else 
#endif
			if(tkey_dev.count >= 1)			//50ms
			{
				if(tkey_dev.oldkeyValue != tkey_dev.curkeyValue)
				{
					
					tkey_dev.press_cnt = 1;
#ifdef PRINT_EN
					uart_printf("c");
#endif
				}
				else
				{
					tkey_dev.press_cnt++;
				}
				
				tkey_dev.oldkeyValue = tkey_dev.curkeyValue;
				
				
				tkey_dev.down_time = 50;
 #ifdef PRINT_EN	
				uart_printf("d");
#endif
				SetBuzzer(RING_ONCE);
			}
		
		}
		else
		{
			tkey_dev.longpressfg = 0;
		}
		tkey_dev.count = 0; 		
		
	}
}


void KeyloosenScan(void)
{
	if(tkey_dev.down_time > 0)
	{
		tkey_dev.down_time--;
		if(tkey_dev.down_time == 0)
		{
			if(tkey_dev.press_cnt == 1)
			{
				tkey_dev.key_sta = SHORT_PRESS;	
			}
			else if(tkey_dev.press_cnt == 2)
			{
				tkey_dev.key_sta = TWICE_PRESS;	
			}
			else if(tkey_dev.press_cnt == 3)
			{
				tkey_dev.key_sta = THREE_PRESS;	
			}			
			else
			{
				tkey_dev.key_sta = NO_PRESS;	
			}
			tkey_dev.press_cnt = 0;
			
			tkey_dev.keyValue = tkey_dev.oldkeyValue;
			tkey_dev.oldkeyValue = KEY_NONO;
			
			tkey_dev.isRead = 1; 
#ifdef PRINT_EN				
			uart_printf("key %d,  %c   \r\n",tkey_dev.keyValue,tkey_dev.key_sta);
#endif
		}
	}
}

uint8_t IsLongPress(KeyCode key_num)
{
	if(tkey_dev.key_on)
	{
		if(tkey_dev.curkeyValue == key_num)
		if(tkey_dev.key_sta == LONG_PRESS)	
			return tkey_dev.longpressfg;
	}

	return 0;
	
}

KeySta CheckKey(KeyCode key_num)
{
	KeySta temp = NO_PRESS;
	
	if(tkey_dev.isRead)
	{
		if(tkey_dev.keyValue == key_num)
			temp = tkey_dev.key_sta;
	}
	return temp;
}





void AllKeyScan(void)
{
	static uint8_t ttemp;

	if(ttemp != (uint8_t)(Time_ms))
	{
		ttemp = (uint8_t)(Time_ms);
		KeyScan();
		KeyloosenScan();
		tkey_dev.KeyPro();
		tkey_dev.isRead = 0;		
	}
	
}


#if 0

void KeyScan(key_msg *key,char key_mun,char key_data) //按键处理
{
	uint16_t temp;
//	CHAR_BIT temp = KEY_DATA; 
	temp = 1 << key_mun;

	key->cur = (key_data & temp) ? 1:0 ; //读取当前按键的状态

	if(key->cur != key->old) //按键按下或者松开检测，按下时，状态一直为1，松开时为0
	{
		
		key->old = key->cur;//把当前状态给旧的状态，以免重复运行该判断
		if(key->cur == 0)
			key->key_on = 1;
		else
			key->key_on = 0; 
	}

	if(key->key_on) //单次和长按都可判断
	{
		key->down_time = 0;
		key->count++;
	}


	if(!key->key_on && key->count > 0) //按键松开时候进行检测，如果一直按着执行长按，这里也可以写1，可用于消抖
	{
		if(key->count >= 300)
		{
//			key->flag = 1;			//
			key->key_sta = LONG_PRESS;			//
		}
		else if(key->count >= 2)			//50ms
		{
			//key->flag = 1;			//
			key->press_cnt++;
			key->down_time = 5;
			//key->key_sta = SHORT_PRESS; 		//短按
		}

		key->count = 0; 
		//蜂鸣器响一下
	}
}

/*
	按键松手之后的计时
*/
void KeyloosenScan(key_msg *key)
{
	if(key->down_time > 0)
	{
		key->down_time--;
		if(key->down_time == 0)
		{
			if(key->press_cnt == 1)
			{
				key->key_sta = SHORT_PRESS;	
			}
			else if(key->press_cnt == 2)
			{
				key->key_sta = TWICE_PRESS;	
			}
			key->press_cnt = 0;
//			key->flag = 1;
		}
	}
}
/*
	按键是否按下
*/
uint8_t IsKeyPress(KeyCode key_num)
{
	if(key_dev[key_num].key_on)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

KeySta CheckKey(KeyCode key_num)
{
	KeySta temp;
	
	temp =  key_dev[key_num].key_sta;
	//key_dev[key_num].key_sta = 	NO_PRESS;
	//key_dev[key_num].flag = 0;
	
	return temp;
}

void ResetKey(KeyCode key_num)
{
	key_dev[key_num].key_sta = 	NO_PRESS;
//	key_dev[key_num].flag = 0;
	key_dev[key_num].count = 0; 
	key_dev[key_num].press_cnt = 0;
	key_dev[key_num].down_time = 0;
}

void AllKeyScan(void)
{
	uint8_t key_data;
	uint8_t i;
	static uint8_t ttemp;

	if(ttemp != (uint8_t)(Time_10ms))
	{
		ttemp = (uint8_t)(Time_10ms);
		
		key_data = ReadKeyPin();
			
		for(i = 0;i < KEY_SUM; i++)
		{
			KeyScan(&key_dev[i],i,key_data);
			KeyloosenScan(&key_dev[i]);
		}

	}

}	
#endif

#endif

#ifndef KEYSCAN_DEF
#define KEYSCAN_DEF

#include "../../includes/system.h"


#define KEY_INIT 0x03FF
#define KEY_SUM  10      //要用的的按键数


typedef enum
{
	KEY_1,
	KEY_2,
	KEY_3,
	KEY_4,
	KEY_5,
	KEY_6,
	KEY_7,
	KEY_8,	
	KEY_UP,
	KEY_DOWN,	
	KEY_NONO,
}KeyCode;	

typedef enum
{
	NO_PRESS,		//无按键	
	SHORT_PRESS,	//短按
	TWICE_PRESS,	//短按两次
	THREE_PRESS,	//短按3次
	LONG_PRESS,	    //长按
}KeySta;

/*
typedef struct   //要改成时间计数，次数，改成一个，不能同时按多个建，按键不同重新计数
{
	uint8_t old;   
	uint8_t cur;
	uint8_t key_on;    //  1    按下 0松开	
	//	uint8_t flag;		//有按键事件	
	KeySta key_sta; //键值  0没有按，1短按 ，2长按
	uint8_t press_cnt;  //按下的次数
	uint8_t down_time;  //按下之后倒计时
	uint16_t count;		//计时
	uint16_t time;	

}key_msg;
*/
typedef struct   //要改成时间计数，次数，改成一个，不能同时按多个建，按键不同重新计数
{
	KeySta key_sta; //键值状态     0没有按，1短按 ，2长按
	uint8_t press_cnt;  //按下的次数
	uint8_t down_time;  //按下之后倒计时	
	uint8_t isRead;		//是否可读取键值	
	uint8_t key_on;    //  1    按下 0松开	
	uint8_t longpressfg;    //  1 长按中 0松开	
	
	KeyCode oldkeyValue;  //上一次按下松开所确定的键值
	KeyCode curkeyValue;  //当前按下松开所确定的键值
	KeyCode keyValue;  //当前按下松开所确定的键值
	
	uint16_t old;   
	uint16_t cur;
	uint16_t count;		//计时
	void (*KeyPro)(void);  //
}stkey_msg;


#define KEY1    		P17
#define KEY2 			P16
#define KEY3    		P15
#define KEY4 			P14
#define KEY5   			P10
#define KEY6 			P11
#define KEY7   			P12
#define KEY8 			P13
#define KEYUP   		P21
#define KEYDOWN 		P37

void KeyInit(void (*KeyProc)(void));  
void AllKeyScan(void);
KeySta CheckKey(KeyCode key_num);
void ResetKey(KeyCode key_num);
uint8_t IsLongPress(KeyCode key_num);

#endif


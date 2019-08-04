#ifndef KEYSCAN_DEF
#define KEYSCAN_DEF

#include "../../includes/system.h"


#define KEY_INIT 0x03FF
#define KEY_SUM  10      //Ҫ�õĵİ�����


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
	NO_PRESS,		//�ް���	
	SHORT_PRESS,	//�̰�
	TWICE_PRESS,	//�̰�����
	THREE_PRESS,	//�̰�3��
	LONG_PRESS,	    //����
}KeySta;

/*
typedef struct   //Ҫ�ĳ�ʱ��������������ĳ�һ��������ͬʱ���������������ͬ���¼���
{
	uint8_t old;   
	uint8_t cur;
	uint8_t key_on;    //  1    ���� 0�ɿ�	
	//	uint8_t flag;		//�а����¼�	
	KeySta key_sta; //��ֵ  0û�а���1�̰� ��2����
	uint8_t press_cnt;  //���µĴ���
	uint8_t down_time;  //����֮�󵹼�ʱ
	uint16_t count;		//��ʱ
	uint16_t time;	

}key_msg;
*/
typedef struct   //Ҫ�ĳ�ʱ��������������ĳ�һ��������ͬʱ���������������ͬ���¼���
{
	KeySta key_sta; //��ֵ״̬     0û�а���1�̰� ��2����
	uint8_t press_cnt;  //���µĴ���
	uint8_t down_time;  //����֮�󵹼�ʱ	
	uint8_t isRead;		//�Ƿ�ɶ�ȡ��ֵ	
	uint8_t key_on;    //  1    ���� 0�ɿ�	
	uint8_t longpressfg;    //  1 ������ 0�ɿ�	
	
	KeyCode oldkeyValue;  //��һ�ΰ����ɿ���ȷ���ļ�ֵ
	KeyCode curkeyValue;  //��ǰ�����ɿ���ȷ���ļ�ֵ
	KeyCode keyValue;  //��ǰ�����ɿ���ȷ���ļ�ֵ
	
	uint16_t old;   
	uint16_t cur;
	uint16_t count;		//��ʱ
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


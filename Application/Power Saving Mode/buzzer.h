#ifndef _BUZZER_H
#define _BUZZER_H




typedef enum{
	NOT_RING,			//不响
	RING_ONCE,			//响一下
	RING_ONCE_LONG,		//响一下久一些
	RING_TWICE,			//响两下
	RING_CONTINUOUS,	//持续响
}BUZZER_MODE;

void BuzzerCrl(void);
void BuzzerGPIOInit(void);
void SetBuzzer(BUZZER_MODE buzzermode);


#endif


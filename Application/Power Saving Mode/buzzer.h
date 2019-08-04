#ifndef _BUZZER_H
#define _BUZZER_H




typedef enum{
	NOT_RING,			//����
	RING_ONCE,			//��һ��
	RING_ONCE_LONG,		//��һ�¾�һЩ
	RING_TWICE,			//������
	RING_CONTINUOUS,	//������
}BUZZER_MODE;

void BuzzerCrl(void);
void BuzzerGPIOInit(void);
void SetBuzzer(BUZZER_MODE buzzermode);


#endif


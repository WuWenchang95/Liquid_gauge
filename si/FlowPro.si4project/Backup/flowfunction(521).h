#ifndef FLOW_FUNCTION_H
#define FLOW_FUNCTION_H

#define PUMP_STOP   0
#define PUMP_READY  1
#define PUMP_OPEN   2

#define PUMP_ON		1
#define PUMP_OFF	0



typedef struct
{
	uint8_t dRationNum;			//��ǰ������
	uint8_t readyCountDown;     //׼������ʱ
	uint8_t  stepRevolve;		//ת������       0.ֹͣ   1.׼��   2.ת��
	uint8_t setMode;   			//����ģʽ
	uint16_t dRationValue;		//��ǰ��ʾ��ֵ
	uint16_t t_dRationValue;	//��ʱ����ֵ
	uint16_t flowValue;			//����ֵ	
	uint16_t rationValue[8];	//��������ֵ	
	uint32_t timeCount;			//ʱ���ʱ
	uint32_t targetTimeCount;	//Ŀ��ʱ���ʱ
	
}FlowInfo_t;


#define PUMP_OUT        P01 

void PoweroffMemoryInit(void);
void FlowProcess(void);
void KeyProcess(void);


#endif


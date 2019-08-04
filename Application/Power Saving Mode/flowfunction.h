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
	uint16_t rationValue[8];	//��������ֵ
	uint16_t dRationValue;		//��ǰ��ʾ��ֵ		
	
	uint32_t t_dRationValue;	//��ʱ����ֵ
	uint32_t pumpTimeCount;		//ʱ���ʱ
	uint32_t tpumpTimeCount;	//Ŀ��ʱ���ʱ
	uint32_t flowCount;			//ת�Ӵ���������ֵ	
	uint32_t tFlowCount;	    //Ŀ��ת�Ӵ���������ֵ	
	
}FlowInfo_t;


#define PUMP_OUT        P36

void PoweroffMemoryInit(void);
void FlowProcess(void);
void KeyProcess(void);


#endif


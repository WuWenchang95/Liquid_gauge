#ifndef FLOW_FUNCTION_H
#define FLOW_FUNCTION_H

#define PUMP_STOP   0
#define PUMP_READY  1
#define PUMP_OPEN   2

#define PUMP_ON		1
#define PUMP_OFF	0



typedef struct
{
	uint8_t dRationNum;			//当前定量号
	uint8_t readyCountDown;     //准备倒计时
	uint8_t  stepRevolve;		//转动步骤       0.停止   1.准备   2.转动
	uint8_t setMode;   			//设置模式
	uint16_t dRationValue;		//当前显示的值
	uint16_t t_dRationValue;	//临时定量值
	uint16_t flowValue;			//流动值	
	uint16_t rationValue[8];	//各个定量值	
	uint32_t timeCount;			//时间计时
	uint32_t targetTimeCount;	//目标时间计时
	
}FlowInfo_t;


#define PUMP_OUT        P01 

void PoweroffMemoryInit(void);
void FlowProcess(void);
void KeyProcess(void);


#endif


//PCON∂®“Â
#define STOP		(1<<1) 
#define IDLE		(1<<0) 


void StopEnter(void);
unsigned char CheckStopCondition(void);
void IdleEnter(void);
unsigned char CheckIdleCondition(void);
void PowerSaving_Loop(void);
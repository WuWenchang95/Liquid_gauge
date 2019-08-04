#ifndef PWM_H
#define PWM_H
//PWMCON
#define TIE(N)			(N<<7)
#define ZIE(N)			(N<<6)
#define PIE(N)			(N<<5)
#define NIE(N)			(N<<4)
#define MS(N)				(N<<3)		//中心对齐模式
#define MOD(N)			(N<<0)	    //互补模式

#define CKS(N)			(N)	//N=0~6


//PWMCFG
#define TOG(N)			(N<<7)


//PWMAIF   PWMBIF	PWMCIF 	PWMDIF
#define  TIF1	(1<<7)
#define  ZIF1	(1<<6)
#define  PIF1	(1<<5)
#define  NIF1	(1<<4)
#define  TIF0	(1<<3)
#define  ZIF0	(1<<2)
#define  PIF0	(1<<1)
#define  NIF0	(1<<0)	


typedef enum {
	PWM_CH0 = 0,
	PWM_CH1 = 1,
	PWM_CH2 = 2,
	PWM_CH3 = 3,
	PWM_CH4 = 4,
	PWM_CH5 = 5,
	PWM_CH6 = 6,
	PWM_CH7 = 7,
} PWM_Channel_TypeDef;

typedef enum {
	Edge_Align = 0,
	Center_Align = 1,
} PWM_AlignMod_TypeDef;

typedef enum {
	CKS_SYS	= 0,
	CKS_IH	= 1,
	CKS_IL	= 2,
	CKS_XH	= 3,
	CKS_XL	= 4,
	CKS_PLL	= 5,
	CKS_TF	= 6,
} PWM_CKS_TypeDef;

typedef enum {
	Toggle_Off = 0,
	Toggle_On = 1,
} PWM_Toggle_TypeDef;

typedef enum {
	Complementary_Off = 0,
	Complementary_On = 1,
} PWM_Complementary_TypeDef ;

typedef enum {
	TIE_Off = 0,
	TIE_On = 1,
} PWM_TIE_TypeDef;

typedef enum {
	ZIE_Off = 0,
	ZIE_On = 1,
} PWM_ZIE_TypeDef;

typedef enum {
	PIE_Off = 0,
	PIE_On = 1,
} PWM_PIE_TypeDef;

typedef enum {
	NIE_Off = 0,
	NIE_On = 1,
} PWM_NIE_TypeDef;


void PWM_init(PWM_Channel_TypeDef pwm_ch,PWM_AlignMod_TypeDef AlignMod,PWM_CKS_TypeDef cks,
							PWM_Toggle_TypeDef toggle,unsigned char Prescal,PWM_Complementary_TypeDef Comp);
void PWM_CfgDivDuty(PWM_Channel_TypeDef pwm_ch,unsigned int DIV,unsigned int DUT);
void PWM_Update(unsigned char PWMCH_Bits);
void PWM_EnContrl(unsigned char PWMEn_Bits);
void WaitPWMUpdateComplete(void);
void PWM_init_Set(void);

#endif
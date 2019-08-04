#ifndef RTC_H
#define RTC_H
//RTCON定义
#define RTCE(N)		(N<<7) 
#define MSE(N)		(N<<6) 
#define HSE(N)		(N<<5)
#define SCE(N)		(N<<4)
#define MCE(N)		(N<<3)
#define HCE(N)		(N<<2)
#define RTCWE(N)	(N<<1)


//RTCIF定义
#define RTC_MF		(1<<2)
#define RTC_HF		(1<<1)
#define RTC_AF		(1<<0)

typedef enum {
	RTCE_Off = 0,
	RTCE_On = 1,
} RTCE_TypeDef;

typedef enum {
	MSE_Off = 0,
	MSE_On = 1,
} MSE_TypeDef;

typedef enum {
	HSE_Off = 0,
	HSE_On = 1,
} HSE_TypeDef;

typedef enum {
	SCE_Off = 0,
	SCE_On = 1,
} SCE_TypeDef;

typedef enum {
	MCE_Off = 0,
	MCE_On = 1,
} MCE_TypeDef;

typedef enum {
	HCE_Off = 0,
	HCE_On = 1,
} HCE_TypeDef;

typedef enum {
	RTM_Off = 0,
	RTM_On = 1,
} RTM_TypeDef;

void RTC_init(RTCE_TypeDef rtce,MSE_TypeDef mse,HSE_TypeDef hse);
void RTC_WriteSecond(unsigned char second);
void RTC_WriteMinute(unsigned char minute);
void RTC_WriteHour(unsigned char hour);
void RTC_WriteWeek(unsigned char week);
void RTC_AlarmCfg(HCE_TypeDef hce,MCE_TypeDef mce,SCE_TypeDef sce);
void RTC_SetAlaramTime(unsigned char al_hr,unsigned char al_min,unsigned char al_sec);
void RTC_SetRTMSS(unsigned char rtmss);
void RTC_ReadTime(void);
void RTM_init(unsigned char _hour, unsigned char _sec, unsigned char _minute);
void RTM_Con(RTM_TypeDef rtm);



#endif

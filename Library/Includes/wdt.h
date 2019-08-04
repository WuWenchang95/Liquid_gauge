#ifndef WDT_H
#define WDT_H
//WDCON定义
#define WDTS(N)		(N<<5) //N=0~6
#define WDRE(N)		(N) //N=0~1

//WDFLG定义
#define WDIF		(1<<1)
#define WDRF		(1<<0)



typedef enum {
  WDTS_DISABLE  	= 0,
  WDTS_IRCH  	= 1,
  WDTS_IRCL  	= 2,
  WDTS_XOSCH  	= 3,
  WDTS_XOSCL  	= 4,
  WDTS_PLL  	= 5,
  WDTS_TFRC  	= 6,
} WDTS_TypeDef;

typedef enum {
  WDRE_int  	= 0,
  WDRE_reset  	= 1,
} WDRE_TypeDef;

void WDT_init(WDTS_TypeDef wdts,WDRE_TypeDef wdre,unsigned int wdtvh);
void WDT_FeedDog(void);
#endif

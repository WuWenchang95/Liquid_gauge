//SMCON定义
#define SMEN_DISABLE	(0<<5)
#define SMEN_SYS_CLK	(1<<5)
#define SMEN_IRCH		(2<<5)
#define SMEN_IRCL		(3<<5)
#define SMEN_XOSCH		(4<<5)
#define SMEN_XOSCL		(5<<5)
#define SMEN_PLL		(6<<5)
#define SMEN_TFRC		(7<<5)

#define SMIE(N)			(N<<4)
#define SMOE(N)			(N<<3)
#define SMMD(N)			(N)	   //N=0-3



//SMSTA定义
#define   SMEF	(1<<1)
#define   SMOF	(1<<0)


#ifndef _TM1640_C_
#define _TM1640_C_

#include "ca51f_config.h"
#if (IC_TYPE == IC_TYPE_CA51F2X)
#include "../../includes/ca51f2sfr.h"
#include "../../includes/ca51f2xsfr.h"
#include "../../includes/gpiodef_f2.h"
#elif (IC_TYPE == IC_TYPE_CA51F3X)
#include "../../includes/ca51f3sfr.h"
#include "../../includes/ca51f3xsfr.h"
#include "../../includes/gpiodef_f3.h"
#endif
#include "../../includes/system.h"
#include "TM1640.h"

#include <intrins.h>


/* 
控制显示 
0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f 分别对应 
1/16,  2/16,  4/16,  10/16, 11/16, 12/16, 13/16, 14/16    
 
设置数据，0x40,0x44分别对应地址自动加一和固定地址模式      
*/  
  
#define SET_DATA_ORDER     		0X40  //数据命令设置 地址自动加1  
#define SET_DISPLAY        		0x8a//0x89//0X8B //0X8F  //显示命令控制  脉冲宽度14/16  
#define SET_DSP1640_DIS      	0x80  //显示关控制  
 
#define     VT_DLY1640              40//450// 80//10//2000  
#define     VT_DLYNOP               20//180//5//80//10 

#define   TM1640_DO     P00
#define   TM1640_CLK    P01


#define    TM1640_DO_LOW        (TM1640_DO = 0)

								   		
#define    TM1640_DO_HIGT       (TM1640_DO = 1)


#define    TM1640_CLK_LOW       (TM1640_CLK = 0)
									
										 
#define    TM1640_CLK_HIGT     (TM1640_CLK = 1)


								   

/*  =========段码与数字对应关系=共阳================
          --a--
          |     |
          f     b
          --g--
	  |     |
          e     c
          --d-- *
 */
	/*	=========段码与数字对应关系=共阳================
			  --a--					f  a b g c dp d e				
			  | 	|       		dp g f e d c b a 
			  f 	b				1  1 0 1 1 1 1 0     0xDE  // 0	
			  --g--					1  0 0 0 1 0 0 0     0x88  // 1 
		  | 	|					0  1 0 1 1 1 0 1     0x5d  // 2
			  e 	c				1  0 0 1 1 1 0 1     0x9d  // 3
			  --d-- *   dp			1  0 0 0 1 0 1 1     0x8b  // 4 
			  						1  0 0 1 0 1 1 1     0x97  //5
			  					    1  1 0 1 0 1 1 1     0xd7  //6
			  					    1  0 0 1 1 0 0 0     0x98  //7
			  					    1  1 0 1 1 1 1 1     0xdf  //8
			  					    1  0 0 1 1 1 1 1     0x9f  //9
	 */
									// f  a b g c dp d e				
/*									   1  1 1 0 1 0  1 1      0xeb //0
									   0  0 1 0 1 0  0 0      0x28 //1  
									   0  1 1 1 0 0  1 1      0x73 //2
									   0  1 1 1 1 0  1 0     0x7A //3
									   1  0 1 1 1 0  0 0     0xb8 //4
									   1  1 0 1 1 0  1 0     0xda //5
									   1  1 0 1 1 0  1 1     0xdb  //6
									   0  1 1 0 1 0  0 0     0x68  //7
									   1  1 1 1 1 0  1 1     0xfb  //8
									   1  1 1 1 1 0  1 0     0xFa  //9
*/
//1、共阳：
//char xdata table_YANG[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x88,0x83,0xc6,0xa1,0x86,0x8e};
//2、共阴：
/*
code unsigned char  table_YIN[]={
	0x3f,//0
	0x06,//1
	0x5b,//2
	0x4f,//3
	0x66,//4
	0x6d,//5
	0x7d,//6
	0x07,//7
	0x7f,//8
	0x6f,//9
	0x77,//a
	0x7c,//b
	0x39,//c
	0x5e,//d
	0x79,//e
	0x71,//f
	0X76,//H
	0X38,//L
	0X00//无
};

*/
//2、共阴：

code unsigned char	table_YIN[]={
	0xeb, //0
	0x28, //1  
	0x73, //2
	0x7A, //3
	0xb8, //4
	0xda,//5
	0xdb,	//6
	0x68,	//7
	0xfb,	//8
	0xfa,	//9

	0X00,//无
};



void TM1640Init(void)
{
	GPIO_Init(P00F,OUTPUT );
	GPIO_Init(P01F,OUTPUT );
	
	TM1640_Init(DSP1640_ENB);
}

//*************************************  
// 函数名称：_Nopt  
// 函数功能：延时函数  
// 入口参数：延时时间  
// 出口参数：无  
//***************************************/  
void Nop1640(uint16_t T_Dly)  
{     
    while(T_Dly--)
	{
		_nop_();     
	}
    return ;  
}  
//****************************************  
// 函数名称：TM1640Start  
// 函数功能：TM1640起始信号  
// 入口参数：无  
// 出口参数：无  
//***************************************/  
void TM1640Start(void)  
{   

    TM1640_DO_HIGT;  
    Nop1640(VT_DLYNOP);  
    TM1640_CLK_HIGT;  
    Nop1640(VT_DLY1640);  
    TM1640_DO_LOW;  
    Nop1640(VT_DLY1640);  
    TM1640_CLK_LOW;  
    Nop1640(VT_DLY1640);  

}  
//*************************************  
// 函数名称：TM1640Stop  
// 函数功能：TM1640结束信号  
// 入口参数：无  
// 出口参数：无  
//***************************************/  
void TM1640Stop(void)  
{  
    TM1640_CLK_LOW;  
    Nop1640(VT_DLY1640) ;

    TM1640_DO_LOW;    
    Nop1640(VT_DLYNOP) ;  
    TM1640_CLK_HIGT;  
    Nop1640(VT_DLY1640) ;  
    TM1640_DO_HIGT;  
    Nop1640(VT_DLY1640) ;  
    TM1640_CLK_LOW;  
    Nop1640(VT_DLY1640) ;  

}  


//*************************************  
// 函数名称：TM1640WriteByte  
// 函数功能：TM1640写一字节数据  
// 入口参数：要写的数据  
// 出口参数：无  
//***************************************/  
void TM1640WriteByte(char date)  
{  
        uint8_t i;  
        uint8_t Tmp;  
          
        Tmp=date;  
        
       	TM1640_DO_LOW;    
        Nop1640(VT_DLYNOP) ;  
        TM1640_CLK_LOW;  
          
        for(i=0;i<8;i++)  
        {  
                TM1640_CLK_LOW;
                Nop1640(VT_DLYNOP) ;  
                if(Tmp & 0x01)  
                {  
                        TM1640_DO_HIGT;  
                        Nop1640(VT_DLY1640) ;  
                }  
                else  
                {  
                        TM1640_DO_LOW;    
                        Nop1640(VT_DLY1640) ;  
                }  
                TM1640_CLK_HIGT;  
                Tmp = Tmp>>1;  
        }  
          
        TM1640_CLK_LOW;  
        Nop1640(VT_DLYNOP) ;  
        TM1640_DO_LOW;   

}  
//*************************************  
// 函数名称：TM1640_Init  
// 函数功能：TM1640设备初始化  
// 入口参数：0 1 代表 显示关 显示开  
// 出口参数：无  
//***************************************/  
 void TM1640_Init(char InValue)    
{  

        TM1640_DO_HIGT;  
        Nop1640(VT_DLYNOP);                 
        TM1640_CLK_HIGT;  
          
        if(InValue == DSP1640_DIS) //显示关  
        {  
                TM1640Start();  
                TM1640WriteByte(SET_DSP1640_DIS);//  
                TM1640Stop();         
        }  
        else  
        {  
                TM1640Start();//数据写模式设置  
                TM1640WriteByte(SET_DATA_ORDER);//  
                TM1640Stop();  
                  
                TM1640Start(); //显示亮度设置  
                TM1640WriteByte(SET_DISPLAY); //0x80关显示            
                TM1640Stop();  
        }  

}  

//*************************************  
// 函数名称：TM1640_SendData  
// 函数功能：发送显示缓存  
// 入口参数：1640要写数据的起始地址  
//           显示数据缓存的起始地址 
//			 数据长度  
// 出口参数：无  
//***************************************/  
void TM1640_SendData(char Addr1640,char *a,char DataLong)  
{  
    uint8_t i;    
      
    TM1640Start();  
    TM1640WriteByte(ADDR_START1640+Addr1640); //设置起始地址  
      
    for(i=0;i<DataLong;i++)  
    {  
        TM1640WriteByte(a[i]);   
    }  
    
    TM1640Stop();   
}

#if 0
void TM1640_SendData_test(void)
{
#if 1
//    uint8_t i;   
    uint8_t tmp = 0x80;
	
    TM1640Start(2);  
    TM1640WriteByte(0xc0+15,2); //设置起始地址  
     
    TM1640WriteByte(tmp,2);   
    
    TM1640Stop(2);   
	
#else
    u8 i;    
    u8 a[16] = {0};
    TM1640Start();  
    TM1640WriteByte(0xc0); //设置起始地址  
      
    for(i=0;i<16;i++)  
    {  
        TM1640WriteByte(a[i]);   
    }  
    
    TM1640Stop();   

#endif
}
#endif
#endif



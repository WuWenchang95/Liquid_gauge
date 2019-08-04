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
������ʾ 
0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f �ֱ��Ӧ 
1/16,  2/16,  4/16,  10/16, 11/16, 12/16, 13/16, 14/16    
 
�������ݣ�0x40,0x44�ֱ��Ӧ��ַ�Զ���һ�͹̶���ַģʽ      
*/  
  
#define SET_DATA_ORDER     		0X40  //������������ ��ַ�Զ���1  
#define SET_DISPLAY        		0x8a//0x89//0X8B //0X8F  //��ʾ�������  ������14/16  
#define SET_DSP1640_DIS      	0x80  //��ʾ�ؿ���  
 
#define     VT_DLY1640              40//450// 80//10//2000  
#define     VT_DLYNOP               20//180//5//80//10 

#define   TM1640_DO     P00
#define   TM1640_CLK    P01


#define    TM1640_DO_LOW        (TM1640_DO = 0)

								   		
#define    TM1640_DO_HIGT       (TM1640_DO = 1)


#define    TM1640_CLK_LOW       (TM1640_CLK = 0)
									
										 
#define    TM1640_CLK_HIGT     (TM1640_CLK = 1)


								   

/*  =========���������ֶ�Ӧ��ϵ=����================
          --a--
          |     |
          f     b
          --g--
	  |     |
          e     c
          --d-- *
 */
	/*	=========���������ֶ�Ӧ��ϵ=����================
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
//1��������
//char xdata table_YANG[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x88,0x83,0xc6,0xa1,0x86,0x8e};
//2��������
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
	0X00//��
};

*/
//2��������

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

	0X00,//��
};



void TM1640Init(void)
{
	GPIO_Init(P00F,OUTPUT );
	GPIO_Init(P01F,OUTPUT );
	
	TM1640_Init(DSP1640_ENB);
}

//*************************************  
// �������ƣ�_Nopt  
// �������ܣ���ʱ����  
// ��ڲ�������ʱʱ��  
// ���ڲ�������  
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
// �������ƣ�TM1640Start  
// �������ܣ�TM1640��ʼ�ź�  
// ��ڲ�������  
// ���ڲ�������  
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
// �������ƣ�TM1640Stop  
// �������ܣ�TM1640�����ź�  
// ��ڲ�������  
// ���ڲ�������  
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
// �������ƣ�TM1640WriteByte  
// �������ܣ�TM1640дһ�ֽ�����  
// ��ڲ�����Ҫд������  
// ���ڲ�������  
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
// �������ƣ�TM1640_Init  
// �������ܣ�TM1640�豸��ʼ��  
// ��ڲ�����0 1 ���� ��ʾ�� ��ʾ��  
// ���ڲ�������  
//***************************************/  
 void TM1640_Init(char InValue)    
{  

        TM1640_DO_HIGT;  
        Nop1640(VT_DLYNOP);                 
        TM1640_CLK_HIGT;  
          
        if(InValue == DSP1640_DIS) //��ʾ��  
        {  
                TM1640Start();  
                TM1640WriteByte(SET_DSP1640_DIS);//  
                TM1640Stop();         
        }  
        else  
        {  
                TM1640Start();//����дģʽ����  
                TM1640WriteByte(SET_DATA_ORDER);//  
                TM1640Stop();  
                  
                TM1640Start(); //��ʾ��������  
                TM1640WriteByte(SET_DISPLAY); //0x80����ʾ            
                TM1640Stop();  
        }  

}  

//*************************************  
// �������ƣ�TM1640_SendData  
// �������ܣ�������ʾ����  
// ��ڲ�����1640Ҫд���ݵ���ʼ��ַ  
//           ��ʾ���ݻ������ʼ��ַ 
//			 ���ݳ���  
// ���ڲ�������  
//***************************************/  
void TM1640_SendData(char Addr1640,char *a,char DataLong)  
{  
    uint8_t i;    
      
    TM1640Start();  
    TM1640WriteByte(ADDR_START1640+Addr1640); //������ʼ��ַ  
      
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
    TM1640WriteByte(0xc0+15,2); //������ʼ��ַ  
     
    TM1640WriteByte(tmp,2);   
    
    TM1640Stop(2);   
	
#else
    u8 i;    
    u8 a[16] = {0};
    TM1640Start();  
    TM1640WriteByte(0xc0); //������ʼ��ַ  
      
    for(i=0;i<16;i++)  
    {  
        TM1640WriteByte(a[i]);   
    }  
    
    TM1640Stop();   

#endif
}
#endif
#endif



#ifndef _TM1640_H
#define _TM1640_H

#define TM1640_1   1
#define TM1640_2   2
#define TM1640_3   3 



#define   ADDR_START1640        (0XC0) 
  
#define     DSP1640_DIS     0  
#define     DSP1640_ENB     1  
  


 
code unsigned char  table_YIN[];


void TM1640Init(void);
void TM1640_Init(char InValue);
void TM1640_SendData(char Addr1640,char *a,char DataLong);
void TM1640_SendData_test(void);


#endif


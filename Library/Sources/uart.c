#ifndef _UART_C_
#define _UART_C_
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
#include "../../Library/includes/uart.h"
#include <intrins.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <absacc.h>
/*********************************************************************************************************************/
/*********************************************************************************************************************/
#ifdef UART0_EN


void Uart0_Initial(unsigned long int baudrate)
{
	unsigned int value_temp;

	GPIO_Init(P31F,P31_UART0_RX_SETTING);
	GPIO_Init(P30F,P30_UART0_TX_SETTING);
	
	uart0_send.head=0;
	uart0_send.tail=0;
	uart0_rev.head=0;
	uart0_rev.tail=0;
	uart0_tx_flag=0;
	uart0_rx_finish_flag=0;
/************************************************************************************************************************/
//TIMER2作为UART0的波特率发生器
	value_temp = 0x10000 - FOSC/(baudrate*32);
	T2CON = 	0x24;
	T2CH  = 	(unsigned char)(value_temp>>8);
	T2CL  = 	(unsigned char)(value_temp);
	TH2   = 	(unsigned char)(value_temp>>8);
	TL2   = 	(unsigned char)(value_temp);;	
	TR2   = 	1;
/************************************************************************************************************************/	


/************************************************************************************************************************/
//TIMER1作为UART0的波特率发生器

//  TMOD = (TMOD&0xCF)|0x20;
//	TH1 = 0xff;		//19200
//	TL1 = 0xff;
//	ET1=0;
//	TR1=1;		
//	PCON |= 0x80;	 	
/************************************************************************************************************************/


	S0CON = 0x50;	 
	ES0 = 1;
}
void Uart0_PutChar(unsigned char bdat)
{
	unsigned char free_space;
	unsigned char tail_tmp;
	while(1)
	{				
		tail_tmp = uart0_send.tail;	
		if(uart0_send.head < tail_tmp)
		{
			free_space = tail_tmp - uart0_send.head;
		}
		else
		{
			free_space = UART0_TX_BUF_SIZE + tail_tmp - uart0_send.head;
		}		
		if(free_space > 1)
		{
			ES0 = 0; 
			uart0_send.head++;
			uart0_send.head %= UART0_TX_BUF_SIZE;
			uart0_tx_buf[uart0_send.head] = bdat;			
			if(!uart0_tx_flag)
			{
				ES0 = 1;				
				uart0_send.tail++;
				uart0_send.tail %= UART0_TX_BUF_SIZE;		
				S0BUF=uart0_tx_buf[uart0_send.tail];				
				uart0_tx_flag = 1;		
			}
			else
			{
				ES0 = 1;	
			}			
			break;
		}
	}
}
void UART0_ISR (void) interrupt 4
{	
	if(RI0)
	{
		RI0 = 0;

		uart0_rev.head %= UART0_RX_BUF_SIZE;
		uart0_rx_buf[uart0_rev.head]=S0BUF;
		if((uart0_rx_buf[uart0_rev.head-1] == 0x0d) && (uart0_rx_buf[uart0_rev.head] == 0x0a))
//		if(uart0_rx_buf[uart0_rev.head] == 0x0A)
			uart0_rx_finish_flag = 1;	
		//S0BUF = uart0_rx_buf[uart0_rev.head];
		uart0_rev.head++;
	}
	if(TI0)
	{	
		TI0 = 0;		
		if(uart0_send.head!=uart0_send.tail)
		{
			uart0_send.tail++;
			uart0_send.tail %= UART0_TX_BUF_SIZE;
			S0BUF=uart0_tx_buf[uart0_send.tail];				
		}
		else
		{
			uart0_tx_flag=0;
		}	
	}
}
#endif
#ifdef UART1_EN
void Uart1_Initial(unsigned long int baudrate)
{
	unsigned int value_temp;

	uart1_send.head=0;
	uart1_send.tail=0;
	uart1_rev.head=0;
	uart1_rev.tail=0;
	uart1_tx_flag=0;

	value_temp = 0x400 - FOSC/(baudrate*32);

#if (IC_TYPE == IC_TYPE_CA51F3X)
	GPIO_Init(P12F,P12_UART1_RX_SETTING);
	GPIO_Init(P11F,P11_UART1_TX_SETTING);
#elif  (IC_TYPE == IC_TYPE_CA51F2X)
	GPIO_Init(P67F,P67_UART1_RX_SETTING);
	GPIO_Init(P66F,P66_UART1_TX_SETTING);
#endif

	S1RELH = (unsigned char)(value_temp>>8);
	S1RELL = (unsigned char)(value_temp);
	
	S1CON = 0xD0;
	ES1 =	1;	
}
void Uart1_PutChar(unsigned char bdat)
{
	unsigned char free_space;
	unsigned char tail_tmp;
	while(1)
	{		
		tail_tmp = uart1_send.tail;
		if(uart1_send.head < tail_tmp)
		{
			free_space = tail_tmp - uart1_send.head;
		}
		else
		{
			free_space = UART1_TX_BUF_SIZE + tail_tmp - uart1_send.head;
		}		
		if(free_space > 1)
		{
			ES1 = 0; 
			uart1_send.head++;
			uart1_send.head %= UART1_TX_BUF_SIZE;
			uart1_tx_buf[uart1_send.head] = bdat;
			if(!uart1_tx_flag)
			{
				ES1 = 1;
				uart1_send.tail++;
				uart1_send.tail %= UART1_TX_BUF_SIZE;		
				S1BUF = uart1_tx_buf[uart1_send.tail];				
				uart1_tx_flag = 1;		
			}
			else
			{
				ES1 = 1;	
			}			
			break;
		}
	}
}
void UART1_ISR (void) interrupt 6	
{
	if(S1CON & BIT0)
	{
		S1CON = (S1CON&~(BIT0|BIT1))|BIT0;			 
		uart1_rev.head++;
	   	uart1_rev.head %= UART1_RX_BUF_SIZE;
		uart1_rx_buf[uart1_rev.head]=S1BUF;
	}
	if(S1CON & BIT1)
	{
		S1CON = (S1CON&~(BIT0|BIT1))|BIT1;	
		if(uart1_send.head!=uart1_send.tail)
		{
			uart1_send.tail++;
			uart1_send.tail %= UART1_TX_BUF_SIZE;
			S1BUF=uart1_tx_buf[uart1_send.tail];				
		}
		else
		{
			uart1_tx_flag=0;
		}		
	}
}
#endif

#ifdef UART2_EN
void Uart2_Initial(unsigned long int baudrate)
{
	unsigned int value_temp;

	uart2_send.head=0;
	uart2_send.tail=0;
	uart2_rev.head=0;
	uart2_rev.tail=0;
	uart2_tx_flag=0;

	GPIO_Init(P60F,P60_UART2_RX_SETTING);
	GPIO_Init(P61F,P61_UART2_TX_SETTING);

	value_temp = 0x400 - FOSC/(baudrate*32);
	S2RELH = (unsigned char)(value_temp>>8);
	S2RELL = (unsigned char)(value_temp);
	
	S2CON = 0xD0;
	INT3EN =	1;	
}
void Uart2_PutChar(unsigned char bdat)
{
	unsigned char free_space;
	unsigned char tail_tmp;
	while(1)
	{		
		tail_tmp = uart2_send.tail;
		if(uart2_send.head < tail_tmp)
		{
			free_space = tail_tmp - uart2_send.head;
		}
		else
		{
			free_space = UART2_TX_BUF_SIZE + tail_tmp - uart2_send.head;
		}		
		if(free_space > 1)
		{
			INT3EN = 0; 
			uart2_send.head++;
			uart2_send.head %= UART2_TX_BUF_SIZE;
			uart2_tx_buf[uart2_send.head] = bdat;			
			if(!uart2_tx_flag)
			{
				INT3EN = 1;
				uart2_send.tail++;
				uart2_send.tail %= UART2_TX_BUF_SIZE;			
				S2BUF = uart2_tx_buf[uart2_send.tail];				
				uart2_tx_flag = 1;		
			}
			else
			{
				INT3EN = 1;	
			}			
			break;
		}
	}
}
void UART2_ISR (void) interrupt 8	
{
	if(S2CON & BIT0)
	{
		S2CON = (S2CON&~(BIT0|BIT1))|BIT0;			 
		uart2_rev.head++;
		uart2_rev.head %= UART2_RX_BUF_SIZE;
		uart2_rx_buf[uart2_rev.head]=S2BUF;
	}
	if(S2CON & BIT1)
	{
		S2CON = (S2CON&~(BIT0|BIT1))|BIT1;	
		if(uart2_send.head!=uart2_send.tail)
		{
			uart2_send.tail++;
			uart2_send.tail %= UART2_TX_BUF_SIZE;
			S2BUF=uart2_tx_buf[uart2_send.tail];				
		}
		else
		{
			uart2_tx_flag=0;
		}		
	}
}
#endif
#ifdef PRINT_EN
	#ifdef UART0_PRINT
		#define Uart_PutChar	Uart0_PutChar
	#elif defined  UART1_PRINT
		#define Uart_PutChar	Uart1_PutChar
	#elif defined  UART2_PRINT
		#define Uart_PutChar	Uart2_PutChar
	#endif
	
void UartPutStr(char *str)
{
	while(*str)
	{	
 		Uart_PutChar(*str++);
	}
}
void uart_printf(char *fmt,...) 
{
    va_list ap;
    char xdata string[256];
    va_start(ap,fmt);
    vsprintf(string,fmt,ap);
    UartPutStr(string);
    va_end(ap);
}
#endif
/*********************************************************************************************************************/
#endif
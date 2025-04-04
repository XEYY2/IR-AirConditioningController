#ifndef _usart_H
#define _usart_H
#include "stm32f10x.h"                  // Device header

#if SYSTEM_SUPPPORT_RTOS
#include "FreeRTOS.h"
#endif

#define USART1_REC_LEN		200  	//设置UATR1的最大数据长度 200
#define USART3_REC_LEN		100  	//设置UATR1的最大数据长度 7

extern u8  USART1_RX_BUF[USART1_REC_LEN]; //外部变量为USART_REC_LEN长度的一个数组 
extern u16 USART1_RX_STA;         		//定义16为数据接收状态信息变量

extern u8  USART3_RX_BUF[USART3_REC_LEN]; //外部变量为USART_REC_LEN长度的一个数组 
extern u16 USART3_RX_STA;         		//定义16为数据接收状态信息变量

void USART1_Init(u32 bound);
void USART3_Init(u32 bound);
void send_float_via_uart(USART_TypeDef* USARTx, float data);


#endif



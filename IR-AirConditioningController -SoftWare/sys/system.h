#ifndef __system_H
#define __system_H
/*System Head doc*/
#include "stm32f10x.h"                  // Device header
#include "delay.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "usart.h"
#include "iic.h"

/*FreeRtos lib*/
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
 typedef enum
{
		State_OFF,
		State_Low,
		State_High,
}RH_State;
typedef enum {Auto,cold,DeHumi,Heat,SentWind}ConducterMode;
//RH_State RH_app_Sta = State_OFF;
typedef struct IR_S
{
	u8 IR_Sent_Data[3];//依次A B C,A固定0xB2，B的高3位控制风速，低5五位保留1，
	  //高3位101为自动风，本程序风速默认自动风
	ConducterMode mode;
	u8 temp;
}IR_Sdate;
typedef struct SystemData
{
	   float curtemp;
	   float curhumid;
	   RH_State RH_Sta;
	   IR_Sdate IR_Sent_Data;
}SysData;


extern SysData SYS_Glabal_varible;
extern IR_Sdate IR_SentData;

#define SYSTEM_SUPPPORT_RTOS 1//FRTOS开关标准位
//#define configTICK_RATE_HZ   1000,Freertos配置里已定义，此处不在定义

/*  IIC_SCL时钟端口、引脚定义 */
#define IIC_SCL_PORT 			GPIOB   
#define IIC_SCL_PIN 			(GPIO_Pin_8)
#define IIC_SCL_PORT_RCC		RCC_APB2Periph_GPIOB

/*  IIC_SDA时钟端口、引脚定义 */
#define IIC_SDA_PORT 			GPIOB  
#define IIC_SDA_PIN 			(GPIO_Pin_9)
#define IIC_SDA_PORT_RCC		RCC_APB2Periph_GPIOB

#define ManualSetSYSCLK		72

//#define IIC_SCL *((volatile unsigned long*)(0x42000000+((GPIOB_BASE+12)-0x40000000)*32+8*4))//写PB8
//#define IIC_SDA *((volatile unsigned long*)(0x42000000+((GPIOB_BASE+12)-0x40000000)*32+9*4))//写PB9
//#define PB10 *((volatile unsigned long*)(0x42000000+((GPIOB_BASE+12)-0x40000000)*32+10*4))//写PB10
//#define READ_SDA  *((volatile unsigned long*)(0x42000000+((GPIOB_BASE+8)-0x40000000)*32+9*4))//读PB9
//unsigned short int returntimeline(void);
void SYSCLK_Config(void);

void PeriodicTask(void *pvParameters);
void TriggeredTask(void *pvParameters);
void MainTask(void *pvParameters);
#endif 

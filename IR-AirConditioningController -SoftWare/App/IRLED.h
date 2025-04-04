#ifndef __IRLED_H
#define __IRLED_H
#include "stm32f10x.h"                  // Device header
#include "delay.h"
#include "system.h"

/* 红外接收相关定义 */
#define IR_TIMER               TIM3
#define IR_TIMER_CHANNEL       TIM_Channel_1
#define IR_GPIO_PORT           GPIOB
#define IR_GPIO_PIN            GPIO_Pin_4
#define IR_CLOCK_FREQ          36000000  // 系统时钟72MHz,此处我设置位36MHZ是应为懒的改，现在定时计数器每计一个数过0.5微妙
#define IR_Lead_MAX            18500
#define IR_Lead_MIN            15000
#define IR_DATA_1_MAX          4500
#define IR_DATA_1_MIN          4100
#define IR_DATA_0_MAX          2300
#define IR_DATA_0_MIN          2000
#define SEPARATION_MAX         13000
#define SEPARATION_MIN         10000
//空调相关控制参数，R05D协议
#define IR_A_Data                   0xB2
#define IR_B_Windspeed_Auto         0xBF
#define IR_B_Windspeed_Low          0x9F
#define IR_B_Windspeed_Mid 		      0x5F
#define IR_B_Windspeed_High		      0x3F
#define IR_B_OFF                    0x7B
//工作模式只低占3-4位，高4位的温度区域先保留为0
#define IR_C_OperationMode_Auto			0x08
#define IR_C_OperationMode_cold			0x00
#define IR_C_OperationMode_DeHumi		0x04
#define IR_C_OperationMode_Heat		  0x0C
#define IR_C_OperationMode_SentWind	0x04 //送风模式没有温度代码
//温度控制参数,低4位都保留为0
#define IR_C_temp_17					0x00
#define IR_C_temp_18					0x10
#define IR_C_temp_19					0x30
#define IR_C_temp_20					0x20
#define IR_C_temp_21					0x60
#define IR_C_temp_22					0x70
#define IR_C_temp_23					0x50
#define IR_C_temp_24					0x40
#define IR_C_temp_25					0xC0
#define IR_C_temp_26					0xD0
#define IR_C_temp_27					0x90
#define IR_C_temp_28					0x80
#define IR_C_temp_29					0xA0
#define IR_C_temp_30					0xB0
#define IR_C_temp_Nop					0xE0
#define IR_C_OFF              0xE0
/* 时间数据buffer */

extern u16 IR_Buffer_Row[125];
void IR_TIM_Init(void);
u8 decode_A(u16* rowdata);
u8 decode_NA(u16* rowdata);
u8 decode_B(u16* rowdata);
u8 decode_NB(u16* rowdata);
u8 decode_C(u16* rowdata);
u8 decode_NC(u16* rowdata);
u8 Decode_IRDta(void);
u8  Lead_cheak(u16* rowdata);
u8 Separation_cheak(u16* rowdata);

	

void IR_Init(void);
void IR_TIM3_PWM_Init(void) ;
void IRLED_GPIO_Init(void);
void Normal_Code(u8 A, u8 B, u8 C);
void TIM3_SETLOW(void);
void TIM3_SETHIG(void);
void Lead_Code(void);
void Stop_Code(void);
void Send_0_Code(void);
void Send_1_Code(void);
void Send_Byte(u8 data);
u8 ConducterControl(ConducterMode mode ,u8 temp);
//     Normal_Code(0xB2, 0x9F, 0x00);//开机 17度
#endif

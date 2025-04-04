#include "stm32f10x.h"                  // Device header
/*FreeRtos lib*/
#include "FreeRTOS.h"
#include "task.h"

#ifndef __delay_H
#define __delay_H

void delay_Init(u8 SYSCLK);
void delay_us(uint32_t nus);
void delay_ms(uint32_t nms);
void delay_xms(uint32_t nms);
void Clock_Test(uint32_t ticks);
#endif 

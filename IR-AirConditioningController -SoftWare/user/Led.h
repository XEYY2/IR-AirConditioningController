#ifndef __Led_H
#define __Led_H
#include "stm32f10x.h"                  // Device header
#define LED_1 *((volatile unsigned long*)(0x42000000+((GPIOB_BASE+12)-0x40000000)*32+5*4))//LED0,PB5
#define LCD_LED *((volatile unsigned long*)(0x42000000+((GPIOE_BASE+12)-0x40000000)*32+5*4))//LED1,PE5
void Ledflow(void);

#endif 

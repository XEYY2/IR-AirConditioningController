#ifndef __Humidifier_H
#define __Humidifier_H
#include "stm32f10x.h"                  // Device header
#include "system.h"
 void RH_GPIO_Init(void);
 void PressKey(u8 num);
 void RH_Control(RH_State sta);
#endif

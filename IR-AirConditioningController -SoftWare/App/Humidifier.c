#include "Humidifier.h"
#include "stdio.h"

void RH_GPIO_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_5;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	GPIO_ResetBits(GPIOB, GPIO_Pin_5);
}
void PressKey(u8 num)
{
	while(num--)
	{
		GPIO_SetBits(GPIOA, GPIO_Pin_5);
		delay_ms(500);
		GPIO_ResetBits(GPIOB, GPIO_Pin_5);
		delay_ms(500);
	}
}
void RH_Control(RH_State sta)
{
	RH_State laststa;
   if(sta!=laststa)
	{
		switch(laststa)
		{
			case State_Low: //…œ¥Œ «µÕ
			{
				if(sta==State_OFF)
				PressKey(2);
				else if(sta==State_High)
				PressKey(1);
			}
			case State_OFF:
			{
				if(sta==State_Low)
				PressKey(1);
				else if(sta==State_High)
				PressKey(2);
			}
			case State_High :
			{
				if(sta==State_OFF)
				PressKey(1);
				else if(sta==State_Low)
				PressKey(2);
			}
			default: printf("Error!\n");
		}
	}
	laststa=sta;
}

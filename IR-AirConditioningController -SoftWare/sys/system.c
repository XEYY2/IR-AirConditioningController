#include "stm32f10x.h"                  // Device header
#include "FreeRTOS.h"
#include "task.h"

uint8_t GetPinstate(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
	uint8_t pin_state = (GPIOx->ODR & GPIO_Pin) ? 1 : 0;
	return pin_state;
}
void SYSCLK_Config(void)
{
	RCC_DeInit();
	RCC_HSEConfig(RCC_HSE_ON);
	if (RCC_WaitForHSEStartUp() == SUCCESS)
    {
        // 关闭HSI（可选）
        RCC_HSICmd(DISABLE);
        
        // 配置PLL：HSE分频1（不分频）后作为PLL输入，倍频9倍
        // 注意：根据具体型号选择分频系数，此处为STM32F1系列典型配置
        RCC_PLLConfig(RCC_PLLSource_HSE_Div2, RCC_PLLMul_9);
        
        // 启动PLL并等待就绪
        RCC_PLLCmd(ENABLE);
        while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
        
        // 切换系统时钟到PLL输出
        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
        
        // 验证时钟切换是否成功
        while(RCC_GetSYSCLKSource() != 0x08);
        
        // 配置总线分频（关键修改点）
        RCC_HCLKConfig(RCC_SYSCLK_Div1);    // AHB 72MHz
        RCC_PCLK1Config(RCC_HCLK_Div2);     // APB1 36MHz（不超过官方限制）
        RCC_PCLK2Config(RCC_HCLK_Div1);     // APB2 72MHz
				
				vPortRaiseBASEPRI();//开启Freertos的中断管理
				NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
				
    }
}

//HSE_VALUE,需要查找修改

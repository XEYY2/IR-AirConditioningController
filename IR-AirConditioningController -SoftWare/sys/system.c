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
        // �ر�HSI����ѡ��
        RCC_HSICmd(DISABLE);
        
        // ����PLL��HSE��Ƶ1������Ƶ������ΪPLL���룬��Ƶ9��
        // ע�⣺���ݾ����ͺ�ѡ���Ƶϵ�����˴�ΪSTM32F1ϵ�е�������
        RCC_PLLConfig(RCC_PLLSource_HSE_Div2, RCC_PLLMul_9);
        
        // ����PLL���ȴ�����
        RCC_PLLCmd(ENABLE);
        while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
        
        // �л�ϵͳʱ�ӵ�PLL���
        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
        
        // ��֤ʱ���л��Ƿ�ɹ�
        while(RCC_GetSYSCLKSource() != 0x08);
        
        // �������߷�Ƶ���ؼ��޸ĵ㣩
        RCC_HCLKConfig(RCC_SYSCLK_Div1);    // AHB 72MHz
        RCC_PCLK1Config(RCC_HCLK_Div2);     // APB1 36MHz���������ٷ����ƣ�
        RCC_PCLK2Config(RCC_HCLK_Div1);     // APB2 72MHz
				
				vPortRaiseBASEPRI();//����Freertos���жϹ���
				NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
				
    }
}

//HSE_VALUE,��Ҫ�����޸�

#include "stm32f10x.h"                  // Device header

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

/* Library includes. */

/* Demo application includes. */
#include "serial.h"
#include <stdio.h>

// 接收缓冲区  
uint8_t receiveBuffer[15]="EMPTY";  
uint16_t bufferIndex = 0;

void SerialPortInit( void )
{
	unsigned long ulWantedBaud=9600;
USART_InitTypeDef USART_InitStructure;
GPIO_InitTypeDef GPIO_InitStructure;


	
	/* If the queue/semaphore was created correctly then setup the serial port
	hardware. */
		/* Enable USART1 clock */
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE );	

		/* Configure USART1 Rx (PA10) as input floating */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_Init( GPIOA, &GPIO_InitStructure );
		
		/* Configure USART1 Tx (PA9) as alternate function push-pull */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_Init( GPIOA, &GPIO_InitStructure );

		USART_InitStructure.USART_BaudRate = ulWantedBaud;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		USART_InitStructure.USART_StopBits = USART_StopBits_1;
		USART_InitStructure.USART_Parity = USART_Parity_No ;
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
		
		USART_Init( USART1, &USART_InitStructure );
		
		USART_Cmd( USART1, ENABLE );	

//NVIC配置
//开启中断输出控制
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
	
	//配置NVIC
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel=USART1_IRQn;   //选择USART2的中断通道
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;		//中断使能
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=1;
	NVIC_Init(&NVIC_InitStruct);

}	

//int fputc(int ch, FILE *f)
//{
//	USART_TypeDef* USARTx =USART1;
//	while((USARTx->SR&(1<<7))==0);//"1<<7"表示01000000，与二进制01000000等价，SR是发送成功标志为，成功为1,不成功就等待
//	USARTx->DR = ch;
//	return ch;
//}

uint8_t* GetBuffers(void)
{
	return receiveBuffer;
}

void USART1_IRQHandler(void)  
{  
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  
    {  
        // 读取接收到的数据  
        uint8_t receivedData = USART_ReceiveData(USART1);  
          
        // 将数据存储在缓冲区中  
        receiveBuffer[bufferIndex++] = receivedData;  
          
        // 如果接收到换行符（\n），则处理缓冲区中的数据  
        if (receivedData == '\n')  
        {  
            bufferIndex = 0; // 重置缓冲区索引  
            // 这里可以添加处理接收到的数据的逻辑  
            //GPIO_ToggleBits(LED_PORT, LED_PIN); // 闪烁LED  
        }  
    }  
}

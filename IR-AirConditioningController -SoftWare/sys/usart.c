#include "usart.h"
#include "stdio.h"
#include "delay.h"
int fputc(int ch,FILE *p)  //该函数用于在使用printf时发送字符到串口
{
	USART_SendData(USART1,(u8)ch);	
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
	return ch;
}
   	
u8 USART1_RX_BUF[USART1_REC_LEN];     //自定义了一个数组，可能用于接收缓冲器
//其他位可能用于表示错误状态或额外的控制信息̬
//bit15接收完成标志位，当接收到完整的数据包（如以特定的结束符结尾）时，该位被置1
//bit14表示是否接收到特定的字符（如回车符\r，即0x0D）
//bit13~0存储接收到的数据长度
u16 USART1_RX_STA=0;       //存储串口接收的状态信息

u8 USART3_RX_BUF[USART3_REC_LEN];     //自定义了一个数组，可能用于接收缓冲器
//其他位可能用于表示错误状态或额外的控制信息̬
//bit15接收完成标志位，当接收到完整的数据包（如以特定的结束符结尾）时，该位被置1
//bit14表示是否接收到特定的字符（如回车符\r，即0x0D）
//bit13~0存储接收到的数据长度
u16 USART3_RX_STA=0;       //存储串口接收的状态信息


/*******************************************************************************
* 函数名   : USART1_Init
* 功能		 : USART1初始化
* 参数     : bound:串口波特率
* 返回值   : 无
*******************************************************************************/ 
void USART1_Init(u32 bound)
{
   //GPIO的IO口、串口及中断管理结构变量定义
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	//开GPIO和串口时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO | RCC_APB2Periph_USART1, ENABLE);
 
	// 重映射USART1到PB6/PB7
  GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE);
	
	/*  GPIO参数设置 */
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6;//TX			   //UART1的TX口设置为PB6
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;	    //推挽输出
	GPIO_Init(GPIOB,&GPIO_InitStructure);  /* 应用GPIO的IO参数 */
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_7;//RX			 //UART1的RX口设置为PB7
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;		  //设置浮空输入
	GPIO_Init(GPIOB,&GPIO_InitStructure); /* 初始化UART1的接收GPIO */
	

	//USART1 结构变量参数设置
	USART_InitStructure.USART_BaudRate = bound;//串口波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//数据帧的位数，此处8位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//停止位数为1位
	USART_InitStructure.USART_Parity = USART_Parity_No;//奇偶效验，此处关
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//硬件流控制选项，既通过再加一条RTS时钟线来防止数据丢失
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//串口模式，输入输出同时使用
	USART_Init(USART1, &USART_InitStructure); //串口参数初始化使用
	
	USART_Cmd(USART1, ENABLE);  //使能UART1 
	
	USART_ClearFlag(USART1, USART_FLAG_TC);//清除UART标准位，在这里是清除UART1的发送完成标志位
		
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//USART_IT_RXNE接收寄存器非空中断，串口中断配置，在这里使能了这个中断

	//Usart1 NVIC 中断管理
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//选择中断函数
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//抢占优先级，数字越低优先级越高
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//等待优先级，在两个中断具备同等的抢占优先级时，等待优先级数字高的优先级越高
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ中断使能
	NVIC_Init(&NVIC_InitStructure);	//初始化优先级
}

/*******************************************************************************
* 函数名      : USART1_IRQHandler
* 功能		    : USART1接收寄存器满了发生中断
* 参数        : 无
* 返回值      : 无
* 描述        : 接收数据，存在USART1_RX_BUF[]数组中，但是接收到回车会进入判断，若只有回车会复位USART1_RX_BUF[]数组从0开始存储，若是回车+换行则不能再接收数据
*******************************************************************************/ 
void USART1_IRQHandler(void)                	
{
	u8 r;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //如果串口接收寄存器非空标准位为1，该中断以开，即已经接收到了数据
	{
		r =USART_ReceiveData(USART1);//(USART1->DR);	//将接收到的数据放入r中
		if((USART1_RX_STA&0x8000)==0)//还没检查到了换行？
		{
			if(USART1_RX_STA&0x4000)//是否检查到回车
			{
				if(r!=0x0a)USART1_RX_STA=0;//若接收到的数据不为换行，则初始化串口接收状态变量，重新接收，直到接收到，0x0a是换行符
				else USART1_RX_STA|=0x8000;	//置串口接收状态变量最高位为1，表接收完成 
			}
			//将USART1_RX_STA|=0x8000;表示此次数据接收任务结束，同时设置此标志位在下次数据输入时使数据进入缓存
			else //未接收到了，回车0X0D
			{	
				if(r==0x0d)USART1_RX_STA|=0x4000;//检查数据是否回车，将第14位置1
				else
				{
					USART1_RX_BUF[USART1_RX_STA&0X3FFF]=r;//将数据放在缓存中，0X3FFF是将0到13位置1，实际按STA来表示数据长度
					USART1_RX_STA++;
					if(USART1_RX_STA>(USART1_REC_LEN-1))USART1_RX_STA=0;//若数据长度超过USART1_REC_LEN，重新开始计数数据长度，覆盖原有数据	  
				}		 
			}
		}   		 
	} 
}

//int fputc(int ch, FILE *f)
//{
//	USART_TypeDef* USARTx =USART1;
//	while((USARTx->SR&(1<<7))==0);//"1<<7"表示01000000，与二进制01000000等价，SR是发送成功标志为，成功为1,不成功就等待
//	USARTx->DR = ch;
//	return ch;
//}

//uint8_t* GetBuffers(void)
//{
//	return receiveBuffer;
//}

//void USART1_IRQHandler(void)  
//{  
//    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  
//    {  
//        // 读取接收到的数据  
//        uint8_t receivedData = USART_ReceiveData(USART1);  
//          
//        // 将数据存储在缓冲区中  
//        USART1_RX_BUF[USART1_RX_STA++] = receivedData;  
//          
//        // 如果接收到换行符（\n），则处理缓冲区中的数据  
//        if (receivedData == '\n')  
//        {  
//            USART1_RX_STA = 0; // 重置缓冲区索引  
//            // 这里可以添加处理接收到的数据的逻辑  
//            //GPIO_ToggleBits(LED_PORT, LED_PIN); // 闪烁LED  
//        }  
//    }  
//}
/*******************************************************************************
* 函数名   : USART3_Init
* 功能		 : USART3初始化
* 参数     : bound:串口波特率
* 返回值   : 无
*******************************************************************************/ 
void USART3_Init(u32 bound)
{
   //GPIO的IO口、串口及中断管理结构变量定义
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	//开GPIO和串口时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	// 重映射USART1到PB6/PB7
    //GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE);
	
	/*  GPIO参数设置 */
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;//UART3的TX口设置为PB10
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;	    //推挽输出
	GPIO_Init(GPIOB,&GPIO_InitStructure);  /* 应用GPIO的IO参数 */
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_11;//RX			 //UART3的RX口设置为PB11
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;		  //设置浮空输入
	GPIO_Init(GPIOB,&GPIO_InitStructure); /* 初始化UART3的接收GPIO */
	

	//USART1 结构变量参数设置
	USART_InitStructure.USART_BaudRate = bound;//串口波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//数据帧的位数，此处8位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//停止位数为1位
	USART_InitStructure.USART_Parity = USART_Parity_No;//奇偶效验，此处关
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//硬件流控制选项，既通过再加一条RTS时钟线来防止数据丢失
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//串口模式，输入输出同时使用
	USART_Init(USART3, &USART_InitStructure); //串口参数初始化使用
	
	USART_Cmd(USART3, ENABLE);  //使能UART1 
	
	USART_ClearFlag(USART3, USART_FLAG_TC);//清除UART标准位，在这里是清除UART1的发送完成标志位
		
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//USART_IT_RXNE接收寄存器非空中断，串口中断配置，在这里使能了这个中断

	//Usart1 NVIC 中断管理
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;//选择中断函数
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//抢占优先级，数字越低优先级越高
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//等待优先级，在两个中断具备同等的抢占优先级时，等待优先级数字高的优先级越高
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ中断使能
	NVIC_Init(&NVIC_InitStructure);	//初始化优先级
}
//发送float4字节
void send_float_via_uart(USART_TypeDef* USARTx, float data) 
{
    uint8_t *bytePtr = (uint8_t*)&data;
    for (int i = 0; i < 4; i++) {
			  delay_ms(100);
        USART_SendData(USART3,bytePtr[i]); // 按内存顺序发送字节
    }
}
void USART3_IRQHandler(void)  
{  
		static u8 FirstBytecheak;
    if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  
    {  
        // 读取接收到的数据  
        uint8_t receivedData = USART_ReceiveData(USART3);  
        if(receivedData==0x47)
           FirstBytecheak=1;
        if(FirstBytecheak==1)				
          USART3_RX_BUF[USART3_RX_STA++] = receivedData; // 将数据存储在缓冲区中   
        if (USART3_RX_STA == 8)  
        {  
            USART3_RX_STA = 0; // 重置缓冲区索引  
					  FirstBytecheak=0 ;
            // 这里可以添加处理接收到的数据的逻辑  
            //GPIO_ToggleBits(LED_PORT, LED_PIN); // 闪烁LED  
        }  
    }  
}

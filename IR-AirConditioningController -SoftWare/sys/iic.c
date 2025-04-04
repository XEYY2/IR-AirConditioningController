#include "iic.h"

/*******************************************************************************
* 函 数 名         : IIC_Init
* 函数功能		       : IIC初始化
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void IIC_Init(void)
{
	// 1. 开启时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	// 2. 配置I2C引脚重映射（PB8->SCL, PB9->SDA）
	GPIO_PinRemapConfig(GPIO_Remap_I2C1, ENABLE);
	// 3. 配置GPIO
	GPIO_InitTypeDef  GPIO_InitStructure;
//	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_OD;
		GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8|GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);


	
	
	I2C_InitTypeDef I2C_InitStructure;
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;              //I2C模式
	I2C_InitStructure.I2C_ClockSpeed = 100000;               //传输速度，最大不能超过400KHZ
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;      //时钟占空比
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;             //主机是否设置应答
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;  //主机地址位数  7位或10位
	I2C_InitStructure.I2C_OwnAddress1 = 0x00;               //主机地址（当STM32位从机时）
	I2C_Init(I2C1, &I2C_InitStructure);
	
	I2C_Cmd(I2C1, ENABLE);
}

/*******************************************************************************
* 函 数 名         : SDA_OUT
* 函数功能		   : SDA输出配置	   
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
//void SDA_OUT(void)
//{
//	GPIO_InitTypeDef  GPIO_InitStructure;
//	
//	GPIO_InitStructure.GPIO_Pin=IIC_SDA_PIN;
//	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
//	GPIO_Init(IIC_SDA_PORT,&GPIO_InitStructure);
//}废弃

/*******************************************************************************
* 函 数 名         : SDA_IN
* 函数功能		   : SDA输入配置	   
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
//void SDA_IN(void)
//{
//	GPIO_InitTypeDef  GPIO_InitStructure;
//	
//	GPIO_InitStructure.GPIO_Pin=IIC_SDA_PIN;
//	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
//	GPIO_Init(IIC_SDA_PORT,&GPIO_InitStructure);
//}废弃

/*******************************************************************************
* 函 数 名         : IIC_Start
* 函数功能		   : 产生IIC起始信号   
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void IIC_Start(I2C_TypeDef* I2Cx)
{
	 I2C_GenerateSTART(I2Cx, ENABLE);
	 IIC_Wait_EVENT(I2Cx, I2C_EVENT_MASTER_MODE_SELECT);//等待总线忙检查通过
}	

/*******************************************************************************
* 函 数 名         : IIC_Stop
* 函数功能		   : 产生IIC停止信号   
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void IIC_Stop(I2C_TypeDef* I2Cx)
{
	I2C_GenerateSTOP(I2Cx, ENABLE);							   	
}

/*******************************************************************************
* 函 数 名         : IIC_Wait_EVENT
* 函数功能		   : 等待事件到来   
* 输    入         : I2C类型，事件
* 输    出         : 无
*******************************************************************************/
void IIC_Wait_EVENT(I2C_TypeDef* I2Cx, uint32_t I2C_EVENT)
{
	uint32_t Timeout;
	Timeout = 10000;
 	while (I2C_CheckEvent(I2Cx,I2C_EVENT) != SUCCESS)
	{
		Timeout --;
		if (Timeout == 0)
		{
			I2C_GenerateSTOP(I2Cx, ENABLE);
			break;
			
		}
	}  
} 

/*******************************************************************************
* 函 数 名         : IIC_Ack
* 函数功能		   : 产生ACK应答  
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void IIC_Ack(I2C_TypeDef* I2Cx)
{
	I2C_AcknowledgeConfig(I2Cx, ENABLE);
}

/*******************************************************************************
* 函 数 名         : IIC_NAck
* 函数功能		   : 产生NACK非应答  
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/		    
void IIC_NAck(I2C_TypeDef* I2Cx)
{
	I2C_AcknowledgeConfig(I2Cx, DISABLE);
}	

/*******************************************************************************
* 函 数 名         : IIC_Send_Byte
* 函数功能		       : IIC发送一个字节 
* 输    入         : Data发送一个字节
* 输    出         : 无
*******************************************************************************/		  
void IIC_Send_Byte(I2C_TypeDef* I2Cx,uint8_t Data)
{ 
	//从机接受到地址确认ack后此事件触发	
  I2C_SendData(I2Cx, Data);                                                //发送数据
	IIC_Wait_EVENT(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED);
} 

/*******************************************************************************
* 函 数 名         : IIC_Read_Byte
* 函数功能		       : IIC读一个字节 
* 输    入         :  
* 输    出         : Data
*******************************************************************************/  
u8 IIC_Read_Byte(I2C_TypeDef* I2Cx)
{
	uint8_t Data;
  IIC_Wait_EVENT(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED);               //检测EV7
	Data = I2C_ReceiveData(I2Cx);
	return Data;
}
void RWAddGenerate(I2C_TypeDef* I2Cx,uint8_t RWtype,uint8_t Address)
{
	if(RWtype==1)//1-Read，接收
	{
		I2C_Send7bitAddress(I2Cx, Address, I2C_Direction_Receiver);
	  IIC_Wait_EVENT(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED);
	}
	else if(RWtype==0)//0-write,发送
	{
		
	I2C_Send7bitAddress(I2Cx,Address, I2C_Direction_Transmitter);   //发送从机地址
	IIC_Wait_EVENT(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);     //检测EV6（发送模式）
	}
}

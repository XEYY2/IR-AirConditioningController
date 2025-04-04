#include "iic.h"

/*******************************************************************************
* �� �� ��         : IIC_Init
* ��������		       : IIC��ʼ��
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void IIC_Init(void)
{
	// 1. ����ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	// 2. ����I2C������ӳ�䣨PB8->SCL, PB9->SDA��
	GPIO_PinRemapConfig(GPIO_Remap_I2C1, ENABLE);
	// 3. ����GPIO
	GPIO_InitTypeDef  GPIO_InitStructure;
//	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_OD;
		GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8|GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);


	
	
	I2C_InitTypeDef I2C_InitStructure;
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;              //I2Cģʽ
	I2C_InitStructure.I2C_ClockSpeed = 100000;               //�����ٶȣ�����ܳ���400KHZ
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;      //ʱ��ռ�ձ�
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;             //�����Ƿ�����Ӧ��
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;  //������ַλ��  7λ��10λ
	I2C_InitStructure.I2C_OwnAddress1 = 0x00;               //������ַ����STM32λ�ӻ�ʱ��
	I2C_Init(I2C1, &I2C_InitStructure);
	
	I2C_Cmd(I2C1, ENABLE);
}

/*******************************************************************************
* �� �� ��         : SDA_OUT
* ��������		   : SDA�������	   
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
//void SDA_OUT(void)
//{
//	GPIO_InitTypeDef  GPIO_InitStructure;
//	
//	GPIO_InitStructure.GPIO_Pin=IIC_SDA_PIN;
//	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
//	GPIO_Init(IIC_SDA_PORT,&GPIO_InitStructure);
//}����

/*******************************************************************************
* �� �� ��         : SDA_IN
* ��������		   : SDA��������	   
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
//void SDA_IN(void)
//{
//	GPIO_InitTypeDef  GPIO_InitStructure;
//	
//	GPIO_InitStructure.GPIO_Pin=IIC_SDA_PIN;
//	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
//	GPIO_Init(IIC_SDA_PORT,&GPIO_InitStructure);
//}����

/*******************************************************************************
* �� �� ��         : IIC_Start
* ��������		   : ����IIC��ʼ�ź�   
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void IIC_Start(I2C_TypeDef* I2Cx)
{
	 I2C_GenerateSTART(I2Cx, ENABLE);
	 IIC_Wait_EVENT(I2Cx, I2C_EVENT_MASTER_MODE_SELECT);//�ȴ�����æ���ͨ��
}	

/*******************************************************************************
* �� �� ��         : IIC_Stop
* ��������		   : ����IICֹͣ�ź�   
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void IIC_Stop(I2C_TypeDef* I2Cx)
{
	I2C_GenerateSTOP(I2Cx, ENABLE);							   	
}

/*******************************************************************************
* �� �� ��         : IIC_Wait_EVENT
* ��������		   : �ȴ��¼�����   
* ��    ��         : I2C���ͣ��¼�
* ��    ��         : ��
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
* �� �� ��         : IIC_Ack
* ��������		   : ����ACKӦ��  
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void IIC_Ack(I2C_TypeDef* I2Cx)
{
	I2C_AcknowledgeConfig(I2Cx, ENABLE);
}

/*******************************************************************************
* �� �� ��         : IIC_NAck
* ��������		   : ����NACK��Ӧ��  
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/		    
void IIC_NAck(I2C_TypeDef* I2Cx)
{
	I2C_AcknowledgeConfig(I2Cx, DISABLE);
}	

/*******************************************************************************
* �� �� ��         : IIC_Send_Byte
* ��������		       : IIC����һ���ֽ� 
* ��    ��         : Data����һ���ֽ�
* ��    ��         : ��
*******************************************************************************/		  
void IIC_Send_Byte(I2C_TypeDef* I2Cx,uint8_t Data)
{ 
	//�ӻ����ܵ���ַȷ��ack����¼�����	
  I2C_SendData(I2Cx, Data);                                                //��������
	IIC_Wait_EVENT(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED);
} 

/*******************************************************************************
* �� �� ��         : IIC_Read_Byte
* ��������		       : IIC��һ���ֽ� 
* ��    ��         :  
* ��    ��         : Data
*******************************************************************************/  
u8 IIC_Read_Byte(I2C_TypeDef* I2Cx)
{
	uint8_t Data;
  IIC_Wait_EVENT(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED);               //���EV7
	Data = I2C_ReceiveData(I2Cx);
	return Data;
}
void RWAddGenerate(I2C_TypeDef* I2Cx,uint8_t RWtype,uint8_t Address)
{
	if(RWtype==1)//1-Read������
	{
		I2C_Send7bitAddress(I2Cx, Address, I2C_Direction_Receiver);
	  IIC_Wait_EVENT(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED);
	}
	else if(RWtype==0)//0-write,����
	{
		
	I2C_Send7bitAddress(I2Cx,Address, I2C_Direction_Transmitter);   //���ʹӻ���ַ
	IIC_Wait_EVENT(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);     //���EV6������ģʽ��
	}
}

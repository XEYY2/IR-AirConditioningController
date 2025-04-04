#ifndef _iic_H
#define _iic_H                
#include "stm32f10x.h"                  // Device header


//IIC���в�������
void IIC_Init(void);                //��ʼ��IIC��IO��				 
void IIC_Start(I2C_TypeDef* I2Cx);				//����IIC��ʼ�ź�
void IIC_Stop(I2C_TypeDef* I2Cx);	  			//����IICֹͣ�ź�
//void IIC_Send_Byte(u8 txd);			//IIC����һ���ֽ�
void IIC_Wait_EVENT(I2C_TypeDef* I2Cx, uint32_t I2C_EVENT);
void IIC_Send_Byte(I2C_TypeDef* I2Cx,uint8_t Data);
//u8 IIC_Read_Byte(u8 ack);			//IIC��ȡһ���ֽ�
u8 IIC_Read_Byte(I2C_TypeDef* I2Cx);
//u8 IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void IIC_Ack(I2C_TypeDef* I2Cx);					//IIC����ACK�ź�
void IIC_NAck(I2C_TypeDef* I2Cx);				//IIC������ACK�ź�
void RWAddGenerate(I2C_TypeDef* I2Cx,uint8_t RWtype,uint8_t Address);

#endif

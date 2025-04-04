#ifndef _iic_H
#define _iic_H                
#include "stm32f10x.h"                  // Device header


//IIC所有操作函数
void IIC_Init(void);                //初始化IIC的IO口				 
void IIC_Start(I2C_TypeDef* I2Cx);				//发送IIC开始信号
void IIC_Stop(I2C_TypeDef* I2Cx);	  			//发送IIC停止信号
//void IIC_Send_Byte(u8 txd);			//IIC发送一个字节
void IIC_Wait_EVENT(I2C_TypeDef* I2Cx, uint32_t I2C_EVENT);
void IIC_Send_Byte(I2C_TypeDef* I2Cx,uint8_t Data);
//u8 IIC_Read_Byte(u8 ack);			//IIC读取一个字节
u8 IIC_Read_Byte(I2C_TypeDef* I2Cx);
//u8 IIC_Wait_Ack(void); 				//IIC等待ACK信号
void IIC_Ack(I2C_TypeDef* I2Cx);					//IIC发送ACK信号
void IIC_NAck(I2C_TypeDef* I2Cx);				//IIC不发送ACK信号
void RWAddGenerate(I2C_TypeDef* I2Cx,uint8_t RWtype,uint8_t Address);

#endif

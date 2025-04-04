#include "MPU6050.h"
#include "stm32f10x.h" 
#include "iic.h"
#include "delay.h"
//#include "usart.h"   


//��ʼ��MPU6050
//����ֵ:0,�ɹ�
//    ����,�������
u8 MPU6050_Init(void)
{ 
	u8 res;
//	IIC_Init();//��ʼ��IIC����
	MPU6050_Write_Byte(MPU6050_PWR_MGMT1_REG,0X80);	//��λMPU6050
  delay_xms(100);
	MPU6050_Write_Byte(MPU6050_PWR_MGMT1_REG,0X00);	//����MPU6050
	MPU6050_Set_Gyro_Fsr(0);					//�����Ǵ�����,��2000dps
	MPU6050_Set_Accel_Fsr(0);					//���ٶȴ�����,��2g
	MPU6050_Set_Rate(50);						//���ò�����50Hz
	MPU6050_Write_Byte(MPU6050_INT_EN_REG,0X00);	//�ر������ж�
	MPU6050_Write_Byte(MPU6050_USER_CTRL_REG,0X00);	//I2C��ģʽ�ر�
	MPU6050_Write_Byte(MPU6050_FIFO_EN_REG,0X00);	//�ر�FIFO
	MPU6050_Write_Byte(MPU6050_INTBP_CFG_REG,0X80);	//INT���ŵ͵�ƽ��Ч
	res=MPU6050_Read_Byte(MPU6050_DEVICE_ID_REG);
	if(res==MPU6050_ADDR)//����ID��ȷ
	{
		MPU6050_Write_Byte(MPU6050_PWR_MGMT1_REG,0X01);	//����CLKSEL,PLL X��Ϊ�ο�
		MPU6050_Write_Byte(MPU6050_PWR_MGMT2_REG,0X00);	//���ٶ��������Ƕ�����
		MPU6050_Set_Rate(25);						//���ò�����Ϊ50Hz
 	}else return 1;
	return 0;
}
//����MPU6050�����Ǵ����������̷�Χ
//fsr:0,��250dps;1,��500dps;2,��1000dps;3,��2000dps
//����ֵ:0,���óɹ�
//    ����,����ʧ�� 
u8 MPU6050_Set_Gyro_Fsr(u8 fsr)
{
	return MPU6050_Write_Byte(MPU6050_GYRO_CFG_REG,fsr<<3);//���������������̷�Χ  
}
//����MPU6050���ٶȴ����������̷�Χ
//fsr:0,��2g;1,��4g;2,��8g;3,��16g
//����ֵ:0,���óɹ�
//    ����,����ʧ�� 
u8 MPU6050_Set_Accel_Fsr(u8 fsr)
{
	return MPU6050_Write_Byte(MPU6050_ACCEL_CFG_REG,fsr<<3);//���ü��ٶȴ����������̷�Χ  
}
//����MPU6050�����ֵ�ͨ�˲���
//lpf:���ֵ�ͨ�˲�Ƶ��(Hz)
//����ֵ:0,���óɹ�
//    ����,����ʧ�� 
u8 MPU6050_Set_LPF(u16 lpf)
{
	u8 data=0;
	if(lpf>=188)data=1;
	else if(lpf>=98)data=2;
	else if(lpf>=42)data=3;
	else if(lpf>=20)data=4;
	else if(lpf>=10)data=5;
	else data=6; 
	return MPU6050_Write_Byte(MPU6050_CFG_REG,data);//�������ֵ�ͨ�˲���  
}
//����MPU6050�Ĳ�����(�ٶ�Fs=1KHz)
//rate:4~1000(Hz)
//����ֵ:0,���óɹ�
//    ����,����ʧ�� 
u8 MPU6050_Set_Rate(u16 rate)
{
	u8 data;
	if(rate>1000)rate=1000;
	if(rate<4)rate=4;
	data=1000/rate-1;
	data=MPU6050_Write_Byte(MPU6050_SAMPLE_RATE_REG,data);	//�������ֵ�ͨ�˲���
 	return MPU6050_Set_LPF(rate/2);	//�Զ�����LPFΪ�����ʵ�һ��
}

//�õ��¶�ֵ
//����ֵ:�¶�ֵ(������100��)
short MPU6050_Get_Temperature(void)
{
    u8 buf[2]; 
    short raw;
	float temp;
	MPU6050_Read_Len(MPU6050_ADDR,MPU6050_TEMP_OUTH_REG,2,buf); 
    raw=((u16)buf[0]<<8)|buf[1];  
    temp=36.53+((double)raw)/340;  
    return temp*100;;
}
//�õ�������ֵ(ԭʼֵ)
//gx,gy,gz:������x,y,z���ԭʼ����(������)
//����ֵ:0,�ɹ�
//    ����,�������
u8 MPU6050_Get_Gyroscope( short *gx,short *gy,short *gz)
{
    u8 buf[6],res;  
	res=MPU6050_Read_Len(MPU6050_ADDR,MPU6050_GYRO_XOUTH_REG,6,buf);
	if(res==0)
	{
		*gx=((u16)buf[0]<<8)|buf[1];  
		*gy=((u16)buf[2]<<8)|buf[3];  
		*gz=((u16)buf[4]<<8)|buf[5];
	} 	
    return res;;
}
//�õ����ٶ�ֵ(ԭʼֵ)
//gx,gy,gz:������x,y,z���ԭʼ����(������)
//����ֵ:0,�ɹ�
//    ����,�������
u8 MPU6050_Get_Accelerometer(short *ax,short *ay,short *az)
{
    u8 buf[6],res;  
	res=MPU6050_Read_Len(MPU6050_ADDR,MPU6050_ACCEL_XOUTH_REG,6,buf);
	if(res==0)
	{
		*ax=((u16)buf[0]<<8)|buf[1];  
		*ay=((u16)buf[2]<<8)|buf[3];  
		*az=((u16)buf[4]<<8)|buf[5];
	} 	
    return res;;
}
//IIC����д
//addr:������ַ 
//reg:�Ĵ�����ַ
//len:д�볤��
//buf:������
//����ֵ:0,����
//    ����,�������
u8 MPU6050_Write_Len(u8 addr,u8 reg,u8 len,u8 *buf)
{
	u8 i; 
  IIC_Start(I2C1);
  RWAddGenerate(I2C1,0,addr<<1);//����������ַ+д����
  IIC_Send_Byte(I2C1,reg);

	for(i=0;i<len;i++)
	{
    IIC_Send_Byte(I2C1,buf[i]);//��������
	}    
    IIC_Stop(I2C1);	 
	  return 0;	
} 
//IIC������
//addr:������ַ
//reg:Ҫ��ȡ�ļĴ�����ַ
//len:Ҫ��ȡ�ĳ���
//buf:��ȡ�������ݴ洢��
//����ֵ:0,����
//    ����,�������
u8 MPU6050_Read_Len(u8 addr,u8 reg,u8 len,u8 *buf)
{ 
	u8 Enterlen=len;
 	IIC_Start(I2C1); 
	RWAddGenerate(I2C1,0,addr<<1);//����������ַ+д����	
  IIC_Send_Byte(I2C1,reg);	//д�Ĵ�����ַ
  IIC_Start(I2C1);
	RWAddGenerate(I2C1,1,addr<<1);//����������ַ+������	
	while(len)
	{
		if(len==1)
		{
      IIC_NAck(I2C1);	
			IIC_Stop(I2C1);
			//������������ʱ��ǰ�ر�ACK��ֹͣ������ɽ��պ������޷�����ACK��ʹ�ӻ���ֹ���Ͷ�����������������ֹͣ�Խ�������
			*buf=IIC_Read_Byte(I2C1);//������,����nACK 
			
		}
		else 
		{
			IIC_Ack(I2C1);//��ACK
			*buf=IIC_Read_Byte(I2C1);   
		}
		len--;
		buf++; 
	}
  IIC_Ack(I2C1);//�ָ�ACK
    	//����һ��ֹͣ���� 
	return 0;	
}
//IICдһ���ֽ� 
//reg:�Ĵ�����ַ
//data:����
//����ֵ:0,����
//    ����,�������
u8 MPU6050_Write_Byte(u8 reg,u8 data) 				 
{ 
  IIC_Start(I2C1);
  RWAddGenerate(I2C1,0,MPU6050_ADDR<<1);//����������ַ+д����	
  IIC_Send_Byte(I2C1,reg);//д�Ĵ�����ַ
	IIC_Send_Byte(I2C1,data);//��������	 
  IIC_Stop(I2C1);	 
	return 0;
}
//IIC��һ���ֽ� 
//reg:�Ĵ�����ַ 
//����ֵ:����������
u8 MPU6050_Read_Byte(u8 reg)
{
	u8 res;
  IIC_Start(I2C1); 
	RWAddGenerate(I2C1,0,MPU6050_ADDR<<1);//����������ַ+д����	
  IIC_Send_Byte(I2C1,reg);//д�Ĵ�����ַ
  IIC_Start(I2C1);
	RWAddGenerate(I2C1,1,MPU6050_ADDR<<1);//����������ַ+������
  IIC_NAck(I2C1);	
  IIC_Stop(I2C1);//�Ѷ����������������ǰ����Nack��ֹͣ�ź�	
	res=IIC_Read_Byte(I2C1);//��ȡ����
	//����һ��ֹͣ���� 
	return res;		
}


#include "MPU6050.h"
#include "stm32f10x.h" 
#include "iic.h"
#include "delay.h"
//#include "usart.h"   


//初始化MPU6050
//返回值:0,成功
//    其他,错误代码
u8 MPU6050_Init(void)
{ 
	u8 res;
//	IIC_Init();//初始化IIC总线
	MPU6050_Write_Byte(MPU6050_PWR_MGMT1_REG,0X80);	//复位MPU6050
  delay_xms(100);
	MPU6050_Write_Byte(MPU6050_PWR_MGMT1_REG,0X00);	//唤醒MPU6050
	MPU6050_Set_Gyro_Fsr(0);					//陀螺仪传感器,±2000dps
	MPU6050_Set_Accel_Fsr(0);					//加速度传感器,±2g
	MPU6050_Set_Rate(50);						//设置采样率50Hz
	MPU6050_Write_Byte(MPU6050_INT_EN_REG,0X00);	//关闭所有中断
	MPU6050_Write_Byte(MPU6050_USER_CTRL_REG,0X00);	//I2C主模式关闭
	MPU6050_Write_Byte(MPU6050_FIFO_EN_REG,0X00);	//关闭FIFO
	MPU6050_Write_Byte(MPU6050_INTBP_CFG_REG,0X80);	//INT引脚低电平有效
	res=MPU6050_Read_Byte(MPU6050_DEVICE_ID_REG);
	if(res==MPU6050_ADDR)//器件ID正确
	{
		MPU6050_Write_Byte(MPU6050_PWR_MGMT1_REG,0X01);	//设置CLKSEL,PLL X轴为参考
		MPU6050_Write_Byte(MPU6050_PWR_MGMT2_REG,0X00);	//加速度与陀螺仪都工作
		MPU6050_Set_Rate(25);						//设置采样率为50Hz
 	}else return 1;
	return 0;
}
//设置MPU6050陀螺仪传感器满量程范围
//fsr:0,±250dps;1,±500dps;2,±1000dps;3,±2000dps
//返回值:0,设置成功
//    其他,设置失败 
u8 MPU6050_Set_Gyro_Fsr(u8 fsr)
{
	return MPU6050_Write_Byte(MPU6050_GYRO_CFG_REG,fsr<<3);//设置陀螺仪满量程范围  
}
//设置MPU6050加速度传感器满量程范围
//fsr:0,±2g;1,±4g;2,±8g;3,±16g
//返回值:0,设置成功
//    其他,设置失败 
u8 MPU6050_Set_Accel_Fsr(u8 fsr)
{
	return MPU6050_Write_Byte(MPU6050_ACCEL_CFG_REG,fsr<<3);//设置加速度传感器满量程范围  
}
//设置MPU6050的数字低通滤波器
//lpf:数字低通滤波频率(Hz)
//返回值:0,设置成功
//    其他,设置失败 
u8 MPU6050_Set_LPF(u16 lpf)
{
	u8 data=0;
	if(lpf>=188)data=1;
	else if(lpf>=98)data=2;
	else if(lpf>=42)data=3;
	else if(lpf>=20)data=4;
	else if(lpf>=10)data=5;
	else data=6; 
	return MPU6050_Write_Byte(MPU6050_CFG_REG,data);//设置数字低通滤波器  
}
//设置MPU6050的采样率(假定Fs=1KHz)
//rate:4~1000(Hz)
//返回值:0,设置成功
//    其他,设置失败 
u8 MPU6050_Set_Rate(u16 rate)
{
	u8 data;
	if(rate>1000)rate=1000;
	if(rate<4)rate=4;
	data=1000/rate-1;
	data=MPU6050_Write_Byte(MPU6050_SAMPLE_RATE_REG,data);	//设置数字低通滤波器
 	return MPU6050_Set_LPF(rate/2);	//自动设置LPF为采样率的一半
}

//得到温度值
//返回值:温度值(扩大了100倍)
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
//得到陀螺仪值(原始值)
//gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
//返回值:0,成功
//    其他,错误代码
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
//得到加速度值(原始值)
//gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
//返回值:0,成功
//    其他,错误代码
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
//IIC连续写
//addr:器件地址 
//reg:寄存器地址
//len:写入长度
//buf:数据区
//返回值:0,正常
//    其他,错误代码
u8 MPU6050_Write_Len(u8 addr,u8 reg,u8 len,u8 *buf)
{
	u8 i; 
  IIC_Start(I2C1);
  RWAddGenerate(I2C1,0,addr<<1);//发送器件地址+写命令
  IIC_Send_Byte(I2C1,reg);

	for(i=0;i<len;i++)
	{
    IIC_Send_Byte(I2C1,buf[i]);//发送数据
	}    
    IIC_Stop(I2C1);	 
	  return 0;	
} 
//IIC连续读
//addr:器件地址
//reg:要读取的寄存器地址
//len:要读取的长度
//buf:读取到的数据存储区
//返回值:0,正常
//    其他,错误代码
u8 MPU6050_Read_Len(u8 addr,u8 reg,u8 len,u8 *buf)
{ 
	u8 Enterlen=len;
 	IIC_Start(I2C1); 
	RWAddGenerate(I2C1,0,addr<<1);//发送器件地址+写命令	
  IIC_Send_Byte(I2C1,reg);	//写寄存器地址
  IIC_Start(I2C1);
	RWAddGenerate(I2C1,1,addr<<1);//发送器件地址+读命令	
	while(len)
	{
		if(len==1)
		{
      IIC_NAck(I2C1);	
			IIC_Stop(I2C1);
			//在最后接收数据时提前关闭ACK及停止，让完成接收后，主机无法发送ACK，使从机终止发送动作，进而主机发送停止以结束任务
			*buf=IIC_Read_Byte(I2C1);//读数据,发送nACK 
			
		}
		else 
		{
			IIC_Ack(I2C1);//打开ACK
			*buf=IIC_Read_Byte(I2C1);   
		}
		len--;
		buf++; 
	}
  IIC_Ack(I2C1);//恢复ACK
    	//产生一个停止条件 
	return 0;	
}
//IIC写一个字节 
//reg:寄存器地址
//data:数据
//返回值:0,正常
//    其他,错误代码
u8 MPU6050_Write_Byte(u8 reg,u8 data) 				 
{ 
  IIC_Start(I2C1);
  RWAddGenerate(I2C1,0,MPU6050_ADDR<<1);//发送器件地址+写命令	
  IIC_Send_Byte(I2C1,reg);//写寄存器地址
	IIC_Send_Byte(I2C1,data);//发送数据	 
  IIC_Stop(I2C1);	 
	return 0;
}
//IIC读一个字节 
//reg:寄存器地址 
//返回值:读到的数据
u8 MPU6050_Read_Byte(u8 reg)
{
	u8 res;
  IIC_Start(I2C1); 
	RWAddGenerate(I2C1,0,MPU6050_ADDR<<1);//发送器件地址+写命令	
  IIC_Send_Byte(I2C1,reg);//写寄存器地址
  IIC_Start(I2C1);
	RWAddGenerate(I2C1,1,MPU6050_ADDR<<1);//发送器件地址+读命令
  IIC_NAck(I2C1);	
  IIC_Stop(I2C1);//已对器件发出读命令，提前发出Nack和停止信号	
	res=IIC_Read_Byte(I2C1);//读取数据
	//产生一个停止条件 
	return res;		
}


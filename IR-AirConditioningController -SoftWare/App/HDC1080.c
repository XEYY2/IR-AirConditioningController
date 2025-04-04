#include "HDC1080.h"
#include "math.h"
#include "delay.h"
void HDC_Init(void)
{
	HDC_Write_Cmd(0x02,0x1000);
	delay_ms(15);
}

u8 HDC_Write_Cmd(u8 regadd,u16 Write_Data)
{
	 IIC_Start(HDC_I2C);
	 RWAddGenerate(HDC_I2C,0,HDCAddress<<1);
	 IIC_Send_Byte(HDC_I2C,regadd);
	 IIC_Send_Byte(HDC_I2C,(u8)(Write_Data>>8));
	 IIC_Send_Byte(HDC_I2C,(u8)(Write_Data&0xFF));
	 IIC_Stop(HDC_I2C);
	 return 0;
}

u8 HDC_ReadData(u8 REG ,u8 *buf)
{
	u8 len=4;
	IIC_Start(HDC_I2C);
	RWAddGenerate(HDC_I2C,0,HDCAddress<<1);
	IIC_Send_Byte(HDC_I2C,REG);
	delay_ms(15);  
	IIC_Ack(HDC_I2C);
	IIC_Start(HDC_I2C);
	RWAddGenerate(HDC_I2C,1,HDCAddress<<1);
	while(len)
	{
		if(len==1)
		{
		IIC_NAck(HDC_I2C);	
		IIC_Stop(HDC_I2C);
		//在最后接收数据时提前关闭ACK及停止，让完成接收后，主机无法发送ACK，
		//使从机终止发送动作，进而主机发送停止以结束任务
		*buf=IIC_Read_Byte(HDC_I2C);
		}else
		  {
			  IIC_Ack(HDC_I2C);//打开ACK
			  *buf=IIC_Read_Byte(I2C1);   
		  }
			len--;
		  buf++;
	}
	IIC_Ack(I2C1);//恢复ACK
	return 0;
}


void Get_HDC1080_THValue(float *temp, float *humid)
{
	uint8_t buf[4] = {0};
	
	//读温湿度寄存器值保存在数组中
	HDC_ReadData(0x00,buf);
	//温度转换
	*temp = (float)(buf[0]<<8|buf[1]);
	*temp = (*temp/pow(2,16))*165-40;
	//湿度转换
	*humid = (float)(buf[2]<<8|buf[3]);
	*humid = (*humid/pow(2, 16))*100;
}

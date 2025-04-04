#include "stm32f10x.h"                  // Device header
#include "MPU6050.h"
#include "IMUParse.h"
#include "delay.h"
#include "system.h"
#include "math.h"


/*初始计算偏移量*/
float i;//计算偏移量时的循环次数
float ax_offset=0;//X轴加速度偏移量
float ay_offset=0;//Y轴加速度偏移量
float gx_offset=0;//X轴角速度偏移量
float gy_offset=0;//Y轴角速度偏移量

/*参数*/
float rad2deg = 57.29578;
float roll_v=0;
float pitch_v=0;

/*定义微分时间*/
float now=0;
static float lasttime=0;
float dt=0;

/*定义三个状态值*/
float gyro_roll=0,gyro_pitch=0;
float acc_roll=0,acc_pitch=0;
float k_roll=0,k_pitch=0;

/*误差协方差矩阵*/
float e_P[2][2]={{1,0},{0,1}};

/*卡尔曼增益*/
float k_k[2][2]={{0,0},{0,0}};

void IMUParse_Init(void)
{
	u16 num;
	short  GryDate[3];
	short  AccDate[3];
	MPU6050_Init();
	for(num=0;num<2000;num++)
	{
		MPU6050_Get_Gyroscope(&GryDate[0],&GryDate[1],&GryDate[2]);//获得角速度
		MPU6050_Get_Accelerometer(&AccDate[0],&AccDate[1],&AccDate[2]);//获得加速度		
		ax_offset+=AccDate[0];//计算2000次X轴加速度偏移总量
		ay_offset+=AccDate[1];//计算2000次X轴加速度偏移总量
		gx_offset+=GryDate[0];//计算2000次X轴角速度偏移总量
		gy_offset+=GryDate[1];//计算2000次X轴角速度偏移总量
	}
	ax_offset=ax_offset/2000;
	ay_offset=ax_offset/2000;
	gx_offset=ax_offset/2000;
	gy_offset=ax_offset/2000;
	delay_ms(100);
}

short* DataParse(void)
{
	short  GryDate[3];
	short  AccDate[3];
	short  static r[3]={0,0,0};
//	now=returntimeline();//获取当前秒数时间戳
//	if(now>lasttime)//如果now>lasttime,now未溢出3600复位
//	dt=now-lasttime;
//	else
//	dt=36000000-lasttime+now;//如果now<lasttime,now溢出3600复位
	delay_ms(200);
	dt=0.2;
	lasttime=now;
	MPU6050_Get_Gyroscope(&GryDate[0],&GryDate[1],&GryDate[2]);//获得角速度
	MPU6050_Get_Accelerometer(&AccDate[0],&AccDate[1],&AccDate[2]);//获得加速度
	
	/*Step1，计算先验状态，即角速度部分*/
	roll_v=(GryDate[0]-gx_offset)+((sin(k_pitch)*sin(k_roll))/cos(k_pitch))*(GryDate[1]-gy_offset)+((sin(k_pitch)*cos(k_roll))/cos(k_pitch))*(GryDate[2]);//roll轴的角速度，翻滚
	pitch_v=cos(k_roll)*(GryDate[1]-gy_offset)-sin(k_roll)*GryDate[2];//pitch轴的角速度，俯仰
	gyro_roll=k_roll+dt*roll_v;//先验roll角度
	gyro_pitch=k_pitch=dt*pitch_v;//先验pitch角度
	
	/*Step2计算先验误差协方差矩阵P*/
	e_P[0][0]=e_P[0][0]+0.0025;//这里的Q矩阵是一个对角阵且对角元均为0.0025
	e_P[0][1]=e_P[0][1]+0;
	e_P[1][0]=e_P[1][0]+0;
	e_P[1][1]=e_P[1][1]+0.0025;
	
	/*Step3更新卡尔曼增益*/
	k_k[0][0]=e_P[0][0]/(e_P[0][0]=0.3);//这里观测噪声矩阵R是一个对角阵且对角元均为0.3
	k_k[0][1]=e_P[0][1];
	k_k[1][0]=e_P[1][0];
	k_k[1][1]=e_P[1][1]/(e_P[1][1]=0.3);
	
	/*Step4计算先验状态*/
	/*观察状态*/
	acc_roll=atan((AccDate[1]-ay_offset)/(AccDate[2]))*rad2deg;
	acc_pitch=(-1)*atan((AccDate[0]-ax_offset)/sqrt((AccDate[1]-ay_offset)*(AccDate[1]-ay_offset)+AccDate[2]*AccDate[2]))*rad2deg;
	/*最优估计状态*/
	k_roll=gyro_roll+k_k[0][0]*(acc_roll-gyro_roll);
	k_pitch=gyro_pitch+k_k[1][1]*(acc_pitch-gyro_pitch);
	
	/*更新协方差方程*/
	e_P[0][0]=(1-k_k[0][0])*e_P[0][0];
	e_P[0][1]=0;
	e_P[1][0]=0;
	e_P[1][1]=(1-k_k[1][1])*e_P[1][1];
	
	r[0]=k_roll;
	r[1]=k_pitch;
	r[2]=0;
	return r;
}

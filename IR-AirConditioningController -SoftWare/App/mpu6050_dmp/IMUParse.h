#ifndef __IMUParse_H
#define __IMUParse_H
///*初始计算偏移量*/
//float i;//计算偏移量时的循环次数
//float ax_offset=0;//X轴加速度偏移量
//float ay_offset=0;//Y轴加速度偏移量
//float gx_offset=0;//X轴角速度偏移量
//float gy_offset=0;//Y轴角速度偏移量

///*参数*/
//float rad2deg = 57.29578;
//float roll_v=0;
//float pitch_v=0;

///*定义微分时间*/
//float now=0;
//static float lasttime=0;
//float dt=0;

///*定义三个状态值*/
//float gyro_roll=0,gyro_pitch=0;
//float acc_roll=0,acc_pitch=0;
//float k_roll=0,k_pitch=0;

///*误差协方差矩阵*/
//float e_P[2][2]={{1,0},{0,1}};

///*卡尔曼增益*/
//float k_k[2][2]={{0,0},{0,0}};

short* DataParse(void);

void IMUParse_Init(void);
#endif

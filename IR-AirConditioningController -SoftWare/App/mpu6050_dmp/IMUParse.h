#ifndef __IMUParse_H
#define __IMUParse_H
///*��ʼ����ƫ����*/
//float i;//����ƫ����ʱ��ѭ������
//float ax_offset=0;//X����ٶ�ƫ����
//float ay_offset=0;//Y����ٶ�ƫ����
//float gx_offset=0;//X����ٶ�ƫ����
//float gy_offset=0;//Y����ٶ�ƫ����

///*����*/
//float rad2deg = 57.29578;
//float roll_v=0;
//float pitch_v=0;

///*����΢��ʱ��*/
//float now=0;
//static float lasttime=0;
//float dt=0;

///*��������״ֵ̬*/
//float gyro_roll=0,gyro_pitch=0;
//float acc_roll=0,acc_pitch=0;
//float k_roll=0,k_pitch=0;

///*���Э�������*/
//float e_P[2][2]={{1,0},{0,1}};

///*����������*/
//float k_k[2][2]={{0,0},{0,0}};

short* DataParse(void);

void IMUParse_Init(void);
#endif

#include "stm32f10x.h"                  // Device header
#include "MPU6050.h"
#include "IMUParse.h"
#include "delay.h"
#include "system.h"
#include "math.h"


/*��ʼ����ƫ����*/
float i;//����ƫ����ʱ��ѭ������
float ax_offset=0;//X����ٶ�ƫ����
float ay_offset=0;//Y����ٶ�ƫ����
float gx_offset=0;//X����ٶ�ƫ����
float gy_offset=0;//Y����ٶ�ƫ����

/*����*/
float rad2deg = 57.29578;
float roll_v=0;
float pitch_v=0;

/*����΢��ʱ��*/
float now=0;
static float lasttime=0;
float dt=0;

/*��������״ֵ̬*/
float gyro_roll=0,gyro_pitch=0;
float acc_roll=0,acc_pitch=0;
float k_roll=0,k_pitch=0;

/*���Э�������*/
float e_P[2][2]={{1,0},{0,1}};

/*����������*/
float k_k[2][2]={{0,0},{0,0}};

void IMUParse_Init(void)
{
	u16 num;
	short  GryDate[3];
	short  AccDate[3];
	MPU6050_Init();
	for(num=0;num<2000;num++)
	{
		MPU6050_Get_Gyroscope(&GryDate[0],&GryDate[1],&GryDate[2]);//��ý��ٶ�
		MPU6050_Get_Accelerometer(&AccDate[0],&AccDate[1],&AccDate[2]);//��ü��ٶ�		
		ax_offset+=AccDate[0];//����2000��X����ٶ�ƫ������
		ay_offset+=AccDate[1];//����2000��X����ٶ�ƫ������
		gx_offset+=GryDate[0];//����2000��X����ٶ�ƫ������
		gy_offset+=GryDate[1];//����2000��X����ٶ�ƫ������
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
//	now=returntimeline();//��ȡ��ǰ����ʱ���
//	if(now>lasttime)//���now>lasttime,nowδ���3600��λ
//	dt=now-lasttime;
//	else
//	dt=36000000-lasttime+now;//���now<lasttime,now���3600��λ
	delay_ms(200);
	dt=0.2;
	lasttime=now;
	MPU6050_Get_Gyroscope(&GryDate[0],&GryDate[1],&GryDate[2]);//��ý��ٶ�
	MPU6050_Get_Accelerometer(&AccDate[0],&AccDate[1],&AccDate[2]);//��ü��ٶ�
	
	/*Step1����������״̬�������ٶȲ���*/
	roll_v=(GryDate[0]-gx_offset)+((sin(k_pitch)*sin(k_roll))/cos(k_pitch))*(GryDate[1]-gy_offset)+((sin(k_pitch)*cos(k_roll))/cos(k_pitch))*(GryDate[2]);//roll��Ľ��ٶȣ�����
	pitch_v=cos(k_roll)*(GryDate[1]-gy_offset)-sin(k_roll)*GryDate[2];//pitch��Ľ��ٶȣ�����
	gyro_roll=k_roll+dt*roll_v;//����roll�Ƕ�
	gyro_pitch=k_pitch=dt*pitch_v;//����pitch�Ƕ�
	
	/*Step2�����������Э�������P*/
	e_P[0][0]=e_P[0][0]+0.0025;//�����Q������һ���Խ����ҶԽ�Ԫ��Ϊ0.0025
	e_P[0][1]=e_P[0][1]+0;
	e_P[1][0]=e_P[1][0]+0;
	e_P[1][1]=e_P[1][1]+0.0025;
	
	/*Step3���¿���������*/
	k_k[0][0]=e_P[0][0]/(e_P[0][0]=0.3);//����۲���������R��һ���Խ����ҶԽ�Ԫ��Ϊ0.3
	k_k[0][1]=e_P[0][1];
	k_k[1][0]=e_P[1][0];
	k_k[1][1]=e_P[1][1]/(e_P[1][1]=0.3);
	
	/*Step4��������״̬*/
	/*�۲�״̬*/
	acc_roll=atan((AccDate[1]-ay_offset)/(AccDate[2]))*rad2deg;
	acc_pitch=(-1)*atan((AccDate[0]-ax_offset)/sqrt((AccDate[1]-ay_offset)*(AccDate[1]-ay_offset)+AccDate[2]*AccDate[2]))*rad2deg;
	/*���Ź���״̬*/
	k_roll=gyro_roll+k_k[0][0]*(acc_roll-gyro_roll);
	k_pitch=gyro_pitch+k_k[1][1]*(acc_pitch-gyro_pitch);
	
	/*����Э�����*/
	e_P[0][0]=(1-k_k[0][0])*e_P[0][0];
	e_P[0][1]=0;
	e_P[1][0]=0;
	e_P[1][1]=(1-k_k[1][1])*e_P[1][1];
	
	r[0]=k_roll;
	r[1]=k_pitch;
	r[2]=0;
	return r;
}

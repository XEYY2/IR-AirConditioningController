#include "stm32f10x.h"
#include "system.h"
#include "stdio.h" 
/*APP Doc*/
#include "HDC1080.h"
#include "oled.h"
#include "IRLED.h"
#include "Humidifier.h"
#include "Led.h"
/*FreeRtos element*/
#define Start_Task_Prio 5//任务优先级，数值越大优先级越高
#define Start_Stk_Size 100//任务堆栈大小,单位Word，4字节
TaskHandle_t StartTask_Handler;
void Start_Task(void* pvParameters);//函数未定义，先声明

#define Taskmgr_Task_Prio 2//任务优先级，数值越大优先级越高
#define Taskmgr_Stk_Size 100//任务堆栈大小,单位Word，4字节
TaskHandle_t Taskmgr_Handler;
void Taskmgr_Task(void* pvParameters);//函数未定义，先声明

#define SentMessage_Task_Prio 4//任务优先级，数值越大优先级越高
#define SentMessage_Stk_Size 100//任务堆栈大小
TaskHandle_t SentMessageTask_Handler;
void SentMessage_Task(void* p_arg);//函数未定义，先声明

#define IR_Task_Prio 3//任务优先级，数值越大优先级越高
#define IR_Stk_Size 100//任务堆栈大小
TaskHandle_t IR_Task_Handler;
void IR_Task(void* p_arg);//函数未定义，先声明

#define Humi_Task_Prio 3//任务优先级，数值越大优先级越高
#define Humi_Stk_Size 100//任务堆栈大小
TaskHandle_t Humi_Task_Handler;
void Humi_Task(void* p_arg);//函数未定义，先声明

//#define debug_Task_Prio 3//任务优先级，数值越大优先级越高
//#define debug_Stk_Size 100//任务堆栈大小
//TaskHandle_t debug_Task_Handler;
//void debug_Task(void* p_arg);//函数未定义，先声明
//全局变量
extern volatile uint8_t ir_data[6];//供接收数据存储，未初始化接收时无用

SysData SYS_Glabal_varible={0,0,State_OFF,{0,Auto,26}};

int main(void)
{
  SYSCLK_Config();
	delay_Init(ManualSetSYSCLK);
/*
  //必须尽早创建任务，不然SCKtick进入中断调用xTaskIncrementTick后会造成野指针，
	//具体内容：https://blog.csdn.net/zhjmyx/article/details/120490993
	//创建任务
  //创建开始任务，此任务在执行后被删除
*/	
	xTaskCreate((TaskFunction_t) Start_Task,//任务函数
							(const char*)"Start_task",//任务名称
						  (uint16_t) Start_Stk_Size,//任务堆栈大小
							(void*)NULL,          //传递给任务函数的参数
							(UBaseType_t)Start_Task_Prio,//任务优先级
							(TaskHandle_t*) &StartTask_Handler);
//通信初始化	
	USART1_Init(9600);
	USART3_Init(9600);
  IIC_Init();
//App 初始化
	Ledflow();
	HDC_Init();
	IR_Init();//默认发射配置
	RH_GPIO_Init();
//	OLED_Init();
//  OLED_ColorTurn(0);
//	OLED_DisplayTurn(0);

  vTaskStartScheduler();//此函数对调用系统Systick初始化，覆盖掉前序配置
}

void Start_Task(void* pvParameters)
{
	taskENTER_CRITICAL();//进入临界区，在此区域的代码不会被打断
	//创建IIC通信相关任务，将IIC功能放在不同的任务中，会导致相互竞争IIC控制权
							
	
							xTaskCreate((TaskFunction_t) SentMessage_Task,//任务函数
							(const char*)"HDC_task",//任务名称
						  (uint16_t) SentMessage_Stk_Size,//任务堆栈大小
							(void*)NULL,          //传递给任务函数的参数
							(UBaseType_t)SentMessage_Task_Prio,//任务优先级
							(TaskHandle_t*) &SentMessageTask_Handler);
		
							xTaskCreate((TaskFunction_t) Humi_Task,//任务函数
							(const char*)"Humi_task",//任务名称
						    (uint16_t)Humi_Stk_Size,//任务堆栈大小
							(void*)NULL,          //传递给任务函数的参数
							(UBaseType_t)Humi_Task_Prio,//任务优先级
							(TaskHandle_t*) &Humi_Task_Handler);
							
							xTaskCreate((TaskFunction_t) IR_Task,//任务函数
							(const char*)"IR_task",//任务名称
						    (uint16_t)IR_Stk_Size,//任务堆栈大小
							(void*)NULL,          //传递给任务函数的参数
							(UBaseType_t)IR_Task_Prio,//任务优先级
							(TaskHandle_t*) &IR_Task_Handler);
							
							xTaskCreate((TaskFunction_t) Taskmgr_Task,//任务函数
							(const char*)"Taskmgr_Task",//任务名称
						  (uint16_t) Taskmgr_Stk_Size,//任务堆栈大小
							(void*)NULL,          //传递给任务函数的参数
							(UBaseType_t)Taskmgr_Task_Prio,//任务优先级
							(TaskHandle_t*) &Taskmgr_Handler);

//							xTaskCreate((TaskFunction_t) debug_Task,//任务函数
//							(const char*)"debug_Task",//任务名称
//						  (uint16_t) debug_Stk_Size,//任务堆栈大小
//							(void*)NULL,          //传递给任务函数的参数
//							(UBaseType_t)debug_Task_Prio,//任务优先级
//							(TaskHandle_t*) &debug_Task_Handler);
							
		vTaskDelete(StartTask_Handler);	
  taskEXIT_CRITICAL();								
}
void Taskmgr_Task(void* p_arg)
{
  u8 lastIR_Temp,lastIR_mode,last_humimode;
	u8 OFF_Sta;
	while(1)
	{
		
 		if((lastIR_Temp!=USART3_RX_BUF[4])||(lastIR_mode!=USART3_RX_BUF[5]))
		{
			//IR_命令有变，执行命令
			lastIR_Temp=USART3_RX_BUF[4];//更新最后一次的IR控制值
	    lastIR_mode=USART3_RX_BUF[5];
			OFF_Sta=0;//其他途径开机
			xTaskNotifyGive(IR_Task_Handler); 
		}
		if(last_humimode!=USART3_RX_BUF[6])
		{			
	    last_humimode=USART3_RX_BUF[6];
			xTaskNotifyGive(Humi_Task_Handler);
		}
		if(((USART3_RX_BUF[7]&0x0F)==0x0F)&&OFF_Sta==0)//低四位变F时关机
		{
			  USART3_RX_BUF[7]|=0xF0;//低四位清零
			  OFF_Sta=1;
			  Normal_Code(0xB2,0x7B,0xE0);//关机
		}
		else if((USART3_RX_BUF[7]==0x00)&&(OFF_Sta==1))
    {
			//否则为开机，执行上一次的空调控制参数
			xTaskNotifyGive(IR_Task_Handler);
			OFF_Sta=0;
		}
		
	}
}
void SentMessage_Task(void* pvParameters)//温湿度读取任务
{
      float temp,humid;
	    u32 NotifyVaule;
		  while(1)
    	{
			NotifyVaule=ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
				if(NotifyVaule==1)
				{
			
			//获取传感器数据
			Get_HDC1080_THValue(&temp,&humid);
			SYS_Glabal_varible.curtemp=temp;
			SYS_Glabal_varible.curhumid=humid;
			//0.发送0xB3
      USART_SendData(USART3,0xB3);
					delay_ms(100);//延时必须，ESP端使用状态机接收，太快了处理不过来
      //1.发送空调温度	
      USART_SendData(USART3,SYS_Glabal_varible.IR_Sent_Data.temp);
					delay_ms(100);
		  //2.发送空调模式
		  USART_SendData(USART3,SYS_Glabal_varible.IR_Sent_Data.mode);
					delay_ms(100);
  		//3.发送加湿器模式
		  USART_SendData(USART3,SYS_Glabal_varible.RH_Sta);
					delay_ms(100);
   		//4-7.发送温度数据
		  send_float_via_uart(USART3,temp);
					
   		//8-11.发送湿度数据
		  send_float_via_uart(USART3,humid);
			delay_ms(100);
				}
	    }		
}
void IR_Task(void* p_arg)
{
	u32 NotifyVaule;
	while(1)
	{
		NotifyVaule=ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
    GPIO_SetBits(GPIOA, GPIO_Pin_5);		
    if(NotifyVaule==1)
		{
		ConducterControl((ConducterMode)USART3_RX_BUF[5] ,USART3_RX_BUF[4]);//第五byte空调温度，第六byte空调模式
		xTaskNotifyGive(SentMessageTask_Handler);
		}
		
	}
}
void Humi_Task(void* p_arg)
{ 
  u32 NotifyVaule;
	while(1)
	{
		 NotifyVaule=ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
		 if(NotifyVaule==1)
		 {			 
		 RH_Control((RH_State)USART3_RX_BUF[6]);
		 xTaskNotifyGive(SentMessageTask_Handler);
		 }
	}
}
void debug_Task(void* p_arg)
{
	while(1)
	{
		delay_ms(1000);
	  Decode_IRDta();
		printf(" A=%#X,NA=%#X", ir_data[0],ir_data[1]);
	  printf(" B=%#X,NB=%#X", ir_data[2],ir_data[3]);
	  printf(" C=%#X,NC=%#X", ir_data[4],ir_data[5]);
	}
}


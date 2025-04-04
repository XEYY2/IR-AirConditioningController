#include "IRLED.h"
#include "system.h"
/* 全局变量 */
volatile uint8_t ir_bit_count = 0;
volatile uint8_t ir_data[6] = {0};
static u8 enter_flag_verify=0;

u16 IR_Buffer_Row[125]={0};
int Buffer_index=0;
u8 frame_num=0;
void IR_Init(void) 
	{
      IR_TIM3_PWM_Init();
//		IR_TIM_Init();
  }
void IR_TIM3_PWM_Init(void) 
	{
    // 使能GPIOA和TIM3时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    // 配置PA6为复用推挽输出（用于Q2栅极）
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 配置TIM3
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Period = 1894; // 38kHz PWM (72MHz / (947 + 1) = 38kHz)
    TIM_TimeBaseStructure.TIM_Prescaler = 0;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    // 配置TIM3通道1为PWM模式
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 947; // 947 50%占空比
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC1Init(TIM3, &TIM_OCInitStructure);
    // 启动TIM3
    TIM_Cmd(TIM3, ENABLE);
 }
//红外不输出，接收器拉高，发送低
 	void 	TIM3_SETLOW(void) 
	{
    TIM3->CCR1 = 0;
//TIM_SetCompare1(TIM3,0);
  }
//CR1=473,输出38khz，接收器拉低，发送高
	 	void 	TIM3_SETHIG() 
	{

    TIM3->CCR1 = 947;
//		TIM_SetCompare1(TIM3,947);
  }
 
void Lead_Code(void)
{
	TIM3_SETHIG(); //接收器拉低,对发送就是高
	delay_us(4400);
	TIM3_SETLOW(); //接收器拉高，对发送就是低
	delay_us(4400);
}

void Stop_Code(void)
{
	TIM3_SETHIG(); //接收器拉低
	delay_us(540);
	TIM3_SETLOW(); //接收器拉高
	delay_us(5220);
}

void Send_0_Code(void)
{
	TIM3_SETHIG(); //接收器拉低
	delay_us(540);
	TIM3_SETLOW(); //接收器拉高
	delay_us(540);
}

void Send_1_Code(void)
{
	TIM3_SETHIG(); //接收器拉低
	delay_us(540);
	TIM3_SETLOW(); //接收器拉高
	delay_us(1620);
}

void Send_Byte(u8 data)
{
	int i;
	for(i=7;i>=0;i--)
	{
		if(data & (1<<i))
		{
			Send_1_Code();
		}
		else
		{
			Send_0_Code();
		}
	}
}

void Normal_Code(u8 A, u8 B, u8 C)
{
	Lead_Code();
	Send_Byte(A);
	Send_Byte(~A);
	Send_Byte(B);
	Send_Byte(~B);
	Send_Byte(C);
	Send_Byte(~C);
	Stop_Code();
	Lead_Code();
	Send_Byte(A);
	Send_Byte(~A);
	Send_Byte(B);
	Send_Byte(~B);
	Send_Byte(C);
	Send_Byte(~C);
	Stop_Code();
}
u8 ConducterControl(ConducterMode mode ,u8 temp)
{
	u8 C_Data,ModeData,TempData;
	switch(mode)
  {
		case Auto     :ModeData=IR_C_OperationMode_Auto;break;
		case cold     :ModeData=IR_C_OperationMode_cold;break;
		case DeHumi   :ModeData=IR_C_OperationMode_DeHumi;break;
		case Heat     :ModeData=IR_C_OperationMode_Heat;break;
		case SentWind :ModeData=IR_C_OperationMode_SentWind;break;
		default: return 0;
	}
	switch(temp)
	{
		case 17 : TempData=IR_C_temp_17;break;
		case 18 : TempData=IR_C_temp_18;break;
		case 19 : TempData=IR_C_temp_19;break;
		case 20 : TempData=IR_C_temp_20;break;
		case 21 : TempData=IR_C_temp_21;break;
		case 22 : TempData=IR_C_temp_22;break;
		case 23 : TempData=IR_C_temp_23;break;
		case 24 : TempData=IR_C_temp_24;break;
		case 25 : TempData=IR_C_temp_25;break;
		case 26 : TempData=IR_C_temp_26;break;
		case 27 : TempData=IR_C_temp_27;break;
		case 28 : TempData=IR_C_temp_28;break;
		case 29 : TempData=IR_C_temp_29;break;
		case 30 : TempData=IR_C_temp_30;break;
		default : TempData=IR_C_temp_Nop;break;
	}
	C_Data=(ModeData&0x0C)|(TempData&0xF0);
	SYS_Glabal_varible.IR_Sent_Data.IR_Sent_Data[0]=0xB2;
	SYS_Glabal_varible.IR_Sent_Data.IR_Sent_Data[1]=IR_B_Windspeed_Auto;
	SYS_Glabal_varible.IR_Sent_Data.IR_Sent_Data[2]=C_Data;
	SYS_Glabal_varible.IR_Sent_Data.temp=temp;
	SYS_Glabal_varible.IR_Sent_Data.mode=mode;
	Normal_Code(0xB2,IR_B_Windspeed_Auto,C_Data);
	return 1;
}
//以下为接收控制
/* 定时器配置 */
void IR_TIM_Init(void) {
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_ICInitTypeDef TIM_ICInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    /* 开启时钟 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	/* 开启AFIO时钟（关键！）*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	/* 配置PB4复用为TIM3_CH1 */
    GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE);  // 部分重映射：TIM3_CH1->PB4

    /* 配置GPIO */
    GPIO_InitStructure.GPIO_Pin = IR_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 
    GPIO_Init(IR_GPIO_PORT, &GPIO_InitStructure);

    /* 定时器基础配置 */
	  //APB1总线上的定时器时钟确实有一个倍频机制。当APB1的预分频系数设置为1以外的值时（即分频系数为2、4、8或16），定时器的时钟频率会是APB1频率的两倍
    TIM_TimeBaseStructure.TIM_Period = 0xFFFF;
    TIM_TimeBaseStructure.TIM_Prescaler = (IR_CLOCK_FREQ/1000000) - 1; // 0.5us计数
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(IR_TIMER, &TIM_TimeBaseStructure);

    /* 输入捕获配置 */
    TIM_ICInitStructure.TIM_Channel = IR_TIMER_CHANNEL;
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling; // 双沿触发
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    TIM_ICInitStructure.TIM_ICFilter = 0x08; // 适当滤波
    TIM_ICInit(IR_TIMER, &TIM_ICInitStructure);

    /* 中断配置 */
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_ClearITPendingBit(IR_TIMER, TIM_IT_CC1 | TIM_IT_Update);
    TIM_ITConfig(IR_TIMER, TIM_IT_CC1 | TIM_IT_Update, ENABLE);
    TIM_Cmd(IR_TIMER, ENABLE);
}
/* 中断服务函数 */
void TIM3_IRQHandler(void) 
	{
//		u16 num;
    if(TIM_GetITStatus(IR_TIMER, TIM_IT_Update) != RESET) 
    {
        TIM_ClearITPendingBit(IR_TIMER, TIM_IT_Update);
			  TIM_SetCounter(IR_TIMER,0);
    }

    
    if(TIM_GetITStatus(IR_TIMER, TIM_IT_CC1) != RESET) 
    {
        if(enter_flag_verify)
        {
        
        if(~GPIO_ReadInputDataBit(IR_GPIO_PORT, IR_GPIO_PIN)) 
			  {
					    IR_Buffer_Row[Buffer_index]=TIM_GetCapture1(IR_TIMER);
					    Buffer_index++;
					    if(Buffer_index==99)
							{
								Buffer_index=0;
							  enter_flag_verify=0;
							}
        }
							TIM_SetCounter(IR_TIMER,0);
        }
        else
				{		
            enter_flag_verify=1;
            TIM_SetCounter(IR_TIMER,0);				
				}
        TIM_ClearITPendingBit(IR_TIMER, TIM_IT_CC1);
    }
	  }
u8  Lead_cheak(u16* rowdata)
{
  if(rowdata[0]>IR_Lead_MIN&&rowdata[0]<IR_Lead_MAX)
  {
    frame_num=1;
    return 1;
  }
  else
      return 0;
	
}
u8 Separation_cheak(u16* rowdata)
{
  if(rowdata[49]>SEPARATION_MIN&&rowdata[49]<SEPARATION_MAX)
  {
    frame_num=2;
    return 1;
  }
  else
    return 0;
	
}

u8 decode_A(u16* rowdata)
{	  
    u8 i=0;
	  if(frame_num==1)//解码第一帧
		{
       for(i=0;i<8;i++)
      {
//        printf("%d\n",rowdata[i+1]);
        if(rowdata[i+1]>IR_DATA_1_MIN&&rowdata[i+1]<IR_DATA_1_MAX)
          ir_data[0]|=(0x80>>i);
        else if (rowdata[i+1]<IR_DATA_0_MAX&&rowdata[i+1]>IR_DATA_0_MIN)
          ir_data[0]&=~(0x80>>i);
      }
		}else if(frame_num==2)//解码第二帧
		{
			 for(i=0;i<8;i++)
      {
//        printf("%d\n",rowdata[i+51]);
        if(rowdata[i+51]>IR_DATA_1_MIN&&rowdata[i+51]<IR_DATA_1_MAX)
          ir_data[0]|=(0x80>>i);
        else if (rowdata[i+51]<IR_DATA_0_MAX&&rowdata[i+51]>IR_DATA_0_MIN)
          ir_data[0]&=~(0x80>>i);
      }
		}
    return ir_data[0];   
}
u8 decode_NA(u16* rowdata)
{
    u8 i=0;
	  if(frame_num==1)
		{
    for(i=0;i<8;i++)
    {
		//printf("%d\n",rowdata[i+9]);
        if(rowdata[i+9]>IR_DATA_1_MIN&&rowdata[i+9]<IR_DATA_1_MAX)
          ir_data[1]|=(0x80>>i);
          else if (rowdata[i+9]<IR_DATA_0_MAX&&rowdata[i+9]>IR_DATA_0_MIN)
          ir_data[1]&=~(0x80>>i);
    }
	  }
		else if(frame_num==2)
		{
			 for(i=0;i<8;i++)
      {
//        printf("%d\n",rowdata[i+59]);
        if(rowdata[i+59]>IR_DATA_1_MIN&&rowdata[i+59]<IR_DATA_1_MAX)
          ir_data[1]|=(0x80>>i);
        else if (rowdata[i+59]<IR_DATA_0_MAX&&rowdata[i+59]>IR_DATA_0_MIN)
          ir_data[1]&=~(0x80>>i);
      }
		}
    return ir_data[1];   
}
u8 decode_B(u16* rowdata)
{
    u8 i=0;
	  if(frame_num==1)
		{
    for(i=0;i<8;i++)
    {
		//printf("%d\n",rowdata[i+17]);
        if(rowdata[i+17]>IR_DATA_1_MIN&&rowdata[i+17]<IR_DATA_1_MAX)
          ir_data[2]|=(0x80>>i);
          else if (rowdata[i+17]<IR_DATA_0_MAX&&rowdata[i+17]>IR_DATA_0_MIN)
          ir_data[2]&=~(0x80>>i);
    }
	  }else if(frame_num==2)
		{
			for(i=0;i<8;i++)
      {
//        printf("%d\n",rowdata[i+67]);
        if(rowdata[i+67]>IR_DATA_1_MIN&&rowdata[i+67]<IR_DATA_1_MAX)
          ir_data[2]|=(0x80>>i);
        else if (rowdata[i+67]<IR_DATA_0_MAX&&rowdata[i+67]>IR_DATA_0_MIN)
          ir_data[2]&=~(0x80>>i);
      }
		}
		
    return ir_data[2];   
}
u8 decode_NB(u16* rowdata)
{
	  
    u8 i=0;
	if(frame_num==1)
	{
    for(i=0;i<8;i++)
    {
		//printf("%d\n",rowdata[i+25]);
        if(rowdata[i+25]>IR_DATA_1_MIN&&rowdata[i+25]<IR_DATA_1_MAX)
          ir_data[3]|=(0x80>>i);
          else if (rowdata[i+25]<IR_DATA_0_MAX&&rowdata[i+25]>IR_DATA_0_MIN)
          ir_data[3]&=~(0x80>>i);
    }
	}else if(frame_num==2)
	{
					for(i=0;i<8;i++)
      {
//        printf("%d\n",rowdata[i+75]);
        if(rowdata[i+75]>IR_DATA_1_MIN&&rowdata[i+75]<IR_DATA_1_MAX)
          ir_data[3]|=(0x80>>i);
        else if (rowdata[i+75]<IR_DATA_0_MAX&&rowdata[i+75]>IR_DATA_0_MIN)
          ir_data[3]&=~(0x80>>i);
      }
	}
    return ir_data[3];   
}
u8 decode_C(u16* rowdata)
{
	  
    u8 i=0;
	  if(frame_num==1)
		{
    for(i=0;i<8;i++)
    {
		//printf("%d\n",rowdata[i+33]);
        if(rowdata[i+33]>IR_DATA_1_MIN&&rowdata[i+33]<IR_DATA_1_MAX)
          ir_data[4]|=(0x80>>i);
          else if (rowdata[i+33]<IR_DATA_0_MAX&&rowdata[i+33]>IR_DATA_0_MIN)
          ir_data[4]&=~(0x80>>i);
    }
	  }else if(frame_num==2)
		{
			for(i=0;i<8;i++)
      {
//        printf("%d\n",rowdata[i+83]);
        if(rowdata[i+83]>IR_DATA_1_MIN&&rowdata[i+83]<IR_DATA_1_MAX)
          ir_data[4]|=(0x80>>i);
        else if (rowdata[i+83]<IR_DATA_0_MAX&&rowdata[i+83]>IR_DATA_0_MIN)
          ir_data[4]&=~(0x80>>i);
      }
		}
    return ir_data[4];   
}
u8 decode_NC(u16* rowdata)
{
    u8 i=0;
	if(frame_num==1)
	{
    for(i=0;i<8;i++)
    {
		//printf("%d\n",rowdata[i+41]);
        if(rowdata[i+41]>IR_DATA_1_MIN&&rowdata[i+41]<IR_DATA_1_MAX)
          ir_data[5]|=(0x80>>i);
          else if (rowdata[i+41]<IR_DATA_0_MAX&&rowdata[i+41]>IR_DATA_0_MIN)
          ir_data[5]&=~(0x80>>i);
    }
	}else if(frame_num==2)
	{
					for(i=0;i<8;i++)
      {
//        printf("%d\n",rowdata[i+91]);
        if(rowdata[i+91]>IR_DATA_1_MIN&&rowdata[i+91]<IR_DATA_1_MAX)
          ir_data[5]|=(0x80>>i);
        else if (rowdata[i+91]<IR_DATA_0_MAX&&rowdata[i+91]>IR_DATA_0_MIN)
          ir_data[5]&=~(0x80>>i);
      }
	}
    return ir_data[5];   
}
/*
  功能：对接收到的原始数据进行解码
  输入：IR_Buffer_Row 原始数据数组首地址
  输出：ir_data[x],x=0->A,x=1->NA,x=2->B,x=3->NB,x=4->C,x=5->NC,
*/
u8 Decode_IRDta(void)
{
	if(Lead_cheak(IR_Buffer_Row))//对第一帧Lead检查
	{

		if((decode_A(IR_Buffer_Row)==(u8)~decode_NA(IR_Buffer_Row))&&
			 (decode_B(IR_Buffer_Row)==(u8)~decode_NB(IR_Buffer_Row))&&
		   (decode_C(IR_Buffer_Row)==(u8)~decode_NC(IR_Buffer_Row)))//对第一帧数据进行解码并反码校验
		{
			  frame_num=0;
			  return 1;
		}
		else{ 
			    if(Separation_cheak(IR_Buffer_Row))//第一帧检查失败，进行间隔码检查
		      {
			       if((decode_A(IR_Buffer_Row)==(u8)~decode_NA(IR_Buffer_Row))&&
			          (decode_B(IR_Buffer_Row)==(u8)~decode_NB(IR_Buffer_Row))&&
		            (decode_C(IR_Buffer_Row)==(u8)~decode_NC(IR_Buffer_Row)))//间隔码检查通过对第二帧数据进行解码并反码晓校验
			           //第二帧解码前没有对第二帧的Lead码进行验证，但我觉得没有必要，那我就不验证哒
			          {
									frame_num=0;
			            return 2;
			          }else
			            {//第二帧反码也校验识别，没得玩了
										frame_num=0;
			              return 3;
			            }
		      }
		      else
			    return 4;//间隔码检测失败
				}
	}
	else
	{
		frame_num=0;
		return 5;
	}
}
//void IR_BaseTempControl(u8 temp)

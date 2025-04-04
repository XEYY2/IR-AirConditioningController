#include "delay.h" 
u8 fac_us,fac_ms;

/*******************************************************************************
* ������      : delay_Init
* ����		    : ��ϵͳ�δ�ʱ����ʼ��
* ����        : SYSCLK,�����õ�ϵͳʱ��Ƶ���磺72�������������ļ�������
* ����ֵ      : ��
* ����        : ��
*******************************************************************************/ 
void delay_Init(u8 SYSCLK)
{
	u32 reload;
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);//����ΪHCLK����Ƶ,������stm32f10x_system.c�ļ��ж�����36MHZϵͳʱ��Ƶ��
	fac_us=SYSCLK;//ÿ΢����Ҫ�Ƶ�������nMHZִ��n��Ϊ΢��
	reload=SYSCLK;
	reload=(reload*1000000/configTICK_RATE_HZ);
	//�ȳ���1000000�ָ�Mhz���ٳ���OS����Ƶ�ʣ��õ�����������
	//������װֵ�������жϿɵõ���Ҫ��OS����Ƶ��
	fac_ms=1000/configTICK_RATE_HZ;//t=1/f(��)
	//ϵͳƵ���µ���С��λʱ�䣬����1000��ΪҪ���Ǻ���
	SysTick->CTRL|=SysTick_CTRL_TICKINT_Msk;//����SYSTICK�ж�
	SysTick->LOAD=reload;//ÿ1/configTICK_RATE_HZ���ж�һ��
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;//����SYSTICK
}
/*******************************************************************************
* ������      : delay_us
* ����		    : ��ʱnus
* ����        : nus,Ҫ��ʱ��΢����
* ����ֵ      : ��
* ����        : ��ΪOS�Ĺ���Ƶ���趨Ϊ1000����Сʱ�䵥λ1ms��
                �޷���ʹ��OS�������ʱus��ʱ��
*******************************************************************************/ 
void delay_us(uint32_t nus)
{
	u32 ticks;
	u32 told ,tnow,tcnt=0;
	u32 reload=SysTick->LOAD;//��ȡ��ǰ��װLOADֵ
	ticks=(nus*fac_us);//��Ҫ�Ľ�����
	told=SysTick->VAL;//����ʱ�ļ�����ֵ
	while(1)
	{
		tnow=SysTick->VAL;//��ȡ��ǰֵ
		if(tnow!=told)//���ǵ�Ƭ�������ˣ�oldֵ��nowֵû�仯���ǾͿ�����while��ȴ��ָ�
		{
			if(tnow<told)
			{
				tcnt+=told-tnow;//��¼���μ���ļ����������ۼ�
			}
			else//Ϊ�������nowֵ��0��װ��ͻ������ӵ���װֵ������
			{
				tcnt+=(told-(tnow-reload));
			}
			told = tnow;//����Oldֵ
			if(tcnt>=ticks)
				break;//ʱ����ڻ򳬹�Ҫ��ʱ��ʱ�䣬�˳�while
		}
	}
}
/*******************************************************************************
* ������      : delay_ms
* ����		    : ��ʱnms
* ����        : nms,Ҫ��ʱ�ĺ�����
* ����ֵ      : ��
* ����        : �������������
*******************************************************************************/ 
void delay_ms(uint32_t nms)
{
//	if(1/*xTaskGetSchedulerState()!=taskSCHEDULER_NOT_STARTED*/)
		if(xTaskGetSchedulerState()!=taskSCHEDULER_NOT_STARTED)
	{
		if(nms>=fac_ms)
		{
			vTaskDelay(nms/fac_ms);//�������ĺ���ʹ�����������ʱ
		}
		nms%=fac_ms;		
	}
	delay_us((u32)(nms*1000));//���²��ֻ���΢������ͨ΢����ʱ
}
/*******************************************************************************
* ������      : delay_ms
* ����		    : ��ʱnms
* ����        : nms,Ҫ��ʱ�ĺ�����
* ����ֵ      : ��
* ����        : ���������������
*******************************************************************************/ 
void delay_xms(uint32_t nms)
{
	u32 i;
	for(i=0;i<nms;i++)
	delay_us(1000);
}
void Clock_Test(uint32_t ticks)
{

	u32 told ,tnow,tcnt=0;
	u32 reload=SysTick->LOAD;//��ȡ��ǰ��װLOADֵ
//	ticks=(nus*fac_us);//��Ҫ�Ľ�����
	told=SysTick->VAL;//����ʱ�ļ�����ֵ
	while(1)
	{
		tnow=SysTick->VAL;//��ȡ��ǰֵ
		if(tnow!=told)//���ǵ�Ƭ�������ˣ�oldֵ��nowֵû�仯���ǾͿ�����while��ȴ��ָ�
		{
			if(tnow<told)
			{
				tcnt+=told-tnow;//��¼���μ���ļ����������ۼ�
			}
			else//Ϊ�������nowֵ��0��װ��ͻ������ӵ���װֵ������
			{
				tcnt+=(told-(tnow-reload));
			}
			told=tnow;
			if(tcnt>=ticks)
				break;//ʱ����ڻ򳬹�Ҫ��ʱ��ʱ�䣬�˳�while
		}
	}
}
void Clock_set(void)
{
	
}

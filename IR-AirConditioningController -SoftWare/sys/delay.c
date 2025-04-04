#include "delay.h" 
u8 fac_us,fac_ms;

/*******************************************************************************
* 函数名      : delay_Init
* 功能		    : 对系统滴答定时器初始化
* 参数        : SYSCLK,已设置的系统时钟频率如：72，其已在启动文件中设置
* 返回值      : 无
* 描述        : 无
*******************************************************************************/ 
void delay_Init(u8 SYSCLK)
{
	u32 reload;
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);//设置为HCLK不分频,本机在stm32f10x_system.c文件中定义了36MHZ系统时钟频率
	fac_us=SYSCLK;//每微秒需要计的数量，nMHZ执计n次为微秒
	reload=SYSCLK;
	reload=(reload*1000000/configTICK_RATE_HZ);
	//先乘以1000000恢复Mhz，再除以OS工作频率，得到计数次数，
	//按此重装值计数的中断可得到想要的OS工作频率
	fac_ms=1000/configTICK_RATE_HZ;//t=1/f(秒)
	//系统频率下的最小单位时间，乘以1000因为要的是毫秒
	SysTick->CTRL|=SysTick_CTRL_TICKINT_Msk;//开启SYSTICK中断
	SysTick->LOAD=reload;//每1/configTICK_RATE_HZ秒中断一次
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;//开启SYSTICK
}
/*******************************************************************************
* 函数名      : delay_us
* 功能		    : 延时nus
* 参数        : nus,要延时的微秒数
* 返回值      : 无
* 描述        : 因为OS的工作频率设定为1000，最小时间单位1ms，
                无法在使用OS框架内延时us级时间
*******************************************************************************/ 
void delay_us(uint32_t nus)
{
	u32 ticks;
	u32 told ,tnow,tcnt=0;
	u32 reload=SysTick->LOAD;//读取当前重装LOAD值
	ticks=(nus*fac_us);//需要的节拍数
	told=SysTick->VAL;//进入时的计数器值
	while(1)
	{
		tnow=SysTick->VAL;//读取当前值
		if(tnow!=told)//除非单片机卡死了，old值和now值没变化，那就卡死在while里等待恢复
		{
			if(tnow<told)
			{
				tcnt+=told-tnow;//记录两次间隔的计数数量并累加
			}
			else//为特殊情况now值到0重装后突变的增加的重装值做修正
			{
				tcnt+=(told-(tnow-reload));
			}
			told = tnow;//更新Old值
			if(tcnt>=ticks)
				break;//时间等于或超过要延时的时间，退出while
		}
	}
}
/*******************************************************************************
* 函数名      : delay_ms
* 功能		    : 延时nms
* 参数        : nms,要延时的毫秒数
* 返回值      : 无
* 描述        : 会引起任务调度
*******************************************************************************/ 
void delay_ms(uint32_t nms)
{
//	if(1/*xTaskGetSchedulerState()!=taskSCHEDULER_NOT_STARTED*/)
		if(xTaskGetSchedulerState()!=taskSCHEDULER_NOT_STARTED)
	{
		if(nms>=fac_ms)
		{
			vTaskDelay(nms/fac_ms);//整数倍的毫秒使用任务调度延时
		}
		nms%=fac_ms;		
	}
	delay_us((u32)(nms*1000));//余下部分化成微秒用普通微秒延时
}
/*******************************************************************************
* 函数名      : delay_ms
* 功能		    : 延时nms
* 参数        : nms,要延时的毫秒数
* 返回值      : 无
* 描述        : 不会引起任务调度
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
	u32 reload=SysTick->LOAD;//读取当前重装LOAD值
//	ticks=(nus*fac_us);//需要的节拍数
	told=SysTick->VAL;//进入时的计数器值
	while(1)
	{
		tnow=SysTick->VAL;//读取当前值
		if(tnow!=told)//除非单片机卡死了，old值和now值没变化，那就卡死在while里等待恢复
		{
			if(tnow<told)
			{
				tcnt+=told-tnow;//记录两次间隔的计数数量并累加
			}
			else//为特殊情况now值到0重装后突变的增加的重装值做修正
			{
				tcnt+=(told-(tnow-reload));
			}
			told=tnow;
			if(tcnt>=ticks)
				break;//时间等于或超过要延时的时间，退出while
		}
	}
}
void Clock_set(void)
{
	
}

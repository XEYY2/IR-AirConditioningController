#include "tftlcd.h"
#include "stdlib.h"
#include "stdint.h"
#include "font.h" 
//#include "usart.h"	 	   
#include "Led.h"
#include "MPU6050.h"
#include "FreeRTOS.h"
#include "task.h"
#include "delay.h"
//LCD   
u16 FRONT_COLOR=WHITE;	//å‰é¢œè‰²
u16 BACK_COLOR=BLACK;  //åé¢œè‰²

_tftlcd_data tftlcd_data; 
  

//æ ¹æ®å±å¹•å‹å·ä¸åŒï¼Œé‡‡ç”¨ä¸åŒæ ¼å¼å‘é€å‘½ä»¤
//cmd:å‘½ä»¤Öµ
void LCD_WriteCmd(u16 cmd)
{
	TFTLCD->LCD_CMD=cmd;
}

//æ ¹æ®å±å¹•å‹å·ä¸åŒï¼Œé‡‡ç”¨ä¸åŒæ ¼å¼å‘é€å‘½ä»¤
//data:æ•°æ®
void LCD_WriteData(u16 data)
{

	TFTLCD->LCD_DATA=data;
}

void LCD_WriteCmdData(u16 cmd,u16 data)
{
	LCD_WriteCmd(cmd);
	LCD_WriteData(data);
}


u32 LCD_RGBColor_Change(u16 color)
{
	u8 r,g,b=0;
	
	r=(color>>11)&0x1f;//å°†çº¢è‰²æ•°æ®ç§»åŠ¨åˆ°ä½5ä½ï¼Œç„¶ååªå–ä½5ä½
	g=(color>>5)&0x3f;//å°†ç»¿è‰²éƒ¨åˆ†ç§»åŠ¨åˆ°ä½6ä½ï¼Œç„¶ååªå–ä½6ä½
	b=color&0x1f;//åªå–ä½5ä½
	
	return ((r<<13)|(g<<6)|(b<<1));
}
void LCD_WriteData_Color(u16 color)
{
	TFTLCD->LCD_DATA=color>>8;//å°†DATAçš„é«˜8ä½æ¸…é›¶ï¼Œ
	TFTLCD->LCD_DATA=color&0xff;//å†å†™å…¥ä½8ä½åˆ°LCD_DATAä½8ä½ä½ç½®
}

//¶ÁÊı¾İ
//·µ»ØÖµ:¶Áµ½µÄÖµ
u16 LCD_ReadData(void)
{
	u16 ram;		
	ram=TFTLCD->LCD_DATA;
	ram=ram<<8;//å°†ä½8ä½ç§»åŠ¨åˆ°é«˜8ä½ä½ç½®
	ram|=TFTLCD->LCD_DATA;//å°†TFTLCD->LCD_DATAä¸­çš„é«˜ä½8ä½æˆ–æ“ä½œï¼Œä½8ä½ä¸å˜è¿›è¡Œåˆå¹¶	
	return ram;		
}


//ÉèÖÃLCDÏÔÊ¾·½Ïò
//dir:0,ç«–å±
void LCD_Display_Dir(u8 dir)
{
	tftlcd_data.dir=dir;         //ºáÆÁ/ÊúÆÁ
	if(dir==0)  //Ä¬ÈÏÊúÆÁ·½Ïò
	{		
		LCD_WriteCmdData(0x36,0x08);
		tftlcd_data.height=480;
		tftlcd_data.width=320;
	}
	else
	{	
		LCD_WriteCmdData(0x36,0x28);//Ô­ox28
		LCD_WriteCmd(0XB6);
		LCD_WriteData(0x00);
		LCD_WriteData(0x02);
		LCD_WriteData(0x3B);
		tftlcd_data.height=320;
		tftlcd_data.width=480;
	}	
}

//ÉèÖÃ´°¿Ú,²¢×Ô¶¯ÉèÖÃ»­µã×ø±êµ½´°¿Ú×óÉÏ½Ç(sx,sy).
//sx,sy:´°¿ÚÆğÊ¼×ø±ê(×óÉÏ½Ç)
//width,height:´°¿Ú¿í¶ÈºÍ¸ß¶È,±ØĞë´óÓÚ0!!
//´°Ìå´óĞ¡:width*height. 
void LCD_Set_Window(u16 sx,u16 sy,u16 width,u16 height)
{    
	LCD_WriteCmd(0x2A);
	LCD_WriteData(sx/256);   
	LCD_WriteData(sx%256); 	 
	LCD_WriteData(width/256); 
	LCD_WriteData(width%256);
	
	LCD_WriteCmd(0x2B);
	LCD_WriteData(sy/256);  
	LCD_WriteData(sy%256);
	LCD_WriteData(height/256); 
	LCD_WriteData(height%256); 	

	LCD_WriteCmd(0x2C);
}

//¶ÁÈ¡¸öÄ³µãµÄÑÕÉ«Öµ	 
//x,y:×ø±ê
//·µ»ØÖµ:´ËµãµÄÑÕÉ«
u16 LCD_ReadPoint(u16 x,u16 y)
{
 	u16 r=0,g=0,b=0;
	if(x>=tftlcd_data.width||y>=tftlcd_data.height)return 0;	//³¬¹ıÁË·¶Î§,Ö±½Ó·µ»Ø		     
	LCD_Set_Window(x, y, x, y);
	
	LCD_WriteCmd(0X2e);
	r=LCD_ReadData();								//dummy Read     		 				    
	r=LCD_ReadData();								//Êµ¼Ê×ø±êÑÕÉ«	
	b=r>>8;
	g=r<<8;
	r=g+b;
//	printf("r=%X\r\n",r);	
	//printf("g=%X\r\n",g);
	//printf("b=%X\r\n",b);
 	return r;						
}

void TFTLCD_GPIO_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE|RCC_APB2Periph_GPIOG,ENABLE);//å¼€å¯PORT:B,D,E,Gæ—¶é’Ÿ
	//9ä¸ªIO
 	//PORTDåˆå§‹åŒ–  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_14|GPIO_Pin_15;				 //	//PORTD¸´ÓÃÍÆÍìÊä³ö  
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 		 //å¤ç”¨æ¨æŒ½è¾“å‡º   
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOD, &GPIO_InitStructure); 
  //9ä¸ª	 
	//PORTEåˆå§‹åŒ–  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;				 //	//PORTD¸´ÓÃÍÆÍìÊä³ö  
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 		 //å¤ç”¨æ¨æŒ½è¾“å‡º   
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOE, &GPIO_InitStructure);    	    	 											 
  //2ä¸ª
  //	//PORTG12 G0åˆå§‹åŒ–	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_12;	 //	//PORTG
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 		 //å¤ç”¨æ¨æŒ½è¾“å‡º   
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOG, &GPIO_InitStructure);  
	//1ä¸ª
	//GPIOE5åˆå§‹åŒ–
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;				 //PE5 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //æ¨æŒ½è¾“å‡º
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOE, &GPIO_InitStructure);
	LCD_LED=1;				//åˆå§‹åŒ–LED
	
}

void TFTLCD_FSMC_Init(void)
{
	//é™æ€å­˜å‚¨å™¨æ§åˆ¶å™¨NOR/SRAMæ¥å£çš„ä¸€ä¸ªç»“æ„ä½“ç±»å‹
	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure; 
   FSMC_NORSRAMTimingInitTypeDef  FSMC_ReadTimingInitStructure; 
	FSMC_NORSRAMTimingInitTypeDef  FSMC_WriteTimingInitStructure;
	
  	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC,ENABLE);	//Ê¹ÄÜFSMCÊ±ÖÓ
	
	FSMC_ReadTimingInitStructure.FSMC_AddressSetupTime = 0x01;	 //µØÖ·½¨Á¢Ê±¼ä£¨ADDSET£©Îª2¸öHCLK 1/36M=27ns
  	FSMC_ReadTimingInitStructure.FSMC_AddressHoldTime = 0x00;	 //µØÖ·±£³ÖÊ±¼ä£¨ADDHLD£©Ä£Ê½AÎ´ÓÃµ½	
  	FSMC_ReadTimingInitStructure.FSMC_DataSetupTime = 0x0f;		 // Êı¾İ±£´æÊ±¼äÎª16¸öHCLK,ÒòÎªÒº¾§Çı¶¯ICµÄ¶ÁÊı¾İµÄÊ±ºò£¬ËÙ¶È²»ÄÜÌ«¿ì£¬ÓÈÆä¶Ô1289Õâ¸öIC¡£
  	FSMC_ReadTimingInitStructure.FSMC_BusTurnAroundDuration = 0x00;
  	FSMC_ReadTimingInitStructure.FSMC_CLKDivision = 0x00;
  	FSMC_ReadTimingInitStructure.FSMC_DataLatency = 0x00;
  	FSMC_ReadTimingInitStructure.FSMC_AccessMode = FSMC_AccessMode_A;	 //Ä£Ê½A 

	FSMC_WriteTimingInitStructure.FSMC_AddressSetupTime = 0x15;	 //µØÖ·½¨Á¢Ê±¼ä£¨ADDSET£©Îª16¸öHCLK  
  	FSMC_WriteTimingInitStructure.FSMC_AddressHoldTime = 0x15;	 //µØÖ·±£³ÖÊ±¼ä		
  	FSMC_WriteTimingInitStructure.FSMC_DataSetupTime = 0x05;		 //Êı¾İ±£´æÊ±¼äÎª6¸öHCLK	
  	FSMC_WriteTimingInitStructure.FSMC_BusTurnAroundDuration = 0x00;
  	FSMC_WriteTimingInitStructure.FSMC_CLKDivision = 0x00;
  	FSMC_WriteTimingInitStructure.FSMC_DataLatency = 0x00;
  	FSMC_WriteTimingInitStructure.FSMC_AccessMode = FSMC_AccessMode_A;	 //Ä£Ê½A  

 
  	FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM4;//  ÕâÀïÎÒÃÇÊ¹ÓÃNE4 £¬Ò²¾Í¶ÔÓ¦BTCR[6],[7]¡£
  	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable; // ²»¸´ÓÃÊı¾İµØÖ·
  	FSMC_NORSRAMInitStructure.FSMC_MemoryType =FSMC_MemoryType_SRAM;// FSMC_MemoryType_SRAM;  //SRAM   
  	FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;//´æ´¢Æ÷Êı¾İ¿í¶ÈÎª16bit   
  	FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode =FSMC_BurstAccessMode_Disable;// FSMC_BurstAccessMode_Disable; 
  	FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait=FSMC_AsynchronousWait_Disable; 
  	FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;   
  	FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;  
  	FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;	//  ´æ´¢Æ÷Ğ´Ê¹ÄÜ
  	FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;   
  	FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable; // ¶ÁĞ´Ê¹ÓÃ²»Í¬µÄÊ±Ğò
  	FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable; 
  	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &FSMC_ReadTimingInitStructure; //¶ÁĞ´Ê±Ğò
  	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &FSMC_WriteTimingInitStructure;  //Ğ´Ê±Ğò

  	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);  //³õÊ¼»¯FSMCÅäÖÃ

 	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);  // Ê¹ÄÜBANK1 
	
}
	 
//SSD1963 ±³¹âÉèÖÃ
//pwm:±³¹âµÈ¼¶,0~100.Ô½´óÔ½ÁÁ.
void LCD_SSD_BackLightSet(u8 pwm)
{	
	LCD_WriteCmd(0xBE);	//ÅäÖÃPWMÊä³ö
	LCD_WriteData(0x05);	//1ÉèÖÃPWMÆµÂÊ
	LCD_WriteData(pwm*2.55);//2ÉèÖÃPWMÕ¼¿Õ±È
	LCD_WriteData(0x01);	//3ÉèÖÃC
	LCD_WriteData(0xFF);	//4ÉèÖÃD
	LCD_WriteData(0x00);	//5ÉèÖÃE
	LCD_WriteData(0x00);	//6ÉèÖÃF
}

void TFTLCD_Init(void)
{

	TFTLCD_GPIO_Init();
	TFTLCD_FSMC_Init();
	
	delay_ms(50); 
	
	LCD_WriteCmd(0xE0); //P-Gamma
	LCD_WriteData(0x00);
	LCD_WriteData(0x13);
	LCD_WriteData(0x18);
	LCD_WriteData(0x04);
	LCD_WriteData(0x0F);
	LCD_WriteData(0x06);
	LCD_WriteData(0x3A);
	LCD_WriteData(0x56);
	LCD_WriteData(0x4D);
	LCD_WriteData(0x03);
	LCD_WriteData(0x0A);
	LCD_WriteData(0x06);
	LCD_WriteData(0x30);
	LCD_WriteData(0x3E);
	LCD_WriteData(0x0F);
	
	LCD_WriteCmd(0XE1); //N-Gamma
	LCD_WriteData(0x00);
	LCD_WriteData(0x13);
	LCD_WriteData(0x18);
	LCD_WriteData(0x01);
	LCD_WriteData(0x11);
	LCD_WriteData(0x06);
	LCD_WriteData(0x38);
	LCD_WriteData(0x34);
	LCD_WriteData(0x4D);
	LCD_WriteData(0x06);
	LCD_WriteData(0x0D);
	LCD_WriteData(0x0B);
	LCD_WriteData(0x31);
	LCD_WriteData(0x37);
	LCD_WriteData(0x0F); 
	
	LCD_WriteCmd(0XC0);   //Power Control 1
	LCD_WriteData(0x18); //Vreg1out
	LCD_WriteData(0x17); //Verg2out
	
	LCD_WriteCmd(0xC1);   //Power Control 2
	LCD_WriteData(0x41); //VGH,VGL
	
	LCD_WriteCmd(0xC5);   //Power Control 3
	LCD_WriteData(0x00);
	LCD_WriteData(0x1A); //Vcom
	LCD_WriteData(0x80);
	
	LCD_WriteCmd(0x36);   //Memory Access
	LCD_WriteData(0x08);   //08
	
	LCD_WriteCmd(0x3A);   // Interface Pixel Format
	LCD_WriteData(0x55); //16bit
	
	LCD_WriteCmd(0XB0);   // Interface Mode Control
	LCD_WriteData(0x00);
	
	LCD_WriteCmd(0xB1);   //Frame rate
	LCD_WriteData(0xA0); //60Hz
	
	LCD_WriteCmd(0xB4);   //Display Inversion Control
	LCD_WriteData(0x02); //2-dot
	
	LCD_WriteCmd(0XB6);   //RGB/MCU Interface Control
	LCD_WriteData(0x02); //MCU RGB
	LCD_WriteData(0x02); //Source,Gate scan dieection
	LCD_WriteData(0x00);
	LCD_WriteData(0x22);//0 GS SS SM ISC[3:0];??GS SS??§¥??§Ó¨¬????R36
	LCD_WriteData(0x3B);
	
	LCD_WriteCmd(0XE9);    // Set Image Function
	LCD_WriteData(0x00);  //disable 24 bit data input
	
	LCD_WriteCmd(0xF7);    // Adjust Control
	LCD_WriteData(0xA9);
	LCD_WriteData(0x51);
	LCD_WriteData(0x2C);
	LCD_WriteData(0x82);  // D7 stream, loose
	
	LCD_WriteCmd(0x11);    //Sleep out
	delay_ms(120);
	LCD_WriteCmd(0x29);  
	LCD_WriteCmd(0x2C);
//	LCD_WriteCmd(0Xda);	
//	tftlcd_data.id=TFTLCD->LCD_DATA;	//dummy read 
//	tftlcd_data.id=TFTLCD->LCD_DATA<<8;
//	LCD_WriteCmd(0Xdb);	
//	tftlcd_data.id|=TFTLCD->LCD_DATA;	//dummy read 
//	tftlcd_data.id|=TFTLCD->LCD_DATA;	
//	


	LCD_Display_Dir(TFTLCD_DIR);		//0£ºÊúÆÁ  1£ººáÆÁ  Ä¬ÈÏÊúÆÁ
//	LCD_Clear(DARKBLUE);
}

//ÇåÆÁº¯Êı
//color:ÒªÇåÆÁµÄÌî³äÉ«
void LCD_Clear(u16 color)
{
	uint16_t i, j ;

	LCD_Set_Window(0, 0, tftlcd_data.width-1, tftlcd_data.height-1);	 //×÷ÓÃÇøÓò
  	for(i=0; i<tftlcd_data.width; i++)
	{
		for (j=0; j<tftlcd_data.height; j++)
		{
			LCD_WriteData_Color(color);
		}
	} 
}


//ÔÚÖ¸¶¨ÇøÓòÄÚÌî³äµ¥¸öÑÕÉ«
//(sx,sy),(ex,ey):Ìî³ä¾ØĞÎ¶Ô½Ç×ø±ê,ÇøÓò´óĞ¡Îª:(ex-sx+1)*(ey-sy+1)   
//color:ÒªÌî³äµÄÑÕÉ«
void LCD_Fill(u16 xState,u16 yState,u16 xEnd,u16 yEnd,u16 color)
{          
	uint16_t temp;

    if((xState > xEnd) || (yState > yEnd))
    {
        return;
    }   
	LCD_Set_Window(xState, yState, xEnd, yEnd); 
    xState = xEnd - xState + 1;
	yState = yEnd - yState + 1;

	while(xState--)
	{
	 	temp = yState;
		while (temp--)
	 	{			
			LCD_WriteData_Color(color);	
		}
	}	
} 

//ÔÚÖ¸¶¨ÇøÓòÄÚÌî³äÖ¸¶¨ÑÕÉ«¿é			 
//(sx,sy),(ex,ey):Ìî³ä¾ØĞÎ¶Ô½Ç×ø±ê,ÇøÓò´óĞ¡Îª:(ex-sx+1)*(ey-sy+1)   
//color:ÒªÌî³äµÄÑÕÉ«
void LCD_Color_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 *color)
{  
	u16 height,width;
	u16 i,j;
	width=ex-sx+1; 			//µÃµ½Ìî³äµÄ¿í¶È
	height=ey-sy+1;			//¸ß¶È
	
	for(i=0;i<height;i++)
	{
		for(j=0;j<width;j++)
		{
			LCD_Set_Window(sx+j, sy+i,ex, ey);
			LCD_WriteData_Color(color[i*width+j]);
		}
	}		  
}
//»­µã
//x,y:×ø±ê
//FRONT_COLOR:´ËµãµÄÑÕÉ«
void LCD_DrawPoint(u16 x,u16 y)
{
	LCD_Set_Window(x, y, x, y);  //ÉèÖÃµãµÄÎ»ÖÃ
	LCD_WriteData_Color(FRONT_COLOR);	
}

//¿ìËÙ»­µã
//x,y:×ø±ê
//color:ÑÕÉ«
void LCD_DrawFRONT_COLOR(u16 x,u16 y,u16 color)
{	   
	LCD_Set_Window(x, y, x, y);
	LCD_WriteData_Color(color);	
} 

//»­Ïß
//x1,y1:Æğµã×ø±ê
//x2,y2:ÖÕµã×ø±ê  
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	delta_x=x2-x1; //¼ÆËã×ø±êÔöÁ¿ 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //ÉèÖÃµ¥²½·½Ïò 
	else if(delta_x==0)incx=0;//´¹Ö±Ïß 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//Ë®Æ½Ïß 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //Ñ¡È¡»ù±¾ÔöÁ¿×ø±êÖá 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//»­ÏßÊä³ö 
	{  
		LCD_DrawPoint(uRow,uCol);//»­µã 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
} 

void LCD_DrawLine_Color(u16 x1, u16 y1, u16 x2, u16 y2,u16 color)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	delta_x=x2-x1; //¼ÆËã×ø±êÔöÁ¿ 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //ÉèÖÃµ¥²½·½Ïò 
	else if(delta_x==0)incx=0;//´¹Ö±Ïß 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//Ë®Æ½Ïß 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //Ñ¡È¡»ù±¾ÔöÁ¿×ø±êÖá 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//»­ÏßÊä³ö 
	{  
		LCD_DrawFRONT_COLOR(uRow,uCol,color);//»­µã 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
} 


// »­Ò»¸öÊ®×ÖµÄ±ê¼Ç
// x£º±ê¼ÇµÄX×ø±ê
// y£º±ê¼ÇµÄY×ø±ê
// color£º±ê¼ÇµÄÑÕÉ«
void LCD_DrowSign(uint16_t x, uint16_t y, uint16_t color)
{
    uint8_t i;

    /* »­µã */
    LCD_Set_Window(x-1, y-1, x+1, y+1);
    for(i=0; i<9; i++)
    {
		LCD_WriteData_Color(color);   
    }

    /* »­Êú */
    LCD_Set_Window(x-4, y, x+4, y);
    for(i=0; i<9; i++)
    {
		LCD_WriteData_Color(color); 
    }

    /* »­ºá */
    LCD_Set_Window(x, y-4, x, y+4);
    for(i=0; i<9; i++)
    {
		LCD_WriteData_Color(color); 
    }
}

//»­¾ØĞÎ	  
//(x1,y1),(x2,y2):¾ØĞÎµÄ¶Ô½Ç×ø±ê
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2)
{
	LCD_DrawLine(x1,y1,x2,y1);
	LCD_DrawLine(x1,y1,x1,y2);
	LCD_DrawLine(x1,y2,x2,y2);
	LCD_DrawLine(x2,y1,x2,y2);
}
//ÔÚÖ¸¶¨Î»ÖÃ»­Ò»¸öÖ¸¶¨´óĞ¡µÄÔ²
//(x,y):ÖĞĞÄµã
//r    :°ë¾¶
void LCD_Draw_Circle(u16 x0,u16 y0,u8 r)
{
	int a,b;
	int di;
	a=0;b=r;	  
	di=3-(r<<1);             //ÅĞ¶ÏÏÂ¸öµãÎ»ÖÃµÄ±êÖ¾
	while(a<=b)
	{
		LCD_DrawPoint(x0+a,y0-b);             //5
 		LCD_DrawPoint(x0+b,y0-a);             //0           
		LCD_DrawPoint(x0+b,y0+a);             //4               
		LCD_DrawPoint(x0+a,y0+b);             //6 
		LCD_DrawPoint(x0-a,y0+b);             //1       
 		LCD_DrawPoint(x0-b,y0+a);             
		LCD_DrawPoint(x0-a,y0-b);             //2             
  		LCD_DrawPoint(x0-b,y0-a);             //7     	         
		a++;
		//Ê¹ÓÃBresenhamËã·¨»­Ô²     
		if(di<0)di +=4*a+6;	  
		else
		{
			di+=10+4*(a-b);   
			b--;
		} 						    
	}
} 



//ÔÚÖ¸¶¨Î»ÖÃÏÔÊ¾Ò»¸ö×Ö·û
//x,y:ÆğÊ¼×ø±ê
//num:ÒªÏÔÊ¾µÄ×Ö·û:" "--->"~"
//size:×ÖÌå´óĞ¡ 12/16/24
//mode:µş¼Ó·½Ê½(1)»¹ÊÇ·Çµş¼Ó·½Ê½(0)
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode)
{  							  
    u8 temp,t1,t;
	u16 y0=y;
	u8 csize=(size/8+((size%8)?1:0))*(size/2);		//µÃµ½×ÖÌåÒ»¸ö×Ö·û¶ÔÓ¦µãÕó¼¯ËùÕ¼µÄ×Ö½ÚÊı	
 	num=num-' ';//µÃµ½Æ«ÒÆºóµÄÖµ£¨ASCII×Ö¿âÊÇ´Ó¿Õ¸ñ¿ªÊ¼È¡Ä££¬ËùÒÔ-' '¾ÍÊÇ¶ÔÓ¦×Ö·ûµÄ×Ö¿â£©
	for(t=0;t<csize;t++)
	{   
		if(size==12)temp=ascii_1206[num][t]; 	 	//µ÷ÓÃ1206×ÖÌå
		else if(size==16)temp=ascii_1608[num][t];	//µ÷ÓÃ1608×ÖÌå
		else if(size==24)temp=ascii_2412[num][t];	//µ÷ÓÃ2412×ÖÌå
		else return;								//Ã»ÓĞµÄ×Ö¿â
		for(t1=0;t1<8;t1++)
		{			    
			if(temp&0x80)LCD_DrawFRONT_COLOR(x,y,FRONT_COLOR);
			else if(mode==0)LCD_DrawFRONT_COLOR(x,y,BACK_COLOR);
			temp<<=1;
			y++;
			if(y>=tftlcd_data.height)return;		//³¬ÇøÓòÁË
			if((y-y0)==size)
			{
				y=y0;
				x++;
				if(x>=tftlcd_data.width)return;	//³¬ÇøÓòÁË
				break;
			}
		}  	 
	}  	    	   	 	  
}   
//m^nº¯Êı
//·µ»ØÖµ:m^n´Î·½.
u32 LCD_Pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}			 
//ÏÔÊ¾Êı×Ö,¸ßÎ»Îª0,Ôò²»ÏÔÊ¾
//x,y :Æğµã×ø±ê	 
//len :Êı×ÖµÄÎ»Êı
//size:×ÖÌå´óĞ¡
//color:ÑÕÉ« 
//num:ÊıÖµ(0~4294967295);	 
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size)
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				LCD_ShowChar(x+(size/2)*t,y,' ',size,0);
				continue;
			}else enshow=1; 
		 	 
		}
	 	LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,0); 
	}
} 

//ÏÔÊ¾Êı×Ö,¸ßÎ»Îª0,»¹ÊÇÏÔÊ¾
//x,y:Æğµã×ø±ê
//num:ÊıÖµ(0~999999999);	 
//len:³¤¶È(¼´ÒªÏÔÊ¾µÄÎ»Êı)
//size:×ÖÌå´óĞ¡
//mode:
//[7]:0,²»Ìî³ä;1,Ìî³ä0.
//[6:1]:±£Áô
//[0]:0,·Çµş¼ÓÏÔÊ¾;1,µş¼ÓÏÔÊ¾.
void LCD_ShowxNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode)
{  
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				if(mode&0X80)LCD_ShowChar(x+(size/2)*t,y,'0',size,mode&0X01);  
				else LCD_ShowChar(x+(size/2)*t,y,' ',size,mode&0X01);  
 				continue;
			}else enshow=1; 
		 	 
		}
	 	LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,mode&0X01); 
	}
} 
//ÏÔÊ¾×Ö·û´®
//x,y:Æğµã×ø±ê
//width,height:ÇøÓò´óĞ¡  
//size:×ÖÌå´óĞ¡
//*p:×Ö·û´®ÆğÊ¼µØÖ·		  
void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p)
{         
	u8 x0=x;
	width+=x;
	height+=y;
    while((*p<='~')&&(*p>=' '))//ÅĞ¶ÏÊÇ²»ÊÇ·Ç·¨×Ö·û!
    {       
        if(x>=width){x=x0;y+=size;}
        if(y>=height)break;//ÍË³ö
        LCD_ShowChar(x,y,*p,size,0);
        x+=size/2;
        p++;
    }  
}

/****************************************************************************
*º¯ÊıÃû£ºLCD_ShowFontHZ
*Êä  Èë£ºx£ººº×ÖÏÔÊ¾µÄX×ø±ê
*      * y£ººº×ÖÏÔÊ¾µÄY×ø±ê
*      * cn£ºÒªÏÔÊ¾µÄºº×Ö
*      * wordColor£ºÎÄ×ÖµÄÑÕÉ«
*      * backColor£º±³¾°ÑÕÉ«
*Êä  ³ö£º
*¹¦  ÄÜ£ºĞ´¶şºÅ¿¬Ìåºº×Ö
****************************************************************************/
#if 0
void LCD_ShowFontHZ(u16 x, u16 y, u8 *cn)	 
{  
	u8 i, j, wordNum;
	u16 color;
	while (*cn != '\0')
	{
		LCD_Set_Window(x, y, x+31, y+28);
		for (wordNum=0; wordNum<20; wordNum++)
		{	//wordNumÉ¨Ãè×Ö¿âµÄ×ÖÊı
			if ((CnChar32x29[wordNum].Index[0]==*cn)
			     &&(CnChar32x29[wordNum].Index[1]==*(cn+1)))
			{
				for(i=0; i<116; i++) 
				{	//MSKµÄÎ»Êı
					color=CnChar32x29[wordNum].Msk[i];
					for(j=0;j<8;j++) 
					{
						if((color&0x80)==0x80)
						{
							LCD_WriteData_Color(FRONT_COLOR); 						
						} 						
						else
						{
							LCD_WriteData_Color(BACK_COLOR); 
						} 
						color<<=1;
					}//for(j=0;j<8;j++)½áÊø
				}    
			}
		} //for (wordNum=0; wordNum<20; wordNum++)½áÊø 	
		cn += 2;
		x += 32;
	}
}
#endif


#if 1
void LCD_ShowFontHZ(u16 x, u16 y, u8 *cn)
{
	u8 i, j, wordNum;
	u16 color;
	u16 x0=x; 
	u16 y0=y; 
	while (*cn != '\0')
	{
		for (wordNum=0; wordNum<20; wordNum++)
		{	//wordNumÉ¨Ãè×Ö¿âµÄ×ÖÊı
			if ((CnChar32x29[wordNum].Index[0]==*cn)
			     &&(CnChar32x29[wordNum].Index[1]==*(cn+1)))
			{
				for(i=0; i<116; i++) 
				{	//MSKµÄÎ»Êı
					color=CnChar32x29[wordNum].Msk[i];
					for(j=0;j<8;j++) 
					{
						if((color&0x80)==0x80)
						{
							LCD_DrawFRONT_COLOR(x,y,FRONT_COLOR);
						} 						
						else
						{
							LCD_DrawFRONT_COLOR(x,y,BACK_COLOR);
						} 
						color<<=1;
						x++;
						if((x-x0)==32)
						{
							x=x0;
							y++;
							if((y-y0)==29)
							{
								y=y0;
							}
						}
					}//for(j=0;j<8;j++)½áÊø
				}	
			}
			
		} //for (wordNum=0; wordNum<20; wordNum++)½áÊø 	
		cn += 2;
		x += 32;
		x0=x;
	}
}	
#endif

void LCD_ShowPicture(u16 x, u16 y, u16 wide, u16 high,u8 *pic)
{
	u16 i,j;
	u16 temp = 0;
	long tmp=0/*,num=0*/;
	LCD_Set_Window(x, y, x+wide-1, y+high-1);
//	num = wide * high*2 ;
	//·½·¨1£ºÌá¸ßÏÔÊ¾ËÙ¶È£¬Ì«¿ì¿ÉÄÜµ¼ÖÂÍ¼ÏñÏÔÊ¾²»È«
//	do
//	{  
//		temp = pic[tmp + 1];
//		temp = temp << 8;
//		temp = temp | pic[tmp];
//		LCD_WriteData_Color(temp);//ÖğµãÏÔÊ¾
//		tmp += 2;
//	}
//	while(tmp < num);
	
	//·½·¨2£º¿ÉÓĞĞ§Ïû³ıLCDÏÔÊ¾ËÙ¶È¹ı¿ìµ¼ÖÂÏÔÊ¾²»È«ÎÊÌâ
	for(i=0;i<high;i++)
	{
		for(j=0;j<wide;j++)
		{
			temp = pic[tmp + 1];
			temp = temp << 8;
			temp = temp | pic[tmp];
			LCD_DrawFRONT_COLOR(x+j,y+i,temp);
			tmp += 2;
		}
	}	
}
/****************************************************************************
*º¯ÊıÃû£ºLCD_Show_signed_u16_Num
*Êä  Èë£ºx£ºÆğÊ¼µÄX×ø±ê
*      * y£ºÆğÊ¼µÄY×ø±ê
*      * num£ºÒªÏÔÊ¾µÄÊı×Ö
*      * len£ºÊı×ÖµÄ³¤¶È,²»°üÀ¨·ûºÅ
*      * size£º×ÖÌå´óĞ¡
*      * mode£ºÏÔÊ¾Ä£Ê½£¬[7]:0,²»Ìî³ä;1,Ìî³ä0.
                        [6:1]:±£Áô
                        [0]:0,·Çµş¼ÓÏÔÊ¾;1,µş¼ÓÏÔÊ¾.
*¹¦  ÄÜ£ºÊä³ö´ø·ûºÅµÄ32Î»Êı×Ö£¨int£©
u16 x,u16 y,u16 num,u8 len,u8 size,u8 mode
****************************************************************************/
void LCD_Show_signed_u16_Num(u16 x,u16 y,int num,u8 len,u8 size,u8 mode)
{
	u8 t,temp;
	u8 enshow=0;
  if(num<0)	
  { 
		LCD_ShowChar(x+(size/2)*t,y,'-',size,mode&0X01);
		num=num*(-1);
		for(t=0;t<len;t++)//¶Ôlen Î»Öğ¸öÏÔÊ¾
	 {
		
		temp=(num/LCD_Pow(10,len-t-1))%10;//Pow 10µÄlen-t-1´Î·½£¬×÷ÓÃÈ¡³ölen³¤¶ÈµÄ×î¸ßÎ»Êı×Ö£¬%10ÊÇ±£Ö¤ÖµÔÚ0-9¼ä£¬¼´Ê¹·Ç·¨ÊäÈëÊı¾İÒ²ÄÜ±£Ö¤
		if(enshow==0&&t<(len-1))//×î¶àÖ»»áÏÔÊ¾len-1¸öÁã
		{
			if(temp==0)//ÏÔÊ¾0µÄ²¿·Ö
			{
				if(mode&0X80)
					LCD_ShowChar(x+(size/2)*(t+1),y,'0',size,mode&0X01);  
				else 
					LCD_ShowChar(x+(size/2)*(t+1),y,' ',size,mode&0X01);  
 				continue;
			}
			else enshow=1; 
		 	 
		}
	 	LCD_ShowChar(x+(size/2)*(t+1),y,temp+'0',size,mode&0X01); 
	 }
	}
	else
  {
	  LCD_ShowChar(x+(size/2)*t,y,'+',size,mode&0X01);
		for(t=0;t<len;t++)//¶Ôlen Î»Öğ¸öÏÔÊ¾
	 {
		temp=(num/LCD_Pow(10,len-t-1))%10;//Pow 10µÄlen-t-1´Î·½£¬×÷ÓÃÈ¡³ölen³¤¶ÈµÄ×î¸ßÎ»Êı×Ö£¬%10ÊÇ±£Ö¤ÖµÔÚ0-9¼ä£¬¼´Ê¹·Ç·¨ÊäÈëÊı¾İÒ²ÄÜ±£Ö¤
		if(enshow==0&&t<(len-1))//×î¶àÖ»»áÏÔÊ¾len-1¸öÁã
		{
			if(temp==0)//ÏÔÊ¾0µÄ²¿·Ö
			{
				if(mode&0X80)
					LCD_ShowChar(x+(size/2)*(t+1),y,'0',size,mode&0X01);  
				else 
					LCD_ShowChar(x+(size/2)*(t+1),y,' ',size,mode&0X01);  
 				continue;
			}
			else enshow=1; 
		 	 
		}
	 	LCD_ShowChar(x+(size/2)*(t+1),y,temp+'0',size,mode&0X01); 
	 }
  }	
}

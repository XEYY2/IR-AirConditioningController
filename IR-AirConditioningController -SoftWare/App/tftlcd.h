#ifndef _tftlcd_H
#define _tftlcd_H	
#include "stm32f10x.h"                  // Device header

//#include "system.h"	 

#define TFTLCD_ILI9488



#define TFTLCD_DIR	1	//0 ç«–å±


//TFTLCDæ§åˆ¶ç»“æ„å‚æ•°ï¼Œæ­¤ç»“æ„ä½“å ç”¨32ä½çš„è¿ç»­ç©ºé—´
typedef struct
{
	u16 LCD_CMD;
	u16 LCD_DATA;
}TFTLCD_TypeDef;

/*
bank1.sector4çš„åœ°å€å°±æ˜¯ä»0X6C000000å¼€å§‹ï¼Œè€Œ0x000007FEï¼Œåˆ™æ˜¯A10çš„åç§»é‡ï¼Œ
ï¼ˆæ­¤å¤„0X6C000000è½¬æ¢æˆäºŒè¿›åˆ¶ï¼š0110 [11]{00 0000 0000 0000 0000 0000 000}0  æ— æ„ä¹‰[ç‰‡é€‰]{25ä½åœ°å€ï¼Œä½†åœ¨LCDä¸Šæ— æ„ä¹‰}A0ä¸ºé›¶ï¼‰
åªè¦æ­¤åœ°å€è¢«ä½¿ç”¨ï¼Œåˆ™ä¼šè§¦å‘å¯¹åº”çš„åŠŸèƒ½
å¹¶ä¸”æ­¤åœ°å€ä¸Šçš„æ•°æ®ä¼šæŒ‰ç…§ç‰¹å®šæ—¶åºåœ¨å¹¶å£ä¸Šè¢«å‘é€
å°†è¿™ä¸ªåœ°å€å¼ºåˆ¶è½¬æ¢æˆLCD_TypeDefç»“æ„ä½“åœ°å€ï¼Œé‚£ä¹ˆå¯ä»¥å¾—åˆ° LCD->LCD_CMD çš„
åœ°å€å°±æ˜¯ 0X6C00,07FEï¼Œå¯¹åº” A10 çš„çŠ¶æ€ä¸º 0(å³ RS=0)ï¼Œè€Œ LCD-> LCD_DATA çš„åœ°
å€å°±æ˜¯ 0X6C00,0800ï¼ˆç»“æ„ä½“åœ°å€è‡ªå¢ï¼‰ï¼Œå¯¹åº” A10 çš„çŠ¶æ€ä¸º 1ï¼ˆå³ RS=1ï¼‰
NOE:è¯»æ“ä½œä½¿èƒ½
NWEï¼šå†™æ“ä½œä½¿èƒ½
NE4:ç‰‡é€‰
A10:å‘½ä»¤/æ•°æ®æ ‡å¿—ï¼ˆ0ï¼Œè¯»å†™å‘½ä»¤ï¼›1ï¼Œè¯»å†™æ•°æ®ï¼‰

*/		    
#define TFTLCD_BASE        ((u32)(0x6C000000 | 0x000007FE))//å®šä¹‰åœ°å€æ˜ å°„çš„åœ°å€å€¼
#define TFTLCD             ((TFTLCD_TypeDef *) TFTLCD_BASE)//å®šä¹‰ä¸€ä¸ªæŒ‡å‘ç‰¹å®šä½ç½®çš„ç»“æ„ä½“å˜é‡fmsc
  
//TFTLCDå±å¹•åŸºæœ¬å‚æ•°æ•°æ®
typedef struct  
{										    
	u16 width;			//LCD å®½
	u16 height;			//LCD é«˜
	u16 id;				//LCD ID
	u8  dir;            //LCD æ¨ªç«–å±æ§åˆ¶
}_tftlcd_data;


extern _tftlcd_data tftlcd_data;	//å®šä¹‰ä¸€ä¸ªç»“æ„ä½“å¤–éƒ¨å˜é‡
  
extern u16  FRONT_COLOR;//å‰é¢œè‰²    
extern u16  BACK_COLOR; //åé¢œè‰²


//é¢œè‰²è¡¨R5G6B5
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE         	 	 0x001F
#define BRED             0XF81F
#define GRED 			       0XFFE0
#define GBLUE			       0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			 0XBC40 //×ØÉ«
#define BRRED 			 0XFC07 //×ØºìÉ«
#define GRAY  			 0X8430 //»ÒÉ«
#define DARKBLUE      	 0X01CF	//ÉîÀ¶É«
#define LIGHTBLUE      	 0X7D7C	//Ç³À¶É«  
#define GRAYBLUE       	 0X5458 //»ÒÀ¶É«
#define LIGHTGREEN     	 0X841F //Ç³ÂÌÉ«
#define LIGHTGRAY        0XEF5B //Ç³»ÒÉ«(PANNEL)
#define LGRAY 			 0XC618 //Ç³»ÒÉ«(PANNEL),´°Ìå±³¾°É«
#define LGRAYBLUE        0XA651 //Ç³»ÒÀ¶É«(ÖĞ¼ä²ãÑÕÉ«)
#define LBBLUE           0X2B12 //Ç³×ØÀ¶É«(Ñ¡ÔñÌõÄ¿µÄ·´É«)


 

void LCD_WriteCmd(u16 cmd);
void LCD_WriteData(u16 data);
void LCD_WriteCmdData(u16 cmd,u16 data);
void LCD_WriteData_Color(u16 color);

void TFTLCD_Init(void); //³õÊ¼»¯
void LCD_Set_Window(u16 sx,u16 sy,u16 width,u16 height);//ÉèÖÃ´°¿Ú
void LCD_Display_Dir(u8 dir);//ÉèÖÃÆÁÄ»ÏÔÊ¾·½Ïò
void LCD_Clear(u16 Color);//ÇåÆÁ
void LCD_Fill(u16 xState,u16 yState,u16 xEnd,u16 yEnd,u16 color);//Ìî³äµ¥É«
void LCD_Color_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 *color);//ÔÚÖ¸¶¨ÇøÓòÄÚÌî³äÖ¸¶¨ÑÕÉ«¿é
void LCD_DrawPoint(u16 x,u16 y);//»­µã
void LCD_DrawFRONT_COLOR(u16 x,u16 y,u16 color);//Ö¸¶¨ÑÕÉ«»­µã
u16 LCD_ReadPoint(u16 x,u16 y);//¶Áµã
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2);//»­Ïß
void LCD_DrawLine_Color(u16 x1, u16 y1, u16 x2, u16 y2,u16 color);//Ö¸¶¨ÑÕÉ«»­Ïß
void LCD_DrowSign(uint16_t x, uint16_t y, uint16_t color);//»­Ê®×Ö±ê¼Ç
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2);//»­¾ØĞÎ
void LCD_Draw_Circle(u16 x0,u16 y0,u8 r);//»­Ô²
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode);//ÏÔÊ¾Ò»¸ö×Ö·û
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size);//ÏÔÊ¾Ò»¸öÊı×Ö
void LCD_ShowxNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode);//ÏÔÊ¾Êı×Ö
void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p);//ÏÔÊ¾×Ö·û´®
void LCD_ShowFontHZ(u16 x, u16 y, u8 *cn);//ÏÔÊ¾ºº×Ö					   						   																			
void LCD_ShowPicture(u16 x, u16 y, u16 wide, u16 high,u8 *pic);//ÏÔÊ¾Í¼Æ¬
void LCD_Show_signed_u16_Num(u16 x,u16 y,int num,u8 len,u8 size,u8 mode);//ÏÔÊ¾´ø·ûºÅµÄintÊı¾İ

#endif  

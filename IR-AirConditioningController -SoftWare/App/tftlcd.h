#ifndef _tftlcd_H
#define _tftlcd_H	
#include "stm32f10x.h"                  // Device header

//#include "system.h"	 

#define TFTLCD_ILI9488



#define TFTLCD_DIR	1	//0 竖屏


//TFTLCD控制结构参数，此结构体占用32位的连续空间
typedef struct
{
	u16 LCD_CMD;
	u16 LCD_DATA;
}TFTLCD_TypeDef;

/*
bank1.sector4的地址就是从0X6C000000开始，而0x000007FE，则是A10的偏移量，
（此处0X6C000000转换成二进制：0110 [11]{00 0000 0000 0000 0000 0000 000}0  无意义[片选]{25位地址，但在LCD上无意义}A0为零）
只要此地址被使用，则会触发对应的功能
并且此地址上的数据会按照特定时序在并口上被发送
将这个地址强制转换成LCD_TypeDef结构体地址，那么可以得到 LCD->LCD_CMD 的
地址就是 0X6C00,07FE，对应 A10 的状态为 0(即 RS=0)，而 LCD-> LCD_DATA 的地
址就是 0X6C00,0800（结构体地址自增），对应 A10 的状态为 1（即 RS=1）
NOE:读操作使能
NWE：写操作使能
NE4:片选
A10:命令/数据标志（0，读写命令；1，读写数据）

*/		    
#define TFTLCD_BASE        ((u32)(0x6C000000 | 0x000007FE))//定义地址映射的地址值
#define TFTLCD             ((TFTLCD_TypeDef *) TFTLCD_BASE)//定义一个指向特定位置的结构体变量fmsc
  
//TFTLCD屏幕基本参数数据
typedef struct  
{										    
	u16 width;			//LCD 宽
	u16 height;			//LCD 高
	u16 id;				//LCD ID
	u8  dir;            //LCD 横竖屏控制
}_tftlcd_data;


extern _tftlcd_data tftlcd_data;	//定义一个结构体外部变量
  
extern u16  FRONT_COLOR;//前颜色    
extern u16  BACK_COLOR; //后颜色


//颜色表R5G6B5
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
#define BROWN 			 0XBC40 //��ɫ
#define BRRED 			 0XFC07 //�غ�ɫ
#define GRAY  			 0X8430 //��ɫ
#define DARKBLUE      	 0X01CF	//����ɫ
#define LIGHTBLUE      	 0X7D7C	//ǳ��ɫ  
#define GRAYBLUE       	 0X5458 //����ɫ
#define LIGHTGREEN     	 0X841F //ǳ��ɫ
#define LIGHTGRAY        0XEF5B //ǳ��ɫ(PANNEL)
#define LGRAY 			 0XC618 //ǳ��ɫ(PANNEL),���屳��ɫ
#define LGRAYBLUE        0XA651 //ǳ����ɫ(�м����ɫ)
#define LBBLUE           0X2B12 //ǳ����ɫ(ѡ����Ŀ�ķ�ɫ)


 

void LCD_WriteCmd(u16 cmd);
void LCD_WriteData(u16 data);
void LCD_WriteCmdData(u16 cmd,u16 data);
void LCD_WriteData_Color(u16 color);

void TFTLCD_Init(void); //��ʼ��
void LCD_Set_Window(u16 sx,u16 sy,u16 width,u16 height);//���ô���
void LCD_Display_Dir(u8 dir);//������Ļ��ʾ����
void LCD_Clear(u16 Color);//����
void LCD_Fill(u16 xState,u16 yState,u16 xEnd,u16 yEnd,u16 color);//��䵥ɫ
void LCD_Color_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 *color);//��ָ�����������ָ����ɫ��
void LCD_DrawPoint(u16 x,u16 y);//����
void LCD_DrawFRONT_COLOR(u16 x,u16 y,u16 color);//ָ����ɫ����
u16 LCD_ReadPoint(u16 x,u16 y);//����
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2);//����
void LCD_DrawLine_Color(u16 x1, u16 y1, u16 x2, u16 y2,u16 color);//ָ����ɫ����
void LCD_DrowSign(uint16_t x, uint16_t y, uint16_t color);//��ʮ�ֱ��
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2);//������
void LCD_Draw_Circle(u16 x0,u16 y0,u8 r);//��Բ
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode);//��ʾһ���ַ�
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size);//��ʾһ������
void LCD_ShowxNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode);//��ʾ����
void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p);//��ʾ�ַ���
void LCD_ShowFontHZ(u16 x, u16 y, u8 *cn);//��ʾ����					   						   																			
void LCD_ShowPicture(u16 x, u16 y, u16 wide, u16 high,u8 *pic);//��ʾͼƬ
void LCD_Show_signed_u16_Num(u16 x,u16 y,int num,u8 len,u8 size,u8 mode);//��ʾ�����ŵ�int����

#endif  

#ifndef __HDC1080_H
#define __HDC1080_H
#include "iic.h"
#define HDCAddress       0x40
#define HDC_I2C          I2C1
#define HDC_ConfigReg    0x02
#define HDC_Temperaturne 0x00
#define HDC_Humidity     0x01

void HDC_Init(void);
u8 HDC_Write_Cmd(u8 regadd,u16 Write_Data);
u8 HDC_ReadData(u8 REG ,u8 *buf);
void Get_HDC1080_THValue(float *temp, float *humid);

#endif

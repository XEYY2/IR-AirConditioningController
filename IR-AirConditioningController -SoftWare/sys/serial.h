/*
 * FreeRTOS V202107.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */
#include <stdio.h>
#ifndef SERIAL_COMMS_H
#define SERIAL_COMMS_H

typedef void * xComPortHandle;

typedef enum
{
    serCOM1,
    serCOM2,
    serCOM3,
    serCOM4,
    serCOM5,
    serCOM6,
    serCOM7,
    serCOM8
} eCOMPort;

typedef enum
{
    serNO_PARITY,
    serODD_PARITY,
    serEVEN_PARITY,
    serMARK_PARITY,
    serSPACE_PARITY
} eParity;

typedef enum
{
    serSTOP_1,
    serSTOP_2
} eStopBits;

typedef enum
{
    serBITS_5,
    serBITS_6,
    serBITS_7,
    serBITS_8
} eDataBits;

typedef enum
{
    ser50,
    ser75,
    ser110,
    ser134,
    ser150,
    ser200,
    ser300,
    ser600,
    ser1200,
    ser1800,
    ser2400,
    ser4800,
    ser9600,
    ser19200,
    ser38400,
    ser57600,
    ser115200
} eBaud;

void SerialPortInit( void );
int fputc(int ch, FILE *f);
uint8_t* GetBuffers(void);

#endif /* ifndef SERIAL_COMMS_H */

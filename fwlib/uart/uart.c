/**
  *****************************************************************************
  * @file    : uart.c
  * @author  : Tuu
  * @version : 1.0.0
  * @date    : 2021-03-13
  * @brief   : stc8 uart api
  ******************************************************************************
  * @lasteditors  : Tuu
  * @lasteditTime : 2021-03-13
  ******************************************************************************
  * @atten   : Copyright (C) by Tuu Inc
  *
  *****************************************************************************
  */

/* Includes -------------------------------------------------------------------*/
#include "uart.h"

#include "STC8HX.h"

/* Defines --------------------------------------------------------------------*/


/* Variables ------------------------------------------------------------------*/
sfr TH2  = 0xD6;
sfr TL2  = 0xD7;

static bit B_TX1_Busy; //发送忙标志

/* Functions ------------------------------------------------------------------*/

static void SetTimer2Baudraye(u16 dat)  // 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer1做波特率.
{
    AUXR &= ~(1<<4);    //Timer stop
    AUXR &= ~(1<<3);    //Timer2 set As Timer
    AUXR |=  (1<<2);    //Timer2 set as 1T mode
    TH2 = dat / 256;
    TL2 = dat % 256;
    IE2  &= ~(1<<2);    //禁止中断
    AUXR |=  (1<<4);    //Timer run enable
}

static void uart1_config(void)
{
    SCON = (SCON & 0x3f) | 0x40;    //UART1模式, 0x00: 同步移位输出, 0x40: 8位数据,可变波特率, 0x80: 9位数据,固定波特率, 0xc0: 9位数据,可变波特率
    // PS  = 1;    //高优先级中断
    ES  = 1;    //允许中断
    REN = 0;    //允许接收
    P_SW1 &= 0x3f;
    P_SW1 |= 0x00;      //UART1 switch to, 0x00: P3.0 P3.1, 0x40: P3.6 P3.7, 0x80: P1.6 P1.7, 0xC0: P4.3 P4.4
    // PCON2 |=  (1<<4);   //内部短路RXD与TXD, 做中继, ENABLE,DISABLE

    B_TX1_Busy = 0;
}

static void uart2_config(void)
{

}

void uart_hw_init(UART_ID id, uint8_t band)
{
    AUXR |= 0x01;       //S1 BRT Use Timer2;
    SetTimer2Baudraye(65536UL - (MAIN_Fosc / 4) / band);

    switch (id)
    {
        case UART_1:
            /* code */
            uart1_config();
            break;

        case UART_2:
            /* code */
            uart2_config();
            break;

        default:
            break;
    }

    EA = 1; //允许总中断

    print_str("log uart init succeed\r\n");
}

void print_str(uint8_t *puts) //发送一个字符串
{
    for (; *puts != 0;  puts++)     //遇到停止符0结束
    {
        SBUF = *puts;
        B_TX1_Busy = 1;
        while(B_TX1_Busy);
    }
}

void UART1_int (void) interrupt 4
{
    if(RI)
    {
        RI = 0;
    }

    if(TI)
    {
        TI = 0;
        B_TX1_Busy = 0;
    }
}

/************************ (C) COPYRIGHT Tuu ********END OF FILE****************/


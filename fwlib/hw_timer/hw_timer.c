/**
  *****************************************************************************
  * @file    : hw_timer.c
  * @author  : Tuu
  * @version : 1.0.0
  * @date    : 2021-03-13
  * @brief   : stc8 hw_timer api
  ******************************************************************************
  * @lasteditors  : Tuu
  * @lasteditTime : 2021-03-13
  ******************************************************************************
  * @atten   : Copyright (C) by Tuu Inc
  *
  *****************************************************************************
  */

/* Includes -------------------------------------------------------------------*/
#include "hw_timer.h"

#include "STC8HX.h"

/* Defines --------------------------------------------------------------------*/


/* Variables ------------------------------------------------------------------*/
sfr INT_CLKO = 0x8F;

static TIMER_HANDER timer0_hander = NULL;
static TIMER_HANDER timer1_hander = NULL;
static TIMER_HANDER timer3_hander = NULL;

/* Functions ------------------------------------------------------------------*/
static void Timer0_stop(void)
{
    TR0 = 0;    //停止计数
    // ET0 = 0;
}

static void Timer0_init(uint32_t tick_us)
{
    uint32_t tick_clk = 0;

    TR0 = 0;    //停止计数

    ET0 = 1;    //允许中断
//  PT0 = 1;    //高优先级中断
    TMOD &= ~0x03;
    TMOD |= 0;  //工作模式, 0: 16位自动重装, 1: 16位定时/计数, 2: 8位自动重装, 3: 16位自动重装, 不可屏蔽中断
//  TMOD |=  0x04;  //对外计数或分频
    TMOD &= ~0x04;  //定时
//  INT_CLKO |=  0x01;  //输出时钟
    INT_CLKO &= ~0x01;  //不输出时钟

    tick_clk = MAIN_Fosc/1000000*tick_us;
    if (tick_clk < 0xffff)
    {
        AUXR |=  0x80;  //1T mode
        TH0 = (u8)((65536UL - tick_clk) / 256);
        TL0 = (u8)((65536UL - tick_clk) % 256);
    }
    else
    {
        AUXR &= ~0x80;  //12T mode
        TH0 = (u8)((65536UL - tick_clk/12) / 256);
        TL0 = (u8)((65536UL - tick_clk/12) % 256);
    }

    TR0 = 1;    //开始运行
}

static void Timer1_stop(void)
{
    TR1 = 0;    //停止计数
}

static void Timer1_init(uint32_t tick_us)
{
    uint32_t tick_clk;

    TR1 = 0;    //停止计数

    ET1 = 1;    //允许中断
//  PT1 = 1;    //高优先级中断
    TMOD &= ~0x30;
    TMOD |= (0 << 4);   //工作模式, 0: 16位自动重装, 1: 16位定时/计数, 2: 8位自动重装
//  TMOD |=  0x40;  //对外计数或分频
    TMOD &= ~0x40;  //定时
//  INT_CLKO |=  0x02;  //输出时钟
    INT_CLKO &= ~0x02;  //不输出时钟

    tick_clk = MAIN_Fosc/1000000*tick_us;
    if (tick_clk < 0xffff)
    {
        AUXR |=  0x40;  //1T mode
        TH1 = (u8)((65536UL - tick_clk) / 256);
        TL1 = (u8)((65536UL - tick_clk) % 256);
    }
    else
    {
        AUXR &= ~0x40;  //12T mode
        TH1 = (u8)((65536UL - tick_clk/12) / 256);
        TL1 = (u8)((65536UL - tick_clk/12) % 256);
    }

    TR1 = 1;    //开始运行
}

static void Timer3_init(uint32_t tick_us)
{
    uint32_t tick_clk;

    T4T3M &= ~0x0f;    //停止计数

    IE2  |=  0x20;    //允许中断

    tick_clk = MAIN_Fosc/1000000*tick_us;
    if (tick_clk < 0xffff)
    {
        T4T3M |=  (1<<1);    //1T mode
        T3H = (u8)((65536UL - tick_clk) / 256);
        T3L = (u8)((65536UL - tick_clk) % 256);
    }
    else
    {
        T3H = (u8)((65536UL - tick_clk/12) / 256);
        T3L = (u8)((65536UL - tick_clk/12) % 256);
    }

    T4T3M |=  (1<<3);    //开始运行
}

static void Timer3_stop(void)
{
    T4T3M &= ~0x0f;    //停止计数
}

void hw_timer_init(TIMER_ID id, uint32_t tick_us, TIMER_HANDER hander)
{
    switch (id)
    {
        case TIMER_0:
            /* code */
            Timer0_init(tick_us);
            timer0_hander = hander;
            break;

        case TIMER_1:
            /* code */
            Timer1_init(tick_us);
            timer1_hander = hander;
            break;

        case TIMER_3:
            /* code */
            Timer3_init(tick_us);
            timer3_hander = hander;
            break;

        default:
            break;
    }
}

void hw_timer_stop(TIMER_ID id)
{
    switch (id)
    {
        case TIMER_0:
            /* code */
            Timer0_stop();
            timer0_hander = NULL;
            break;

        case TIMER_1:
            /* code */
            Timer1_stop();
            timer1_hander = NULL;
            break;

        case TIMER_3:
            /* code */
            Timer3_stop();
            timer3_hander = NULL;
            break;

        default:
            break;
    }
}

void timer0_int (void) interrupt 1
{
    if (NULL != timer0_hander)
    {
        timer0_hander(TIMER_0);
    }
}

void timer1_int (void) interrupt 3
{
    if (NULL != timer1_hander)
    {
        timer1_hander(TIMER_1);
    }
}

void timer3_int(void) interrupt 19
{
    if (NULL != timer3_hander)
    {
        timer3_hander(TIMER_3);
    }
}

/************************ (C) COPYRIGHT Tuu ********END OF FILE****************/


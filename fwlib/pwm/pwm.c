/**
  *****************************************************************************
  * @file    : pwm.c
  * @author  : Tuu
  * @version : 1.0.0
  * @date    : 2021-03-13
  * @brief   : stc8 pwm api
  ******************************************************************************
  * @lasteditors  : Tuu
  * @lasteditTime : 2021-03-13
  ******************************************************************************
  * @atten   : Copyright (C) by Tuu Inc
  *
  *****************************************************************************
  */

/* Includes -------------------------------------------------------------------*/
#include "pwm.h"

#include "STC8HX.h"

/* Defines --------------------------------------------------------------------*/
#define PWM1_0      0x00	//P:P1.0  N:P1.1
#define PWM2_0      0x00	//P:P1.2  N:P1.3
#define PWM3_0      0x00	//P:P1.4  N:P1.5
#define PWM4_0      0x00	//P:P1.6  N:P1.7
#define PWM5_0      0x00	//P2.0
#define PWM6_0      0x00	//P2.1
#define PWM7_0      0x00	//P2.2
#define PWM8_0      0x00	//P2.3

#define ENO1P       0x01
#define ENO1N       0x02
#define ENO2P       0x04
#define ENO2N       0x08
#define ENO3P       0x10
#define ENO3N       0x20
#define ENO4P       0x40
#define ENO4N       0x80
#define ENO5P       0x01
#define ENO6P       0x04
#define ENO7P       0x10
#define ENO8P       0x40

/* Variables ------------------------------------------------------------------*/


/* Functions ------------------------------------------------------------------*/
static void pwma_config(PWM_ID id, uint16_t period)
{
    P_SW2 |= 0x80;

    // PWM1_BKR = 0x00; //使能主输出

    PWM1_CCER1 = 0x00; //写 CCMRx 前必须先清零 CCxE 关闭通道
    PWM1_CCER2 = 0x00;
    PWM1_CCMR1 = 0x60; //通道模式配置
    PWM1_CCMR2 = 0x60;
    PWM1_CCMR3 = 0x60;
    PWM1_CCMR4 = 0x60;
    PWM1_CCER1 = 0x55; //配置通道输出使能和极性
    PWM1_CCER2 = 0x55;


    PWM1_ARRH = (u8)(period >> 8); //设置周期时间
    PWM1_ARRL = (u8)(period);

    // PWM1_ENO = 0x00;
    // PWM1_PS = 0x00;  //高级 PWM 通道输出脚选择位

    switch (id)
    {
        case PWM_1:
            /* code */
            PWM1_ENO |= ENO1P; //使能输出
            PWM1_ENO &= ~ENO1N; //使能输出
            PWM1_PS |= PWM1_0; //选择 PWM1_2 通道
            break;
        case PWM_2:
            /* code */
            PWM1_ENO |= ENO2P; //使能输出
            PWM1_ENO &= ~ENO2N; //使能输出
            PWM1_PS |= PWM2_0; //选择 PWM2_2 通道
            break;
        case PWM_3:
            /* code */
            PWM1_ENO |= ENO3P; //使能输出
            PWM1_ENO &= ~ENO3N; //使能输出
            PWM1_PS |= PWM3_0; //选择 PWM3_2 通道
            break;
        case PWM_4:
            /* code */
            PWM1_ENO |= ENO4P; //使能输出
            PWM1_ENO &= ~ENO4N; //使能输出
            PWM1_PS |= PWM4_0; //选择 PWM4_2 通道
            break;

        default:
            break;
    }

    PWM1_BKR = 0x80; //使能主输出
    PWM1_CR1 |= 0x01; //开始计时
    P_SW2 &= 0x7f;
}

static void pwma_duty(PWM_ID id, uint16_t duty)
{
    P_SW2 |= 0x80;

    switch (id)
    {
        case PWM_1:
            /* code */
            PWM1_CCR1H = (u8)(duty >> 8); //设置占空比时间
            PWM1_CCR1L = (u8)(duty);
            break;
        case PWM_2:
            /* code */
            PWM1_CCR2H = (u8)(duty >> 8); //设置占空比时间
            PWM1_CCR2L = (u8)(duty);
            break;
        case PWM_3:
            /* code */
            PWM1_CCR3H = (u8)(duty >> 8); //设置占空比时间
            PWM1_CCR3L = (u8)(duty);
            break;
        case PWM_4:
            /* code */
            PWM1_CCR4H = (u8)(duty >> 8); //设置占空比时间
            PWM1_CCR4L = (u8)(duty);
            break;

        default:
            break;
    }

    P_SW2 &= 0x7f;
}

static void pwmb_duty(PWM_ID id, uint16_t duty)
{
    P_SW2 |= 0x80;

    switch (id)
    {
        case PWM_5:
            /* code */
            PWM2_CCR1H = (u8)(duty >> 8); //设置占空比时间
            PWM2_CCR1L = (u8)(duty);
            break;
        case PWM_6:
            /* code */
            PWM2_CCR2H = (u8)(duty >> 8); //设置占空比时间
            PWM2_CCR2L = (u8)(duty);
            break;
        case PWM_7:
            /* code */
            PWM2_CCR3H = (u8)(duty >> 8); //设置占空比时间
            PWM2_CCR3L = (u8)(duty);
            break;
        case PWM_8:
            /* code */
            PWM2_CCR4H = (u8)(duty >> 8); //设置占空比时间
            PWM2_CCR4L = (u8)(duty);
            break;

        default:
            break;
    }

    P_SW2 &= 0x7f;
}

static void pwmb_config(PWM_ID id, uint16_t period)
{
    P_SW2 |= 0x80;

    // PWM2_BKR = 0x00; //使能主输出

    PWM2_CCER1 = 0x00; //写 CCMRx 前必须先清零 CCxE 关闭通道
    PWM2_CCER2 = 0x00;
    PWM2_CCMR1 = 0x60; //通道模式配置
    PWM2_CCMR2 = 0x60;
    PWM2_CCMR3 = 0x60;
    PWM2_CCMR4 = 0x60;
    PWM2_CCER1 = 0x33; //配置通道输出使能和极性
    PWM2_CCER2 = 0x33;

    PWM2_ARRH = period >> 8; //设置周期时间
    PWM2_ARRL = period & 0xff;

    // PWM2_ENO = 0x00;
    // PWM2_PS = 0x00;  //高级 PWM 通道输出脚选择位

    switch (id)
    {
        case PWM_5:
            /* code */
            PWM2_ENO |= ENO5P; //使能输出
            PWM2_PS |= PWM5_0; //选择 PWM5_0 通道
            break;
        case PWM_6:
            /* code */
            PWM2_ENO |= ENO6P; //使能输出
            PWM2_PS |= PWM6_0; //选择 PWM6_0 通道
            break;
        case PWM_7:
            /* code */
            PWM2_ENO |= ENO7P; //使能输出
            PWM2_PS |= PWM7_0; //选择 PWM7_0 通道
            break;
        case PWM_8:
            /* code */
            PWM2_ENO |= ENO8P; //使能输出
            PWM2_PS |= PWM8_0; //选择 PWM8_0 通道
            break;

        default:
            break;
    }

    PWM2_BKR = 0x80; //使能主输出
    PWM2_CR1 |= 0x01; //开始计时
    P_SW2 &= 0x7f;
}


void pwm_hw_init(PWM_ID id, uint16_t period)
{
    if (id <= PWM_4)
    {
        pwma_config(id, period);
    }
    else
    {
        pwmb_config(id, period);
    }
}

void pwm_change_duty(PWM_ID id, uint16_t duty)
{
    if (id <= PWM_4)
    {
        pwma_duty(id, duty);
    }
    else
    {
        pwmb_duty(id, duty);
    }
}

/************************ (C) COPYRIGHT Tuu ********END OF FILE****************/


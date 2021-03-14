/**
  *****************************************************************************
  * @file    : rcc.c
  * @author  : Tuu
  * @version : 1.0.0
  * @date    : 2021-03-13
  * @brief   : stc8 rcc api
  ******************************************************************************
  * @lasteditors  : Tuu
  * @lasteditTime : 2021-03-13
  ******************************************************************************
  * @atten   : Copyright (C) by Tuu Inc
  *
  *****************************************************************************
  */

/* Includes -------------------------------------------------------------------*/
#include "rcc.h"

#include "STC8HX.h"

/* Defines --------------------------------------------------------------------*/


/* Variables ------------------------------------------------------------------*/

/* Functions ------------------------------------------------------------------*/

void rcc_hw_init(RCC_DIV div)
{
    P_SW2 = 0x80;

    HIRCCR = 0x80;              //启动内部 IRC
    while (!(HIRCCR & 0x01));   //等待时钟稳定

    switch (div)
    {
        case RCC_DEV_24M:
            /* code */
            CLKDIV = 0;
            CKSEL = 0x00;               //选择内部 IRC ( 默认 )
            break;

        case RCC_DEV_8M:
            /* code */
            CLKDIV = 3;
            CKSEL = 0x00;               //选择内部 IRC ( 默认 )
            break;

        case RCC_DEV_2M:
            /* code */
            CLKDIV = 12;
            CKSEL = 0x00;               //选择内部 IRC ( 默认 )
            break;

        case RCC_DEV_32_7K:
            /* code */
            CLKDIV = 0;
            CKSEL = 0x03;               //选择内部 IRC ( 默认 )
            break;

        default:
            break;
    }

    P_SW2 = 0x00;
}


/************************ (C) COPYRIGHT Tuu ********END OF FILE****************/


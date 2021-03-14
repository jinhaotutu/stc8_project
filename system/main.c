/**
  *****************************************************************************
  * @file    : main.c
  * @author  : Tuu
  * @version : 1.0.0
  * @date    : 2021-03-13
  * @brief   : stc8 main
  ******************************************************************************
  * @lasteditors  : Tuu
  * @lasteditTime : 2021-03-13
  ******************************************************************************
  * @atten   : Copyright (C) by Tuu Inc
  *
  *****************************************************************************
  */

/* Includes -------------------------------------------------------------------*/
#include "main.h"

#include "rcc.h"
#include "uart.h"

/* Defines --------------------------------------------------------------------*/


/* Variables ------------------------------------------------------------------*/


/* Functions ------------------------------------------------------------------*/
extern int user_main(void *p);

/**
  * @note   Main program.
  * @brief  None
  * @param  None
  * @retval None
  */
int main(void)
{
    /* stc8 系统配置 */

    // 时钟
#ifndef LOW_POWER
    rcc_hw_init(RCC_DEV_24M);
#else
    rcc_hw_init(RCC_DEV_32_7K);
#endif

    // 串口打印
    uart_hw_init(UART_1, 115200);

    // others
    user_main(NULL);

    return 0;
}

/************************ (C) COPYRIGHT Tuu ********END OF FILE****************/


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
#include "stdio.h"

#include "rcc.h"
#include "uart.h"
#include "STC8HX.h"

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
    // P0M1 = 0;   P0M0 = 0;   //设置为准双向口
    // P1M1 = 0;   P1M0 = 0;   //设置为准双向口
    // P2M1 = 0;   P2M0 = 0;   //设置为准双向口
    // P3M1 = 0;   P3M0 = 0;   //设置为准双向口
    // P4M1 = 0;   P4M0 = 0;   //设置为准双向口
    // P5M1 = 0;   P5M0 = 0;   //设置为准双向口
    // P6M1 = 0;   P6M0 = 0;   //设置为准双向口
    // P7M1 = 0;   P7M0 = 0;   //设置为准双向口

    // 时钟
#ifndef LOW_POWER
    // rcc_hw_init(RCC_DEV_24M);
#else
    rcc_hw_init(RCC_DEV_32_7K);
#endif

    // 串口打印
    uart_hw_init(UART_1, 9600);

    print_str("main init succeed\r\n");
    // printf("test printf out\n");

    // others
    user_main(NULL);

    return 0;
}

/************************ (C) COPYRIGHT Tuu ********END OF FILE****************/


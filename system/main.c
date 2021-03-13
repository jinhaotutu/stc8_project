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

    // 串口打印

    // others
    user_main(0);
}

/************************ (C) COPYRIGHT Tuu ********END OF FILE****************/

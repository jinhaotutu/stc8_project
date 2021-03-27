/**
  *****************************************************************************
  * @file    : user_main.c
  * @author  : Tuu
  * @version : 1.0.0
  * @date    : 2021-03-13
  * @brief   : user app code start
  ******************************************************************************
  * @lasteditors  : Tuu
  * @lasteditTime : 2021-03-13
  ******************************************************************************
  * @atten   : Copyright (C) by Tuu Inc
  *
  * this is user app demo start
  *****************************************************************************
  */

/* Includes -------------------------------------------------------------------*/
#include "type.h"
#include "soft_timer.h"
#include "button.h"

#include "control.h"
#include "uart.h"
#include "stdio.h"
#include "gpio.h"
#include "ev1527.h"
#include "hw_timer.h"

/* Defines --------------------------------------------------------------------*/

/* Variables ------------------------------------------------------------------*/
static struct tk_timer timer_log;

/* Functions ------------------------------------------------------------------*/
static void timer_log_handle(struct tk_timer *timer)
{
    print_str("system is runing\r\n");
}

static int user_init(void)
{
    int ret = 0;

    soft_timer_init();
    soft_timer_register(&timer_log, timer_log_handle);
    soft_timer_start(&timer_log, TIMER_MODE_LOOP, 1000);

    control_user_init();

    print_str("user init finish\r\n");

    return 0;
}


/**
  * @note   user_main
  * @brief  None
  * @param  None
  * @retval None
  */
int user_main(void *p)
{
    print_str("user_main\r\n");

    user_init();

    while(1){
        // soft timer
        soft_timer_loop();

        // user loop
        control_loop();
    }

    //return 0;
}



/************************ (C) COPYRIGHT Tuu ********END OF FILE****************/


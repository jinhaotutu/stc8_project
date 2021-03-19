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

/* Defines --------------------------------------------------------------------*/

/* Variables ------------------------------------------------------------------*/

/* Functions ------------------------------------------------------------------*/
static int user_init(void)
{
    int ret = 0;

    soft_timer_init();

    button_init();

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
    user_init();

    while(1){
        // soft timer
        soft_timer_loop();
    }

    //return 0;
}



/************************ (C) COPYRIGHT Tuu ********END OF FILE****************/


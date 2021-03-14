/**
  *****************************************************************************
  * @file    : hw_timer.h
  * @author  : Tuu
  * @version : 1.0.0
  * @date    : 2021-03-13
  * @brief   : Header for hw_timer.c module
  ******************************************************************************
  * @lasteditors  : Tuu
  * @lasteditTime : 2021-03-13
  ******************************************************************************
  * @atten   : Copyright (C) by Tuu Inc
  *
  *****************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HW_TIMER_H
#define __HW_TIMER_H

/* Includes ------------------------------------------------------------------*/
#include "type.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Defines --------------------------------------------------------------------*/
typedef unsigned char TIMER_ID;
#define TIMER_0      0
#define TIMER_1      1

typedef void (*TIMER_HANDER)(void *p);

/* Variables ------------------------------------------------------------------*/


/* Functions ------------------------------------------------------------------*/
void hw_timer_init(TIMER_ID id, uint16_t tick_ms, TIMER_HANDER hander);


#ifdef __cplusplus
}
#endif

#endif /* __HW_TIMER_H */

/************************ (C) COPYRIGHT Tuu ********END OF FILE****************/


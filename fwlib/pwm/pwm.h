/**
  *****************************************************************************
  * @file    : pwm.h
  * @author  : Tuu
  * @version : 1.0.0
  * @date    : 2021-03-13
  * @brief   : Header for pwm.c module
  ******************************************************************************
  * @lasteditors  : Tuu
  * @lasteditTime : 2021-03-13
  ******************************************************************************
  * @atten   : Copyright (C) by Tuu Inc
  *
  *****************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PWM_H
#define __PWM_H

/* Includes ------------------------------------------------------------------*/
#include "type.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Defines --------------------------------------------------------------------*/
typedef unsigned char PWM_ID;
#define PWM_1      0
#define PWM_2      1
#define PWM_3      2
#define PWM_4      3
#define PWM_5      4
#define PWM_6      5
#define PWM_7      6
#define PWM_8      7

/* Variables ------------------------------------------------------------------*/


/* Functions ------------------------------------------------------------------*/
void pwm_hw_init(PWM_ID id, uint16_t period);
void pwm_change_duty(PWM_ID id, uint16_t duty);

#ifdef __cplusplus
}
#endif

#endif /* __PWM_H */

/************************ (C) COPYRIGHT Tuu ********END OF FILE****************/


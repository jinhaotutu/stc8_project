/**
  *****************************************************************************
  * @file    : gpio.h
  * @author  : Tuu
  * @version : 1.0.0
  * @date    : 2021-03-13
  * @brief   : Header for gpio.c module
  ******************************************************************************
  * @lasteditors  : Tuu
  * @lasteditTime : 2021-03-13
  ******************************************************************************
  * @atten   : Copyright (C) by Tuu Inc
  *
  *****************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GPIO_H
#define __GPIO_H

/* Includes ------------------------------------------------------------------*/
#include "type.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Defines --------------------------------------------------------------------*/
typedef unsigned char GPIO_GROUP;
#define GROUP_0      0
#define GROUP_1      1
#define GROUP_2      2
#define GROUP_3      3

typedef unsigned char GPIO_PIN;
#define PIN_0      0
#define PIN_1      1
#define PIN_2      2
#define PIN_3      3
#define PIN_4      4
#define PIN_5      5
#define PIN_6      6
#define PIN_7      7

typedef unsigned char GPIO_DIC;
#define PIN_OUTPUT      0
#define PIN_INPUT       1

/* Variables ------------------------------------------------------------------*/


/* Functions ------------------------------------------------------------------*/
void gpio_hw_init(GPIO_GROUP group, GPIO_PIN pin, GPIO_DIC dic);
u8 gpio_read(GPIO_GROUP group, GPIO_PIN pin);
void gpio_write(GPIO_GROUP group, GPIO_PIN pin, u8 value);

#ifdef __cplusplus
}
#endif

#endif /* __GPIO_H */

/************************ (C) COPYRIGHT Tuu ********END OF FILE****************/


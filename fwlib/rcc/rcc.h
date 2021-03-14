/**
  *****************************************************************************
  * @file    : rcc.h
  * @author  : Tuu
  * @version : 1.0.0
  * @date    : 2021-03-13
  * @brief   : Header for rcc.c module
  ******************************************************************************
  * @lasteditors  : Tuu
  * @lasteditTime : 2021-03-13
  ******************************************************************************
  * @atten   : Copyright (C) by Tuu Inc
  *
  *****************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RCC_H
#define __RCC_H

/* Includes ------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/* Defines --------------------------------------------------------------------*/
typedef unsigned char RCC_DIV;
#define RCC_DEV_24M     0
#define RCC_DEV_8M      1
#define RCC_DEV_2M      2
#define RCC_DEV_32_7K   3


/* Variables ------------------------------------------------------------------*/


/* Functions ------------------------------------------------------------------*/
void rcc_hw_init(RCC_DIV div);

#ifdef __cplusplus
}
#endif

#endif /* __RCC_H */

/************************ (C) COPYRIGHT Tuu ********END OF FILE****************/


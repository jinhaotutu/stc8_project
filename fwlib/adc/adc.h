/**
  *****************************************************************************
  * @file    : adc.h
  * @author  : Tuu
  * @version : 1.0.0
  * @date    : 2021-03-13
  * @brief   : Header for adc.c module
  ******************************************************************************
  * @lasteditors  : Tuu
  * @lasteditTime : 2021-03-13
  ******************************************************************************
  * @atten   : Copyright (C) by Tuu Inc
  *
  *****************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ADC_H
#define __ADC_H

/* Includes ------------------------------------------------------------------*/
#include "type.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Defines --------------------------------------------------------------------*/
typedef unsigned char ADC_ID;
#define ADC_0      0
#define ADC_1      1
#define ADC_2      2
#define ADC_3      3
#define ADC_4      4

/* Variables ------------------------------------------------------------------*/


/* Functions ------------------------------------------------------------------*/
void adc_hw_init(ADC_ID id);
uint16_t get_adc_result(ADC_ID id);

#ifdef __cplusplus
}
#endif

#endif /* __ADC_H */

/************************ (C) COPYRIGHT Tuu ********END OF FILE****************/


/**
  *****************************************************************************
  * @file    : uart.h
  * @author  : Tuu
  * @version : 1.0.0
  * @date    : 2021-03-13
  * @brief   : Header for uart.c module
  ******************************************************************************
  * @lasteditors  : Tuu
  * @lasteditTime : 2021-03-13
  ******************************************************************************
  * @atten   : Copyright (C) by Tuu Inc
  *
  *****************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __UART_H
#define __UART_H

/* Includes ------------------------------------------------------------------*/
#include "type.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Defines --------------------------------------------------------------------*/
typedef unsigned char UART_ID;
#define UART_1      0
#define UART_2      1


/* Variables ------------------------------------------------------------------*/


/* Functions ------------------------------------------------------------------*/
void uart_hw_init(UART_ID num, uint32_t band);
void print_str(uint8_t *puts);
void print_hex(uint8_t *puts, uint8_t len);

#ifdef __cplusplus
}
#endif

#endif /* __UART_H */

/************************ (C) COPYRIGHT Tuu ********END OF FILE****************/


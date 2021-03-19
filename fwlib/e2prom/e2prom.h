/**
  *****************************************************************************
  * @file    : e2prom.h
  * @author  : Tuu
  * @version : 1.0.0
  * @date    : 2021-03-13
  * @brief   : Header for e2prom.c module
  ******************************************************************************
  * @lasteditors  : Tuu
  * @lasteditTime : 2021-03-13
  ******************************************************************************
  * @atten   : Copyright (C) by Tuu Inc
  *
  *****************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __E2PROM_H
#define __E2PROM_H

/* Includes ------------------------------------------------------------------*/
#include "type.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Defines --------------------------------------------------------------------*/
#define     E2PROM_BLOCK    512

/* Variables ------------------------------------------------------------------*/


/* Functions ------------------------------------------------------------------*/
void e2prom_hw_init(void);
void e2prom_read(u16 addr, u8 *buf, u16 len);
u8 e2prom_write(u16 addr, u8 *buf, u16 len);


#ifdef __cplusplus
}
#endif

#endif /* __E2PROM_H */

/************************ (C) COPYRIGHT Tuu ********END OF FILE****************/


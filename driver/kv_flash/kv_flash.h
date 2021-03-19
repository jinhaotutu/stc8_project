/**
  *****************************************************************************
  * @file    : kv_flash.h
  * @author  : Tuu
  * @version : 1.0.0
  * @date    : 2021-03-13
  * @brief   : Header for kv_flash.c module
  ******************************************************************************
  * @lasteditors  : Tuu
  * @lasteditTime : 2021-03-13
  ******************************************************************************
  * @atten   : Copyright (C) by Tuu Inc
  *
  *****************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __KV_FLASH_H
#define __KV_FLASH_H

/* Includes ------------------------------------------------------------------*/
#include "type.h"
#include "e2prom.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Defines --------------------------------------------------------------------*/
#define TINY_START_ADDR  0x0000        //tinyFlash起始地址
#define TINY_SECTOR_SIZE E2PROM_BLOCK   //flash扇区大小
#define TINY_BUFFER_SIZE 128            //tiny缓冲区大小

#define TINY_SECHAD_SIZE 32             //记录扇区使用情况的扇区头大小

/* Variables ------------------------------------------------------------------*/


/* Functions ------------------------------------------------------------------*/


#ifdef __cplusplus
}
#endif

#endif /* __KV_FLASH_H */

/************************ (C) COPYRIGHT Tuu ********END OF FILE****************/


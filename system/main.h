/**
  *****************************************************************************
  * @file    : main.h
  * @author  : Tuu
  * @version : 1.0.0
  * @date    : 2021-03-13
  * @brief   : Header for main.c module
  ******************************************************************************
  * @lasteditors  : Tuu
  * @lasteditTime : 2021-03-13
  ******************************************************************************
  * @atten   : Copyright (C) by Tuu Inc
  *
  *****************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stdio.h"
#include "string.h"
// #include "stdarg.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
  *****************************************************************************
  * @version brief  :
  *****************************************************************************
  */


/* Defines --------------------------------------------------------------------*/
#if 0
#ifndef __FILENAME__
#ifdef __GNUC__
#define __FILENAME__ (strrchr(__FILE__, '/') ? (strrchr(__FILE__, '/') + 1):__FILE__)
#else
#define __FILENAME__ (strrchr(__FILE__, '\\') ? (strrchr(__FILE__, '\\') + 1):__FILE__)
#endif
#endif

#define tt_printf(_fmt_, ...) printf("[log ] %s:%d | "_fmt_"\r\n", __FILENAME__, __LINE__, __VA_ARGS__)
#endif


/* Variables ------------------------------------------------------------------*/


/* Functions ------------------------------------------------------------------*/


#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT Tuu ********END OF FILE****************/


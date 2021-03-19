/**
  *****************************************************************************
  * @file    : ev1527.h
  * @author  : Tuu
  * @version : 1.0.0
  * @date    : 2021-03-13
  * @brief   : Header for ev1527.c module
  ******************************************************************************
  * @lasteditors  : Tuu
  * @lasteditTime : 2021-03-13
  ******************************************************************************
  * @atten   : Copyright (C) by Tuu Inc
  *
  *****************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __EV1527_H
#define __EV1527_H

/* Includes ------------------------------------------------------------------*/
#include "type.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Defines --------------------------------------------------------------------*/
#define EV1527_ENCODE   1
#define EV1527_DECODE   1

/* Variables ------------------------------------------------------------------*/
typedef struct ev_data
{
    /* data */
    uint8_t id[20];
    uint8_t key[4];
    uint8_t bit_cnt;
}ev_data;

typedef u8 (*io_read)(void);
typedef u8 (*io_write)(u8 level);
typedef u8 (*timer_restart)(uint32_t time_us);
typedef u8 (*timer_stop)(void);
typedef u8 (*ev_decode_handle)(ev_data *ev_buf);

typedef enum{
    EV_LEVEL_0,
    EV_LEVEL_1,
    EV_SYNC,
}EV_STATUS;

typedef struct ev_decode
{
    /* data */
    io_read level_read;
    ev_decode_handle handle;

    uint8_t last_level;
    uint16_t high_cnt;
    uint16_t low_cnt;
    uint8_t sync;
    uint8_t check_cnt;
    ev_data buf;
}ev_decode;

typedef struct ev_encode
{
    /* data */
    io_write level_write;
    ev_data buf;
    uint8_t loop_cnt;
    timer_restart restart;
    timer_stop stop;
}ev_encode;

/* Functions ------------------------------------------------------------------*/
int ev1527_decode_init(io_read read, ev_decode_handle handle);
int ev1527_encode_post(io_write write, uint8_t *id, uint8_t id_len, uint8_t *key, uint8_t key_len);


#ifdef __cplusplus
}
#endif

#endif /* __EV1527_H */

/************************ (C) COPYRIGHT Tuu ********END OF FILE****************/


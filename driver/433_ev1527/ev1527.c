/**
  *****************************************************************************
  * @file    : ev1527.c
  * @author  : Tuu
  * @version : 1.0.0
  * @date    : 2021-03-13
  * @brief   : ev1527 api
  ******************************************************************************
  * @lasteditors  : Tuu
  * @lasteditTime : 2021-03-13
  ******************************************************************************
  * @atten   : Copyright (C) by Tuu Inc
  *
  *****************************************************************************
  */

/* Includes -------------------------------------------------------------------*/
#include "ev1527.h"

#include "hw_timer.h"
#include "string.h"

/* Defines --------------------------------------------------------------------*/
#define EV_DECODE_TIMER_TICK_US     10
#define EV_DECODE_ERROR_RANGE       50

#define EV_LCK          (90)

#define EV_SYNC_HIGH    (4*EV_LCK)
#define EV_SYNC_LOW     (124*EV_LCK)

#define EV_LEVEL1_HIGH  (12*EV_LCK)
#define EV_LEVEL1_LOW   (4*EV_LCK)

#define EV_LEVEL0_HIGH  (4*EV_LCK)
#define EV_LEVEL0_LOW   (12*EV_LCK)


/* Variables ------------------------------------------------------------------*/
static ev_encode ev_post = {0};
static ev_decode ev_params = {0};

/* Functions ------------------------------------------------------------------*/

// code lib

static u8 allow_range(uint16_t cnt, uint16_t norm)
{
    if (cnt >= (norm-EV_DECODE_ERROR_RANGE) && cnt <= (norm+EV_DECODE_ERROR_RANGE))
    {
        return true;
    }
    else
    {
        return false;
    }
}

static u8 ev1527_status_parse(uint16_t high_cnt, uint16_t low_cnt)
{
    if (true == allow_range(high_cnt, EV_SYNC_HIGH)
        && true == allow_range(low_cnt, EV_SYNC_LOW))
    {
        return EV_SYNC;
    }
    else if (true == allow_range(high_cnt, EV_LEVEL1_HIGH)
            && true == allow_range(low_cnt, EV_LEVEL1_LOW))
    {
        return EV_LEVEL_1;
    }
    else if (true == allow_range(high_cnt, EV_LEVEL0_HIGH)
            && true == allow_range(low_cnt, EV_LEVEL0_LOW))
    {
        return EV_LEVEL_0;
    }
    else
    {
        return -1;
    }
}

static u8 ev1527_decode(void)
{
    uint8_t level = 0;

    level = ev_params.level_read();
    if (1 == level)
    {
        if (0 == ev_params.last_level)
        {
            level = ev1527_status_parse(ev_params.high_cnt, ev_params.low_cnt);
            switch (level)
            {
                case EV_SYNC:
                    /* code */
                    memset(&(ev_params.buf), 0, sizeof(ev_data));
                    ev_params.sync = 1;
                    break;

                case EV_LEVEL_1:
                case EV_LEVEL_0:
                    /* code */
                    if (1 == ev_params.sync)
                    {
                        if (ev_params.buf.bit_cnt < (20*8))
                        {
                            if (1 == level)
                            {
                                ev_params.buf.id[20-1-(ev_params.buf.bit_cnt/8)] |= 0x80 >> (ev_params.buf.bit_cnt%8);
                            }
                            else
                            {
                                ev_params.buf.id[20-1-(ev_params.buf.bit_cnt/8)] &= ~(0x80 >> (ev_params.buf.bit_cnt%8));
                            }
                        }
                        else if (ev_params.buf.bit_cnt >= (20*8) && ev_params.buf.bit_cnt < (24*8))
                        {
                            if (1 == level)
                            {
                                ev_params.buf.key[4-1-(ev_params.buf.bit_cnt/8-20)] |= 0x80 >> (ev_params.buf.bit_cnt%8);
                            }
                            else
                            {
                                ev_params.buf.key[4-1-(ev_params.buf.bit_cnt/8-20)] &= ~(0x80 >> (ev_params.buf.bit_cnt%8));
                            }
                        }
                        else
                        {
                        }

                        ev_params.buf.bit_cnt++;

                        if (24*8 == ev_params.buf.bit_cnt)
                        {
                            ev_params.check_cnt++;

                            if (2 == ev_params.check_cnt)
                            {
                                //callback
                                ev_params.handle(&(ev_params.buf));
                                memset(&ev_params, 0, sizeof(ev_decode));
                            }
                            else
                            {
                                memset(&(ev_params.buf), 0, sizeof(ev_data));
                            }
                        }
                    }
                    else
                    {
                        memset(&ev_params, 0, sizeof(ev_decode));
                    }
                    break;

                default:
                    memset(&ev_params, 0, sizeof(ev_decode));
                    break;
            }
        }

        ev_params.high_cnt += EV_DECODE_TIMER_TICK_US;
        ev_params.last_level = 1;
    }
    else
    {
        ev_params.low_cnt += EV_DECODE_TIMER_TICK_US;
        ev_params.last_level = 0;
    }

    return 0;
}

//user code

//decode
static void ev1527_decode_handle(void *p)
{
    ev1527_decode();
}

int ev1527_decode_init(io_read read, ev_decode_handle handle)
{
    if (NULL == read || NULL == handle)
    {
        return -1;
    }
    memset(&ev_params, 0, sizeof(ev_decode));

    ev_params.level_read = read;
    ev_params.handle = handle;

    hw_timer_init(TIMER_0, EV_DECODE_TIMER_TICK_US, ev1527_decode_handle);

    return 0;
}

//encode
static u8 ev1527_encode(void);

static void ev1527_encode_handle(void *p)
{
    ev1527_encode();
}

static u8 ev1527_encode(void)
{
    u8 level = 0, cnt = 0;

    if (ev_post.loop_cnt >= 3)
    {
        ev_post.stop();
        return 0;
    }

    if (ev_post.buf.bit_cnt >= (2+2*24))
    {
        ev_post.loop_cnt++;
        ev_post.buf.bit_cnt = 0;
    }

    if (ev_post.buf.bit_cnt == 0)
    {
        ev_post.level_write(1);
        ev_post.restart(EV_SYNC_HIGH);
    }
    else if (ev_post.buf.bit_cnt == 1)
    {
        ev_post.level_write(0);
        ev_post.restart(EV_SYNC_LOW);
    }
    else if (ev_post.buf.bit_cnt%2 == 0)
    {
        if (ev_post.buf.bit_cnt >= 2 && ev_post.buf.bit_cnt < (2+2*20))
        {
            cnt = ev_post.buf.bit_cnt/2-1;
            level = (ev_post.buf.id[cnt/8] >> (7-cnt%8)) & 0x01;
            if (1 == level)
            {
                ev_post.level_write(1);
                ev_post.restart(EV_LEVEL1_HIGH);
            }
            else
            {
                ev_post.level_write(0);
                ev_post.restart(EV_LEVEL0_HIGH);
            }
        }
        else if (ev_post.buf.bit_cnt >= (2+2*20) && ev_post.buf.bit_cnt < (2+2*24))
        {
            cnt = ev_post.buf.bit_cnt/2-1-20;
            level = (ev_post.buf.key[cnt/8] >> (7-cnt%8)) & 0x01;
            if (1 == level)
            {
                ev_post.level_write(1);
                ev_post.restart(EV_LEVEL1_HIGH);
            }
            else
            {
                ev_post.level_write(0);
                ev_post.restart(EV_LEVEL0_HIGH);
            }
        }
        else
        {
        }
    }
    else if (ev_post.buf.bit_cnt%2 == 1)
    {
        if (ev_post.buf.bit_cnt >= 2 && ev_post.buf.bit_cnt < (2+2*20))
        {
            cnt = ev_post.buf.bit_cnt/2-1;
            level = (ev_post.buf.id[cnt/8] >> (7-cnt%8)) & 0x01;
            if (1 == level)
            {
                ev_post.level_write(1);
                ev_post.restart(EV_LEVEL1_LOW);
            }
            else
            {
                ev_post.level_write(0);
                ev_post.restart(EV_LEVEL0_LOW);
            }
        }
        else if (ev_post.buf.bit_cnt >= (2+2*20) && ev_post.buf.bit_cnt < (2+2*24))
        {
            cnt = ev_post.buf.bit_cnt/2-1-20;
            level = (ev_post.buf.key[cnt/8] >> (7-cnt%8)) & 0x01;
            if (1 == level)
            {
                ev_post.level_write(1);
                ev_post.restart(EV_LEVEL1_LOW);
            }
            else
            {
                ev_post.level_write(0);
                ev_post.restart(EV_LEVEL0_LOW);
            }
        }
    }
    else
    {
    }

    ev_post.buf.bit_cnt++;

    return 0;
}

static u8 encode_timer_restart(uint32_t time_us)
{
    hw_timer_init(TIMER_0, time_us, ev1527_encode_handle);

    return 0;
}

static u8 encode_timer_stop(void)
{
    hw_timer_stop(TIMER_0);

    return 0;
}

int ev1527_encode_post(io_write write, uint8_t *id, uint8_t id_len, uint8_t *key, uint8_t key_len)
{
    if (NULL == write || NULL == id || NULL == key ||
        20 < id_len || 4 < key_len)
    {
        return -1;
    }
    memset(&ev_post, 0, sizeof(ev_encode));

    ev_post.level_write = write;
    ev_post.restart = encode_timer_restart;
    ev_post.stop = encode_timer_stop;
    memcpy(&(ev_post.buf.id), id, id_len);
    memcpy(&(ev_post.buf.key), key, key_len);

    ev_post.level_write(0);
    ev_post.restart(1000);

    return 0;
}

/************************ (C) COPYRIGHT Tuu ********END OF FILE****************/


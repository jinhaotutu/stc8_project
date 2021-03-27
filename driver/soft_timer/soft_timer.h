/**
  *****************************************************************************
  * @file    : soft_timer.h
  * @author  : Tuu
  * @version : 1.0.0
  * @date    : 2021-03-13
  * @brief   : Header for soft_timer.c module
  ******************************************************************************
  * @lasteditors  : Tuu
  * @lasteditTime : 2021-03-13
  ******************************************************************************
  * @atten   : Copyright (C) by Tuu Inc
  *
  *****************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SOFT_TIMER_H
#define __SOFT_TIMER_H

/* Includes ------------------------------------------------------------------*/
#include "type.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Defines --------------------------------------------------------------------*/
#define TK_TIMER_USING_TIMEOUT_CALLBACK

#define TIMER_TICK_US  1000
#define TK_ASSERT(EXPR) (void)EXPR

// #define TK_TIMER_USING_TIMEOUT_CALLBACK
// #define TK_TIMER_USING_CREATE

/* Variables ------------------------------------------------------------------*/
typedef enum
{
    TIMER_STATE_RUNNING = 0,
    TIMER_STATE_STOP,
    TIMER_STATE_TIMEOUT,
} tk_timer_state;

typedef enum
{
    TIMER_MODE_SINGLE = 0,
    TIMER_MODE_LOOP,
} tk_timer_mode;

struct tk_timer
{
    bool enable;
    tk_timer_state state;
    tk_timer_mode mode;
    uint32_t delay_tick;
    uint32_t timer_tick_timeout;
    struct tk_timer *prev;
    struct tk_timer *next;
#ifdef TK_TIMER_USING_TIMEOUT_CALLBACK
	void(*timeout_callback)(struct tk_timer *timer);
#endif /* TK_TIMER_USING_TIMEOUT_CALLBACK */
};
typedef struct tk_timer *tk_timer_t;

// typedef void (*timeout_callback)(struct tk_timer *timer);

/* Functions ------------------------------------------------------------------*/
int soft_timer_update(void);
int soft_timer_init(void);
int soft_timer_loop(void);
int soft_timer_register(struct tk_timer *timer, void (*timeout_callback)(struct tk_timer *timer));
int soft_timer_start(struct tk_timer *timer, tk_timer_mode mode, uint32_t delay_tick);
int soft_timer_stop(struct tk_timer *timer);

#ifdef __cplusplus
}
#endif

#endif /* __SOFT_TIMER_H */

/************************ (C) COPYRIGHT Tuu ********END OF FILE****************/


/**
  *****************************************************************************
  * @file    : button.h
  * @author  : Tuu
  * @version : 1.0.0
  * @date    : 2021-03-13
  * @brief   : Header for button.c module
  ******************************************************************************
  * @lasteditors  : Tuu
  * @lasteditTime : 2021-03-13
  ******************************************************************************
  * @atten   : Copyright (C) by Tuu Inc
  *
  *****************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BUTTON_H
#define __BUTTON_H

/* Includes ------------------------------------------------------------------*/
#include "type.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Defines --------------------------------------------------------------------*/
#define FLEX_BTN_SCAN_FREQ_HZ (20) // How often flex_button_scan () is called
#define FLEX_MS_TO_SCAN_CNT(ms) (ms / (1000 / FLEX_BTN_SCAN_FREQ_HZ))

/* Multiple clicks interval, default 300ms */
#define MAX_MULTIPLE_CLICKS_INTERVAL (FLEX_MS_TO_SCAN_CNT(50))

/* Variables ------------------------------------------------------------------*/
typedef void (*flex_button_response_callback)(void*);

typedef enum
{
    FLEX_BTN_PRESS_DOWN = 0,
    FLEX_BTN_PRESS_CLICK,
    FLEX_BTN_PRESS_DOUBLE_CLICK,
    FLEX_BTN_PRESS_REPEAT_CLICK,
    FLEX_BTN_PRESS_SHORT_START,
    FLEX_BTN_PRESS_SHORT_UP,
    FLEX_BTN_PRESS_LONG_START,
    FLEX_BTN_PRESS_LONG_UP,
    FLEX_BTN_PRESS_LONG_HOLD,
    FLEX_BTN_PRESS_LONG_HOLD_UP,
    FLEX_BTN_PRESS_MAX,
    FLEX_BTN_PRESS_NONE,
} flex_button_event_t;

typedef struct flex_button
{
    struct flex_button* next;

    uint8_t  (*usr_button_read)(void *);
    flex_button_response_callback  cb;

    uint16_t scan_cnt;
    uint16_t click_cnt;
    uint16_t max_multiple_clicks_interval;

    uint16_t debounce_tick;
    uint16_t short_press_start_tick;
    uint16_t long_press_start_tick;
    uint16_t long_hold_start_tick;

    uint8_t id;
    uint8_t pressed_logic_level;
    uint8_t event;
    uint8_t status;
} flex_button_t;

/* Functions ------------------------------------------------------------------*/
int button_init(void);
int button_register(flex_button_t *button);
flex_button_event_t button_event(flex_button_t* button);

#ifdef __cplusplus
}
#endif

#endif /* __BUTTON_H */

/************************ (C) COPYRIGHT Tuu ********END OF FILE****************/


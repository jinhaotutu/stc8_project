/**
  *****************************************************************************
  * @file    : remote.c
  * @author  : Tuu
  * @version : 1.0.0
  * @date    : 2021-03-13
  * @brief   : remote api
  ******************************************************************************
  * @lasteditors  : Tuu
  * @lasteditTime : 2021-03-13
  ******************************************************************************
  * @atten   : Copyright (C) by Tuu Inc
  *
  *****************************************************************************
  */

/* Includes -------------------------------------------------------------------*/
#include "remote.h"
#include "string.h"

#include "uart.h"
#include "button.h"
#include "gpio.h"
#include "ev1527.h"
#include "soft_timer.h"
#include "e2prom.h"
#include "adc.h"

/* Defines --------------------------------------------------------------------*/


/* Variables ------------------------------------------------------------------*/
typedef enum{
    UNKOWN,
    POWER,
    TIMER,
    MOTOR,
    HOT,
    AIR,
    MATCH = 0xaa,
}MODE_DEF;

typedef enum{
    OFF,
    LOW,
    MIDDLE,
    HIGH,
    MATCH_L = 0xaa,
}LEVEL_DEF;

typedef struct protocol_info
{
    /* data */
    uint8_t head;
    uint8_t mode;
    uint8_t level;
    uint8_t crc;
}protocol_info;

typedef struct control_info
{
    /* data */
    uint8_t power;
    uint8_t timer;
    uint8_t motor_level;
    uint8_t hot_level;
    uint8_t air_level;

    uint8_t low_cnt;
    uint8_t match;
    uint8_t sleep;

    uint8_t id[8];
}control_info;

static control_info rmt_params;
static struct tk_timer low_timer;
static struct tk_timer sleep_timer;
static struct tk_timer match_timer;

//
typedef enum{
    POWER_ID,
    TIMER_ID,
    MOTOR_ID,
    HOT_ID,
    AIR_ID,
}BUTTON_ID;

static flex_button_t user_button[5];

typedef struct led_info
{
    /* data */
    GPIO_GROUP group;
    GPIO_PIN pin;
    uint8_t color[4];
}led_info;


static const led_info motor_map[3]={
    {GROUP_2, PIN_3, {0, 0, 0, 1}},
    {GROUP_2, PIN_5, {0, 1, 0, 0}},
    {GROUP_2, PIN_4, {0, 1, 1, 1}},
};
static const led_info hot_map[3]={
    {GROUP_1, PIN_7, {0, 0, 0, 1}},
    {GROUP_1, PIN_5, {0, 1, 0, 0}},
    {GROUP_1, PIN_6, {0, 1, 1, 1}},
};
static const led_info air_map[3]={
    {GROUP_0, PIN_3, {0, 0, 0, 1}},
    {GROUP_0, PIN_1, {0, 1, 0, 0}},
    {GROUP_0, PIN_2, {0, 1, 1, 1}},
};

/* Functions ------------------------------------------------------------------*/
static void write_remote_params(void);
static void sleep_timer_start(void);

static u8 io_wrtie(u8 level)
{
    gpio_write(GROUP_1, PIN_3, level);

    return 0;
}

static u8 calc_crc(u8 *buf, u16 len)
{
    u16 i = 0;
    u16 crc = 0;
    for (i=0;i<len;i++){
        crc += buf[i];
    }

    return (crc & 0x00FF);
}

static u8 key[4] = {0};

void rf_post(u8 mode, u8 level)
{
    uint8_t ret;

    // protocol_info *info = (protocol_info *)key;
    // info->head = 0x55;
    // info->mode = mode;
    // info->level = level;
    // info->crc = calc_crc(key, 3);

    key[0] = 0x55;
    key[1] = mode;
    key[2] = level;
    key[3] = calc_crc(key, 3);

    ret = ev1527_encode_post(io_wrtie, rmt_params.id, 7, key, 4);
    // print_hex(&ret, 1);
}

static void timer_low_handle(struct tk_timer *timer)
{
    print_str("timer_low_handle\n");

    if (rmt_params.low_cnt%2 == 0)
    {
        gpio_write(GROUP_2, PIN_1, 0);
    }
    else
    {
        gpio_write(GROUP_2, PIN_1, 1);
    }

    rmt_params.low_cnt++;

    if (rmt_params.low_cnt >= 6)
    {
        gpio_write(GROUP_2, PIN_1, !rmt_params.power);
        soft_timer_stop(&(low_timer));
    }
}

static u8 low_power_read(void)
{
    u16 adc_ref = get_adc_result(ADC_4);
    print_hex(&adc_ref, 2);

    if (adc_ref <= 220/2)
    {
        return 1;
    }

    return 0;
}

static void led_status_change(u8 mode, u8 level)
{
    static u8 i=0;

    switch (mode)
    {
        case POWER:
            /* code */
            if (1 == low_power_read())
            {
                gpio_write(GROUP_2, PIN_1, 1);
                rmt_params.low_cnt = 0;
                soft_timer_start(&(low_timer), TIMER_MODE_LOOP, 500);
            }
            else
            {
                gpio_write(GROUP_2, PIN_1, !level);
            }
            break;

        case TIMER:
            /* code */
            gpio_write(GROUP_2, PIN_0, !level);
            break;

        case MOTOR:
            /* code */
            for(i=0;i<3;i++)
            {
                gpio_write(motor_map[i].group, motor_map[i].pin, !motor_map[i].color[level]);
            }
            break;

        case HOT:
            /* code */
            for(i=0;i<3;i++)
            {
                gpio_write(hot_map[i].group, hot_map[i].pin, !hot_map[i].color[level]);
            }
            break;

        case AIR:
            /* code */
            for(i=0;i<3;i++)
            {
                gpio_write(air_map[i].group, air_map[i].pin, !air_map[i].color[level]);
            }
            break;

        default:
            break;
    }
}

static void motor_remote_post(void)
{
    if (rmt_params.power != 1)
    {
        print_str("power already off\r\n");
        return;
    }

    if (rmt_params.air_level !=  OFF)
    {
        print_str("air is already run\r\n");
        return;
    }

    if (rmt_params.motor_level >= HIGH)
    {
        rmt_params.motor_level = OFF;
    }
    else
    {
        rmt_params.motor_level++;
    }

    led_status_change(MOTOR, rmt_params.motor_level);
    write_remote_params();
    rf_post(MOTOR, rmt_params.motor_level);
}

static void air_remote_post(void)
{
    if (rmt_params.power != 1)
    {
        print_str("power already off\r\n");
        return;
    }

    if (rmt_params.motor_level !=  OFF)
    {
        print_str("motor is already run\r\n");
        return;
    }

    if (rmt_params.air_level >= HIGH)
    {
        rmt_params.air_level = OFF;
    }
    else
    {
        rmt_params.air_level++;
    }

    led_status_change(AIR, rmt_params.air_level);
    write_remote_params();
    rf_post(AIR, rmt_params.air_level);
}

static void hot_remote_post(void)
{
    if (rmt_params.power != 1)
    {
        print_str("power already off\r\n");
        return;
    }

    if (rmt_params.hot_level >= HIGH)
    {
        rmt_params.hot_level = OFF;
    }
    else
    {
        rmt_params.hot_level++;
    }

    led_status_change(HOT, rmt_params.hot_level);
    write_remote_params();
    rf_post(HOT, rmt_params.hot_level);
}

void remote_power_light(u8 level)
{
    if (level == 1)
    {
        print_str("power on\r\n");
        led_status_change(POWER, rmt_params.power);
        led_status_change(TIMER, rmt_params.timer);
        led_status_change(MOTOR, rmt_params.motor_level);
        led_status_change(HOT, rmt_params.hot_level);
        led_status_change(AIR, rmt_params.air_level);

        sleep_timer_start();
    }
    else
    {
        print_str("power off\r\n");
        led_status_change(POWER, 0);
        led_status_change(TIMER, 0);
        led_status_change(MOTOR, 0);
        led_status_change(HOT, 0);
        led_status_change(AIR, 0);
    }
}

static void power_remote_post(void)
{
    if (rmt_params.sleep == 1 && rmt_params.power == 1)
    {
        print_str("wake up\r\n");
        rmt_params.sleep = 0;
        remote_power_light(rmt_params.power);
    }
    else
    {
        rmt_params.sleep = 0;
        if (rmt_params.power == 1)
        {
            rmt_params.power = 0;
            rmt_params.timer = 0;
        }
        else
        {
            rmt_params.power = 1;
        }

        // led_status_change(POWER, rmt_params.power);
        remote_power_light(rmt_params.power);
        write_remote_params();
        rf_post(POWER, rmt_params.power);
    }
}

static void timer_remote_post(void)
{
    if (rmt_params.power != 1)
    {
        print_str("power already off\r\n");
        return;
    }

    if (rmt_params.timer == 1)
    {
        rmt_params.timer = 0;
    }
    else
    {
        rmt_params.timer = 1;
    }

    led_status_change(TIMER, rmt_params.timer);
    // write_remote_params();
    rf_post(TIMER, rmt_params.timer);
}

static void match_remote_post(void)
{
    gpio_write(GROUP_2, PIN_1, 0);
    gpio_write(GROUP_2, PIN_0, 0);
    rmt_params.match = 1;
    soft_timer_start(&(match_timer), TIMER_MODE_LOOP, 500);
}

static void match_remote_stop(void)
{
    if (rmt_params.match == 0)
    {
        return;
    }

    rmt_params.match = 0;
    soft_timer_stop(&(match_timer));
    gpio_write(GROUP_2, PIN_1, 1);
    gpio_write(GROUP_2, PIN_0, 1);
}

static void read_chip_id(void)
{
    char *id_buf;

    id_buf = (char code *)0x6ff9;

    memcpy(rmt_params.id, id_buf, 7);

    print_hex(rmt_params.id, 7);
}

#define CONTROL_PARAMS_ADDR     0x0000
static u8 e2_buf[6]={0};

static void read_remote_params(void)
{
    memset(e2_buf, 0, 6);
    e2prom_read(CONTROL_PARAMS_ADDR, e2_buf, 6);

    print_hex(e2_buf, 6);

    if (e2_buf[5] != calc_crc(e2_buf, 5))
    {
        print_str("check crc error\n");
        return;
    }

    rmt_params.power = e2_buf[0];
    // rmt_params.timer = buf[1];
    rmt_params.motor_level = e2_buf[2];
    rmt_params.hot_level = e2_buf[3];
    rmt_params.air_level = e2_buf[4];

    print_str("read data succeed\n");
}

static void write_remote_params(void)
{
    memset(e2_buf, 0, 6);

    e2_buf[0] = rmt_params.power;
    // buf[1] = rmt_params.timer;
    e2_buf[2] = rmt_params.motor_level;
    e2_buf[3] = rmt_params.hot_level;
    e2_buf[4] = rmt_params.air_level;
    e2_buf[5] = calc_crc(e2_buf, 5);

    print_hex(e2_buf, 6);

    if (0 != e2prom_write(CONTROL_PARAMS_ADDR, e2_buf, 6))
    {
        print_str("write data error\n");
    }

    print_str("write data succeed\n");
}

static void remote_status_init(void)
{
    memset(&rmt_params, 0, sizeof(control_info));

    //ID
    read_chip_id();

    read_remote_params();
    remote_power_light(rmt_params.power);
}

static uint8_t common_btn_read(void *arg)
{
    uint8_t value = 0;

    flex_button_t *btn = (flex_button_t *)arg;

    switch (btn->id)
    {
        case POWER_ID:
            value = gpio_read(GROUP_3, PIN_3);
            break;

        case TIMER_ID:
            value = gpio_read(GROUP_3, PIN_4);
            break;

        case MOTOR_ID:
            value = gpio_read(GROUP_3, PIN_7);
            break;

        case HOT_ID:
            value = gpio_read(GROUP_3, PIN_2);
            break;

        case AIR_ID:
            value = gpio_read(GROUP_3, PIN_6);
            break;

        default:
            break;
    }

    return value;
}

static void sleep_timer_start(void)
{
    soft_timer_start(&sleep_timer, TIMER_MODE_SINGLE, 5000);
}

static void common_btn_evt_cb(void *arg)
{
    static flex_button_t *btn;

    btn = (flex_button_t *)arg;

    if (((button_event(&user_button[POWER_ID]) == FLEX_BTN_PRESS_LONG_START) ||
        button_event(&user_button[POWER_ID]) == FLEX_BTN_PRESS_LONG_HOLD) &&
        ((button_event(&user_button[HOT_ID]) == FLEX_BTN_PRESS_LONG_START) ||
        button_event(&user_button[HOT_ID]) == FLEX_BTN_PRESS_LONG_HOLD))
    {
        match_remote_post();
        print_str("match key push\n");
    }

    if ((button_event(&user_button[POWER_ID]) == FLEX_BTN_PRESS_LONG_UP) ||
        (button_event(&user_button[HOT_ID]) == FLEX_BTN_PRESS_LONG_UP) ||
        (button_event(&user_button[POWER_ID]) == FLEX_BTN_PRESS_LONG_HOLD_UP) ||
        (button_event(&user_button[HOT_ID]) == FLEX_BTN_PRESS_LONG_HOLD_UP))
    {
        match_remote_stop();
        print_str("match key up\n");
    }

    if (button_event(&user_button[POWER_ID]) == FLEX_BTN_PRESS_SHORT_START)
    {
        power_remote_post();
        print_str("power key push\n");
    }

    if (rmt_params.sleep == 1 || rmt_params.power == 0)
    {
        print_str("sleep, push power\r\n");
        return;
    }

    if (button_event(&user_button[TIMER_ID]) == FLEX_BTN_PRESS_SHORT_START)
    {
        timer_remote_post();
        sleep_timer_start();
        print_str("timer key push\n");
    }

    if (button_event(&user_button[MOTOR_ID]) == FLEX_BTN_PRESS_SHORT_START)
    {
        motor_remote_post();
        sleep_timer_start();
        print_str("motor key push\n");
    }

    if (button_event(&user_button[HOT_ID]) == FLEX_BTN_PRESS_SHORT_START)
    {
        hot_remote_post();
        sleep_timer_start();
        print_str("hot key push\n");
    }

    if (button_event(&user_button[AIR_ID]) == FLEX_BTN_PRESS_SHORT_START)
    {
        air_remote_post();
        sleep_timer_start();
        print_str("air key push\n");
    }
}

static void timer_sleep_handle(struct tk_timer *timer)
{
    print_str("timer_sleep_handle\n");

    // TODO: 休眠
    remote_power_light(0);
    rmt_params.sleep = 1;
}

static void timer_match_handle(struct tk_timer *timer)
{
    print_str("timer_match_handle\n");

    if (rmt_params.match%2 == 0)
    {
        gpio_write(GROUP_2, PIN_1, 0);
        gpio_write(GROUP_2, PIN_0, 0);
    }
    else
    {
        gpio_write(GROUP_2, PIN_1, 1);
        gpio_write(GROUP_2, PIN_0, 1);
    }

    rmt_params.match++;
    rf_post(MATCH, MATCH_L);
}

void remote_user_init(void)
{
    u8 i = 0;

    //button
    gpio_hw_init(GROUP_3, PIN_7, PIN_INPUT);
    gpio_hw_init(GROUP_3, PIN_2, PIN_INPUT);
    gpio_hw_init(GROUP_3, PIN_6, PIN_INPUT);
    gpio_hw_init(GROUP_3, PIN_4, PIN_INPUT);
    gpio_hw_init(GROUP_3, PIN_3, PIN_INPUT);

    // //rf
    gpio_hw_init(GROUP_1, PIN_3, PIN_OUTPUT);
    gpio_write(GROUP_1, PIN_3, 0);

    // //led
    gpio_hw_init(GROUP_2, PIN_0, PIN_OUTPUT);
    gpio_hw_init(GROUP_2, PIN_1, PIN_OUTPUT);
    gpio_write(GROUP_2, PIN_0, 1);
    gpio_write(GROUP_2, PIN_1, 1);

    for(i=0;i<3;i++)
    {
        gpio_hw_init(motor_map[i].group, motor_map[i].pin, PIN_OUTPUT);
        gpio_write(motor_map[i].group, motor_map[i].pin, 1);
        gpio_hw_init(hot_map[i].group, hot_map[i].pin, PIN_OUTPUT);
        gpio_write(hot_map[i].group, hot_map[i].pin, 1);
        gpio_hw_init(air_map[i].group, air_map[i].pin, PIN_OUTPUT);
        gpio_write(air_map[i].group, air_map[i].pin, 1);
    }

    //adc
    adc_hw_init(ADC_4);

    memset(&user_button[0], 0x0, sizeof(user_button));

    for (i=0;i<5;i++)
    {
        user_button[i].id = i;
        user_button[i].usr_button_read = common_btn_read;
        user_button[i].cb = common_btn_evt_cb;
        user_button[i].pressed_logic_level = 1;
        user_button[i].short_press_start_tick = FLEX_MS_TO_SCAN_CNT(100);
        user_button[i].long_press_start_tick = FLEX_MS_TO_SCAN_CNT(3000);
        user_button[i].long_hold_start_tick = FLEX_MS_TO_SCAN_CNT(4500);

        button_register(&user_button[i]);
    }

    soft_timer_register(&(low_timer), timer_low_handle);
    soft_timer_register(&(sleep_timer), timer_sleep_handle);
    soft_timer_register(&match_timer, timer_match_handle);

    print_str("control init succeed\n");

    remote_status_init();
}

void remote_loop(void)
{

}

/************************ (C) COPYRIGHT Tuu ********END OF FILE****************/


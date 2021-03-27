/**
  *****************************************************************************
  * @file    : control.c
  * @author  : Tuu
  * @version : 1.0.0
  * @date    : 2021-03-13
  * @brief   : control api
  ******************************************************************************
  * @lasteditors  : Tuu
  * @lasteditTime : 2021-03-13
  ******************************************************************************
  * @atten   : Copyright (C) by Tuu Inc
  *
  *****************************************************************************
  */

/* Includes -------------------------------------------------------------------*/
#include "control.h"
#include "string.h"
#include "stdio.h"

#include "pwm.h"
#include "gpio.h"
#include "uart.h"
#include "hw_timer.h"
#include "ev1527.h"
#include "e2prom.h"
#include "soft_timer.h"

/* Defines --------------------------------------------------------------------*/
#define MOTOR_PERIOD_US     500
#define HOT_PERIOD_US       1000
#define AIR_PERIOD_US       1000
#define BEEP_PERIOD_US      1000

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

typedef struct rf_info
{
    /* data */
    ev_data buf;
    uint8_t recv_flag;
}rf_info;

typedef struct control_info
{
    /* data */
    uint8_t power;
    uint8_t timer;
    uint8_t motor_level;
    uint8_t hot_level;
    uint8_t air_level;

    uint8_t motor_cnt;
    uint8_t air_cnt;
    uint8_t t_cnt;
    uint8_t power_cnt;
    uint8_t mp_cnt;

    uint8_t id[8];
}control_info;

static struct tk_timer motor_timer;
static struct tk_timer power_timer;
static struct tk_timer t_timer;
static struct tk_timer air_timer;
static struct tk_timer mp_timer;

static control_info ctl_params={0};
static rf_info rf_params={0};
static const u16 mode_pwm_map[4][4]={
    {0, (MOTOR_PERIOD_US/2), (MOTOR_PERIOD_US*2/3), MOTOR_PERIOD_US},
    {0, (HOT_PERIOD_US/2), (HOT_PERIOD_US*3/4), HOT_PERIOD_US},
    {0, (AIR_PERIOD_US/2), (AIR_PERIOD_US*2/3), AIR_PERIOD_US},
    {0, (BEEP_PERIOD_US/2), (BEEP_PERIOD_US*2/3), BEEP_PERIOD_US},
};

/* Functions ------------------------------------------------------------------*/
static void write_control_id(void);
static void power_change(u8 level);
static void timer_change(u8 level);
static void write_control_params(void);

static void motor_init(void)
{
    // pwm_hw_init(PWM_1, MOTOR_PERIOD_US);
    // pwm_change_duty(PWM_1, 500);

    // pwm_hw_init(PWM_2, MOTOR_PERIOD_US);
    // pwm_change_duty(PWM_2, 0);

    gpio_hw_init(GROUP_1, PIN_0, PIN_OUTPUT);
    gpio_write(GROUP_1, PIN_0, 0);

    gpio_hw_init(GROUP_1, PIN_2, PIN_OUTPUT);
    gpio_write(GROUP_1, PIN_2, 0);

    pwm_hw_init(PWM_7, BEEP_PERIOD_US);
    pwm_change_duty(PWM_7, 0);
}

static void hot_init(void)
{
    pwm_hw_init(PWM_6, HOT_PERIOD_US);
    pwm_change_duty(PWM_6, 0);
}

static void air_init(void)
{
    gpio_hw_init(GROUP_2, PIN_0, PIN_OUTPUT);
    gpio_write(GROUP_2, PIN_0, 0);

    // pwm_hw_init(PWM_8, AIR_PERIOD_US);
    // pwm_change_duty(PWM_8, 0);

    gpio_hw_init(GROUP_2, PIN_3, PIN_OUTPUT);
    gpio_write(GROUP_2, PIN_3, 0);
}

static u8 rf_io_read(void)
{
    return gpio_read(GROUP_1, PIN_5);
}

static u8 ef_data_handle(ev_data *ev_buf)
{
    if (rf_params.recv_flag == 1)
    {
        return 0;
    }

    memcpy(&(rf_params.buf), ev_buf, sizeof(ev_data));
    rf_params.recv_flag = 1;

    return 1;
}

static void rf_init(void)
{
    memset(&rf_params, 0, sizeof(rf_info));

    gpio_hw_init(GROUP_1, PIN_5, PIN_INPUT);

    ev1527_decode_init(rf_io_read, ef_data_handle);
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

static u8 rf_data_parse(uint8_t *mode, uint8_t *level)
{
    static u8 crc=0;

#if 1
    print_hex(rf_params.buf.id, 8);

    print_hex(rf_params.buf.key, 4);
#endif

    // print_str("parse\r\n");

    if (0x55 != rf_params.buf.key[0])
    {
        print_str("head ckeck error\n");
        return UNKOWN;
    }

    // print_str("head\r\n");

    crc = calc_crc(rf_params.buf.key, 3);//rf_params.buf.key[0]+rf_params.buf.key[1]+rf_params.buf.key[2];
    if (crc != rf_params.buf.key[3])
    {
        print_str("crc ckeck error\n");
        return UNKOWN;
    }

    // print_str("crc\r\n");

    if (rf_params.buf.key[1] == MATCH && rf_params.buf.key[2] == MATCH_L)
    {
        *mode = MATCH;
        *level = MATCH_L;
        return *mode;
    }
    else
    {
        for (crc=0;crc<7;crc++)
        {
            if (ctl_params.id[crc] != rf_params.buf.id[crc])
            {
                print_str("rf id not path\n");
                return UNKOWN;
            }
            else
            {
                // print_str("rf id same\n");
            }
        }
    }

    // print_str("id\r\n");

    if (!(UNKOWN < rf_params.buf.key[1] && rf_params.buf.key[1] <= AIR))
    {
        print_str("mode ckeck error\n");
        return UNKOWN;
    }

    *mode = rf_params.buf.key[1];
    *level = rf_params.buf.key[2];

    return *mode;
}

static void timer_motor_handle(struct tk_timer *timer)
{
    print_str("timer_motor_handle\r\n");

    ctl_params.motor_cnt++;

    if (ctl_params.motor_level == 1)
    {
    }
    else if (ctl_params.motor_level == 2)
    {
        if (ctl_params.motor_cnt >= 4)
        {
            ctl_params.motor_cnt = 0;
            // pwm_change_duty(PWM_1, 0);
            // pwm_change_duty(PWM_2, 0);
            gpio_write(GROUP_1, PIN_0, 0);
            gpio_write(GROUP_1, PIN_2, 0);
        }
        else
        {
            // pwm_change_duty(PWM_1, mode_pwm_map[0][3]);
            // pwm_change_duty(PWM_2, 0);
            gpio_write(GROUP_1, PIN_0, 1);
            gpio_write(GROUP_1, PIN_2, 0);
        }
    }
    else if (ctl_params.motor_level == 3)
    {
        if (ctl_params.motor_cnt >= 13)
        {
            ctl_params.motor_cnt = 0;
            // pwm_change_duty(PWM_1, 0);
            // pwm_change_duty(PWM_2, 0);
            gpio_write(GROUP_1, PIN_0, 0);
            gpio_write(GROUP_1, PIN_2, 0);
        }
        else if (ctl_params.motor_cnt == 6)
        {
            // pwm_change_duty(PWM_1, 0);
            // pwm_change_duty(PWM_2, 0);
            gpio_write(GROUP_1, PIN_0, 0);
            gpio_write(GROUP_1, PIN_2, 0);
        }
        else if (ctl_params.motor_cnt < 6)
        {
            // pwm_change_duty(PWM_1, mode_pwm_map[0][3]);
            // pwm_change_duty(PWM_2, 0);
            gpio_write(GROUP_1, PIN_0, 1);
            gpio_write(GROUP_1, PIN_2, 0);
        }
        else if (ctl_params.motor_cnt > 6 && ctl_params.motor_cnt < 13)
        {
            // pwm_change_duty(PWM_1, 0);
            // pwm_change_duty(PWM_2, mode_pwm_map[0][3]);
            gpio_write(GROUP_1, PIN_0, 0);
            gpio_write(GROUP_1, PIN_2, 1);
        }
        else
        {
        }
    }
    else
    {}
}

static void motor_change(u8 level)
{
    if (ctl_params.power != 1)
    {
        print_str("power already off\r\n");
        return;
    }

    if (ctl_params.air_level !=  OFF)
    {
        print_str("air is already run\r\n");
        return;
    }

    if (level > HIGH)
    {
        print_str("motor level err\r\n");
        return;
    }

    if (level == 1)
    {
        print_str("motor level----1\r\n");
    }
    else if (level == 2)
    {
        print_str("motor level----2\r\n");
    }
    else if (level == 3)
    {
        print_str("motor level----3\r\n");
    }
    else
    {}

    print_str("motor level\r\n");
#if 0
    pwm_change_duty(PWM_1, mode_pwm_map[0][level]);
    pwm_change_duty(PWM_2, 0);
    pwm_change_duty(PWM_7, mode_pwm_map[3][level]);
#else
    pwm_change_duty(PWM_7, mode_pwm_map[3][level]);

    if (level == OFF)
    {
        // pwm_change_duty(PWM_1, 0);
        // pwm_change_duty(PWM_2, 0);
        gpio_write(GROUP_1, PIN_0, 1);
        gpio_write(GROUP_1, PIN_2, 0);

        ctl_params.motor_cnt = 0;
        // soft_timer_stop(&(motor_timer));
    }
    else
    {
        // pwm_change_duty(PWM_1, mode_pwm_map[0][3]);
        // pwm_change_duty(PWM_2, 0);
        gpio_write(GROUP_1, PIN_0, 1);
        gpio_write(GROUP_1, PIN_2, 0);

        ctl_params.motor_cnt = 0;
        soft_timer_start(&(motor_timer), TIMER_MODE_LOOP, 2000);
    }
#endif
}

static void hot_change(u8 level)
{
    if (ctl_params.power != 1)
    {
        print_str("power already off\r\n");
        return;
    }

    if (level > HIGH)
    {
        print_str("hot level err\r\n");
        return;
    }

    if (level == 1)
    {
        print_str("hot level----1\r\n");
    }
    else if (level == 2)
    {
        print_str("hot level----2\r\n");
    }
    else if (level == 3)
    {
        print_str("hot level----3\r\n");
    }
    else
    {}

    print_str("hot level\r\n");
    pwm_change_duty(PWM_6, mode_pwm_map[1][level]);
}

#define AIR_IN_TIME     (6/2)
#define AIR_KEEP_TIME   (40/2)
#define AIR_OUT_TIME    (24/2)

static void timer_air_handle(struct tk_timer *timer)
{
    static u8 level = 0;

    print_str("timer_air_handle\r\n");

    if (ctl_params.air_cnt == 0)
    {
        level = ctl_params.air_level;
    }

    ctl_params.air_cnt++;

    if (ctl_params.air_cnt >= (level*AIR_IN_TIME+AIR_KEEP_TIME+AIR_OUT_TIME))
    {
        ctl_params.air_cnt = 0;
        gpio_write(GROUP_2, PIN_0, 1);
        // pwm_change_duty(PWM_8, mode_pwm_map[2][3]);
        gpio_write(GROUP_2, PIN_3, 1);
        print_str("air in\r\n");
    }
    else if (ctl_params.air_cnt < (level*AIR_IN_TIME))
    {
        gpio_write(GROUP_2, PIN_0, 1);
        // pwm_change_duty(PWM_8, mode_pwm_map[2][3]);
        gpio_write(GROUP_2, PIN_3, 1);
        print_str("air in\r\n");
    }
    else if (ctl_params.air_cnt >= (level*AIR_IN_TIME) && ctl_params.air_cnt < (level*AIR_IN_TIME+AIR_KEEP_TIME))
    {
        gpio_write(GROUP_2, PIN_0, 1);
        // pwm_change_duty(PWM_8, 0);
        gpio_write(GROUP_2, PIN_3, 0);
        print_str("air keep\r\n");
    }
    else if (ctl_params.air_cnt >= (level*AIR_IN_TIME+AIR_KEEP_TIME) && ctl_params.air_cnt < (level*AIR_IN_TIME+AIR_KEEP_TIME+AIR_OUT_TIME))
    {
        gpio_write(GROUP_2, PIN_0, 0);
        // pwm_change_duty(PWM_8, 0);
        gpio_write(GROUP_2, PIN_3, 0);
        print_str("air out\r\n");
    }
    else
    {}
}

static void air_change(u8 level)
{
    if (ctl_params.power != 1)
    {
        print_str("power already off\r\n");
        return;
    }

    if (ctl_params.motor_level !=  OFF)
    {
        print_str("motor is already run\r\n");
        return;
    }

    if (level > HIGH)
    {
        print_str("air level err\r\n");
        return;
    }

    if (level == 1)
    {
        print_str("air level----1\r\n");
    }
    else if (level == 2)
    {
        print_str("air level----2\r\n");
    }
    else if (level == 3)
    {
        print_str("air level----3\r\n");
    }
    else
    {}

    if (level == OFF)
    {
        print_str("air off\r\n");
        gpio_write(GROUP_2, PIN_0, 0);
        // pwm_change_duty(PWM_8, 0);
        gpio_write(GROUP_2, PIN_3, 0);

        ctl_params.air_cnt = 0;
        soft_timer_stop(&(air_timer));
    }
    else
    {
        print_str("air level\r\n");
        gpio_write(GROUP_2, PIN_0, 1);
        // pwm_change_duty(PWM_8, mode_pwm_map[2][3]);
        gpio_write(GROUP_2, PIN_3, 1);

        // ctl_params.motor_cnt = 0;
        if (ctl_params.air_cnt == 0)
        {
            soft_timer_start(&(air_timer), TIMER_MODE_LOOP, 2000);
        }
    }
}

static void timer_power_handle(struct tk_timer *timer)
{
    print_str("timer_power_handle\r\n");

    ctl_params.power_cnt++;

    if (ctl_params.power_cnt >= 60)
    {
        ctl_params.power_cnt = 0;
        power_change(OFF);
        soft_timer_stop(&power_timer);
    }
}

static void timer_mp_handle(struct tk_timer *timer)
{
    print_str("timer_mp_handle\r\n");

    if (ctl_params.mp_cnt%2==0)
    {
        // pwm_change_duty(PWM_1, mode_pwm_map[0][3]);
        // pwm_change_duty(PWM_2, 0);
        gpio_write(GROUP_1, PIN_0, 1);
        gpio_write(GROUP_1, PIN_2, 0);
        pwm_change_duty(PWM_7, mode_pwm_map[3][3]);
    }
    else
    {
        // pwm_change_duty(PWM_1, 0);
        // pwm_change_duty(PWM_2, 0);
        gpio_write(GROUP_1, PIN_0, 0);
        gpio_write(GROUP_1, PIN_2, 0);
        pwm_change_duty(PWM_7, 0);
    }

    ctl_params.mp_cnt++;

    if (ctl_params.mp_cnt >= (!ctl_params.power)*2+2)
    {
        ctl_params.mp_cnt = 0;
        // pwm_change_duty(PWM_1, 0);
        // pwm_change_duty(PWM_2, 0);
        gpio_write(GROUP_1, PIN_0, 0);
        gpio_write(GROUP_1, PIN_2, 0);
        pwm_change_duty(PWM_7, 0);
        soft_timer_stop(&mp_timer);
    }
}

static void power_change(u8 level)
{
    if (level > 1)
    {
        print_str("power level err\r\n");
        return;
    }
    ctl_params.power = level;

    if (level == 1)
    {
        print_str("power on\r\n");
        motor_change(ctl_params.motor_level);
        hot_change(ctl_params.hot_level);
        air_change(ctl_params.air_level);

        ctl_params.power_cnt = 0;
        soft_timer_start(&power_timer, TIMER_MODE_LOOP, 60000);

        if (ctl_params.motor_level == 0)
        {
            soft_timer_start(&mp_timer, TIMER_MODE_LOOP, 200);
        }
    }
    else
    {
        ctl_params.power_cnt = 0;
        print_str("power off\r\n");
        timer_change(OFF);
        motor_change(OFF);
        hot_change(OFF);
        air_change(OFF);

        soft_timer_start(&mp_timer, TIMER_MODE_LOOP, 200);
    }
}

static void timer_t_handle(struct tk_timer *timer)
{
    print_str("timer_air_handle\r\n");

    ctl_params.t_cnt++;

    if (ctl_params.t_cnt >= 60)
    {
        ctl_params.hot_level = OFF;
        hot_change(ctl_params.hot_level);
        ctl_params.t_cnt = 0;
        ctl_params.timer = 0;
        soft_timer_stop(&t_timer);
        // write_control_params();
    }
    else if (ctl_params.t_cnt == 30)
    {
        ctl_params.air_level = OFF;
        air_change(ctl_params.air_level);
        // write_control_params();
    }
    else if (ctl_params.t_cnt == 15)
    {
        ctl_params.motor_level = OFF;
        motor_change(ctl_params.motor_level);
        // write_control_params();
    }
    else
    {
    }
}

static void timer_change(u8 level)
{
    if (ctl_params.power != 1)
    {
        print_str("power already off\r\n");
        return;
    }

    if (level > 1)
    {
        print_str("timer level err\r\n");
        return;
    }

    ctl_params.timer = level;
    if (level == 1)
    {
        ctl_params.t_cnt = 0;
        soft_timer_start(&t_timer, TIMER_MODE_LOOP, 60000);
        print_str("timer on\r\n");
    }
    else
    {
        ctl_params.t_cnt = 0;
        soft_timer_stop(&t_timer);
        print_str("timer off\r\n");
    }
}

static void match_id(u8 level)
{
    if (level == MATCH_L)
    {
        memcpy(ctl_params.id, rf_params.buf.id, 7);
        write_control_id();

        print_hex(ctl_params.id, 8);
    }
}

static void change_mode_level(u8 mode, u8 level)
{
    // print_str("mode change\n");
    switch (mode)
    {
        case POWER:
            /* code */
            power_change(level);
            write_control_params();
            break;

        case TIMER:
            /* code */
            timer_change(level);
            break;

        case MOTOR:
            /* code */
            motor_change(level);
            ctl_params.motor_level = level;
            write_control_params();
            break;

        case HOT:
            /* code */
            hot_change(level);
            ctl_params.hot_level = level;
            write_control_params();
            break;

        case AIR:
            /* code */
            air_change(level);
            ctl_params.air_level = level;
            write_control_params();
            break;

        case MATCH:
            match_id(level);
            write_control_id();
            break;

        default:
            print_str("unkown mode\n");
            break;
    }
}

#define CONTROL_PARAMS_ADDR     0x0000
#define ID_PARAMS_ADDR          (CONTROL_PARAMS_ADDR+E2PROM_BLOCK)
static u8 e2_buf[6]={0};

static void read_control_params(void)
{
    memset(e2_buf, 0, 6);
    e2prom_read(CONTROL_PARAMS_ADDR, e2_buf, 6);

    print_hex(&e2_buf, 6);

    if (e2_buf[5] != calc_crc(e2_buf, 5))
    {
        print_str("check crc error\n");
        return;
    }

    ctl_params.power = e2_buf[0];
    // ctl_params.timer = buf[1];
    ctl_params.motor_level = e2_buf[2];
    ctl_params.hot_level = e2_buf[3];
    ctl_params.air_level = e2_buf[4];

    print_str("read data succeed\n");
}

static void write_control_params(void)
{
    memset(e2_buf, 0, 6);

    e2_buf[0] = ctl_params.power;
    e2_buf[1] = ctl_params.timer;
    e2_buf[2] = ctl_params.motor_level;
    e2_buf[3] = ctl_params.hot_level;
    e2_buf[4] = ctl_params.air_level;
    e2_buf[5] = calc_crc(e2_buf, 5);

    print_hex(&e2_buf, 6);

    if (0 != e2prom_write(CONTROL_PARAMS_ADDR, e2_buf, 6))
    {
        print_str("write data error\n");
    }

    print_str("write data succeed\n");
}

static void read_control_id(void)
{
    e2prom_read(ID_PARAMS_ADDR, ctl_params.id, 8);

    print_hex(ctl_params.id, 8);

    if (ctl_params.id[7] != calc_crc(ctl_params.id, 7))
    {
        memset(ctl_params.id, 0, 8);
        print_str("check crc error\n");
        return;
    }

    print_str("read id succeed\n");
}

static void write_control_id(void)
{
    ctl_params.id[7] = calc_crc(ctl_params.id, 7);

    print_hex(ctl_params.id, 8);

    if (0 != e2prom_write(ID_PARAMS_ADDR, ctl_params.id, 8))
    {
        print_str("write id error\n");
    }

    print_str("write id succeed\n");
}

static void read_chip_id(void)
{
    char *id_buf;

    id_buf = (char code *)0x6ff9;

    memcpy(ctl_params.id, id_buf, 7);

    print_hex(ctl_params.id, 7);
}

static void control_status_init(void)
{
    memset(&ctl_params, 0, sizeof(control_info));
    read_control_params();
    read_control_id();

    power_change(ctl_params.power);
}

void control_user_init(void)
{
    motor_init();
    hot_init();
    air_init();

    rf_init();

    soft_timer_register(&(motor_timer), timer_motor_handle);
    soft_timer_register(&(air_timer), timer_air_handle);
    soft_timer_register(&(t_timer), timer_t_handle);
    soft_timer_register(&power_timer, timer_power_handle);
    soft_timer_register(&mp_timer, timer_mp_handle);

    print_str("control init succeed\r\n");

    control_status_init();
}

void control_loop(void)
{
    static uint8_t mode = 0,level = 0;

    if (rf_params.recv_flag == 1)
    {
        if (UNKOWN == rf_data_parse(&mode, &level))
        {
            print_str("rf parse error\n");
        }
        else
        {
            change_mode_level(mode, level);
        }

        rf_params.recv_flag = 0;
    }
}


/************************ (C) COPYRIGHT Tuu ********END OF FILE****************/


/**
  *****************************************************************************
  * @file    : gpio.c
  * @author  : Tuu
  * @version : 1.0.0
  * @date    : 2021-03-13
  * @brief   : stc8 gpio api
  ******************************************************************************
  * @lasteditors  : Tuu
  * @lasteditTime : 2021-03-13
  ******************************************************************************
  * @atten   : Copyright (C) by Tuu Inc
  *
  *****************************************************************************
  */

/* Includes -------------------------------------------------------------------*/
#include "gpio.h"

#include "STC8HX.h"

/* Defines --------------------------------------------------------------------*/


/* Variables ------------------------------------------------------------------*/


/* Functions ------------------------------------------------------------------*/

void gpio_hw_init(GPIO_GROUP group, GPIO_PIN pin, GPIO_DIC dic)
{
    P4M1 = 0;   P4M0 = 0;   //设置为准双向口
    P5M1 = 0;   P5M0 = 0;   //设置为准双向口
    P6M1 = 0;   P6M0 = 0;   //设置为准双向口
    P7M1 = 0;   P7M0 = 0;   //设置为准双向口

    if (dic == PIN_OUTPUT)
    {
        switch (group)
        {
            case GROUP_0:
                /* code */
                P0M0 = 0x01 << pin;
                P0M1 = 0;
                break;
            case GROUP_1:
                /* code */
                P1M0 = 0x01 << pin;
                P1M1 = 0;
                break;
            case GROUP_2:
                /* code */
                P2M0 = 0x01 << pin;
                P2M1 = 0;
                break;
            case GROUP_3:
                /* code */
                P3M0 = 0x01 << pin;
                P3M1 = 0;
                break;

            default:
                break;
        }
    }
    else if (dic == PIN_INPUT)
    {
        switch (group)
        {
            case GROUP_0:
                /* code */
                P0M0 = 0;
                P0M1 = 0x01 << pin;
                break;
            case GROUP_1:
                /* code */
                P1M0 = 0;
                P1M1 = 0x01 << pin;
                break;
            case GROUP_2:
                /* code */
                P2M0 = 0;
                P2M1 = 0x01 << pin;
                break;
            case GROUP_3:
                /* code */
                P3M0 = 0;
                P3M1 = 0x01 << pin;
                break;

            default:
                break;
        }
    }
    else
    {
        switch (group)
        {
            case GROUP_0:
                /* code */
                P0M0 = 0;
                P0M1 = 0;
                break;
            case GROUP_1:
                /* code */
                P1M0 = 0;
                P1M1 = 0;
                break;
            case GROUP_2:
                /* code */
                P2M0 = 0;
                P2M1 = 0;
                break;
            case GROUP_3:
                /* code */
                P3M0 = 0;
                P3M1 = 0;
                break;

            default:
                break;
        }
    }
}

u8 gpio_read(GPIO_GROUP group, GPIO_PIN pin)
{
    u8 value = 0;

    switch (group)
    {
        case GROUP_0:
            /* code */
            value = P0 & (0x01 << pin);
            break;
        case GROUP_1:
            /* code */
            value = P1 & (0x01 << pin);
            break;
        case GROUP_2:
            /* code */
            value = P2 & (0x01 << pin);
            break;
        case GROUP_3:
            /* code */
            value = P3 & (0x01 << pin);
            break;

        default:
            break;
    }

    return value;
}

void gpio_write(GPIO_GROUP group, GPIO_PIN pin, u8 value)
{
    switch (group)
    {
        case GROUP_0:
            /* code */
            if (1 == value)
            {
                P0 |= 0x01 << pin;
            }
            else
            {
                P0 &= ~(0x01 << pin);
            }
            break;
        case GROUP_1:
            /* code */
            if (1 == value)
            {
                P1 |= 0x01 << pin;
            }
            else
            {
                P1 &= ~(0x01 << pin);
            }
            break;
        case GROUP_2:
            /* code */
            if (1 == value)
            {
                P2 |= 0x01 << pin;
            }
            else
            {
                P2 &= ~(0x01 << pin);
            }
            break;
        case GROUP_3:
            /* code */
            if (1 == value)
            {
                P3 |= 0x01 << pin;
            }
            else
            {
                P3 &= ~(0x01 << pin);
            }
            break;

        default:
            break;
    }
}

/************************ (C) COPYRIGHT Tuu ********END OF FILE****************/


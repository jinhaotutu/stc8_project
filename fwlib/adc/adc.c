/**
  *****************************************************************************
  * @file    : adc.c
  * @author  : Tuu
  * @version : 1.0.0
  * @date    : 2021-03-13
  * @brief   : stc8 adc api
  ******************************************************************************
  * @lasteditors  : Tuu
  * @lasteditTime : 2021-03-13
  ******************************************************************************
  * @atten   : Copyright (C) by Tuu Inc
  *
  *****************************************************************************
  */

/* Includes -------------------------------------------------------------------*/
#include "adc.h"

#include "STC8HX.h"

/* Defines --------------------------------------------------------------------*/
#define ADC_TRANS_CNT   16
#define ADC_STANDARD    119

/* Variables ------------------------------------------------------------------*/


/* Functions ------------------------------------------------------------------*/

static u16 adc_trans(u8 channel)
{
    ADC_RES = 0;
    ADC_RESL = 0;

    ADC_CONTR = (ADC_CONTR & 0xF0) | 0x40 | channel;    //启动 AD 转换
    _nop_();
    _nop_();
    _nop_();
    _nop_();

    while((ADC_CONTR & 0x20) == 0)  ;   //wait for ADC finish
    ADC_CONTR &= ~0x20;     //清除ADC结束标志
    return  (((u16)ADC_RES << 8) | ADC_RESL);
}

void adc_hw_init(ADC_ID id)
{
    if (id > ADC_4)
    {
        return;
    }

    P1M1 = 0x01 << id;
    P1M0 = 0;       //设置 P1.4 为 ADC 输入口

    P_SW2 |= 0x80;
    ADCTIM = 0x3f;      //设置 ADC 内部时序，ADC采样时间建议设最大值
    P_SW2 &= 0x7f;
    ADCCFG = 0x2f;      //设置 ADC 时钟为系统时钟/2/16
    ADC_CONTR = 0x80;   //使能 ADC 模块
}

uint16_t get_adc_result(ADC_ID id)
{
    uint16_t i = 0, j = 0, Bandgap = 0;

    if (id > ADC_4)
    {
        return 0;
    }

    adc_trans(15);  //先读一次并丢弃结果, 让内部的采样电容的电压等于输入值.
    for(j=0, i=0; i<ADC_TRANS_CNT; i++)
    {
        j += adc_trans(15); //读内部基准ADC, 读15通道
    }
    Bandgap = j/ADC_TRANS_CNT;   //16次平均

    for(j=0, i=0; i<ADC_TRANS_CNT; i++)
    {
        j += adc_trans(id); //读外部电压ADC
    }
    j = j/ADC_TRANS_CNT;     //16次平均

    j = (u16)((u32)j * ADC_STANDARD / Bandgap);  //计算外部电压, Bandgap为1.19V, 测电压分辨率0.01V

    return j;
}

/************************ (C) COPYRIGHT Tuu ********END OF FILE****************/


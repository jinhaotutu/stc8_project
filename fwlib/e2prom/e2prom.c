/**
  *****************************************************************************
  * @file    : e2prom.c
  * @author  : Tuu
  * @version : 1.0.0
  * @date    : 2021-03-13
  * @brief   : stc8 e2prom api
  ******************************************************************************
  * @lasteditors  : Tuu
  * @lasteditTime : 2021-03-13
  ******************************************************************************
  * @atten   : Copyright (C) by Tuu Inc
  *
  *****************************************************************************
  */

/* Includes -------------------------------------------------------------------*/
#include "e2prom.h"

#include "STC8HX.h"
#include "intrins.h"

/* Defines --------------------------------------------------------------------*/
#define     IAP_STANDBY()   IAP_CMD = 0     //IAP空闲命令（禁止）
#define     IAP_READ()      IAP_CMD = 1     //IAP读出命令
#define     IAP_WRITE()     IAP_CMD = 2     //IAP写入命令
#define     IAP_ERASE()     IAP_CMD = 3     //IAP擦除命令

#define     IAP_ENABLE()    IAP_CONTR = IAP_EN; IAP_TPS = (MAIN_Fosc / 1000000)
#define     IAP_DISABLE()   IAP_CONTR = 0; IAP_CMD = 0; IAP_TRIG = 0; IAP_ADDRH = 0xff; IAP_ADDRL = 0xff

#define     IAP_EN          (1<<7)
#define     IAP_SWBS        (1<<6)
#define     IAP_SWRST       (1<<5)
#define     IAP_CMD_FAIL    (1<<4)

/* Variables ------------------------------------------------------------------*/


/* Functions ------------------------------------------------------------------*/

//========================================================================
// 函数: void DisableEEPROM(void)
// 描述: 禁止EEPROM.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2014-6-30
//========================================================================
static void DisableEEPROM(void)        //禁止访问EEPROM
{
    IAP_CONTR = 0;          //关闭 IAP 功能
    IAP_CMD = 0;            //清除命令寄存器
    IAP_TRIG = 0;           //清除触发寄存器
    IAP_ADDRH = 0xff;       //将地址设置到非 IAP 区域
    IAP_ADDRL = 0xff;
}

//========================================================================
// 函数: void EEPROM_Trig(void)
// 描述: 触发EEPROM操作.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2014-6-30
//========================================================================
static void EEPROM_Trig(void)
{
    F0 = EA;    //保存全局中断
    EA = 0;     //禁止中断, 避免触发命令无效
    IAP_TRIG = 0x5A;
    IAP_TRIG = 0xA5;                    //先送5AH，再送A5H到IAP触发寄存器，每次都需要如此
                                        //送完A5H后，IAP命令立即被触发启动
                                        //CPU等待IAP完成后，才会继续执行程序。
    _nop_();
    _nop_();
    _nop_();
    EA = F0;    //恢复全局中断
}

//========================================================================
// 函数: void EEPROM_SectorErase(u16 EE_address)
// 描述: 擦除一个扇区.
// 参数: EE_address:  要擦除的EEPROM的扇区中的一个字节地址.
// 返回: none.
// 版本: V1.0, 2014-6-30
//========================================================================
static void EEPROM_SectorErase(u16 EE_address)
{
    IAP_ENABLE();                       //设置等待时间，允许IAP操作，送一次就够
    IAP_ERASE();                        //宏调用, 送扇区擦除命令，命令不需改变时，不需重新送命令
                                        //只有扇区擦除，没有字节擦除，512字节/扇区。
                                        //扇区中任意一个字节地址都是扇区地址。
    IAP_ADDRH = EE_address / 256;       //送扇区地址高字节（地址需要改变时才需重新送地址）
    IAP_ADDRL = EE_address % 256;       //送扇区地址低字节
    EEPROM_Trig();                      //触发EEPROM操作
    DisableEEPROM();                    //禁止EEPROM操作
}

//========================================================================
// 函数: void EEPROM_read_n(u16 EE_address,u8 *DataAddress,u8 lenth)
// 描述: 读N个字节函数.
// 参数: EE_address:  要读出的EEPROM的首地址.
//       DataAddress: 要读出数据的指针.
//       length:      要读出的长度
// 返回: 0: 写入正确.  1: 写入长度为0错误.  2: 写入数据错误.
// 版本: V1.0, 2014-6-30
//========================================================================
static void EEPROM_read_n(u16 EE_address,u8 *DataAddress,u8 length)
{
    IAP_ENABLE();                           //设置等待时间，允许IAP操作，送一次就够
    IAP_READ();                             //送字节读命令，命令不需改变时，不需重新送命令
    do
    {
        IAP_ADDRH = EE_address / 256;       //送地址高字节（地址需要改变时才需重新送地址）
        IAP_ADDRL = EE_address % 256;       //送地址低字节
        EEPROM_Trig();                      //触发EEPROM操作
        *DataAddress = IAP_DATA;            //读出的数据送往
        EE_address++;
        DataAddress++;
    }while(--length);

    DisableEEPROM();
}


//========================================================================
// 函数: u8 EEPROM_write_n(u16 EE_address,u8 *DataAddress,u8 length)
// 描述: 写N个字节函数.
// 参数: EE_address:  要写入的EEPROM的首地址.
//       DataAddress: 要写入数据的指针.
//       length:      要写入的长度
// 返回: 0: 写入正确.  1: 写入长度为0错误.  2: 写入数据错误.
// 版本: V1.0, 2014-6-30
//========================================================================
static u8 EEPROM_write_n(u16 EE_address,u8 *DataAddress,u8 length)
{
    static u8  i;
    static u16 j;
    static u8  *p;

    if(length == 0) return 1;   //长度为0错误

    IAP_ENABLE();                       //设置等待时间，允许IAP操作，送一次就够
    i = length;
    j = EE_address;
    p = DataAddress;
    IAP_WRITE();                            //宏调用, 送字节写命令
    do
    {
        IAP_ADDRH = EE_address / 256;       //送地址高字节（地址需要改变时才需重新送地址）
        IAP_ADDRL = EE_address % 256;       //送地址低字节
        IAP_DATA  = *DataAddress;           //送数据到IAP_DATA，只有数据改变时才需重新送
        EEPROM_Trig();                      //触发EEPROM操作
        EE_address++;                       //下一个地址
        DataAddress++;                      //下一个数据
    }while(--length);                       //直到结束

    EE_address = j;
    length = i;
    DataAddress = p;
    i = 0;
    IAP_READ();                             //读N个字节并比较
    do
    {
        IAP_ADDRH = EE_address / 256;       //送地址高字节（地址需要改变时才需重新送地址）
        IAP_ADDRL = EE_address % 256;       //送地址低字节
        EEPROM_Trig();                      //触发EEPROM操作
        if(*DataAddress != IAP_DATA)        //读出的数据与源数据比较
        {
            i = 2;
            break;
        }
        EE_address++;
        DataAddress++;
    }while(--length);

    DisableEEPROM();
    return i;
}

void e2prom_hw_init(void)
{

}

void e2prom_read(u16 addr, u8 *buf, u16 len)
{
    EEPROM_read_n(addr, buf, len);
}

u8 e2prom_write(u16 addr, u8 *buf, u16 len)
{
    if (len == 0 || len > 512)
    {
        return 1;
    }

    EEPROM_SectorErase(addr);

    return EEPROM_write_n(addr, buf, len);
}


/************************ (C) COPYRIGHT Tuu ********END OF FILE****************/


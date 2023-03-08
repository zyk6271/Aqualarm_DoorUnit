/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-02-16     Rick       the first version
 */

#include "rtthread.h"
#include "main.h"
#include "radio.h"
#include "radio_app.h"
#include "radio_encoder.h"

#define DBG_TAG "RF_API"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>


void RF_Request_Main_Learn(void)
{
    RadioEnqueue(Storage_Main_Read(),0,3,3);
}
void RF_Learn_Request(void)
{
    RadioEnqueue(99999999,0,3,1);
}
void RF_Learn_Confirm(uint32_t ID)
{
    RadioEnqueue(ID,0,3,2);
}
void RF_Open_Valve(void)
{
    RadioEnqueue(Storage_Main_Read(),0,5,0);
}
void RF_Close_Valve(void)
{
    RadioEnqueue(Storage_Main_Read(),0,6,0);
}
void RF_Delay_Open(void)
{
    RadioEnqueue(Storage_Main_Read(),0,8,1);
}
void RF_Delay_Close(void)
{
    RadioEnqueue(Storage_Main_Read(),0,8,0);
}
void RF_HeartWithMain(void)
{
    RadioEnqueue(Storage_Main_Read(),0,2,Get_BAT_Level());
}

/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-11-22     Rick       the first version
 */
#include <rtthread.h>
#include <rtdevice.h>
#include <stdio.h>
#include "radio_encoder.h"
#include "radio_app.h"

#define DBG_TAG "RADIO_ENCODER"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

rt_mq_t rf_en_mq;
rt_thread_t rf_encode_t = RT_NULL;

uint32_t Self_Id = 0;
uint32_t Self_Default_Id = 30000002;

char radio_send_buf[255];

typedef struct
{
    uint32_t Taget_Id;
    uint8_t Counter;
    uint8_t Command;
    uint8_t Data;
}Radio_Normal_Format;

void RadioEnqueue(uint32_t Taget_Id, uint8_t Counter, uint8_t Command, uint8_t Data)
{
    Radio_Normal_Format Send_Buf;
    Send_Buf.Taget_Id = Taget_Id;
    Send_Buf.Counter = Counter;
    Send_Buf.Command = Command;
    Send_Buf.Data = Data;

    rt_mq_send(rf_en_mq, &Send_Buf, sizeof(Radio_Normal_Format));
    rt_pm_module_delay_sleep(PM_RF_ID, 5000);
}

void SendPrepare(Radio_Normal_Format Send)
{
    rt_memset(radio_send_buf, 0, sizeof(radio_send_buf));
    uint8_t check = 0;
    Send.Counter++ <= 255 ? Send.Counter : 0;
    rt_sprintf(radio_send_buf, "{%08ld,%08ld,%03d,%02d,%d}", Send.Taget_Id, Self_Id, Send.Counter, Send.Command, Send.Data);
    for (uint8_t i = 0; i < 28; i++)
    {
        check += radio_send_buf[i];
    }
    radio_send_buf[28] = ((check >> 4) < 10) ? (check >> 4) + '0' : (check >> 4) - 10 + 'A';
    radio_send_buf[29] = ((check & 0xf) < 10) ? (check & 0xf) + '0' : (check & 0xf) - 10 + 'A';
    radio_send_buf[30] = '\r';
    radio_send_buf[31] = '\n';
}

void Reponse_Control(uint8_t command)
{
    switch(command)
    {
    case 2://heart
        Start_Heart_Timer();
        break;
    default:break;
    }
}

void rf_encode_entry(void *paramaeter)
{
    Radio_Normal_Format Send_Data;
    while (1)
    {
        if (rt_mq_recv(rf_en_mq,&Send_Data, sizeof(Radio_Normal_Format), RT_WAITING_FOREVER) == RT_EOK)
        {
            rt_pm_module_delay_sleep(PM_RF_ID, 5000);
            SendPrepare(Send_Data);
            Reponse_Control(Send_Data.Command);
            rt_thread_mdelay(100);
            RF_Send(radio_send_buf, rt_strlen(radio_send_buf));
            rt_thread_mdelay(200);
        }
    }
}

void RadioQueue_Init(void)
{
    int *p;
    p=(int *)(0x0803FFF0);//这就是已知的地址，要强制类型转换
    Self_Id = *p;//从Flash加载ID
    if (Self_Id == 0xFFFFFFFF || Self_Id == 0)
    {
        Self_Id = Self_Default_Id;
    }
    LOG_I("Self ID is %d\r\n", Self_Id);

    rf_en_mq = rt_mq_create("rf_en_mq", sizeof(Radio_Normal_Format), 10, RT_IPC_FLAG_PRIO);
    rf_encode_t = rt_thread_create("radio_send", rf_encode_entry, RT_NULL, 1024, 9, 10);
    if (rf_encode_t)rt_thread_startup(rf_encode_t);
}
uint8_t Get_Factory_Self_ID(void)
{
    if(Self_Id == 30000000)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

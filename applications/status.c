/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-11-27     Rick       the first version
 */
#include <rtthread.h>
#include <rtdevice.h>
#include "board.h"
#include <stdint.h>
#include "pin_config.h"
#include "key.h"
#include "led.h"
#include "status.h"
#include "radio_encoder.h"

#define DBG_TAG "status"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

enum Warning_Type Warning_Status;

WariningEvent NowStatusEvent;
WariningEvent OfflineEvent;
WariningEvent LowPowerEvent;
WariningEvent UltraLowPowerEvent;

uint8_t Get_Warning_Status(void)
{
    return Warning_Status;
}
void Warning_Enable(WariningEvent event)
{
    if(event.priority >= NowStatusEvent.priority)
    {
        NowStatusEvent.last_id = event.warning_id;
        NowStatusEvent.priority = event.priority;
        if(event.callback!=RT_NULL)
        {
            NowStatusEvent.callback = event.callback;
            NowStatusEvent.callback(RT_NULL);
        }
        LOG_D("Warning_Enable Success,warning id is %d,now priority is %d\r\n",event.warning_id,event.priority);
    }
    else
    {
        LOG_D("Warning_Enable Fail last is %d Now is %d\r\n",NowStatusEvent.priority,event.priority);
    }
}
void Warning_Active_Num(uint8_t id)
{
    switch(id)
    {
    case 1:Warning_Enable(OfflineEvent);break;
    case 2:Warning_Enable(LowPowerEvent);break;
    case 3:Warning_Enable(UltraLowPowerEvent);break;
    }
}
void WarningEventInit(uint8_t warning_id,uint8_t priority,WariningEvent *event,void (*callback)(void*))
{
    rt_memset(event,0,sizeof(&event));
    event->warning_id = warning_id;
    event->last_id = 0;
    event->priority = priority;
    event->callback = callback;
}
void Warning_Disable(enum Warning_Type type)
{
    if(Warning_Status <= type)
    {
        Warning_Status = Normal;
        NowStatusEvent.last_id = 0;
        NowStatusEvent.priority = 0;
        Led_Alarm_Disable();
        LOG_I("Warning is Disable\r\n");
    }
    else
    {
        LOG_I("Warning Disable fail,now is %d,request is %d\r\n",Warning_Status,type);
    }
}

void OfflineEvent_Callback(void *parameter)
{
    Warning_Status = Offline;
    Led_Alarm_Enable(4,15);
    LOG_I("OfflineEvent_Callback\r\n");
}
void LowPowerEvent_Callback(void *parameter)
{
    Warning_Status = LowPower;
    Led_Alarm_Enable(1,30);
    RF_HeartWithMain();
    LOG_I("LowPowerEvent_Callback\r\n");
}
void UltraLowPowerEvent_Callback(void *parameter)
{
    Warning_Status = UltraLowPower;
    Led_Alarm_Enable(1,15);
    RF_HeartWithMain();
    LOG_I("UltraLowPowerEvent_Callback\r\n");
}
void WarningInit(void)
{
    WarningEventInit(0,0,&NowStatusEvent,RT_NULL);//本地存储器
    WarningEventInit(1,1,&LowPowerEvent,LowPowerEvent_Callback);
    WarningEventInit(2,2,&UltraLowPowerEvent,UltraLowPowerEvent_Callback);
    WarningEventInit(3,3,&OfflineEvent,OfflineEvent_Callback);
}

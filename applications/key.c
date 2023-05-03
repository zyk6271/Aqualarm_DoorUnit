/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-11-25     Rick       the first version
 */
#include <rtthread.h>
#include <stdint.h>
#include "pin_config.h"
#include "key.h"
#include "button.h"
#include "status.h"

#define DBG_TAG "key"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

rt_thread_t button_t = RT_NULL;
rt_thread_t key_response_t = RT_NULL;

Button_t KEY_OFF;
Button_t KEY_ON;
Button_t KEY_DELAY;

rt_event_t Key_Event;

uint8_t KEY_OFF_Long_Counter,KEY_OFF_OnceFlag;
uint8_t KEY_ON_Long_Counter,KEY_ON_OnceFlag;
uint8_t KEY_DELAY_Long_Counter,KEY_DELAY_OnceFlag;

#define KEY_OFF_EVENT           1<<0
#define KEY_OFF_LONG_EVENT      1<<1
#define KEY_ON_EVENT           1<<2
#define KEY_ON_LONG_EVENT      1<<3
#define KEY_DELAY_EVENT           1<<4
#define KEY_DELAY_LONG_EVENT      1<<5

uint8_t Read_KEY_OFF_Level(void)
{
    return rt_pin_read(KEY_OFF_PIN);
}
uint8_t Read_KEY_ON_Level(void)
{
    return rt_pin_read(KEY_ON_PIN);
}
uint8_t Read_KEY_DELAY_Level(void)
{
    return rt_pin_read(KEY_DELAY_PIN);
}
void KEY_OFF_Press(void *parameter) //off
{
    rt_pm_module_delay_sleep(PM_BUTTON_ID, 3000);
    LOG_D("KEY_OFF_Press\r\n");
    rt_event_send(Key_Event,KEY_OFF_EVENT);
}

void KEY_OFF_Long(void *parameter) //off
{
    rt_pm_module_delay_sleep(PM_BUTTON_ID, 3000);
    if (KEY_OFF_OnceFlag == 0)
    {
        if (KEY_OFF_Long_Counter > 3)
        {
            KEY_OFF_OnceFlag = 1;
            rt_event_send(Key_Event,KEY_OFF_LONG_EVENT);
            LOG_D("KEY_OFF is Long\r\n");
        }
        else
        {
            KEY_OFF_Long_Counter++;
        }
    }
}
void KEY_OFF_Long_Free(void *parameter) //off
{
    rt_pm_module_delay_sleep(PM_BUTTON_ID, 3000);
    KEY_OFF_OnceFlag = 0;
    KEY_OFF_Long_Counter = 0;
    LOG_D("KEY_OFF_Long_Free\r\n");
}

void KEY_ON_Press(void *parameter) //off
{
    rt_pm_module_delay_sleep(PM_BUTTON_ID, 3000);
    LOG_D("KEY_ON_Press\r\n");
    rt_event_send(Key_Event,KEY_ON_EVENT);
}

void KEY_ON_Long(void *parameter) //off
{
    rt_pm_module_delay_sleep(PM_BUTTON_ID, 3000);
    if (KEY_ON_OnceFlag == 0)
    {
        if (KEY_ON_Long_Counter > 3)
        {
            KEY_ON_OnceFlag = 1;
            rt_event_send(Key_Event,KEY_ON_LONG_EVENT);
            LOG_D("KEY_ON is Long\r\n");
        }
        else
        {
            KEY_ON_Long_Counter++;
        }
    }
}
void KEY_ON_Long_Free(void *parameter) //off
{
    rt_pm_module_delay_sleep(PM_BUTTON_ID, 3000);
    KEY_ON_OnceFlag = 0;
    KEY_ON_Long_Counter = 0;
    LOG_D("KEY_ON_Long_Free\r\n");
}

void KEY_DELAY_Press(void *parameter) //off
{
    rt_pm_module_delay_sleep(PM_BUTTON_ID, 3000);
    LOG_D("KEY_DELAY_Press\r\n");
    rt_event_send(Key_Event,KEY_DELAY_EVENT);
}

void KEY_DELAY_Long(void *parameter) //off
{
    rt_pm_module_delay_sleep(PM_BUTTON_ID, 3000);
    if (KEY_DELAY_OnceFlag == 0)
    {
        if (KEY_DELAY_Long_Counter > 3)
        {
            KEY_DELAY_OnceFlag = 1;
            rt_event_send(Key_Event,KEY_DELAY_LONG_EVENT);
            LOG_D("KEY_DELAY is Long\r\n");
        }
        else
        {
            KEY_DELAY_Long_Counter++;
        }
    }
}
void KEY_DELAY_Long_Free(void *parameter) //off
{
    rt_pm_module_delay_sleep(PM_BUTTON_ID, 3000);
    KEY_DELAY_OnceFlag = 0;
    KEY_DELAY_Long_Counter = 0;
    LOG_D("KEY_DELAY_Long_Free\r\n");
}

void button_task_entry(void *parameter)
{
    rt_pin_mode(KEY_OFF_PIN, PIN_MODE_INPUT);
    rt_pin_mode(KEY_ON_PIN, PIN_MODE_INPUT);
    rt_pin_mode(KEY_DELAY_PIN, PIN_MODE_INPUT);
    Button_Create("KEY_OFF", &KEY_OFF, Read_KEY_OFF_Level, 0);
    Button_Attach(&KEY_OFF, BUTTON_DOWM, KEY_OFF_Press);
    Button_Attach(&KEY_OFF, BUTTON_LONG, KEY_OFF_Long);
    Button_Attach(&KEY_OFF, BUTTON_LONG_FREE, KEY_OFF_Long_Free);
    Button_Create("KEY_ON", &KEY_ON, Read_KEY_ON_Level, 0);
    Button_Attach(&KEY_ON, BUTTON_DOWM, KEY_ON_Press);
    Button_Attach(&KEY_ON, BUTTON_LONG, KEY_ON_Long);
    Button_Attach(&KEY_ON, BUTTON_LONG_FREE, KEY_ON_Long_Free);
    Button_Create("KEY_DELAY", &KEY_DELAY, Read_KEY_DELAY_Level, 0);
    Button_Attach(&KEY_DELAY, BUTTON_DOWM, KEY_DELAY_Press);
    Button_Attach(&KEY_DELAY, BUTTON_LONG, KEY_DELAY_Long);
    Button_Attach(&KEY_DELAY, BUTTON_LONG_FREE, KEY_DELAY_Long_Free);
    while (1)
    {
        Button_Process();
        rt_thread_mdelay(10);
    }
}
void Key_Reponse_Callback(void *parameter)
{
    rt_uint32_t ret;
    while(1)
    {
        if (rt_event_recv(Key_Event, KEY_OFF_EVENT | KEY_OFF_LONG_EVENT | KEY_ON_EVENT | KEY_ON_LONG_EVENT \
                | KEY_DELAY_EVENT | KEY_DELAY_LONG_EVENT,RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,RT_WAITING_FOREVER, &ret) == RT_EOK)
        {
            rt_pm_module_delay_sleep(PM_BUTTON_ID, 3000);
            if(ret & KEY_OFF_EVENT)//OFF
            {
                Led_KeyOff();
                RF_Close_Valve();
            }
            else if(ret & KEY_OFF_LONG_EVENT)//OFF LONG
            {
                RF_Learn_Request();
            }
            else if(ret & KEY_ON_EVENT)//ON
            {
                Led_KeyOn();
                RF_Open_Valve();
            }
            else if(ret & KEY_ON_LONG_EVENT)//ON LONG
            {
                led_bat_show();
            }
            else if(ret & KEY_DELAY_EVENT)//DELAY
            {
                Led_DelayOff();
                RF_Delay_Close();
            }
            else if(ret & KEY_DELAY_LONG_EVENT)//DELAY LONG
            {
                led_rssi_show();
            }
        }
    }
}
void Button_Init(void)
{
    Key_Event = rt_event_create("Key_Event", RT_IPC_FLAG_PRIO);
    button_t = rt_thread_create("button_t", button_task_entry, RT_NULL, 256, 10, 20);
    if (button_t != RT_NULL)
    {
        rt_thread_startup(button_t);
    }
    key_response_t = rt_thread_create("key_response_t", Key_Reponse_Callback, RT_NULL, 2048, 10, 10);
    if(key_response_t!=RT_NULL)
    {
        rt_thread_startup(key_response_t);
    }

}

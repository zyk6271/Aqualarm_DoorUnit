#include <rtthread.h>
#include <rtdevice.h>
#include "pin_config.h"
#include "rtc.h"
#include "board.h"

#define DBG_TAG "RTC"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

rt_sem_t RTC_IRQ_Sem;
rt_thread_t RTC_Scan = RT_NULL;

uint8_t RTC_Counter;

uint8_t heart_count;
struct rt_lptimer heart_timer;
struct rt_lptimer rtc_timer;
struct rt_lptimer power_heart_timer;

void heart_timer_callback(void *parameter)
{
    LOG_D("Heart Retry Count is %d\r\n",heart_count);
    if(heart_count++ < 10)
    {
        RF_HeartWithMain();
    }
    else
    {
        Warning_Active_Num(1);
    }
}

void rtc_timer_callback(void *parameter)
{
    rt_pm_sleep_request(PM_RTC_ID, PM_SLEEP_MODE_NONE);
    rt_sem_release(RTC_IRQ_Sem);
}

void Start_Heart_Timer(void)
{
    LOG_I("Start Watting Heart Response\r\n");
    rt_lptimer_start(&heart_timer);
}
void Stop_Heart_Timer(void)
{
    LOG_D("Stop_Heart_Timer\r\n");
    rt_lptimer_stop(&heart_timer);
}
void Period_Heart(void)
{
    LOG_I("Period_Heart\r\n");
    heart_count = 0;
    RF_HeartWithMain();
}
void RTC_Timer_Entry(void *parameter)
{
    while(1)
    {
        static rt_err_t result;
        result = rt_sem_take(RTC_IRQ_Sem, RT_WAITING_FOREVER);
        if (result == RT_EOK)
        {
            if(RTC_Counter<23)
            {
                RTC_Counter++;
            }
            else
            {
                RTC_Counter=0;
                Period_Heart();
            }
            rt_pm_sleep_release(PM_RTC_ID, PM_SLEEP_MODE_NONE);
        }
    }
}
void power_heart_timer_callback(void *parameter)
{
    Period_Heart();
}
void RTC_Init(void)
{
    RTC_IRQ_Sem = rt_sem_create("RTC_IRQ", 0, RT_IPC_FLAG_FIFO);
    rt_lptimer_init(&heart_timer, "heart_timer", heart_timer_callback, RT_NULL,5*60*1000, RT_TIMER_FLAG_ONE_SHOT | RT_TIMER_FLAG_SOFT_TIMER);
    rt_lptimer_init(&rtc_timer, "rtc_timer", rtc_timer_callback, RT_NULL,60*60*1000, RT_TIMER_FLAG_PERIODIC | RT_TIMER_FLAG_SOFT_TIMER);
    rt_lptimer_init(&power_heart_timer, "power_heart_timer", power_heart_timer_callback, RT_NULL,30*1000, RT_TIMER_FLAG_ONE_SHOT | RT_TIMER_FLAG_SOFT_TIMER);
    rt_lptimer_start(&rtc_timer);
    rt_lptimer_start(&power_heart_timer);

    RTC_Scan = rt_thread_create("RTC_Scan", RTC_Timer_Entry, RT_NULL, 2048, 10, 10);
    if(RTC_Scan!=RT_NULL)
    {
        rt_thread_startup(RTC_Scan);
    }
    else
    {
        LOG_W("RTC Init Fail\r\n");
    }


}

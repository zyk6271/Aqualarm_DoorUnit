/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-05-06     Zero-Free    first version
 */
#include <rtthread.h>
#include <board.h>
#include "stm32wlxx_hal_pwr_ex.h"

#ifdef RT_USING_PM

//#define DRV_DEBUG
#define LOG_TAG                "drv.pm"
#include <drv_log.h>

void uart_reconfig(void)
{
    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;

    rt_device_control(rt_console_get_device(), RT_DEVICE_CTRL_CONFIG, &config);
}
void msh_reconfig(void)
{
    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;

    rt_device_control(rt_console_get_device(), RT_DEVICE_CTRL_SET_INT, RT_NULL);
}

/**
 * This function will put STM32WLxx into sleep mode.
 *
 * @param pm pointer to power manage structure
 */
static void sleep(struct rt_pm *pm, uint8_t mode)
{
    switch (mode)
    {
    case PM_SLEEP_MODE_NONE:

        break;

    case PM_SLEEP_MODE_IDLE:
        break;

    case PM_SLEEP_MODE_LIGHT:
        if (pm->run_mode == PM_RUN_MODE_LOW_SPEED)
        {
            /* Enter LP SLEEP Mode, Enable low-power regulator */
            HAL_PWR_EnterSLEEPMode(PWR_LOWPOWERREGULATOR_ON, PWR_SLEEPENTRY_WFI);
        }
        else
        {
            /* Enter SLEEP Mode, Main regulator is ON */
            HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
        }
        break;

    case PM_SLEEP_MODE_DEEP:
        /* Request of CPU to enter in Stop2 mode */
        HAL_PWREx_EnterSTOP2Mode(PWR_STOPENTRY_WFI);
        /* ReConfig System Main Clock  */
        TCXO_Clock_Config();
        break;

    case PM_SLEEP_MODE_STANDBY:
        /* Enter STANDBY mode */
        HAL_PWR_EnterSTANDBYMode();
        break;

    case PM_SLEEP_MODE_SHUTDOWN:
        /* Enter SHUTDOWNN mode */
        HAL_PWREx_EnterSHUTDOWNMode();
        break;

    default:
        RT_ASSERT(0);
        break;
    }

}

/**
 * This function caculate the PM tick from OS tick
 *
 * @param tick OS tick
 *
 * @return the PM tick
 */
static rt_tick_t stm32wl_pm_tick_from_os_tick(rt_tick_t tick)
{
    rt_uint32_t freq = stm32wl_lptim_get_countfreq();

    return (freq * tick / RT_TICK_PER_SECOND);
}

/**
 * This function caculate the OS tick from PM tick
 *
 * @param tick PM tick
 *
 * @return the OS tick
 */
static rt_tick_t stm32wl_os_tick_from_pm_tick(rt_uint32_t tick)
{
    static rt_uint32_t os_tick_remain = 0;
    rt_uint32_t ret, freq;

    freq = stm32wl_lptim_get_countfreq();
    ret = (tick * RT_TICK_PER_SECOND + os_tick_remain) / freq;

    os_tick_remain += (tick * RT_TICK_PER_SECOND);
    os_tick_remain %= freq;

    return ret;
}

/**
 * This function start the timer of pm
 *
 * @param pm Pointer to power manage structure
 * @param timeout How many OS Ticks that MCU can sleep
 */
static void pm_timer_start(struct rt_pm *pm, rt_uint32_t timeout)
{
    RT_ASSERT(pm != RT_NULL);
    RT_ASSERT(timeout > 0);

    if (timeout != RT_TICK_MAX)
    {
        /* Convert OS Tick to pmtimer timeout value */
        timeout = stm32wl_pm_tick_from_os_tick(timeout);
        if (timeout > stm32wl_lptim_get_tick_max())
        {
            timeout = stm32wl_lptim_get_tick_max();
        }
        /* Enter PM_TIMER_MODE */
        stm32wl_lptim_start(timeout);
    }
}

/**
 * This function stop the timer of pm
 *
 * @param pm Pointer to power manage structure
 */
static void pm_timer_stop(struct rt_pm *pm)
{
    RT_ASSERT(pm != RT_NULL);

    /* Reset pmtimer status */
    stm32wl_lptim_stop();
}

/**
 * This function calculate how many OS Ticks that MCU have suspended
 *
 * @param pm Pointer to power manage structure
 *
 * @return OS Ticks
 */
static rt_tick_t pm_timer_get_tick(struct rt_pm *pm)
{
    rt_uint32_t timer_tick;

    RT_ASSERT(pm != RT_NULL);

    timer_tick = stm32wl_lptim_get_current_tick();

    return stm32wl_os_tick_from_pm_tick(timer_tick);
}

/**
 * This function initialize the power manager
 */
int drv_pm_hw_init(void)
{
    static const struct rt_pm_ops _ops =
    {
        sleep,
        RT_NULL,
        pm_timer_start,
        pm_timer_stop,
        pm_timer_get_tick
    };

    rt_uint8_t timer_mask = 0;

    /* initialize timer mask */
    timer_mask = 1UL << PM_SLEEP_MODE_DEEP;

    /* initialize system pm module */
    rt_system_pm_init(&_ops, timer_mask, RT_NULL);

    return 0;
}
INIT_BOARD_EXPORT(drv_pm_hw_init);

#endif

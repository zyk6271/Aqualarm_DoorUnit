/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author          Notes
 * 2019-05-06     Zero-Free       first version
 */

#include <board.h>

static LPTIM_HandleTypeDef LptimHandle;
/**
 * This function get current count value of LPTIM
 *
 * @return the count vlaue
 */
rt_uint32_t stm32wl_lptim_get_current_tick(void)
{
    return HAL_LPTIM_ReadCounter(&LptimHandle);
}

void LPTIM1_IRQHandler(void)
{
    HAL_LPTIM_IRQHandler(&LptimHandle);
}

void HAL_LPTIM_CompareMatchCallback(LPTIM_HandleTypeDef *hlptim)
{
    /* enter interrupt */
    rt_interrupt_enter();

    /* leave interrupt */
    rt_interrupt_leave();
}
/**
 * This function get the max value that LPTIM can count
 *
 * @return the max count
 */
rt_uint32_t stm32wl_lptim_get_tick_max(void)
{
    return (1024*60);
}

/**
 * This function start LPTIM with reload value
 *
 * @param reload The value that LPTIM count down from
 *
 * @return RT_EOK
 */
rt_err_t stm32wl_lptim_start(rt_uint32_t reload)
{
    HAL_LPTIM_TimeOut_Start_IT(&LptimHandle, 0xFFFF, reload);

    return (RT_EOK);
}

/**
 * This function stop LPTIM
 */
void stm32wl_lptim_stop(void)
{
    rt_uint32_t _ier;

    _ier = LptimHandle.Instance->IER;
    LptimHandle.Instance->ICR = LptimHandle.Instance->ISR & _ier;
}

/**
 * This function get the count clock of LPTIM
 *
 * @return the count clock frequency in Hz
 */
rt_uint32_t stm32wl_lptim_get_countfreq(void)
{
    return 256;
}

/**
 * This function initialize the lptim
 */
int stm32wl_hw_lptim_init(void)
{
    LptimHandle.Instance = LPTIM1;
    LptimHandle.Init.Clock.Source = LPTIM_CLOCKSOURCE_APBCLOCK_LPOSC;
    LptimHandle.Init.Clock.Prescaler = LPTIM_PRESCALER_DIV128;
    LptimHandle.Init.Trigger.Source = LPTIM_TRIGSOURCE_SOFTWARE;
    LptimHandle.Init.OutputPolarity = LPTIM_OUTPUTPOLARITY_HIGH;
    LptimHandle.Init.UpdateMode = LPTIM_UPDATE_IMMEDIATE;
    LptimHandle.Init.CounterSource = LPTIM_COUNTERSOURCE_INTERNAL;
    LptimHandle.Init.Input1Source = LPTIM_INPUT1SOURCE_GPIO;
    LptimHandle.Init.Input2Source = LPTIM_INPUT2SOURCE_GPIO;
    if (HAL_LPTIM_Init(&LptimHandle) != HAL_OK)
    {
        return -1;
    }

    NVIC_ClearPendingIRQ(LPTIM1_IRQn);
    NVIC_SetPriority(LPTIM1_IRQn, 0);
    NVIC_EnableIRQ(LPTIM1_IRQn);

    return 0;
}

INIT_DEVICE_EXPORT(stm32wl_hw_lptim_init);

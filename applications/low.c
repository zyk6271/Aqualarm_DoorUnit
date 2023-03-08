/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-08-19     Rick       the first version
 */
#include <rtthread.h>
#include <board.h>
#include <stm32wlxx.h>
#include "pin_config.h"

#define DBG_TAG "low"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

uint8_t Button_Wakeup_Flag;

UART_HandleTypeDef huart2;

void Pin_DeInit(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_10  \
            | GPIO_PIN_11 | GPIO_PIN_12| GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 |  \
            GPIO_PIN_6| GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_12;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = GPIO_PIN_13;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}
void Pin_Init(void)
{
    RF_Switch_Init();

    rt_pin_mode(LED_BAT_GREEN_PIN,PIN_MODE_OUTPUT);
    //rt_pin_mode(LED_ON_RED_PIN,PIN_MODE_OUTPUT);
    rt_pin_mode(LED_ON_GREEN_PIN,PIN_MODE_OUTPUT);
    rt_pin_mode(LED_WARN_RED_PIN,PIN_MODE_OUTPUT);
    rt_pin_mode(LED_WARN_GREEN_PIN,PIN_MODE_OUTPUT);
    rt_pin_mode(LED_OFF_RED_PIN,PIN_MODE_OUTPUT);
    rt_pin_mode(LED_OFF_GREEN_PIN,PIN_MODE_OUTPUT);
    rt_pin_mode(LED_DELAY_RED_PIN,PIN_MODE_OUTPUT);
    rt_pin_mode(LED_DELAY_GREEN_PIN,PIN_MODE_OUTPUT);
    rt_pin_mode(LED_INTENSITY_GREEN_PIN,PIN_MODE_OUTPUT);
    rt_pin_mode(BUZZER_PIN,PIN_MODE_OUTPUT);
}
void Debug_DeInit(void)
{
    huart2.Instance = USART2;
    HAL_UART_MspDeInit(&huart2);
}
void Debug_Init(void)
{
    huart2.Instance = USART2;
    HAL_UART_MspInit(&huart2);
    uart_reconfig();
}

void button_wakeup(void *parameter)
{
    Button_Wakeup_Flag = 1;
}
void IRQ_Bind(void)
{
    rt_pin_attach_irq(KEY_OFF_PIN, PIN_IRQ_MODE_RISING_FALLING, button_wakeup, RT_NULL);
    rt_pin_attach_irq(KEY_ON_PIN, PIN_IRQ_MODE_RISING_FALLING, button_wakeup, RT_NULL);
    rt_pin_attach_irq(KEY_DELAY_PIN, PIN_IRQ_MODE_RISING_FALLING, button_wakeup, RT_NULL);
    rt_pin_irq_enable(KEY_OFF_PIN, PIN_IRQ_ENABLE);
    rt_pin_irq_enable(KEY_ON_PIN, PIN_IRQ_ENABLE);
    rt_pin_irq_enable(KEY_DELAY_PIN, PIN_IRQ_ENABLE);
}
void IRQ_Unbind(void)
{
    rt_pin_detach_irq(KEY_OFF_PIN);
    rt_pin_detach_irq(KEY_ON_PIN);
    rt_pin_detach_irq(KEY_DELAY_PIN);
    rt_pin_irq_enable(KEY_OFF_PIN, PIN_IRQ_DISABLE);
    rt_pin_irq_enable(KEY_ON_PIN, PIN_IRQ_DISABLE);
    rt_pin_irq_enable(KEY_DELAY_PIN, PIN_IRQ_DISABLE);
    rt_pin_mode(KEY_OFF_PIN,PIN_MODE_INPUT);
    rt_pin_mode(KEY_ON_PIN,PIN_MODE_INPUT);
    rt_pin_mode(KEY_DELAY_PIN,PIN_MODE_INPUT);
}
void BeforeSleep(void)
{
    //RF
    RF_Sleep();
    //DEBUG UART
    Debug_DeInit();
    //PIN
    Pin_DeInit();
    //KEY
    IRQ_Bind();
}
void AfterWake(void)
{
    //DEBUG UART
    Debug_Init();
    //KEY
    IRQ_Unbind();
    //PIN
    Pin_Init();
    //RF
    RF_Wake();
}
void user_notify(rt_uint8_t event, rt_uint8_t mode, void *data)
{
    if (event == RT_PM_ENTER_SLEEP)
    {
        Button_Wakeup_Flag=0;
        LOG_D("Go to Sleep\r\n");
        BeforeSleep();
    }
    else
    {
        AfterWake();
        if(Button_Wakeup_Flag)
        {
            rt_pm_module_delay_sleep(PM_BUTTON_ID,5000);
            Button_Wakeup_Flag = 0;
            LOG_D("Button Wake Up Now\r\n");
        }
    }
}
void Low_Init(void)
{
    rt_pm_notify_set(user_notify, RT_NULL);
    rt_pm_request(PM_SLEEP_MODE_DEEP);
    rt_pm_release(PM_SLEEP_MODE_NONE);
}

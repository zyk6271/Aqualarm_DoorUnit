/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-08-06     Rick       the first version
 */
#ifndef APPLICATIONS_PIN_CONFIG_H_
#define APPLICATIONS_PIN_CONFIG_H_
/*
 * RF
 */
#define RF_SW1_PIN                      6
#define RF_SW2_PIN                      7
#define TCXO_PWR_PIN                    16
/*
 * LED
 */
#define LED_BAT_GREEN_PIN               0
#define LED_ON_RED_PIN                  1
#define LED_ON_GREEN_PIN                28
#define LED_WARN_RED_PIN                10
#define LED_WARN_GREEN_PIN              11
#define LED_OFF_RED_PIN                 12
#define LED_OFF_GREEN_PIN               45
#define LED_DELAY_RED_PIN               15
#define LED_DELAY_GREEN_PIN             19
#define LED_INTENSITY_GREEN_PIN         20

#define BUZZER_PIN                      18
/*
 * KEY
 */
#define KEY_OFF_PIN                     5
#define KEY_ON_PIN                      8
#define KEY_DELAY_PIN                   4

#endif /* APPLICATIONS_PIN_CONFIG_H_ */

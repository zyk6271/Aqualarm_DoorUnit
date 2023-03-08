#include <agile_led.h>
#include "led.h"
#include "pin_config.h"

struct rt_lptimer alarm_timer;

static agile_led_t *alarm_led_red = RT_NULL;
static agile_led_t *alarm_beep = RT_NULL;

static agile_led_t *key_press_red = RT_NULL;
static agile_led_t *key_press_green = RT_NULL;

static agile_led_t *delay_green = RT_NULL;
static agile_led_t *delay_red = RT_NULL;

static agile_led_t *recv_beep = RT_NULL;

static agile_led_t *learn_success_beep = RT_NULL;
static agile_led_t *learn_success_green = RT_NULL;

static agile_led_t *power_led_red = RT_NULL;
static agile_led_t *power_led_green = RT_NULL;
static agile_led_t *power_beep = RT_NULL;

static agile_led_t *rssi_prompt_led = RT_NULL;
static agile_led_t *rssi_low_led = RT_NULL;
static agile_led_t *rssi_mid_led = RT_NULL;
static agile_led_t *rssi_high_led = RT_NULL;
static agile_led_t *rssi_ultra_led = RT_NULL;

static agile_led_t *bat_prompt_led = RT_NULL;
static agile_led_t *bat_low_led = RT_NULL;
static agile_led_t *bat_mid_led = RT_NULL;
static agile_led_t *bat_high_led = RT_NULL;
static agile_led_t *bat_ultra_led = RT_NULL;

void alarm_timer_callback(void *parameter)
{
    rt_pm_sleep_request(PM_LED_ID,PM_SLEEP_MODE_NONE);
    agile_led_start(alarm_led_red);
    agile_led_start(alarm_beep);
}
void alarm_led_callback(void *parameter)
{
    rt_lptimer_start(&alarm_timer);
    rt_pm_sleep_release(PM_LED_ID,PM_SLEEP_MODE_NONE);
}
void Led_Alarm_Enable(uint8_t count,uint8_t sec)
{
    uint32_t timer_tick = sec * 1000;
    rt_timer_control(&alarm_timer,RT_TIMER_CTRL_SET_TIME,&timer_tick);
    rt_pm_sleep_request(PM_LED_ID,PM_SLEEP_MODE_NONE);
    agile_led_dynamic_change_light_mode(alarm_led_red,"200,200", count);
    agile_led_dynamic_change_light_mode(alarm_beep,"200,200", count);
    agile_led_start(alarm_led_red);
    agile_led_start(alarm_beep);
}
void Led_Alarm_Disable(uint8_t count,uint8_t sec)
{
    rt_lptimer_stop(&alarm_timer);
    agile_led_stop(alarm_led_red);
    agile_led_stop(alarm_beep);
}
void Led_Alarm_DisableBeep(void)
{
    agile_led_stop(alarm_beep);
}
void Led_Beep_Powerup(void)
{
    rt_pm_module_delay_sleep(PM_LED_ID,3000);
    agile_led_start(power_beep);
}
void Led_Init(void)
{
    rt_lptimer_init(&alarm_timer, "alarm_timer", alarm_timer_callback, RT_NULL, 3000, RT_TIMER_FLAG_ONE_SHOT | RT_TIMER_FLAG_SOFT_TIMER);
    alarm_led_red = agile_led_create(LED_WARN_RED_PIN, 0, "200,200", 1);
    alarm_beep = agile_led_create(BUZZER_PIN, 1, "200,200", 1);
    agile_led_set_compelete_callback(alarm_led_red,alarm_led_callback);

    key_press_red = agile_led_create(LED_OFF_RED_PIN, 0, "200,1", 1);
    key_press_green = agile_led_create(LED_ON_GREEN_PIN, 0, "200,1", 1);

    delay_green = agile_led_create(LED_DELAY_GREEN_PIN, 0, "200,1", 1);
    delay_red = agile_led_create(LED_DELAY_RED_PIN, 0, "200,1", 1);

    recv_beep = agile_led_create(BUZZER_PIN, 1, "200,1", 1);
    learn_success_green = agile_led_create(LED_WARN_GREEN_PIN, 0, "200,200", 5);
    learn_success_beep = agile_led_create(BUZZER_PIN, 1, "200,200", 5);

    power_led_red = agile_led_create(LED_OFF_RED_PIN, 0, "500,1", 1);
    power_led_green = agile_led_create(LED_ON_GREEN_PIN, 0, "500,1", 1);
    power_beep = agile_led_create(BUZZER_PIN, 1, "500,1", 1);

    rssi_prompt_led = agile_led_create(LED_INTENSITY_GREEN_PIN, 0, "200,600", 2);
    rssi_low_led = agile_led_create(LED_ON_GREEN_PIN, 0, "0,400,200,600,200,1", 1);
    rssi_mid_led = agile_led_create(LED_WARN_GREEN_PIN, 0, "0,400,200,600,200,1", 1);
    rssi_high_led = agile_led_create(LED_OFF_GREEN_PIN, 0, "0,400,200,600,200,1", 1);
    rssi_ultra_led = agile_led_create(LED_DELAY_GREEN_PIN, 0, "0,400,200,600,200,1", 1);

    bat_prompt_led = agile_led_create(LED_BAT_GREEN_PIN, 0, "200,600", 2);
    bat_low_led = agile_led_create(LED_DELAY_GREEN_PIN, 0, "0,400,200,600,200,1", 1);
    bat_mid_led = agile_led_create(LED_OFF_GREEN_PIN, 0, "0,400,200,600,200,1", 1);
    bat_high_led = agile_led_create(LED_WARN_GREEN_PIN, 0, "0,400,200,600,200,1", 1);
    bat_ultra_led = agile_led_create(LED_ON_GREEN_PIN, 0, "0,400,200,600,200,1", 1);

    Led_Beep_Powerup();
}
void Led_KeyOn(void)
{
    agile_led_start(key_press_green);
}
void Led_KeyOff(void)
{
    agile_led_start(key_press_red);
}
void Led_DelayOn(void)
{
    agile_led_start(delay_green);
}
void Led_DelayOff(void)
{
    agile_led_start(delay_red);
}
void Beep_Recv(void)
{
    agile_led_start(recv_beep);
}

void Led_LearnSuceess(void)
{
    agile_led_start(learn_success_green);
    agile_led_start(learn_success_beep);
}

void led_rssi_show(void)
{
    agile_led_start(rssi_prompt_led);
    switch(Get_RSSI_Level())
    {
    case 0:
        agile_led_start(rssi_ultra_led);
        break;
    case 1:
        agile_led_start(rssi_high_led);
        break;
    case 2:
        agile_led_start(rssi_mid_led);
        break;
    case 3:
        agile_led_start(rssi_low_led);
        break;
    case 4:
        agile_led_start(rssi_low_led);
        break;
    default:
        break;
    }
}

void led_bat_show(void)
{
    agile_led_start(bat_prompt_led);
    switch(Get_BAT_Level())
    {
    case 0:
        agile_led_start(bat_ultra_led);
        break;
    case 1:
        agile_led_start(bat_high_led);
        break;
    case 2:
        agile_led_start(bat_mid_led);
        break;
    case 3:
        agile_led_start(bat_low_led);
        break;
    case 4:
        agile_led_start(bat_low_led);
        break;
    default:
        break;
    }
}

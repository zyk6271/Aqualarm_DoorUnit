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
#include <stdio.h>
#include "radio_app.h"
#include "radio_decoder.h"

#define DBG_TAG "RADIO_DECODER"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

extern uint32_t Self_Id;

uint8_t RSSI_Level;
uint8_t Delay_Flag;

uint8_t Get_RSSI_Level(void)
{
    return RSSI_Level;
}
void Set_RSSI_Level(uint8_t value)
{
    if(value >=-30)
    {
        RSSI_Level = 0;
    }
    else if(value < -30 && value >= -60)
    {
        RSSI_Level = 1;
    }
    else if(value < -60 && value >= -90)
    {
        RSSI_Level = 2;
    }
    else if(value < -90)
    {
        RSSI_Level = 3;
    }
}
uint8_t Get_Delay_Flag(void)
{
    return Delay_Flag;
}
void Set_Delay_Flag(uint8_t value)
{
    Delay_Flag = !value;
}
void NormalSolve(int rssi,uint8_t *rx_buffer,uint8_t rx_len)
{
    Message_Format Rx_message = {0};
    if(rx_buffer[rx_len-1]==0x0A&&rx_buffer[rx_len-2]==0x0D)
    {
        sscanf((const char *)&rx_buffer[1],"{%ld,%ld,%d,%d,%d}",&Rx_message.Target_ID,&Rx_message.From_ID,&Rx_message.Counter,&Rx_message.Command,&Rx_message.Data);
        if(Rx_message.Target_ID != Self_Id)return;
        Set_RSSI_Level(rssi);
        LOG_I("Target_ID : %d\r\n",Rx_message.Target_ID);
        switch(Rx_message.Command)
        {
        case 2:
            //Recv Heart
            Stop_Heart_Timer();
            break;
        case 3:
            //Learn Request and Confirm
            if(Rx_message.Target_ID == Self_Id)
            {
                if(Rx_message.Data == 1)
                {
                    RF_Learn_Confirm(Rx_message.From_ID);
                    Storage_Main_Write(Rx_message.From_ID);
                }
                else if(Rx_message.Data == 2)
                {
                    Led_LearnSuceess();
                }
            }
            break;
        case 5:
            //Open Confirm
            Beep_Recv();
            break;
        case 6:
            //Close Confirm
            Beep_Recv();
            break;
        case 8:
            //Delay
            Beep_Recv();
            Set_Delay_Flag(Rx_message.Data);
            break;
        default:
            break;
        }
    }
}

void Radio_Parse(int rssi,uint8_t* data,size_t len)
{
    switch(data[1])
    {
    case '{':
        NormalSolve(rssi,data,len);
        break;
    default:
        break;
    }
}


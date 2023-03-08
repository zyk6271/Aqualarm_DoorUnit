/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-02-16     Rick       the first version
 */
#include <fal.h>
#include <rtthread.h>
#include <rtdevice.h>
#include <string.h>
#include <stdlib.h>

#define DBG_TAG "Storage"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

uint32_t Main_ID = 0;
static const struct fal_partition *part_dev = NULL;

void StorageInit(void)
{
    if (!fal_init())
    {
        return;
    }
    part_dev = fal_partition_find("cfg");
    if (part_dev != RT_NULL)
    {
        rt_kprintf("Probed a flash partition | %s | flash_dev: %s | offset: %ld | len: %d |.\n",
                part_dev->name, part_dev->flash_name, part_dev->offset, part_dev->len);
    }
}
void Storage_Main_Write(uint32_t id)
{
    fal_partition_erase(part_dev, 0, 8);
    fal_partition_write(part_dev, 0, &id, 8);
    Main_ID = id;
}
uint32_t Storage_Main_Read(void)
{
    uint32_t id;
    if(Main_ID == 0)
    {
        fal_partition_read(part_dev, 0, &id, 4);
        Main_ID = id;
    }
    return Main_ID;
}

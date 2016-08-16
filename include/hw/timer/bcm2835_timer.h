/*
 * BCM2835 System Timer Module
 *
 * Copyright (c) 2016 Tray Torrance.
 * Written by Tray Torrance
 *
 * This code is licensed under the GPL.
 */
#ifndef HW_TIMER_BCM2835_TIMER_H
#define HW_TIMER_BCM2835_TIMER_H

#include "qemu/timer.h"
#include "hw/sysbus.h"

#define TYPE_BCM2835_TIMER "bcm2835-timer"
#define BCM2835_TIMER(obj) \
    OBJECT_CHECK(BCM2835TimerState, (obj), TYPE_BCM2835_TIMER)

/* BCM2835 System Timer State */
typedef struct BCM2835TimerState {
    SysBusDevice parent_obj;

    MemoryRegion iomem;
    QEMUTimer *timer;
    qemu_irq irq;
    
    uint32_t control;
    uint32_t count_lo;
    uint32_t count_hi;
    uint32_t cmp0;
    uint32_t cmp1;
    uint32_t cmp2;
    uint32_t cmp3;
} BCM2835TimerState;

#endif

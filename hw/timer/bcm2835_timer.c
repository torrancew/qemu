/*
 * BCM2835 System Timer Module
 *
 * Copyright (c) 2016 Tray Torrance
 * Written by Tray Torrance
 *
 * This code is licensed under the GPL.
 */

#include "qemu/osdep.h"
#include "qemu/log.h"
#include "hw/timer/bcm2835_timer.h"

#define TIMER_RST   0x00
#define TIMER_CR    0x00
#define TIMER_CLO   0x04
#define TIMER_CHI   0x08
#define TIMER_C0    0x0c
#define TIMER_C1    0x10
#define TIMER_C2    0x14
#define TIMER_C3    0x18
#define TIMER_M0   (1<<0)
#define TIMER_M1   (1<<1)
#define TIMER_M2   (1<<2)
#define TIMER_M3   (1<<3)

static void bcm2835_timer_interrupt(BCM2835TimerState *s)
{
    /* TODO: Update IRQ States */
}

static void bcm2835_timer_update(BCM2835TimerState *s)
{
      uint64_t cur_us = qemu_clock_get_us(QEMU_CLOCK_VIRTUAL);
      s->count_lo = cur_us & 0xffffffff;
      s->count_hi = cur_us>>32;
}

static void bcm2835_timer_tick(void *opaque)
{
    BCM2835TimerState *s = (BCM2835TimerState*)opaque;

    bcm2835_timer_update(s);
    bcm2835_timer_interrupt(s);

    timer_mod(s->timer, qemu_clock_get_us(QEMU_CLOCK_VIRTUAL)+8);
}

static uint64_t bcm2835_timer_read(void *opaque, hwaddr offset,
                                   unsigned size)
{
    BCM2835TimerState *s = (BCM2835TimerState*)opaque;

    switch (offset) {
    case TIMER_CR:
        return s->control;
    case TIMER_CLO:
        return s->count_lo;
    case TIMER_CHI:
        return s->count_hi;
    case TIMER_C0:
        return s->cmp0;
    case TIMER_C1:
        return s->cmp1;
    case TIMER_C2:
        return s->cmp2;
    case TIMER_C3:
        return s->cmp3;
    default:
        qemu_log_mask(LOG_GUEST_ERROR,
                      "bcm2835_timer_read: Bad offset 0x%x\n", (int)offset);
        break;
    }

    return 0;
}

static void bcm2835_timer_write(void *opaque, hwaddr offset,
                                uint64_t val, unsigned size)
{
    BCM2835TimerState *s = (BCM2835TimerState*)opaque;

    switch (offset) {
    case TIMER_CR:
        s->control = val;
        break;
    case TIMER_C0:
        s->cmp0 = val;
        break;
    case TIMER_C1:
        s->cmp1 = val;
        break;
    case TIMER_C2:
        s->cmp2 = val;
        break;
    case TIMER_C3:
        s->cmp3 = val;
        break;
    case TIMER_CLO:
    case TIMER_CHI:
        qemu_log_mask(LOG_GUEST_ERROR,
                      "bcm2835_timer: write to read-only register at offset 0x%x\n",
                      (int)offset);
        break;
    default:
        qemu_log_mask(LOG_GUEST_ERROR,
                      "bcm2835_timer_write: Bad offset 0x%x\n", (int)offset);
        break;
    }
}

static const MemoryRegionOps bcm2835_timer_ops = {
    .read       = bcm2835_timer_read,
    .write      = bcm2835_timer_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

static void bcm2835_timer_init(Object *obj)
{
    BCM2835TimerState *s = BCM2835_TIMER(obj);
    SysBusDevice *dev = SYS_BUS_DEVICE(obj);

    memory_region_init_io(&s->iomem, obj, &bcm2835_timer_ops,
                          s, "bcm2835-timer", 0x20);
    sysbus_init_mmio(dev, &s->iomem);
    sysbus_init_irq(dev, &s->irq);

    s->control = 0x00000000;
    s->cmp0 = s->cmp1 = s->cmp2 = s->cmp3 = 0x00000000;
    s->timer = timer_new_us(QEMU_CLOCK_VIRTUAL, bcm2835_timer_tick, s);

    bcm2835_timer_tick(s);
}

static void bcm2835_timer_pre_save(void *opaque)
{
    /* TODO: Handle VM Migration */
}

static int bcm2835_timer_post_load(void *opaque, int version_id)
{
    /* TODO: Handle VM Migration */
    return 0;
}

static const VMStateDescription vmstate_bcm2835_timer = {
    .name = "bcm2835-timer",
    .version_id = 1,
    .minimum_version_id = 1,
    .pre_save = bcm2835_timer_pre_save,
    .post_load = bcm2835_timer_post_load,
    .fields = (VMStateField[]) {
        VMSTATE_UINT32(control, BCM2835TimerState),
        VMSTATE_UINT32(count_lo, BCM2835TimerState),
        VMSTATE_UINT32(count_hi, BCM2835TimerState),
        VMSTATE_UINT32(cmp0, BCM2835TimerState),
        VMSTATE_UINT32(cmp1, BCM2835TimerState),
        VMSTATE_UINT32(cmp2, BCM2835TimerState),
        VMSTATE_UINT32(cmp3, BCM2835TimerState),
        VMSTATE_END_OF_LIST()
    }
};

static void bcm2835_timer_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    dc->vmsd = &vmstate_bcm2835_timer;
}

static const TypeInfo bcm2835_timer_info = {
    .name          = TYPE_BCM2835_TIMER,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(BCM2835TimerState),
    .instance_init = bcm2835_timer_init,
    .class_init    = bcm2835_timer_class_init,
};

static void bcm2835_timer_register_types(void)
{
    type_register_static(&bcm2835_timer_info);
}

type_init(bcm2835_timer_register_types)

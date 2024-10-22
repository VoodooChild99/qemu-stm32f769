#include "qemu/osdep.h"
#include "qemu/log.h"
#include "qemu/bitops.h"
#include "hw/sysbus.h"
#include "hw/irq.h"
#include "migration/vmstate.h"
#include "hw/registerfields.h"
#include "hw/resettable.h"
#include "hw/qdev-properties-system.h"
#include "exec/cpu-common.h"
#include "hw/misc/stm32f7-exti.h"

#define STM32F7_EXTI_SIZE                               0x18

REG32(IMR, 0x0)
    FIELD(IMR, MR0, 0, 1)
    FIELD(IMR, MR1, 1, 1)
    FIELD(IMR, MR2, 2, 1)
    FIELD(IMR, MR3, 3, 1)
    FIELD(IMR, MR4, 4, 1)
    FIELD(IMR, MR5, 5, 1)
    FIELD(IMR, MR6, 6, 1)
    FIELD(IMR, MR7, 7, 1)
    FIELD(IMR, MR8, 8, 1)
    FIELD(IMR, MR9, 9, 1)
    FIELD(IMR, MR10, 10, 1)
    FIELD(IMR, MR11, 11, 1)
    FIELD(IMR, MR12, 12, 1)
    FIELD(IMR, MR13, 13, 1)
    FIELD(IMR, MR14, 14, 1)
    FIELD(IMR, MR15, 15, 1)
    FIELD(IMR, MR16, 16, 1)
    FIELD(IMR, MR17, 17, 1)
    FIELD(IMR, MR18, 18, 1)
    FIELD(IMR, MR19, 19, 1)
    FIELD(IMR, MR20, 20, 1)
    FIELD(IMR, MR21, 21, 1)
    FIELD(IMR, MR22, 22, 1)
REG32(EMR, 0x4)
    FIELD(EMR, MR0, 0, 1)
    FIELD(EMR, MR1, 1, 1)
    FIELD(EMR, MR2, 2, 1)
    FIELD(EMR, MR3, 3, 1)
    FIELD(EMR, MR4, 4, 1)
    FIELD(EMR, MR5, 5, 1)
    FIELD(EMR, MR6, 6, 1)
    FIELD(EMR, MR7, 7, 1)
    FIELD(EMR, MR8, 8, 1)
    FIELD(EMR, MR9, 9, 1)
    FIELD(EMR, MR10, 10, 1)
    FIELD(EMR, MR11, 11, 1)
    FIELD(EMR, MR12, 12, 1)
    FIELD(EMR, MR13, 13, 1)
    FIELD(EMR, MR14, 14, 1)
    FIELD(EMR, MR15, 15, 1)
    FIELD(EMR, MR16, 16, 1)
    FIELD(EMR, MR17, 17, 1)
    FIELD(EMR, MR18, 18, 1)
    FIELD(EMR, MR19, 19, 1)
    FIELD(EMR, MR20, 20, 1)
    FIELD(EMR, MR21, 21, 1)
    FIELD(EMR, MR22, 22, 1)
REG32(RTSR, 0x8)
    FIELD(RTSR, TR0, 0, 1)
    FIELD(RTSR, TR1, 1, 1)
    FIELD(RTSR, TR2, 2, 1)
    FIELD(RTSR, TR3, 3, 1)
    FIELD(RTSR, TR4, 4, 1)
    FIELD(RTSR, TR5, 5, 1)
    FIELD(RTSR, TR6, 6, 1)
    FIELD(RTSR, TR7, 7, 1)
    FIELD(RTSR, TR8, 8, 1)
    FIELD(RTSR, TR9, 9, 1)
    FIELD(RTSR, TR10, 10, 1)
    FIELD(RTSR, TR11, 11, 1)
    FIELD(RTSR, TR12, 12, 1)
    FIELD(RTSR, TR13, 13, 1)
    FIELD(RTSR, TR14, 14, 1)
    FIELD(RTSR, TR15, 15, 1)
    FIELD(RTSR, TR16, 16, 1)
    FIELD(RTSR, TR17, 17, 1)
    FIELD(RTSR, TR18, 18, 1)
    FIELD(RTSR, TR19, 19, 1)
    FIELD(RTSR, TR20, 20, 1)
    FIELD(RTSR, TR21, 21, 1)
    FIELD(RTSR, TR22, 22, 1)
REG32(FTSR, 0xc)
    FIELD(FTSR, TR0, 0, 1)
    FIELD(FTSR, TR1, 1, 1)
    FIELD(FTSR, TR2, 2, 1)
    FIELD(FTSR, TR3, 3, 1)
    FIELD(FTSR, TR4, 4, 1)
    FIELD(FTSR, TR5, 5, 1)
    FIELD(FTSR, TR6, 6, 1)
    FIELD(FTSR, TR7, 7, 1)
    FIELD(FTSR, TR8, 8, 1)
    FIELD(FTSR, TR9, 9, 1)
    FIELD(FTSR, TR10, 10, 1)
    FIELD(FTSR, TR11, 11, 1)
    FIELD(FTSR, TR12, 12, 1)
    FIELD(FTSR, TR13, 13, 1)
    FIELD(FTSR, TR14, 14, 1)
    FIELD(FTSR, TR15, 15, 1)
    FIELD(FTSR, TR16, 16, 1)
    FIELD(FTSR, TR17, 17, 1)
    FIELD(FTSR, TR18, 18, 1)
    FIELD(FTSR, TR19, 19, 1)
    FIELD(FTSR, TR20, 20, 1)
    FIELD(FTSR, TR21, 21, 1)
    FIELD(FTSR, TR22, 22, 1)
REG32(SWIER, 0x10)
    FIELD(SWIER, SWIER0, 0, 1)
    FIELD(SWIER, SWIER1, 1, 1)
    FIELD(SWIER, SWIER2, 2, 1)
    FIELD(SWIER, SWIER3, 3, 1)
    FIELD(SWIER, SWIER4, 4, 1)
    FIELD(SWIER, SWIER5, 5, 1)
    FIELD(SWIER, SWIER6, 6, 1)
    FIELD(SWIER, SWIER7, 7, 1)
    FIELD(SWIER, SWIER8, 8, 1)
    FIELD(SWIER, SWIER9, 9, 1)
    FIELD(SWIER, SWIER10, 10, 1)
    FIELD(SWIER, SWIER11, 11, 1)
    FIELD(SWIER, SWIER12, 12, 1)
    FIELD(SWIER, SWIER13, 13, 1)
    FIELD(SWIER, SWIER14, 14, 1)
    FIELD(SWIER, SWIER15, 15, 1)
    FIELD(SWIER, SWIER16, 16, 1)
    FIELD(SWIER, SWIER17, 17, 1)
    FIELD(SWIER, SWIER18, 18, 1)
    FIELD(SWIER, SWIER19, 19, 1)
    FIELD(SWIER, SWIER20, 20, 1)
    FIELD(SWIER, SWIER21, 21, 1)
    FIELD(SWIER, SWIER22, 22, 1)
REG32(PR, 0x14)
    FIELD(PR, PR0, 0, 1)
    FIELD(PR, PR1, 1, 1)
    FIELD(PR, PR2, 2, 1)
    FIELD(PR, PR3, 3, 1)
    FIELD(PR, PR4, 4, 1)
    FIELD(PR, PR5, 5, 1)
    FIELD(PR, PR6, 6, 1)
    FIELD(PR, PR7, 7, 1)
    FIELD(PR, PR8, 8, 1)
    FIELD(PR, PR9, 9, 1)
    FIELD(PR, PR10, 10, 1)
    FIELD(PR, PR11, 11, 1)
    FIELD(PR, PR12, 12, 1)
    FIELD(PR, PR13, 13, 1)
    FIELD(PR, PR14, 14, 1)
    FIELD(PR, PR15, 15, 1)
    FIELD(PR, PR16, 16, 1)
    FIELD(PR, PR17, 17, 1)
    FIELD(PR, PR18, 18, 1)
    FIELD(PR, PR19, 19, 1)
    FIELD(PR, PR20, 20, 1)
    FIELD(PR, PR21, 21, 1)
    FIELD(PR, PR22, 22, 1)

static void stm32f7_exti_register_reset(STM32F7EXTI *t) {
    t->IMR = 0x0;
    t->EMR = 0x0;
    t->RTSR = 0x0;
    t->FTSR = 0x0;
    t->SWIER = 0x0;
    t->PR = 0x0;
}

static uint64_t stm32f7_exti_read(void *opaque, hwaddr offset, unsigned size) {
    STM32F7EXTI *t = STM32F7_EXTI(opaque);
    uint64_t ret;

    switch (offset) {
        case A_IMR:
            ret = t->IMR;
            break;
        case A_EMR:
            ret = t->EMR;
            break;
        case A_RTSR:
            ret = t->RTSR;
            break;
        case A_FTSR:
            ret = t->FTSR;
            break;
        case A_SWIER:
            ret = t->SWIER;
            break;
        case A_PR:
            ret = t->PR;
            break;

        default:
            qemu_log_mask(LOG_GUEST_ERROR, "STM32F7 EXTI read: bad offset %x\n", (int)offset);
            ret = 0;
            break;
    }
    return ret;
}

static void stm32f7_exti_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
    STM32F7EXTI *t = STM32F7_EXTI(opaque);


    switch (offset) {
        case A_IMR ... A_IMR + 3:
            value = value << ((offset - A_IMR) << 3);
            offset = A_IMR;
            break;
        case A_EMR ... A_EMR + 3:
            value = value << ((offset - A_EMR) << 3);
            offset = A_EMR;
            break;
        case A_RTSR ... A_RTSR + 3:
            value = value << ((offset - A_RTSR) << 3);
            offset = A_RTSR;
            break;
        case A_FTSR ... A_FTSR + 3:
            value = value << ((offset - A_FTSR) << 3);
            offset = A_FTSR;
            break;
        case A_SWIER ... A_SWIER + 3:
            value = value << ((offset - A_SWIER) << 3);
            offset = A_SWIER;
            break;
        case A_PR ... A_PR + 3:
            value = value << ((offset - A_PR) << 3);
            offset = A_PR;
            break;

        default: break;
    }

    switch (offset) {
        case A_IMR:
            t->IMR = value;
            break;
        case A_EMR:
            t->EMR = value;
            break;
        case A_RTSR:
            t->RTSR = value;
            break;
        case A_FTSR:
            t->FTSR = value;
            break;
        case A_SWIER:
            t->SWIER = value;
            break;
        case A_PR:
            t->PR = value;
            break;

        default:
            qemu_log_mask(LOG_GUEST_ERROR, "STM32F7 EXTI write: bad offset %x\n", (int)offset);
            break;
    }
}

static const MemoryRegionOps stm32f7_exti_ops = {
    .read = stm32f7_exti_read,
    .write = stm32f7_exti_write,
    .endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32f7_exti_init(Object *obj) {
    SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
    STM32F7EXTI *t = STM32F7_EXTI(obj);
    memory_region_init_io(&(t->iomem), obj, &stm32f7_exti_ops, t, "stm32f7-exti", STM32F7_EXTI_SIZE);
    sysbus_init_mmio(sbd, &(t->iomem));
    for (int i = 0; i < 9; ++i) {
        sysbus_init_irq(sbd, &(t->irq[i]));
    }

}

static void stm32f7_exti_realize(DeviceState *dev, Error **errp) {
}

static void stm32f7_exti_reset_enter(Object *obj, ResetType type) {
    STM32F7EXTI *t = STM32F7_EXTI(obj);
    stm32f7_exti_register_reset(t);
}

static const VMStateDescription stm32f7_exti_vmstate = {
    .name = "stm32f7-exti",
    .version_id = 0,
    .minimum_version_id = 0,
    .fields = (VMStateField[]) {
        VMSTATE_UINT32(IMR, STM32F7EXTI),
        VMSTATE_UINT32(EMR, STM32F7EXTI),
        VMSTATE_UINT32(RTSR, STM32F7EXTI),
        VMSTATE_UINT32(FTSR, STM32F7EXTI),
        VMSTATE_UINT32(SWIER, STM32F7EXTI),
        VMSTATE_UINT32(PR, STM32F7EXTI),

        VMSTATE_END_OF_LIST()
    }
};

static void stm32f7_exti_class_init(ObjectClass *oc, void *data) {
    DeviceClass *dc = DEVICE_CLASS(oc);
    ResettableClass *rc = RESETTABLE_CLASS(oc);
    dc->vmsd = &stm32f7_exti_vmstate;
    dc->realize = stm32f7_exti_realize;
    rc->phases.enter = stm32f7_exti_reset_enter;
}

static const TypeInfo stm32f7_exti_info = {
    .name = TYPE_STM32F7_EXTI,
    .parent = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(STM32F7EXTI),
    .instance_init = stm32f7_exti_init,
    .class_init = stm32f7_exti_class_init,
};

static void stm32f7_exti_register_types(void) {
    type_register_static(&stm32f7_exti_info);
}

type_init(stm32f7_exti_register_types);
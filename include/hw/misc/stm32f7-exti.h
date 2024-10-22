#ifndef __STM32F7_EXTI_H__
#define __STM32F7_EXTI_H__
#include "hw/sysbus.h"
#include "qom/object.h"


#define TYPE_STM32F7_EXTI "stm32f7-exti"
OBJECT_DECLARE_SIMPLE_TYPE(STM32F7EXTI, STM32F7_EXTI)

struct STM32F7EXTI {
        /* parent */
        SysBusDevice parent;

        /* memory mapping */
        MemoryRegion iomem;

        /* irqs */
        qemu_irq irq[9];

        /*registers*/
        uint32_t IMR;
        uint32_t EMR;
        uint32_t RTSR;
        uint32_t FTSR;
        uint32_t SWIER;
        uint32_t PR;
};

#endif
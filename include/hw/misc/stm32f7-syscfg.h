#ifndef __STM32F7_SYSCFG_H__
#define __STM32F7_SYSCFG_H__

#include "hw/sysbus.h"
#include "qom/object.h"

#define TYPE_STM32F7_SYSCFG "stm32f7-syscfg"
OBJECT_DECLARE_SIMPLE_TYPE(STM32F7SYSCFG, STM32F7_SYSCFG)

struct STM32F7SYSCFG {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/*registers*/
	uint32_t MEMRM;
	uint32_t PMC;
	uint32_t EXTICR1;
	uint32_t EXTICR2;
	uint32_t EXTICR3;
	uint32_t EXTICR4;
	uint32_t CMPCR;
};

#endif
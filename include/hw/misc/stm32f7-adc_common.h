#ifndef __STM32F7_ADC_COMMON_H__
#define __STM32F7_ADC_COMMON_H__
#include "hw/sysbus.h"
#include "qom/object.h"


#define TYPE_STM32F7_ADC_COMMON "stm32f7-adc_common"
OBJECT_DECLARE_SIMPLE_TYPE(STM32F7ADC_COMMON, STM32F7_ADC_COMMON)

struct STM32F7ADC_COMMON {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/*registers*/
	uint32_t CSR;
	uint32_t CCR;
	uint32_t CDR;

};

#endif
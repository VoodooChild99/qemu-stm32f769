#ifndef __STM32F7_ADC_H__
#define __STM32F7_ADC_H__
#include "hw/sysbus.h"
#include "qom/object.h"


#define TYPE_STM32F7_ADC "stm32f7-adc"
OBJECT_DECLARE_SIMPLE_TYPE(STM32F7ADC, STM32F7_ADC)

struct STM32F7ADC {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[1];

	/*registers*/
	uint32_t SR;
	uint32_t CR1;
	uint32_t CR2;
	uint32_t SMPR1;
	uint32_t SMPR2;
	uint32_t JOFR1;
	uint32_t JOFR2;
	uint32_t JOFR3;
	uint32_t JOFR4;
	uint32_t HTR;
	uint32_t LTR;
	uint32_t SQR1;
	uint32_t SQR2;
	uint32_t SQR3;
	uint32_t JSQR;
	uint32_t JDR1;
	uint32_t JDR2;
	uint32_t JDR3;
	uint32_t JDR4;
	uint32_t DR;

};

#endif
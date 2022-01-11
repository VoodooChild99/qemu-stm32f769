/*
 * STM32F7XX TIM3
 * 
 * Copyright (C) 2022 Southeast University
 * 
 * Authors:
 * 	Rayhub <leicq@seu.edu.cn>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __STM32F7_TIM3_H__
#define __STM32F7_TIM3_H__

#include "hw/sysbus.h"
#include "qom/object.h"

#define TYPE_STM32F7_TIM3 "stm32f7-tim3"
OBJECT_DECLARE_SIMPLE_TYPE(STM32F7TIM3, STM32F7_TIM3)

struct STM32F7TIM3 {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[1];

	/*registers*/
	uint32_t CR1;
	uint32_t CR2;
	uint32_t SMCR;
	uint32_t DIER;
	uint32_t SR;
	uint32_t EGR;
	uint32_t CCMR1;
	uint32_t CCMR2;
	uint32_t CCER;
	uint32_t CNT;
	uint32_t PSC;
	uint32_t ARR;
	uint32_t CCR1;
	uint32_t CCR2;
	uint32_t CCR3;
	uint32_t CCR4;
	uint32_t DCR;
	uint32_t DMAR;
	uint32_t OR1;
	uint32_t OR2;
};

#endif
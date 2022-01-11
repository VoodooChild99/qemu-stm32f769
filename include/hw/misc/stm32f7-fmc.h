/*
 * STM32F7XX FMC
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

#ifndef __STM32F7_FMC_H__
#define __STM32F7_FMC_H__

#include "hw/sysbus.h"
#include "qom/object.h"

#define TYPE_STM32F7_FMC "stm32f7-fmc"
OBJECT_DECLARE_SIMPLE_TYPE(STM32F7FMC, STM32F7_FMC)

struct STM32F7FMC {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[1];

	/*registers*/
	uint32_t BCR1;
	uint32_t BTR1;
	uint32_t BCR2;
	uint32_t BTR2;
	uint32_t BCR3;
	uint32_t BTR3;
	uint32_t BCR4;
	uint32_t BTR4;
	uint32_t PCR;
	uint32_t SR;
	uint32_t PMEM;
	uint32_t PATT;
	uint32_t ECCR;
	uint32_t BWTR1;
	uint32_t BWTR2;
	uint32_t BWTR3;
	uint32_t BWTR4;
	uint32_t SDCR1;
	uint32_t SDCR2;
	uint32_t SDTR1;
	uint32_t SDTR2;
	uint32_t SDCMR;
	uint32_t SDRTR;
	uint32_t SDSR;
};

#endif
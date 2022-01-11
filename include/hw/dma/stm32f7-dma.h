/*
 * STM32F7XX DMA
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

#ifndef __STM32F7_DMA_H__
#define __STM32F7_DMA_H__

#include "hw/sysbus.h"
#include "qom/object.h"

#define TYPE_STM32F7_DMA "stm32f7-dma"
OBJECT_DECLARE_SIMPLE_TYPE(STM32F7DMA, STM32F7_DMA)

struct STM32F7DMA {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[8];

	/*registers*/
	uint32_t LISR;
	uint32_t HISR;
	uint32_t LIFCR;
	uint32_t HIFCR;
	uint32_t S0CR;
	uint32_t S0NDTR;
	uint32_t S0PAR;
	uint32_t S0M0AR;
	uint32_t S0M1AR;
	uint32_t S0FCR;
	uint32_t S1CR;
	uint32_t S1NDTR;
	uint32_t S1PAR;
	uint32_t S1M0AR;
	uint32_t S1M1AR;
	uint32_t S1FCR;
	uint32_t S2CR;
	uint32_t S2NDTR;
	uint32_t S2PAR;
	uint32_t S2M0AR;
	uint32_t S2M1AR;
	uint32_t S2FCR;
	uint32_t S3CR;
	uint32_t S3NDTR;
	uint32_t S3PAR;
	uint32_t S3M0AR;
	uint32_t S3M1AR;
	uint32_t S3FCR;
	uint32_t S4CR;
	uint32_t S4NDTR;
	uint32_t S4PAR;
	uint32_t S4M0AR;
	uint32_t S4M1AR;
	uint32_t S4FCR;
	uint32_t S5CR;
	uint32_t S5NDTR;
	uint32_t S5PAR;
	uint32_t S5M0AR;
	uint32_t S5M1AR;
	uint32_t S5FCR;
	uint32_t S6CR;
	uint32_t S6NDTR;
	uint32_t S6PAR;
	uint32_t S6M0AR;
	uint32_t S6M1AR;
	uint32_t S6FCR;
	uint32_t S7CR;
	uint32_t S7NDTR;
	uint32_t S7PAR;
	uint32_t S7M0AR;
	uint32_t S7M1AR;
	uint32_t S7FCR;
};

#endif
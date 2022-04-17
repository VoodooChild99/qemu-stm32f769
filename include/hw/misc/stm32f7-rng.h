/*
 * STM32F7XX RNG
 * 
 * Copyright (C) 2022 JSSEC
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

#ifndef __STM32F7_RNG_H__
#define __STM32F7_RNG_H__

#include "hw/sysbus.h"
#include "qom/object.h"

#define TYPE_STM32F7_RNG "stm32f7-rng"
OBJECT_DECLARE_SIMPLE_TYPE(STM32F7RNG, STM32F7_RNG)

struct STM32F7RNG {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[1];

	/*registers*/
	uint32_t CR;
	uint32_t SR;
	uint32_t DR;

	/* random */
	GRand* rnd;
};

#endif
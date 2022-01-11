/*
 * STM32F7XX GPIOA
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

#ifndef __STM32F7_GPIOA_H__
#define __STM32F7_GPIOA_H__

#include "hw/sysbus.h"
#include "qom/object.h"

#define TYPE_STM32F7_GPIOA "stm32f7-gpioa"
OBJECT_DECLARE_SIMPLE_TYPE(STM32F7GPIOA, STM32F7_GPIOA)

struct STM32F7GPIOA {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/*registers*/
	uint32_t MODER;
	uint32_t OTYPER;
	uint32_t GPIOB_OSPEEDR;
	uint32_t PUPDR;
	uint32_t IDR;
	uint32_t ODR;
	uint32_t BSRR;
	uint32_t LCKR;
	uint32_t AFRL;
	uint32_t AFRH;
	uint32_t BRR;
};

#endif
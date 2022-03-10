/*
 * STM32F7XX PWR
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

#ifndef __STM32F7_PWR_H__
#define __STM32F7_PWR_H__

#include "hw/sysbus.h"
#include "qom/object.h"

#define TYPE_STM32F7_PWR "stm32f7-pwr"
OBJECT_DECLARE_SIMPLE_TYPE(STM32F7PWR, STM32F7_PWR)

struct STM32F7PWR {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/*registers*/
	uint32_t CR1;
	uint32_t CSR1;
	uint32_t CR2;
	uint32_t CSR2;
};

#endif
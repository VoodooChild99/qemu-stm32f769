/*
 * STM32F7XX PWR
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

#include "qemu/osdep.h"
#include "qemu/log.h"
#include "qemu/bitops.h"
#include "hw/sysbus.h"
#include "hw/irq.h"
#include "migration/vmstate.h"
#include "hw/registerfields.h"
#include "hw/resettable.h"
#include "hw/misc/stm32f7-pwr.h"
#include "hw/qdev-properties.h"
#include "exec/cpu-common.h"

#define STM32F7_PWR_SIZE				0x400

REG32(CR1, 0x0)
	FIELD(CR1, LPDS, 0, 1)
	FIELD(CR1, PDDS, 1, 1)
	FIELD(CR1, CSBF, 3, 1)
	FIELD(CR1, PVDE, 4, 1)
	FIELD(CR1, PLS, 5, 3)
	FIELD(CR1, DBP, 8, 1)
	FIELD(CR1, FPDS, 9, 1)
	FIELD(CR1, LPUDS, 10, 1)
	FIELD(CR1, MRUDS, 11, 1)
	FIELD(CR1, ADCDC1, 13, 1)
	FIELD(CR1, VOS, 14, 2)
	FIELD(CR1, ODEN, 16, 1)
	FIELD(CR1, ODSWEN, 17, 1)
	FIELD(CR1, UDEN, 18, 2)
REG32(CSR1, 0x4)
	FIELD(CSR1, WUIF, 0, 1)
	FIELD(CSR1, SBF, 1, 1)
	FIELD(CSR1, PVDO, 2, 1)
	FIELD(CSR1, BRR, 3, 1)
	FIELD(CSR1, BRE, 9, 1)
	FIELD(CSR1, VOSRDY, 14, 1)
	FIELD(CSR1, ODRDY, 16, 1)
	FIELD(CSR1, ODSWRDY, 17, 1)
	FIELD(CSR1, UDRDY, 18, 2)
REG32(CR2, 0x8)
	FIELD(CR2, CWUPF1, 0, 1)
	FIELD(CR2, CWUPF2, 1, 1)
	FIELD(CR2, CWUPF3, 2, 1)
	FIELD(CR2, CWUPF4, 3, 1)
	FIELD(CR2, CWUPF5, 4, 1)
	FIELD(CR2, CWUPF6, 5, 1)
	FIELD(CR2, WUPP1, 8, 1)
	FIELD(CR2, WUPP2, 9, 1)
	FIELD(CR2, WUPP3, 10, 1)
	FIELD(CR2, WUPP4, 11, 1)
	FIELD(CR2, WUPP5, 12, 1)
	FIELD(CR2, WUPP6, 13, 1)
REG32(CSR2, 0xc)
	FIELD(CSR2, WUPF1, 0, 1)
	FIELD(CSR2, WUPF2, 1, 1)
	FIELD(CSR2, WUPF3, 2, 1)
	FIELD(CSR2, WUPF4, 3, 1)
	FIELD(CSR2, WUPF5, 4, 1)
	FIELD(CSR2, WUPF6, 5, 1)
	FIELD(CSR2, EWUP1, 8, 1)
	FIELD(CSR2, EWUP2, 9, 1)
	FIELD(CSR2, EWUP3, 10, 1)
	FIELD(CSR2, EWUP4, 11, 1)
	FIELD(CSR2, EWUP5, 12, 1)
	FIELD(CSR2, EWUP6, 13, 1)

static void stm32f7_pwr_register_reset(STM32F7PWR *t) {
	t->CR1 = 0x0000c000;
	t->CSR1 = 0x00004000;
	t->CR2 = 0x00000000;
	t->CSR2 = 0x00000000;
}

static uint64_t stm32f7_pwr_read(void *opaque, hwaddr offset, unsigned size) {
	STM32F7PWR *t = STM32F7_PWR(opaque);
	uint64_t ret;

	switch (offset) {
		case A_CR1:
			ret = t->CR1;
			break;
		case A_CSR1:
			ret = t->CSR1;
			break;
		case A_CR2:
			ret = t->CR2;
			break;
		case A_CSR2:
			ret = t->CSR2;
			break;
		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F7 PWR read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32f7_pwr_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32F7PWR *t = STM32F7_PWR(opaque);
	switch (offset) {
		case A_CR1:
            if ((value & R_CR1_UDEN_MASK) == R_CR1_UDEN_MASK) {
                t->CSR1 |= R_CSR1_UDRDY_MASK;
            } else if ((value & R_CR1_UDEN_MASK) == 0x00) {
                t->CSR1 &= (~R_CSR1_UDRDY_MASK);
            }

            if (value & R_CR1_ODSWEN_MASK) {
                t->CSR1 |= R_CSR1_ODSWRDY_MASK;
            }

            if (value & R_CR1_ODEN_MASK) {
                t->CSR1 |= R_CSR1_ODRDY_MASK;
            }
			t->CR1 = value;
			break;
		case A_CSR1:
			t->CSR1 = value;
			break;
		case A_CR2:
			t->CR2 = value;
			break;
		case A_CSR2:
			t->CSR2 = value;
			break;
		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F7 PWR write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32f7_pwr_ops = {
	.read = stm32f7_pwr_read,
	.write = stm32f7_pwr_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32f7_pwr_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32F7PWR *t = STM32F7_PWR(obj);

	memory_region_init_io(&(t->iomem), obj, &stm32f7_pwr_ops, t, "stm32f7-pwr", STM32F7_PWR_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
}

static void stm32f7_pwr_realize(DeviceState *dev, Error **errp) {
	return;
}

static void stm32f7_pwr_reset_enter(Object *obj, ResetType type) {
	STM32F7PWR *t = STM32F7_PWR(obj);

	stm32f7_pwr_register_reset(t);
}

static const VMStateDescription stm32f7_pwr_vmstate = {
	.name = "stm32f7-pwr",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CR1, STM32F7PWR),
		VMSTATE_UINT32(CSR1, STM32F7PWR),
		VMSTATE_UINT32(CR2, STM32F7PWR),
		VMSTATE_UINT32(CSR2, STM32F7PWR),
		VMSTATE_END_OF_LIST()
	}
};

static void stm32f7_pwr_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);

	dc->vmsd = &stm32f7_pwr_vmstate;
	dc->realize = stm32f7_pwr_realize;
	rc->phases.enter = stm32f7_pwr_reset_enter;
}

static const TypeInfo stm32f7_pwr_info = {
	.name = TYPE_STM32F7_PWR,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32F7PWR),
	.instance_init = stm32f7_pwr_init,
	.class_init = stm32f7_pwr_class_init,
};

static void stm32f7_pwr_register_types(void) {
	type_register_static(&stm32f7_pwr_info);
}

type_init(stm32f7_pwr_register_types);
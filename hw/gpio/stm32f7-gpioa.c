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

#include "qemu/osdep.h"
#include "qemu/log.h"
#include "qemu/bitops.h"
#include "hw/sysbus.h"
#include "hw/irq.h"
#include "migration/vmstate.h"
#include "hw/registerfields.h"
#include "hw/resettable.h"
#include "hw/gpio/stm32f7-gpioa.h"
#include "hw/qdev-properties.h"
#include "exec/cpu-common.h"

#define STM32F7_GPIOA_SIZE				0x400

REG32(MODER, 0x0)
	FIELD(MODER, MODER15, 30, 2)
	FIELD(MODER, MODER14, 28, 2)
	FIELD(MODER, MODER13, 26, 2)
	FIELD(MODER, MODER12, 24, 2)
	FIELD(MODER, MODER11, 22, 2)
	FIELD(MODER, MODER10, 20, 2)
	FIELD(MODER, MODER9, 18, 2)
	FIELD(MODER, MODER8, 16, 2)
	FIELD(MODER, MODER7, 14, 2)
	FIELD(MODER, MODER6, 12, 2)
	FIELD(MODER, MODER5, 10, 2)
	FIELD(MODER, MODER4, 8, 2)
	FIELD(MODER, MODER3, 6, 2)
	FIELD(MODER, MODER2, 4, 2)
	FIELD(MODER, MODER1, 2, 2)
	FIELD(MODER, MODER0, 0, 2)
REG32(OTYPER, 0x4)
	FIELD(OTYPER, OT15, 15, 1)
	FIELD(OTYPER, OT14, 14, 1)
	FIELD(OTYPER, OT13, 13, 1)
	FIELD(OTYPER, OT12, 12, 1)
	FIELD(OTYPER, OT11, 11, 1)
	FIELD(OTYPER, OT10, 10, 1)
	FIELD(OTYPER, OT9, 9, 1)
	FIELD(OTYPER, OT8, 8, 1)
	FIELD(OTYPER, OT7, 7, 1)
	FIELD(OTYPER, OT6, 6, 1)
	FIELD(OTYPER, OT5, 5, 1)
	FIELD(OTYPER, OT4, 4, 1)
	FIELD(OTYPER, OT3, 3, 1)
	FIELD(OTYPER, OT2, 2, 1)
	FIELD(OTYPER, OT1, 1, 1)
	FIELD(OTYPER, OT0, 0, 1)
REG32(GPIOB_OSPEEDR, 0x8)
	FIELD(GPIOB_OSPEEDR, OSPEEDR15, 30, 2)
	FIELD(GPIOB_OSPEEDR, OSPEEDR14, 28, 2)
	FIELD(GPIOB_OSPEEDR, OSPEEDR13, 26, 2)
	FIELD(GPIOB_OSPEEDR, OSPEEDR12, 24, 2)
	FIELD(GPIOB_OSPEEDR, OSPEEDR11, 22, 2)
	FIELD(GPIOB_OSPEEDR, OSPEEDR10, 20, 2)
	FIELD(GPIOB_OSPEEDR, OSPEEDR9, 18, 2)
	FIELD(GPIOB_OSPEEDR, OSPEEDR8, 16, 2)
	FIELD(GPIOB_OSPEEDR, OSPEEDR7, 14, 2)
	FIELD(GPIOB_OSPEEDR, OSPEEDR6, 12, 2)
	FIELD(GPIOB_OSPEEDR, OSPEEDR5, 10, 2)
	FIELD(GPIOB_OSPEEDR, OSPEEDR4, 8, 2)
	FIELD(GPIOB_OSPEEDR, OSPEEDR3, 6, 2)
	FIELD(GPIOB_OSPEEDR, OSPEEDR2, 4, 2)
	FIELD(GPIOB_OSPEEDR, OSPEEDR1, 2, 2)
	FIELD(GPIOB_OSPEEDR, OSPEEDR0, 0, 2)
REG32(PUPDR, 0xc)
	FIELD(PUPDR, PUPDR15, 30, 2)
	FIELD(PUPDR, PUPDR14, 28, 2)
	FIELD(PUPDR, PUPDR13, 26, 2)
	FIELD(PUPDR, PUPDR12, 24, 2)
	FIELD(PUPDR, PUPDR11, 22, 2)
	FIELD(PUPDR, PUPDR10, 20, 2)
	FIELD(PUPDR, PUPDR9, 18, 2)
	FIELD(PUPDR, PUPDR8, 16, 2)
	FIELD(PUPDR, PUPDR7, 14, 2)
	FIELD(PUPDR, PUPDR6, 12, 2)
	FIELD(PUPDR, PUPDR5, 10, 2)
	FIELD(PUPDR, PUPDR4, 8, 2)
	FIELD(PUPDR, PUPDR3, 6, 2)
	FIELD(PUPDR, PUPDR2, 4, 2)
	FIELD(PUPDR, PUPDR1, 2, 2)
	FIELD(PUPDR, PUPDR0, 0, 2)
REG32(IDR, 0x10)
	FIELD(IDR, IDR15, 15, 1)
	FIELD(IDR, IDR14, 14, 1)
	FIELD(IDR, IDR13, 13, 1)
	FIELD(IDR, IDR12, 12, 1)
	FIELD(IDR, IDR11, 11, 1)
	FIELD(IDR, IDR10, 10, 1)
	FIELD(IDR, IDR9, 9, 1)
	FIELD(IDR, IDR8, 8, 1)
	FIELD(IDR, IDR7, 7, 1)
	FIELD(IDR, IDR6, 6, 1)
	FIELD(IDR, IDR5, 5, 1)
	FIELD(IDR, IDR4, 4, 1)
	FIELD(IDR, IDR3, 3, 1)
	FIELD(IDR, IDR2, 2, 1)
	FIELD(IDR, IDR1, 1, 1)
	FIELD(IDR, IDR0, 0, 1)
REG32(ODR, 0x14)
	FIELD(ODR, ODR15, 15, 1)
	FIELD(ODR, ODR14, 14, 1)
	FIELD(ODR, ODR13, 13, 1)
	FIELD(ODR, ODR12, 12, 1)
	FIELD(ODR, ODR11, 11, 1)
	FIELD(ODR, ODR10, 10, 1)
	FIELD(ODR, ODR9, 9, 1)
	FIELD(ODR, ODR8, 8, 1)
	FIELD(ODR, ODR7, 7, 1)
	FIELD(ODR, ODR6, 6, 1)
	FIELD(ODR, ODR5, 5, 1)
	FIELD(ODR, ODR4, 4, 1)
	FIELD(ODR, ODR3, 3, 1)
	FIELD(ODR, ODR2, 2, 1)
	FIELD(ODR, ODR1, 1, 1)
	FIELD(ODR, ODR0, 0, 1)
REG32(BSRR, 0x18)
	FIELD(BSRR, BR15, 31, 1)
	FIELD(BSRR, BR14, 30, 1)
	FIELD(BSRR, BR13, 29, 1)
	FIELD(BSRR, BR12, 28, 1)
	FIELD(BSRR, BR11, 27, 1)
	FIELD(BSRR, BR10, 26, 1)
	FIELD(BSRR, BR9, 25, 1)
	FIELD(BSRR, BR8, 24, 1)
	FIELD(BSRR, BR7, 23, 1)
	FIELD(BSRR, BR6, 22, 1)
	FIELD(BSRR, BR5, 21, 1)
	FIELD(BSRR, BR4, 20, 1)
	FIELD(BSRR, BR3, 19, 1)
	FIELD(BSRR, BR2, 18, 1)
	FIELD(BSRR, BR1, 17, 1)
	FIELD(BSRR, BR0, 16, 1)
	FIELD(BSRR, BS15, 15, 1)
	FIELD(BSRR, BS14, 14, 1)
	FIELD(BSRR, BS13, 13, 1)
	FIELD(BSRR, BS12, 12, 1)
	FIELD(BSRR, BS11, 11, 1)
	FIELD(BSRR, BS10, 10, 1)
	FIELD(BSRR, BS9, 9, 1)
	FIELD(BSRR, BS8, 8, 1)
	FIELD(BSRR, BS7, 7, 1)
	FIELD(BSRR, BS6, 6, 1)
	FIELD(BSRR, BS5, 5, 1)
	FIELD(BSRR, BS4, 4, 1)
	FIELD(BSRR, BS3, 3, 1)
	FIELD(BSRR, BS2, 2, 1)
	FIELD(BSRR, BS1, 1, 1)
	FIELD(BSRR, BS0, 0, 1)
REG32(LCKR, 0x1c)
	FIELD(LCKR, LCKK, 16, 1)
	FIELD(LCKR, LCK15, 15, 1)
	FIELD(LCKR, LCK14, 14, 1)
	FIELD(LCKR, LCK13, 13, 1)
	FIELD(LCKR, LCK12, 12, 1)
	FIELD(LCKR, LCK11, 11, 1)
	FIELD(LCKR, LCK10, 10, 1)
	FIELD(LCKR, LCK9, 9, 1)
	FIELD(LCKR, LCK8, 8, 1)
	FIELD(LCKR, LCK7, 7, 1)
	FIELD(LCKR, LCK6, 6, 1)
	FIELD(LCKR, LCK5, 5, 1)
	FIELD(LCKR, LCK4, 4, 1)
	FIELD(LCKR, LCK3, 3, 1)
	FIELD(LCKR, LCK2, 2, 1)
	FIELD(LCKR, LCK1, 1, 1)
	FIELD(LCKR, LCK0, 0, 1)
REG32(AFRL, 0x20)
	FIELD(AFRL, AFRL7, 28, 4)
	FIELD(AFRL, AFRL6, 24, 4)
	FIELD(AFRL, AFRL5, 20, 4)
	FIELD(AFRL, AFRL4, 16, 4)
	FIELD(AFRL, AFRL3, 12, 4)
	FIELD(AFRL, AFRL2, 8, 4)
	FIELD(AFRL, AFRL1, 4, 4)
	FIELD(AFRL, AFRL0, 0, 4)
REG32(AFRH, 0x24)
	FIELD(AFRH, AFRH15, 28, 4)
	FIELD(AFRH, AFRH14, 24, 4)
	FIELD(AFRH, AFRH13, 20, 4)
	FIELD(AFRH, AFRH12, 16, 4)
	FIELD(AFRH, AFRH11, 12, 4)
	FIELD(AFRH, AFRH10, 8, 4)
	FIELD(AFRH, AFRH9, 4, 4)
	FIELD(AFRH, AFRH8, 0, 4)
REG32(BRR, 0x28)
	FIELD(BRR, BR0, 0, 1)
	FIELD(BRR, BR1, 1, 1)
	FIELD(BRR, BR2, 2, 1)
	FIELD(BRR, BR3, 3, 1)
	FIELD(BRR, BR4, 4, 1)
	FIELD(BRR, BR5, 5, 1)
	FIELD(BRR, BR6, 6, 1)
	FIELD(BRR, BR7, 7, 1)
	FIELD(BRR, BR8, 8, 1)
	FIELD(BRR, BR9, 9, 1)
	FIELD(BRR, BR10, 10, 1)
	FIELD(BRR, BR11, 11, 1)
	FIELD(BRR, BR12, 12, 1)
	FIELD(BRR, BR13, 13, 1)
	FIELD(BRR, BR14, 14, 1)
	FIELD(BRR, BR15, 15, 1)

static void stm32f7_gpioa_register_reset(STM32F7GPIOA *t) {
	t->MODER = 0xa8000000;
	t->OTYPER = 0x00000000;
	t->GPIOB_OSPEEDR = 0x00000000;
	t->PUPDR = 0x64000000;
	t->IDR = 0x00000000;
	t->ODR = 0x00000000;
	t->BSRR = 0x00000000;
	t->LCKR = 0x00000000;
	t->AFRL = 0x00000000;
	t->AFRH = 0x00000000;
	t->BRR = 0x00000000;
}

static uint64_t stm32f7_gpioa_read(void *opaque, hwaddr offset, unsigned size) {
	STM32F7GPIOA *t = STM32F7_GPIOA(opaque);
	uint64_t ret;

	switch (offset) {
		case A_MODER:
			ret = t->MODER;
			break;
		case A_OTYPER:
			ret = t->OTYPER;
			break;
		case A_GPIOB_OSPEEDR:
			ret = t->GPIOB_OSPEEDR;
			break;
		case A_PUPDR:
			ret = t->PUPDR;
			break;
		case A_IDR:
			ret = t->IDR;
			break;
		case A_ODR:
			ret = t->ODR;
			break;
		case A_LCKR:
			ret = t->LCKR;
			break;
		case A_AFRL:
			ret = t->AFRL;
			break;
		case A_AFRH:
			ret = t->AFRH;
			break;
		case A_BRR:
			ret = t->BRR;
			break;
		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F7 GPIOA read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32f7_gpioa_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32F7GPIOA *t = STM32F7_GPIOA(opaque);
	switch (offset) {
		case A_MODER:
			t->MODER = value;
			break;
		case A_OTYPER:
			t->OTYPER = value;
			break;
		case A_GPIOB_OSPEEDR:
			t->GPIOB_OSPEEDR = value;
			break;
		case A_PUPDR:
			t->PUPDR = value;
			break;
		case A_ODR:
			t->ODR = value;
			break;
		case A_BSRR:
			t->BSRR = value;
			break;
		case A_LCKR:
			t->LCKR = value;
			break;
		case A_AFRL:
			t->AFRL = value;
			break;
		case A_AFRH:
			t->AFRH = value;
			break;
		case A_BRR:
			t->BRR = value;
			break;
		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F7 GPIOA write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32f7_gpioa_ops = {
	.read = stm32f7_gpioa_read,
	.write = stm32f7_gpioa_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32f7_gpioa_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32F7GPIOA *t = STM32F7_GPIOA(obj);

	memory_region_init_io(&(t->iomem), obj, &stm32f7_gpioa_ops, t, "stm32f7-gpioa", STM32F7_GPIOA_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
}

static void stm32f7_gpioa_realize(DeviceState *dev, Error **errp) {
	return;
}

static void stm32f7_gpioa_reset_enter(Object *obj, ResetType type) {
	STM32F7GPIOA *t = STM32F7_GPIOA(obj);

	stm32f7_gpioa_register_reset(t);
}

static const VMStateDescription stm32f7_gpioa_vmstate = {
	.name = "stm32f7-gpioa",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(MODER, STM32F7GPIOA),
		VMSTATE_UINT32(OTYPER, STM32F7GPIOA),
		VMSTATE_UINT32(GPIOB_OSPEEDR, STM32F7GPIOA),
		VMSTATE_UINT32(PUPDR, STM32F7GPIOA),
		VMSTATE_UINT32(IDR, STM32F7GPIOA),
		VMSTATE_UINT32(ODR, STM32F7GPIOA),
		VMSTATE_UINT32(BSRR, STM32F7GPIOA),
		VMSTATE_UINT32(LCKR, STM32F7GPIOA),
		VMSTATE_UINT32(AFRL, STM32F7GPIOA),
		VMSTATE_UINT32(AFRH, STM32F7GPIOA),
		VMSTATE_UINT32(BRR, STM32F7GPIOA),
		VMSTATE_END_OF_LIST()
	}
};

static void stm32f7_gpioa_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);

	dc->vmsd = &stm32f7_gpioa_vmstate;
	dc->realize = stm32f7_gpioa_realize;
	rc->phases.enter = stm32f7_gpioa_reset_enter;
}

static const TypeInfo stm32f7_gpioa_info = {
	.name = TYPE_STM32F7_GPIOA,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32F7GPIOA),
	.instance_init = stm32f7_gpioa_init,
	.class_init = stm32f7_gpioa_class_init,
};

static void stm32f7_gpioa_register_types(void) {
	type_register_static(&stm32f7_gpioa_info);
}

type_init(stm32f7_gpioa_register_types);
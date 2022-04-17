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

#include "qemu/osdep.h"
#include "qemu/log.h"
#include "qemu/bitops.h"
#include "hw/sysbus.h"
#include "hw/irq.h"
#include "migration/vmstate.h"
#include "hw/registerfields.h"
#include "hw/resettable.h"
#include "hw/misc/stm32f7-rng.h"
#include "hw/qdev-properties.h"
#include "exec/cpu-common.h"

#define STM32F7_RNG_SIZE				0x400

REG32(CR, 0x0)
	FIELD(CR, IE, 3, 1)
	FIELD(CR, RNGEN, 2, 1)
REG32(SR, 0x4)
	FIELD(SR, SEIS, 6, 1)
	FIELD(SR, CEIS, 5, 1)
	FIELD(SR, SECS, 2, 1)
	FIELD(SR, CECS, 1, 1)
	FIELD(SR, DRDY, 0, 1)
REG32(DR, 0x8)
	FIELD(DR, RNDATA, 0, 32)

static void stm32f7_rng_register_reset(STM32F7RNG *t) {
	t->CR = 0x00000000;
	t->SR = 0x00000000;
	t->DR = 0x00000000;
	t->rnd = g_rand_new_with_seed(0xdeadbeef);
}

static uint64_t stm32f7_rng_read(void *opaque, hwaddr offset, unsigned size) {
	STM32F7RNG *t = STM32F7_RNG(opaque);
	uint64_t ret;

	switch (offset) {
		case A_CR:
			ret = t->CR;
			break;
		case A_SR:
			ret = (t->SR | R_SR_DRDY_MASK);
			break;
		case A_DR:
			ret = g_rand_int(t->rnd);
			break;
		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F7 RNG read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32f7_rng_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32F7RNG *t = STM32F7_RNG(opaque);
	switch (offset) {
		case A_CR:
			t->CR = value;
			break;
		case A_SR:
			t->SR = value;
			break;
		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F7 RNG write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32f7_rng_ops = {
	.read = stm32f7_rng_read,
	.write = stm32f7_rng_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32f7_rng_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32F7RNG *t = STM32F7_RNG(obj);

	memory_region_init_io(&(t->iomem), obj, &stm32f7_rng_ops, t, "stm32f7-rng", STM32F7_RNG_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0 ; i < 1 ; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}
}

static void stm32f7_rng_realize(DeviceState *dev, Error **errp) {
	return;
}

static void stm32f7_rng_reset_enter(Object *obj, ResetType type) {
	STM32F7RNG *t = STM32F7_RNG(obj);

	stm32f7_rng_register_reset(t);
}

static const VMStateDescription stm32f7_rng_vmstate = {
	.name = "stm32f7-rng",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CR, STM32F7RNG),
		VMSTATE_UINT32(SR, STM32F7RNG),
		VMSTATE_UINT32(DR, STM32F7RNG),
		VMSTATE_END_OF_LIST()
	}
};

static void stm32f7_rng_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);

	dc->vmsd = &stm32f7_rng_vmstate;
	dc->realize = stm32f7_rng_realize;
	rc->phases.enter = stm32f7_rng_reset_enter;
}

static const TypeInfo stm32f7_rng_info = {
	.name = TYPE_STM32F7_RNG,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32F7RNG),
	.instance_init = stm32f7_rng_init,
	.class_init = stm32f7_rng_class_init,
};

static void stm32f7_rng_register_types(void) {
	type_register_static(&stm32f7_rng_info);
}

type_init(stm32f7_rng_register_types);
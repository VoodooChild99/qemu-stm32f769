/*
 * STM32F7XX Flash
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
#include "hw/misc/stm32f7-flash.h"
#include "hw/qdev-properties.h"
#include "exec/cpu-common.h"

#define STM32F7_FLASH_SIZE				0x400

REG32(ACR, 0x0)
	FIELD(ACR, LATENCY, 0, 4)
	FIELD(ACR, PRFTEN, 8, 1)
	FIELD(ACR, ARTEN, 9, 1)
	FIELD(ACR, ARTRST, 11, 1)
REG32(KEYR, 0x4)
	FIELD(KEYR, KEY, 0, 32)
REG32(OPTKEYR, 0x8)
	FIELD(OPTKEYR, OPTKEY, 0, 32)
REG32(SR, 0xc)
	FIELD(SR, EOP, 0, 1)
	FIELD(SR, OPERR, 1, 1)
	FIELD(SR, WRPERR, 4, 1)
	FIELD(SR, PGAERR, 5, 1)
	FIELD(SR, PGPERR, 6, 1)
	FIELD(SR, PGSERR, 7, 1)
	FIELD(SR, BSY, 16, 1)
REG32(CR, 0x10)
	FIELD(CR, PG, 0, 1)
	FIELD(CR, SER, 1, 1)
	FIELD(CR, MER, 2, 1)
	FIELD(CR, SNB, 3, 5)
	FIELD(CR, PSIZE, 8, 2)
	FIELD(CR, MER1, 15, 1)
	FIELD(CR, STRT, 16, 1)
	FIELD(CR, EOPIE, 24, 1)
	FIELD(CR, ERRIE, 25, 1)
	FIELD(CR, LOCK, 31, 1)
REG32(OPTCR, 0x14)
	FIELD(OPTCR, OPTLOCK, 0, 1)
	FIELD(OPTCR, OPTSTRT, 1, 1)
	FIELD(OPTCR, BOR_LEV, 2, 2)
	FIELD(OPTCR, WWDG_SW, 4, 1)
	FIELD(OPTCR, IWDG_SW, 5, 1)
	FIELD(OPTCR, NRST_STOP, 6, 1)
	FIELD(OPTCR, NRST_STDBY, 7, 1)
	FIELD(OPTCR, RDP, 8, 8)
	FIELD(OPTCR, NWRP, 16, 12)
	FIELD(OPTCR, NDBOOT, 28, 1)
	FIELD(OPTCR, NDBANK, 29, 1)
	FIELD(OPTCR, IWDG_STDBY, 30, 1)
	FIELD(OPTCR, IWDG_STOP, 31, 1)
REG32(OPTCR1, 0x18)
	FIELD(OPTCR1, BOOT_ADD0, 0, 16)
	FIELD(OPTCR1, BOOT_ADD1, 16, 16)

static void stm32f7_flash_register_reset(STM32F7FLASH *t) {
	t->ACR = 0x00000000;
	t->KEYR = 0x00000000;
	t->OPTKEYR = 0x00000000;
	t->SR = 0x00000000;
	t->CR = 0x80000000;
	t->OPTCR = 0x0fffaaed;
	t->OPTCR1 = 0x0fff0000;
}

static uint64_t stm32f7_flash_read(void *opaque, hwaddr offset, unsigned size) {
	STM32F7FLASH *t = STM32F7_FLASH(opaque);
	uint64_t ret;

	switch (offset) {
		case A_ACR:
			ret = t->ACR;
			break;
		case A_SR:
			ret = t->SR;
			break;
		case A_CR:
			ret = t->CR;
			break;
		case A_OPTCR:
			ret = t->OPTCR;
			break;
		case A_OPTCR1:
			ret = t->OPTCR1;
			break;
		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F7 FLASH read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32f7_flash_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32F7FLASH *t = STM32F7_FLASH(opaque);
	switch (offset) {
		case A_ACR:
			t->ACR = value;
			break;
		case A_KEYR:
			t->KEYR = value;
			break;
		case A_OPTKEYR:
			t->OPTKEYR = value;
			break;
		case A_SR:
			t->SR = value;
			break;
		case A_CR:
			t->CR = value;
			break;
		case A_OPTCR:
			t->OPTCR = value;
			break;
		case A_OPTCR1:
			t->OPTCR1 = value;
			break;
		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F7 FLASH write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32f7_flash_ops = {
	.read = stm32f7_flash_read,
	.write = stm32f7_flash_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32f7_flash_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32F7FLASH *t = STM32F7_FLASH(obj);

	memory_region_init_io(&(t->iomem), obj, &stm32f7_flash_ops, t, "stm32f7-flash", STM32F7_FLASH_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0 ; i < 1 ; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}
}

static void stm32f7_flash_realize(DeviceState *dev, Error **errp) {
	return;
}

static void stm32f7_flash_reset_enter(Object *obj, ResetType type) {
	STM32F7FLASH *t = STM32F7_FLASH(obj);

	stm32f7_flash_register_reset(t);
}

static const VMStateDescription stm32f7_flash_vmstate = {
	.name = "stm32f7-flash",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(ACR, STM32F7FLASH),
		VMSTATE_UINT32(KEYR, STM32F7FLASH),
		VMSTATE_UINT32(OPTKEYR, STM32F7FLASH),
		VMSTATE_UINT32(SR, STM32F7FLASH),
		VMSTATE_UINT32(CR, STM32F7FLASH),
		VMSTATE_UINT32(OPTCR, STM32F7FLASH),
		VMSTATE_UINT32(OPTCR1, STM32F7FLASH),
		VMSTATE_END_OF_LIST()
	}
};

static void stm32f7_flash_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);

	dc->vmsd = &stm32f7_flash_vmstate;
	dc->realize = stm32f7_flash_realize;
	rc->phases.enter = stm32f7_flash_reset_enter;
}

static const TypeInfo stm32f7_flash_info = {
	.name = TYPE_STM32F7_FLASH,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32F7FLASH),
	.instance_init = stm32f7_flash_init,
	.class_init = stm32f7_flash_class_init,
};

static void stm32f7_flash_register_types(void) {
	type_register_static(&stm32f7_flash_info);
}

type_init(stm32f7_flash_register_types);
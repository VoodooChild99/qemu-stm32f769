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

#include "qemu/osdep.h"
#include "qemu/log.h"
#include "qemu/bitops.h"
#include "hw/sysbus.h"
#include "hw/irq.h"
#include "migration/vmstate.h"
#include "hw/registerfields.h"
#include "hw/resettable.h"
#include "hw/misc/stm32f7-fmc.h"
#include "hw/qdev-properties.h"
#include "exec/cpu-common.h"

#define STM32F7_FMC_SIZE				0x1000

REG32(BCR1, 0x0)
	FIELD(BCR1, CCLKEN, 20, 1)
	FIELD(BCR1, CBURSTRW, 19, 1)
	FIELD(BCR1, ASYNCWAIT, 15, 1)
	FIELD(BCR1, EXTMOD, 14, 1)
	FIELD(BCR1, WAITEN, 13, 1)
	FIELD(BCR1, WREN, 12, 1)
	FIELD(BCR1, WAITCFG, 11, 1)
	FIELD(BCR1, WAITPOL, 9, 1)
	FIELD(BCR1, BURSTEN, 8, 1)
	FIELD(BCR1, FACCEN, 6, 1)
	FIELD(BCR1, MWID, 4, 2)
	FIELD(BCR1, MTYP, 2, 2)
	FIELD(BCR1, MUXEN, 1, 1)
	FIELD(BCR1, MBKEN, 0, 1)
REG32(BTR1, 0x4)
	FIELD(BTR1, ACCMOD, 28, 2)
	FIELD(BTR1, DATLAT, 24, 4)
	FIELD(BTR1, CLKDIV, 20, 4)
	FIELD(BTR1, BUSTURN, 16, 4)
	FIELD(BTR1, DATAST, 8, 8)
	FIELD(BTR1, ADDHLD, 4, 4)
	FIELD(BTR1, ADDSET, 0, 4)
REG32(BCR2, 0x8)
	FIELD(BCR2, CBURSTRW, 19, 1)
	FIELD(BCR2, ASYNCWAIT, 15, 1)
	FIELD(BCR2, EXTMOD, 14, 1)
	FIELD(BCR2, WAITEN, 13, 1)
	FIELD(BCR2, WREN, 12, 1)
	FIELD(BCR2, WAITCFG, 11, 1)
	FIELD(BCR2, WRAPMOD, 10, 1)
	FIELD(BCR2, WAITPOL, 9, 1)
	FIELD(BCR2, BURSTEN, 8, 1)
	FIELD(BCR2, FACCEN, 6, 1)
	FIELD(BCR2, MWID, 4, 2)
	FIELD(BCR2, MTYP, 2, 2)
	FIELD(BCR2, MUXEN, 1, 1)
	FIELD(BCR2, MBKEN, 0, 1)
REG32(BTR2, 0xc)
	FIELD(BTR2, ACCMOD, 28, 2)
	FIELD(BTR2, DATLAT, 24, 4)
	FIELD(BTR2, CLKDIV, 20, 4)
	FIELD(BTR2, BUSTURN, 16, 4)
	FIELD(BTR2, DATAST, 8, 8)
	FIELD(BTR2, ADDHLD, 4, 4)
	FIELD(BTR2, ADDSET, 0, 4)
REG32(BCR3, 0x10)
	FIELD(BCR3, CBURSTRW, 19, 1)
	FIELD(BCR3, ASYNCWAIT, 15, 1)
	FIELD(BCR3, EXTMOD, 14, 1)
	FIELD(BCR3, WAITEN, 13, 1)
	FIELD(BCR3, WREN, 12, 1)
	FIELD(BCR3, WAITCFG, 11, 1)
	FIELD(BCR3, WRAPMOD, 10, 1)
	FIELD(BCR3, WAITPOL, 9, 1)
	FIELD(BCR3, BURSTEN, 8, 1)
	FIELD(BCR3, FACCEN, 6, 1)
	FIELD(BCR3, MWID, 4, 2)
	FIELD(BCR3, MTYP, 2, 2)
	FIELD(BCR3, MUXEN, 1, 1)
	FIELD(BCR3, MBKEN, 0, 1)
REG32(BTR3, 0x14)
	FIELD(BTR3, ACCMOD, 28, 2)
	FIELD(BTR3, DATLAT, 24, 4)
	FIELD(BTR3, CLKDIV, 20, 4)
	FIELD(BTR3, BUSTURN, 16, 4)
	FIELD(BTR3, DATAST, 8, 8)
	FIELD(BTR3, ADDHLD, 4, 4)
	FIELD(BTR3, ADDSET, 0, 4)
REG32(BCR4, 0x18)
	FIELD(BCR4, CBURSTRW, 19, 1)
	FIELD(BCR4, ASYNCWAIT, 15, 1)
	FIELD(BCR4, EXTMOD, 14, 1)
	FIELD(BCR4, WAITEN, 13, 1)
	FIELD(BCR4, WREN, 12, 1)
	FIELD(BCR4, WAITCFG, 11, 1)
	FIELD(BCR4, WRAPMOD, 10, 1)
	FIELD(BCR4, WAITPOL, 9, 1)
	FIELD(BCR4, BURSTEN, 8, 1)
	FIELD(BCR4, FACCEN, 6, 1)
	FIELD(BCR4, MWID, 4, 2)
	FIELD(BCR4, MTYP, 2, 2)
	FIELD(BCR4, MUXEN, 1, 1)
	FIELD(BCR4, MBKEN, 0, 1)
REG32(BTR4, 0x1c)
	FIELD(BTR4, ACCMOD, 28, 2)
	FIELD(BTR4, DATLAT, 24, 4)
	FIELD(BTR4, CLKDIV, 20, 4)
	FIELD(BTR4, BUSTURN, 16, 4)
	FIELD(BTR4, DATAST, 8, 8)
	FIELD(BTR4, ADDHLD, 4, 4)
	FIELD(BTR4, ADDSET, 0, 4)
REG32(PCR, 0x80)
	FIELD(PCR, ECCPS, 17, 3)
	FIELD(PCR, TAR, 13, 4)
	FIELD(PCR, TCLR, 9, 4)
	FIELD(PCR, ECCEN, 6, 1)
	FIELD(PCR, PWID, 4, 2)
	FIELD(PCR, PTYP, 3, 1)
	FIELD(PCR, PBKEN, 2, 1)
	FIELD(PCR, PWAITEN, 1, 1)
REG32(SR, 0x84)
	FIELD(SR, FEMPT, 6, 1)
	FIELD(SR, IFEN, 5, 1)
	FIELD(SR, ILEN, 4, 1)
	FIELD(SR, IREN, 3, 1)
	FIELD(SR, IFS, 2, 1)
	FIELD(SR, ILS, 1, 1)
	FIELD(SR, IRS, 0, 1)
REG32(PMEM, 0x88)
	FIELD(PMEM, MEMHIZX, 24, 8)
	FIELD(PMEM, MEMHOLDX, 16, 8)
	FIELD(PMEM, MEMWAITX, 8, 8)
	FIELD(PMEM, MEMSETX, 0, 8)
REG32(PATT, 0x8c)
	FIELD(PATT, ATTHIZX, 24, 8)
	FIELD(PATT, ATTHOLDX, 16, 8)
	FIELD(PATT, ATTWAITX, 8, 8)
	FIELD(PATT, ATTSETX, 0, 8)
REG32(ECCR, 0x94)
	FIELD(ECCR, ECCX, 0, 32)
REG32(BWTR1, 0x104)
	FIELD(BWTR1, ACCMOD, 28, 2)
	FIELD(BWTR1, DATLAT, 24, 4)
	FIELD(BWTR1, CLKDIV, 20, 4)
	FIELD(BWTR1, DATAST, 8, 8)
	FIELD(BWTR1, ADDHLD, 4, 4)
	FIELD(BWTR1, ADDSET, 0, 4)
REG32(BWTR2, 0x10c)
	FIELD(BWTR2, ACCMOD, 28, 2)
	FIELD(BWTR2, DATLAT, 24, 4)
	FIELD(BWTR2, CLKDIV, 20, 4)
	FIELD(BWTR2, DATAST, 8, 8)
	FIELD(BWTR2, ADDHLD, 4, 4)
	FIELD(BWTR2, ADDSET, 0, 4)
REG32(BWTR3, 0x114)
	FIELD(BWTR3, ACCMOD, 28, 2)
	FIELD(BWTR3, DATLAT, 24, 4)
	FIELD(BWTR3, CLKDIV, 20, 4)
	FIELD(BWTR3, DATAST, 8, 8)
	FIELD(BWTR3, ADDHLD, 4, 4)
	FIELD(BWTR3, ADDSET, 0, 4)
REG32(BWTR4, 0x11c)
	FIELD(BWTR4, ACCMOD, 28, 2)
	FIELD(BWTR4, DATLAT, 24, 4)
	FIELD(BWTR4, CLKDIV, 20, 4)
	FIELD(BWTR4, DATAST, 8, 8)
	FIELD(BWTR4, ADDHLD, 4, 4)
	FIELD(BWTR4, ADDSET, 0, 4)
REG32(SDCR1, 0x140)
	FIELD(SDCR1, NC, 0, 2)
	FIELD(SDCR1, NR, 2, 2)
	FIELD(SDCR1, MWID, 4, 2)
	FIELD(SDCR1, NB, 6, 1)
	FIELD(SDCR1, CAS, 7, 2)
	FIELD(SDCR1, WP, 9, 1)
	FIELD(SDCR1, SDCLK, 10, 2)
	FIELD(SDCR1, RBURST, 12, 1)
	FIELD(SDCR1, RPIPE, 13, 2)
REG32(SDCR2, 0x144)
	FIELD(SDCR2, NC, 0, 2)
	FIELD(SDCR2, NR, 2, 2)
	FIELD(SDCR2, MWID, 4, 2)
	FIELD(SDCR2, NB, 6, 1)
	FIELD(SDCR2, CAS, 7, 2)
	FIELD(SDCR2, WP, 9, 1)
	FIELD(SDCR2, SDCLK, 10, 2)
	FIELD(SDCR2, RBURST, 12, 1)
	FIELD(SDCR2, RPIPE, 13, 2)
REG32(SDTR1, 0x148)
	FIELD(SDTR1, TMRD, 0, 4)
	FIELD(SDTR1, TXSR, 4, 4)
	FIELD(SDTR1, TRAS, 8, 4)
	FIELD(SDTR1, TRC, 12, 4)
	FIELD(SDTR1, TWR, 16, 4)
	FIELD(SDTR1, TRP, 20, 4)
	FIELD(SDTR1, TRCD, 24, 4)
REG32(SDTR2, 0x14c)
	FIELD(SDTR2, TMRD, 0, 4)
	FIELD(SDTR2, TXSR, 4, 4)
	FIELD(SDTR2, TRAS, 8, 4)
	FIELD(SDTR2, TRC, 12, 4)
	FIELD(SDTR2, TWR, 16, 4)
	FIELD(SDTR2, TRP, 20, 4)
	FIELD(SDTR2, TRCD, 24, 4)
REG32(SDCMR, 0x150)
	FIELD(SDCMR, MODE, 0, 3)
	FIELD(SDCMR, CTB2, 3, 1)
	FIELD(SDCMR, CTB1, 4, 1)
	FIELD(SDCMR, NRFS, 5, 4)
	FIELD(SDCMR, MRD, 9, 13)
REG32(SDRTR, 0x154)
	FIELD(SDRTR, CRE, 0, 1)
	FIELD(SDRTR, COUNT, 1, 13)
	FIELD(SDRTR, REIE, 14, 1)
REG32(SDSR, 0x158)
	FIELD(SDSR, RE, 0, 1)
	FIELD(SDSR, MODES1, 1, 2)
	FIELD(SDSR, MODES2, 3, 2)
	FIELD(SDSR, BUSY, 5, 1)

static void stm32f7_fmc_register_reset(STM32F7FMC *t) {
	t->BCR1 = 0x000030d0;
	t->BTR1 = 0xffffffff;
	t->BCR2 = 0x000030d0;
	t->BTR2 = 0xffffffff;
	t->BCR3 = 0x000030d0;
	t->BTR3 = 0xffffffff;
	t->BCR4 = 0x000030d0;
	t->BTR4 = 0xffffffff;
	t->PCR = 0x00000018;
	t->SR = 0x00000040;
	t->PMEM = 0xfcfcfcfc;
	t->PATT = 0xfcfcfcfc;
	t->ECCR = 0x00000000;
	t->BWTR1 = 0x0fffffff;
	t->BWTR2 = 0x0fffffff;
	t->BWTR3 = 0x0fffffff;
	t->BWTR4 = 0x0fffffff;
	t->SDCR1 = 0x000002d0;
	t->SDCR2 = 0x000002d0;
	t->SDTR1 = 0x0fffffff;
	t->SDTR2 = 0x0fffffff;
	t->SDCMR = 0x00000000;
	t->SDRTR = 0x00000000;
	t->SDSR = 0x00000000;
}

static uint64_t stm32f7_fmc_read(void *opaque, hwaddr offset, unsigned size) {
	STM32F7FMC *t = STM32F7_FMC(opaque);
	uint64_t ret;

	switch (offset) {
		case A_BCR1:
			ret = t->BCR1;
			break;
		case A_BTR1:
			ret = t->BTR1;
			break;
		case A_BCR2:
			ret = t->BCR2;
			break;
		case A_BTR2:
			ret = t->BTR2;
			break;
		case A_BCR3:
			ret = t->BCR3;
			break;
		case A_BTR3:
			ret = t->BTR3;
			break;
		case A_BCR4:
			ret = t->BCR4;
			break;
		case A_BTR4:
			ret = t->BTR4;
			break;
		case A_PCR:
			ret = t->PCR;
			break;
		case A_SR:
			ret = t->SR;
			break;
		case A_PMEM:
			ret = t->PMEM;
			break;
		case A_PATT:
			ret = t->PATT;
			break;
		case A_ECCR:
			ret = t->ECCR;
			break;
		case A_BWTR1:
			ret = t->BWTR1;
			break;
		case A_BWTR2:
			ret = t->BWTR2;
			break;
		case A_BWTR3:
			ret = t->BWTR3;
			break;
		case A_BWTR4:
			ret = t->BWTR4;
			break;
		case A_SDCR1:
			ret = t->SDCR1;
			break;
		case A_SDCR2:
			ret = t->SDCR2;
			break;
		case A_SDTR1:
			ret = t->SDTR1;
			break;
		case A_SDTR2:
			ret = t->SDTR2;
			break;
		case A_SDCMR:
			ret = t->SDCMR;
			break;
		case A_SDRTR:
			ret = t->SDRTR;
			break;
		case A_SDSR:
			ret = t->SDSR;
			break;
		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F7 FMC read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32f7_fmc_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32F7FMC *t = STM32F7_FMC(opaque);
	switch (offset) {
		case A_BCR1:
			t->BCR1 = value;
			break;
		case A_BTR1:
			t->BTR1 = value;
			break;
		case A_BCR2:
			t->BCR2 = value;
			break;
		case A_BTR2:
			t->BTR2 = value;
			break;
		case A_BCR3:
			t->BCR3 = value;
			break;
		case A_BTR3:
			t->BTR3 = value;
			break;
		case A_BCR4:
			t->BCR4 = value;
			break;
		case A_BTR4:
			t->BTR4 = value;
			break;
		case A_PCR:
			t->PCR = value;
			break;
		case A_SR:
			t->SR = value;
			break;
		case A_PMEM:
			t->PMEM = value;
			break;
		case A_PATT:
			t->PATT = value;
			break;
		case A_BWTR1:
			t->BWTR1 = value;
			break;
		case A_BWTR2:
			t->BWTR2 = value;
			break;
		case A_BWTR3:
			t->BWTR3 = value;
			break;
		case A_BWTR4:
			t->BWTR4 = value;
			break;
		case A_SDCR1:
			t->SDCR1 = value;
			break;
		case A_SDCR2:
			t->SDCR2 = value;
			break;
		case A_SDTR1:
			t->SDTR1 = value;
			break;
		case A_SDTR2:
			t->SDTR2 = value;
			break;
		case A_SDCMR:
			t->SDCMR = value;
			break;
		case A_SDRTR:
			t->SDRTR = value;
			break;
		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F7 FMC write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32f7_fmc_ops = {
	.read = stm32f7_fmc_read,
	.write = stm32f7_fmc_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32f7_fmc_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32F7FMC *t = STM32F7_FMC(obj);

	memory_region_init_io(&(t->iomem), obj, &stm32f7_fmc_ops, t, "stm32f7-fmc", STM32F7_FMC_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0 ; i < 1 ; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}
}

static void stm32f7_fmc_realize(DeviceState *dev, Error **errp) {
	return;
}

static void stm32f7_fmc_reset_enter(Object *obj, ResetType type) {
	STM32F7FMC *t = STM32F7_FMC(obj);

	stm32f7_fmc_register_reset(t);
}

static const VMStateDescription stm32f7_fmc_vmstate = {
	.name = "stm32f7-fmc",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(BCR1, STM32F7FMC),
		VMSTATE_UINT32(BTR1, STM32F7FMC),
		VMSTATE_UINT32(BCR2, STM32F7FMC),
		VMSTATE_UINT32(BTR2, STM32F7FMC),
		VMSTATE_UINT32(BCR3, STM32F7FMC),
		VMSTATE_UINT32(BTR3, STM32F7FMC),
		VMSTATE_UINT32(BCR4, STM32F7FMC),
		VMSTATE_UINT32(BTR4, STM32F7FMC),
		VMSTATE_UINT32(PCR, STM32F7FMC),
		VMSTATE_UINT32(SR, STM32F7FMC),
		VMSTATE_UINT32(PMEM, STM32F7FMC),
		VMSTATE_UINT32(PATT, STM32F7FMC),
		VMSTATE_UINT32(ECCR, STM32F7FMC),
		VMSTATE_UINT32(BWTR1, STM32F7FMC),
		VMSTATE_UINT32(BWTR2, STM32F7FMC),
		VMSTATE_UINT32(BWTR3, STM32F7FMC),
		VMSTATE_UINT32(BWTR4, STM32F7FMC),
		VMSTATE_UINT32(SDCR1, STM32F7FMC),
		VMSTATE_UINT32(SDCR2, STM32F7FMC),
		VMSTATE_UINT32(SDTR1, STM32F7FMC),
		VMSTATE_UINT32(SDTR2, STM32F7FMC),
		VMSTATE_UINT32(SDCMR, STM32F7FMC),
		VMSTATE_UINT32(SDRTR, STM32F7FMC),
		VMSTATE_UINT32(SDSR, STM32F7FMC),
		VMSTATE_END_OF_LIST()
	}
};

static void stm32f7_fmc_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);

	dc->vmsd = &stm32f7_fmc_vmstate;
	dc->realize = stm32f7_fmc_realize;
	rc->phases.enter = stm32f7_fmc_reset_enter;
}

static const TypeInfo stm32f7_fmc_info = {
	.name = TYPE_STM32F7_FMC,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32F7FMC),
	.instance_init = stm32f7_fmc_init,
	.class_init = stm32f7_fmc_class_init,
};

static void stm32f7_fmc_register_types(void) {
	type_register_static(&stm32f7_fmc_info);
}

type_init(stm32f7_fmc_register_types);
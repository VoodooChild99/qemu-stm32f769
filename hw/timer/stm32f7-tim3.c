/*
 * STM32F7XX TIM3
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
#include "hw/timer/stm32f7-tim3.h"
#include "hw/qdev-properties.h"
#include "exec/cpu-common.h"

#define STM32F7_TIM3_SIZE				0x400

REG32(CR1, 0x0)
	FIELD(CR1, CKD, 8, 2)
	FIELD(CR1, ARPE, 7, 1)
	FIELD(CR1, CMS, 5, 2)
	FIELD(CR1, DIR, 4, 1)
	FIELD(CR1, OPM, 3, 1)
	FIELD(CR1, URS, 2, 1)
	FIELD(CR1, UDIS, 1, 1)
	FIELD(CR1, CEN, 0, 1)
REG32(CR2, 0x4)
	FIELD(CR2, TI1S, 7, 1)
	FIELD(CR2, MMS, 4, 3)
	FIELD(CR2, CCDS, 3, 1)
REG32(SMCR, 0x8)
	FIELD(SMCR, ETP, 15, 1)
	FIELD(SMCR, ECE, 14, 1)
	FIELD(SMCR, ETPS, 12, 2)
	FIELD(SMCR, ETF, 8, 4)
	FIELD(SMCR, MSM, 7, 1)
	FIELD(SMCR, TS, 4, 3)
	FIELD(SMCR, SMS, 0, 3)
REG32(DIER, 0xc)
	FIELD(DIER, TDE, 14, 1)
	FIELD(DIER, CC4DE, 12, 1)
	FIELD(DIER, CC3DE, 11, 1)
	FIELD(DIER, CC2DE, 10, 1)
	FIELD(DIER, CC1DE, 9, 1)
	FIELD(DIER, UDE, 8, 1)
	FIELD(DIER, TIE, 6, 1)
	FIELD(DIER, CC4IE, 4, 1)
	FIELD(DIER, CC3IE, 3, 1)
	FIELD(DIER, CC2IE, 2, 1)
	FIELD(DIER, CC1IE, 1, 1)
	FIELD(DIER, UIE, 0, 1)
REG32(SR, 0x10)
	FIELD(SR, CC4OF, 12, 1)
	FIELD(SR, CC3OF, 11, 1)
	FIELD(SR, CC2OF, 10, 1)
	FIELD(SR, CC1OF, 9, 1)
	FIELD(SR, TIF, 6, 1)
	FIELD(SR, CC4IF, 4, 1)
	FIELD(SR, CC3IF, 3, 1)
	FIELD(SR, CC2IF, 2, 1)
	FIELD(SR, CC1IF, 1, 1)
	FIELD(SR, UIF, 0, 1)
REG32(EGR, 0x14)
	FIELD(EGR, TG, 6, 1)
	FIELD(EGR, CC4G, 4, 1)
	FIELD(EGR, CC3G, 3, 1)
	FIELD(EGR, CC2G, 2, 1)
	FIELD(EGR, CC1G, 1, 1)
	FIELD(EGR, UG, 0, 1)
REG32(CCMR1, 0x18)
	FIELD(CCMR1, OC2CE, 15, 1)
	FIELD(CCMR1, OC2M, 12, 3)
	FIELD(CCMR1, OC2PE, 11, 1)
	FIELD(CCMR1, OC2FE, 10, 1)
	FIELD(CCMR1, CC2S, 8, 2)
	FIELD(CCMR1, OC1CE, 7, 1)
	FIELD(CCMR1, OC1M, 4, 3)
	FIELD(CCMR1, OC1PE, 3, 1)
	FIELD(CCMR1, OC1FE, 2, 1)
	FIELD(CCMR1, IC2F, 12, 4)
	FIELD(CCMR1, IC2PCS, 10, 2)
	FIELD(CCMR1, IC1F, 4, 4)
	FIELD(CCMR1, ICPCS, 2, 2)
	FIELD(CCMR1, CC1S, 0, 2)
REG32(CCMR2, 0x1c)
	FIELD(CCMR2, O24CE, 15, 1)
	FIELD(CCMR2, OC4M, 12, 3)
	FIELD(CCMR2, OC4PE, 11, 1)
	FIELD(CCMR2, OC4FE, 10, 1)
	FIELD(CCMR2, OC3CE, 7, 1)
	FIELD(CCMR2, OC3M, 4, 3)
	FIELD(CCMR2, OC3PE, 3, 1)
	FIELD(CCMR2, OC3FE, 2, 1)
	FIELD(CCMR2, IC4F, 12, 4)
	FIELD(CCMR2, IC4PSC, 10, 2)
	FIELD(CCMR2, CC4S, 8, 2)
	FIELD(CCMR2, IC3F, 4, 4)
	FIELD(CCMR2, IC3PSC, 2, 2)
	FIELD(CCMR2, CC3S, 0, 2)
REG32(CCER, 0x20)
	FIELD(CCER, CC4NP, 15, 1)
	FIELD(CCER, CC4P, 13, 1)
	FIELD(CCER, CC4E, 12, 1)
	FIELD(CCER, CC3NP, 11, 1)
	FIELD(CCER, CC3P, 9, 1)
	FIELD(CCER, CC3E, 8, 1)
	FIELD(CCER, CC2NP, 7, 1)
	FIELD(CCER, CC2P, 5, 1)
	FIELD(CCER, CC2E, 4, 1)
	FIELD(CCER, CC1NP, 3, 1)
	FIELD(CCER, CC1P, 1, 1)
	FIELD(CCER, CC1E, 0, 1)
REG32(CNT, 0x24)
	FIELD(CNT, CNT_H, 16, 16)
	FIELD(CNT, CNT_L, 0, 16)
REG32(PSC, 0x28)
	FIELD(PSC, PSC, 0, 16)
REG32(ARR, 0x2c)
	FIELD(ARR, ARR_H, 16, 16)
	FIELD(ARR, ARR_L, 0, 16)
REG32(CCR1, 0x34)
	FIELD(CCR1, CCR1_H, 16, 16)
	FIELD(CCR1, CCR1_L, 0, 16)
REG32(CCR2, 0x38)
	FIELD(CCR2, CCR2_H, 16, 16)
	FIELD(CCR2, CCR2_L, 0, 16)
REG32(CCR3, 0x3c)
	FIELD(CCR3, CCR3_H, 16, 16)
	FIELD(CCR3, CCR3_L, 0, 16)
REG32(CCR4, 0x40)
	FIELD(CCR4, CCR4_H, 16, 16)
	FIELD(CCR4, CCR4_L, 0, 16)
REG32(DCR, 0x48)
	FIELD(DCR, DBL, 8, 5)
	FIELD(DCR, DBA, 0, 5)
REG32(DMAR, 0x4c)
	FIELD(DMAR, DMAB, 0, 16)
REG32(OR1, 0x50)
	FIELD(OR1, TI1_RMP, 0, 2)
REG32(OR2, 0x60)
	FIELD(OR2, ETRSEL, 14, 3)

static void stm32f7_tim3_register_reset(STM32F7TIM3 *t) {
	t->CR1 = 0x00000000;
	t->CR2 = 0x00000000;
	t->SMCR = 0x00000000;
	t->DIER = 0x00000000;
	t->SR = 0x00000000;
	t->EGR = 0x00000000;
	t->CCMR1 = 0x00000000;
	t->CCMR2 = 0x00000000;
	t->CCER = 0x00000000;
	t->CNT = 0x00000000;
	t->PSC = 0x00000000;
	t->ARR = 0x00000000;
	t->CCR1 = 0x00000000;
	t->CCR2 = 0x00000000;
	t->CCR3 = 0x00000000;
	t->CCR4 = 0x00000000;
	t->DCR = 0x00000000;
	t->DMAR = 0x00000000;
	t->OR1 = 0x00000000;
	t->OR2 = 0x00000000;
}

static uint64_t stm32f7_tim3_read(void *opaque, hwaddr offset, unsigned size) {
	STM32F7TIM3 *t = STM32F7_TIM3(opaque);
	uint64_t ret;

	switch (offset) {
		case A_CR1:
			ret = t->CR1;
			break;
		case A_CR2:
			ret = t->CR2;
			break;
		case A_SMCR:
			ret = t->SMCR;
			break;
		case A_DIER:
			ret = t->DIER;
			break;
		case A_SR:
			ret = t->SR;
			break;
		case A_CCMR1:
			ret = t->CCMR1;
			break;
		case A_CCMR2:
			ret = t->CCMR2;
			break;
		case A_CCER:
			ret = t->CCER;
			break;
		case A_CNT:
			ret = t->CNT;
			break;
		case A_PSC:
			ret = t->PSC;
			break;
		case A_ARR:
			ret = t->ARR;
			break;
		case A_CCR1:
			ret = t->CCR1;
			break;
		case A_CCR2:
			ret = t->CCR2;
			break;
		case A_CCR3:
			ret = t->CCR3;
			break;
		case A_CCR4:
			ret = t->CCR4;
			break;
		case A_DCR:
			ret = t->DCR;
			break;
		case A_DMAR:
			ret = t->DMAR;
			break;
		case A_OR1:
			ret = t->OR1;
			break;
		case A_OR2:
			ret = t->OR2;
			break;
		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F7 TIM3 read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32f7_tim3_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32F7TIM3 *t = STM32F7_TIM3(opaque);
	switch (offset) {
		case A_CR1:
			t->CR1 = value;
			break;
		case A_CR2:
			t->CR2 = value;
			break;
		case A_SMCR:
			t->SMCR = value;
			break;
		case A_DIER:
			t->DIER = value;
			break;
		case A_SR:
			t->SR = value;
			break;
		case A_EGR:
			t->EGR = value;
			break;
		case A_CCMR1:
			t->CCMR1 = value;
			break;
		case A_CCMR2:
			t->CCMR2 = value;
			break;
		case A_CCER:
			t->CCER = value;
			break;
		case A_CNT:
			t->CNT = value;
			break;
		case A_PSC:
			t->PSC = value;
			break;
		case A_ARR:
			t->ARR = value;
			break;
		case A_CCR1:
			t->CCR1 = value;
			break;
		case A_CCR2:
			t->CCR2 = value;
			break;
		case A_CCR3:
			t->CCR3 = value;
			break;
		case A_CCR4:
			t->CCR4 = value;
			break;
		case A_DCR:
			t->DCR = value;
			break;
		case A_DMAR:
			t->DMAR = value;
			break;
		case A_OR1:
			t->OR1 = value;
			break;
		case A_OR2:
			t->OR2 = value;
			break;
		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F7 TIM3 write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32f7_tim3_ops = {
	.read = stm32f7_tim3_read,
	.write = stm32f7_tim3_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32f7_tim3_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32F7TIM3 *t = STM32F7_TIM3(obj);

	memory_region_init_io(&(t->iomem), obj, &stm32f7_tim3_ops, t, "stm32f7-tim3", STM32F7_TIM3_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0 ; i < 1 ; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}
}

static void stm32f7_tim3_realize(DeviceState *dev, Error **errp) {
	return;
}

static void stm32f7_tim3_reset_enter(Object *obj, ResetType type) {
	STM32F7TIM3 *t = STM32F7_TIM3(obj);

	stm32f7_tim3_register_reset(t);
}

static const VMStateDescription stm32f7_tim3_vmstate = {
	.name = "stm32f7-tim3",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CR1, STM32F7TIM3),
		VMSTATE_UINT32(CR2, STM32F7TIM3),
		VMSTATE_UINT32(SMCR, STM32F7TIM3),
		VMSTATE_UINT32(DIER, STM32F7TIM3),
		VMSTATE_UINT32(SR, STM32F7TIM3),
		VMSTATE_UINT32(EGR, STM32F7TIM3),
		VMSTATE_UINT32(CCMR1, STM32F7TIM3),
		VMSTATE_UINT32(CCMR2, STM32F7TIM3),
		VMSTATE_UINT32(CCER, STM32F7TIM3),
		VMSTATE_UINT32(CNT, STM32F7TIM3),
		VMSTATE_UINT32(PSC, STM32F7TIM3),
		VMSTATE_UINT32(ARR, STM32F7TIM3),
		VMSTATE_UINT32(CCR1, STM32F7TIM3),
		VMSTATE_UINT32(CCR2, STM32F7TIM3),
		VMSTATE_UINT32(CCR3, STM32F7TIM3),
		VMSTATE_UINT32(CCR4, STM32F7TIM3),
		VMSTATE_UINT32(DCR, STM32F7TIM3),
		VMSTATE_UINT32(DMAR, STM32F7TIM3),
		VMSTATE_UINT32(OR1, STM32F7TIM3),
		VMSTATE_UINT32(OR2, STM32F7TIM3),
		VMSTATE_END_OF_LIST()
	}
};

static void stm32f7_tim3_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);

	dc->vmsd = &stm32f7_tim3_vmstate;
	dc->realize = stm32f7_tim3_realize;
	rc->phases.enter = stm32f7_tim3_reset_enter;
}

static const TypeInfo stm32f7_tim3_info = {
	.name = TYPE_STM32F7_TIM3,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32F7TIM3),
	.instance_init = stm32f7_tim3_init,
	.class_init = stm32f7_tim3_class_init,
};

static void stm32f7_tim3_register_types(void) {
	type_register_static(&stm32f7_tim3_info);
}

type_init(stm32f7_tim3_register_types);
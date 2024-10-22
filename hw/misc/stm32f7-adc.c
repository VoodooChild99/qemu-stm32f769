#include "qemu/osdep.h"
#include "qemu/log.h"
#include "qemu/bitops.h"
#include "hw/sysbus.h"
#include "hw/irq.h"
#include "migration/vmstate.h"
#include "hw/registerfields.h"
#include "hw/resettable.h"
#include "hw/qdev-properties-system.h"
#include "exec/cpu-common.h"
#include "hw/misc/stm32f7-adc.h"

#define STM32F7_ADC_SIZE				0x50

REG32(SR, 0x0)
	FIELD(SR, OVR, 5, 1)
	FIELD(SR, STRT, 4, 1)
	FIELD(SR, JSTRT, 3, 1)
	FIELD(SR, JEOC, 2, 1)
	FIELD(SR, EOC, 1, 1)
	FIELD(SR, AWD, 0, 1)
REG32(CR1, 0x4)
	FIELD(CR1, OVRIE, 26, 1)
	FIELD(CR1, RES, 24, 2)
	FIELD(CR1, AWDEN, 23, 1)
	FIELD(CR1, JAWDEN, 22, 1)
	FIELD(CR1, DISCNUM, 13, 3)
	FIELD(CR1, JDISCEN, 12, 1)
	FIELD(CR1, DISCEN, 11, 1)
	FIELD(CR1, JAUTO, 10, 1)
	FIELD(CR1, AWDSGL, 9, 1)
	FIELD(CR1, SCAN, 8, 1)
	FIELD(CR1, JEOCIE, 7, 1)
	FIELD(CR1, AWDIE, 6, 1)
	FIELD(CR1, EOCIE, 5, 1)
	FIELD(CR1, AWDCH, 0, 5)
REG32(CR2, 0x8)
	FIELD(CR2, SWSTART, 30, 1)
	FIELD(CR2, EXTEN, 28, 2)
	FIELD(CR2, EXTSEL, 24, 4)
	FIELD(CR2, JSWSTART, 22, 1)
	FIELD(CR2, JEXTEN, 20, 2)
	FIELD(CR2, JEXTSEL, 16, 4)
	FIELD(CR2, ALIGN, 11, 1)
	FIELD(CR2, EOCS, 10, 1)
	FIELD(CR2, DDS, 9, 1)
	FIELD(CR2, DMA, 8, 1)
	FIELD(CR2, CONT, 1, 1)
	FIELD(CR2, ADON, 0, 1)
REG32(SMPR1, 0xc)
	FIELD(SMPR1, SMPx_x, 0, 32)
REG32(SMPR2, 0x10)
	FIELD(SMPR2, SMPx_x, 0, 32)
REG32(JOFR1, 0x14)
	FIELD(JOFR1, JOFFSET1, 0, 12)
REG32(JOFR2, 0x18)
	FIELD(JOFR2, JOFFSET2, 0, 12)
REG32(JOFR3, 0x1c)
	FIELD(JOFR3, JOFFSET3, 0, 12)
REG32(JOFR4, 0x20)
	FIELD(JOFR4, JOFFSET4, 0, 12)
REG32(HTR, 0x24)
	FIELD(HTR, HT, 0, 12)
REG32(LTR, 0x28)
	FIELD(LTR, LT, 0, 12)
REG32(SQR1, 0x2c)
	FIELD(SQR1, L, 20, 4)
	FIELD(SQR1, SQ16, 15, 5)
	FIELD(SQR1, SQ15, 10, 5)
	FIELD(SQR1, SQ14, 5, 5)
	FIELD(SQR1, SQ13, 0, 5)
REG32(SQR2, 0x30)
	FIELD(SQR2, SQ12, 25, 5)
	FIELD(SQR2, SQ11, 20, 5)
	FIELD(SQR2, SQ10, 15, 5)
	FIELD(SQR2, SQ9, 10, 5)
	FIELD(SQR2, SQ8, 5, 5)
	FIELD(SQR2, SQ7, 0, 5)
REG32(SQR3, 0x34)
	FIELD(SQR3, SQ6, 25, 5)
	FIELD(SQR3, SQ5, 20, 5)
	FIELD(SQR3, SQ4, 15, 5)
	FIELD(SQR3, SQ3, 10, 5)
	FIELD(SQR3, SQ2, 5, 5)
	FIELD(SQR3, SQ1, 0, 5)
REG32(JSQR, 0x38)
	FIELD(JSQR, JL, 20, 2)
	FIELD(JSQR, JSQ4, 15, 5)
	FIELD(JSQR, JSQ3, 10, 5)
	FIELD(JSQR, JSQ2, 5, 5)
	FIELD(JSQR, JSQ1, 0, 5)
REG32(JDR1, 0x3c)
	FIELD(JDR1, JDATA, 0, 16)
REG32(JDR2, 0x40)
	FIELD(JDR2, JDATA, 0, 16)
REG32(JDR3, 0x44)
	FIELD(JDR3, JDATA, 0, 16)
REG32(JDR4, 0x48)
	FIELD(JDR4, JDATA, 0, 16)
REG32(DR, 0x4c)
	FIELD(DR, DATA, 0, 16)


static void stm32f7_adc_register_reset(STM32F7ADC *t) {
	t->SR = 0x0;
	t->CR1 = 0x0;
	t->CR2 = 0x0;
	t->SMPR1 = 0x0;
	t->SMPR2 = 0x0;
	t->JOFR1 = 0x0;
	t->JOFR2 = 0x0;
	t->JOFR3 = 0x0;
	t->JOFR4 = 0x0;
	t->HTR = 0xfff;
	t->LTR = 0x0;
	t->SQR1 = 0x0;
	t->SQR2 = 0x0;
	t->SQR3 = 0x0;
	t->JSQR = 0x0;
	t->JDR1 = 0x0;
	t->JDR2 = 0x0;
	t->JDR3 = 0x0;
	t->JDR4 = 0x0;
	t->DR = 0x0;

}

static uint64_t stm32f7_adc_read(void *opaque, hwaddr offset, unsigned size) {
	STM32F7ADC *t = STM32F7_ADC(opaque);
	uint64_t ret;

	switch (offset) {
		case A_SR:
			ret = t->SR;
			break;
		case A_CR1:
			ret = t->CR1;
			break;
		case A_CR2:
			ret = t->CR2;
			break;
		case A_SMPR1:
			ret = t->SMPR1;
			break;
		case A_SMPR2:
			ret = t->SMPR2;
			break;
		case A_JOFR1:
			ret = t->JOFR1;
			break;
		case A_JOFR2:
			ret = t->JOFR2;
			break;
		case A_JOFR3:
			ret = t->JOFR3;
			break;
		case A_JOFR4:
			ret = t->JOFR4;
			break;
		case A_HTR:
			ret = t->HTR;
			break;
		case A_LTR:
			ret = t->LTR;
			break;
		case A_SQR1:
			ret = t->SQR1;
			break;
		case A_SQR2:
			ret = t->SQR2;
			break;
		case A_SQR3:
			ret = t->SQR3;
			break;
		case A_JSQR:
			ret = t->JSQR;
			break;
		case A_JDR1:
			ret = t->JDR1;
			break;
		case A_JDR2:
			ret = t->JDR2;
			break;
		case A_JDR3:
			ret = t->JDR3;
			break;
		case A_JDR4:
			ret = t->JDR4;
			break;
		case A_DR:
			ret = t->DR;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F7 ADC read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32f7_adc_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32F7ADC *t = STM32F7_ADC(opaque);


	switch (offset) {
		case A_SR ... A_SR + 3:
			value = value << ((offset - A_SR) << 3);
			offset = A_SR;
			break;
		case A_CR1 ... A_CR1 + 3:
			value = value << ((offset - A_CR1) << 3);
			offset = A_CR1;
			break;
		case A_CR2 ... A_CR2 + 3:
			value = value << ((offset - A_CR2) << 3);
			offset = A_CR2;
			break;
		case A_SMPR1 ... A_SMPR1 + 3:
			value = value << ((offset - A_SMPR1) << 3);
			offset = A_SMPR1;
			break;
		case A_SMPR2 ... A_SMPR2 + 3:
			value = value << ((offset - A_SMPR2) << 3);
			offset = A_SMPR2;
			break;
		case A_JOFR1 ... A_JOFR1 + 3:
			value = value << ((offset - A_JOFR1) << 3);
			offset = A_JOFR1;
			break;
		case A_JOFR2 ... A_JOFR2 + 3:
			value = value << ((offset - A_JOFR2) << 3);
			offset = A_JOFR2;
			break;
		case A_JOFR3 ... A_JOFR3 + 3:
			value = value << ((offset - A_JOFR3) << 3);
			offset = A_JOFR3;
			break;
		case A_JOFR4 ... A_JOFR4 + 3:
			value = value << ((offset - A_JOFR4) << 3);
			offset = A_JOFR4;
			break;
		case A_HTR ... A_HTR + 3:
			value = value << ((offset - A_HTR) << 3);
			offset = A_HTR;
			break;
		case A_LTR ... A_LTR + 3:
			value = value << ((offset - A_LTR) << 3);
			offset = A_LTR;
			break;
		case A_SQR1 ... A_SQR1 + 3:
			value = value << ((offset - A_SQR1) << 3);
			offset = A_SQR1;
			break;
		case A_SQR2 ... A_SQR2 + 3:
			value = value << ((offset - A_SQR2) << 3);
			offset = A_SQR2;
			break;
		case A_SQR3 ... A_SQR3 + 3:
			value = value << ((offset - A_SQR3) << 3);
			offset = A_SQR3;
			break;
		case A_JSQR ... A_JSQR + 3:
			value = value << ((offset - A_JSQR) << 3);
			offset = A_JSQR;
			break;

		default: break;
	}

	switch (offset) {
		case A_SR:
			t->SR = value;
			break;
		case A_CR1:
			t->CR1 = value;
			break;
		case A_CR2:
			t->CR2 = value;
			break;
		case A_SMPR1:
			t->SMPR1 = value;
			break;
		case A_SMPR2:
			t->SMPR2 = value;
			break;
		case A_JOFR1:
			t->JOFR1 = value;
			break;
		case A_JOFR2:
			t->JOFR2 = value;
			break;
		case A_JOFR3:
			t->JOFR3 = value;
			break;
		case A_JOFR4:
			t->JOFR4 = value;
			break;
		case A_HTR:
			t->HTR = value;
			break;
		case A_LTR:
			t->LTR = value;
			break;
		case A_SQR1:
			t->SQR1 = value;
			break;
		case A_SQR2:
			t->SQR2 = value;
			break;
		case A_SQR3:
			t->SQR3 = value;
			break;
		case A_JSQR:
			t->JSQR = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F7 ADC write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32f7_adc_ops = {
	.read = stm32f7_adc_read,
	.write = stm32f7_adc_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32f7_adc_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32F7ADC *t = STM32F7_ADC(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32f7_adc_ops, t, "stm32f7-adc", STM32F7_ADC_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0; i < 1; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

}

static void stm32f7_adc_realize(DeviceState *dev, Error **errp) {
	return;

}

static void stm32f7_adc_reset_enter(Object *obj, ResetType type) {
	STM32F7ADC *t = STM32F7_ADC(obj);
	stm32f7_adc_register_reset(t);
}

static const VMStateDescription stm32f7_adc_vmstate = {
	.name = "stm32f7-adc",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(SR, STM32F7ADC),
		VMSTATE_UINT32(CR1, STM32F7ADC),
		VMSTATE_UINT32(CR2, STM32F7ADC),
		VMSTATE_UINT32(SMPR1, STM32F7ADC),
		VMSTATE_UINT32(SMPR2, STM32F7ADC),
		VMSTATE_UINT32(JOFR1, STM32F7ADC),
		VMSTATE_UINT32(JOFR2, STM32F7ADC),
		VMSTATE_UINT32(JOFR3, STM32F7ADC),
		VMSTATE_UINT32(JOFR4, STM32F7ADC),
		VMSTATE_UINT32(HTR, STM32F7ADC),
		VMSTATE_UINT32(LTR, STM32F7ADC),
		VMSTATE_UINT32(SQR1, STM32F7ADC),
		VMSTATE_UINT32(SQR2, STM32F7ADC),
		VMSTATE_UINT32(SQR3, STM32F7ADC),
		VMSTATE_UINT32(JSQR, STM32F7ADC),
		VMSTATE_UINT32(JDR1, STM32F7ADC),
		VMSTATE_UINT32(JDR2, STM32F7ADC),
		VMSTATE_UINT32(JDR3, STM32F7ADC),
		VMSTATE_UINT32(JDR4, STM32F7ADC),
		VMSTATE_UINT32(DR, STM32F7ADC),

		VMSTATE_END_OF_LIST()
	}
};

static void stm32f7_adc_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32f7_adc_vmstate;
	dc->realize = stm32f7_adc_realize;
	rc->phases.enter = stm32f7_adc_reset_enter;

}

static const TypeInfo stm32f7_adc_info = {
	.name = TYPE_STM32F7_ADC,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32F7ADC),
	.instance_init = stm32f7_adc_init,
	.class_init = stm32f7_adc_class_init,
};

static void stm32f7_adc_register_types(void) {
	type_register_static(&stm32f7_adc_info);
}

type_init(stm32f7_adc_register_types);
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
#include "hw/misc/stm32f7-adc_common.h"

#define STM32F7_ADC_COMMON_SIZE				0xc

REG32(CSR, 0x0)
	FIELD(CSR, AWD1, 0, 1)
	FIELD(CSR, EOC1, 1, 1)
	FIELD(CSR, JEOC1, 2, 1)
	FIELD(CSR, JSTRT1, 3, 1)
	FIELD(CSR, STRT1, 4, 1)
	FIELD(CSR, OVR1, 5, 1)
	FIELD(CSR, AWD2, 8, 1)
	FIELD(CSR, EOC2, 9, 1)
	FIELD(CSR, JEOC2, 10, 1)
	FIELD(CSR, JSTRT2, 11, 1)
	FIELD(CSR, STRT2, 12, 1)
	FIELD(CSR, OVR2, 13, 1)
	FIELD(CSR, AWD3, 16, 1)
	FIELD(CSR, EOC3, 17, 1)
	FIELD(CSR, JEOC3, 18, 1)
	FIELD(CSR, JSTRT3, 19, 1)
	FIELD(CSR, STRT3, 20, 1)
	FIELD(CSR, OVR3, 21, 1)
REG32(CCR, 0x4)
	FIELD(CCR, MULTI, 0, 5)
	FIELD(CCR, DELAY, 8, 4)
	FIELD(CCR, DDS, 13, 1)
	FIELD(CCR, DMA, 14, 2)
	FIELD(CCR, ADCPRE, 16, 2)
	FIELD(CCR, VBATE, 22, 1)
	FIELD(CCR, TSVREFE, 23, 1)
REG32(CDR, 0x8)
	FIELD(CDR, DATA1, 0, 16)
	FIELD(CDR, DATA2, 16, 16)


static void stm32f7_adc_common_register_reset(STM32F7ADC_COMMON *t) {
	t->CSR = 0x0;
	t->CCR = 0x0;
	t->CDR = 0x0;

}

static uint64_t stm32f7_adc_common_read(void *opaque, hwaddr offset, unsigned size) {
	STM32F7ADC_COMMON *t = STM32F7_ADC_COMMON(opaque);
	uint64_t ret;

	switch (offset) {
		case A_CSR:
			ret = t->CSR;
			break;
		case A_CCR:
			ret = t->CCR;
			break;
		case A_CDR:
			ret = t->CDR;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F7 ADC_COMMON read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32f7_adc_common_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32F7ADC_COMMON *t = STM32F7_ADC_COMMON(opaque);


	switch (offset) {
		case A_CCR ... A_CCR + 3:
			value = value << ((offset - A_CCR) << 3);
			offset = A_CCR;
			break;

		default: break;
	}

	switch (offset) {
		case A_CCR:
			t->CCR = value;
			break;

		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F7 ADC_COMMON write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32f7_adc_common_ops = {
	.read = stm32f7_adc_common_read,
	.write = stm32f7_adc_common_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32f7_adc_common_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32F7ADC_COMMON *t = STM32F7_ADC_COMMON(obj);
	memory_region_init_io(&(t->iomem), obj, &stm32f7_adc_common_ops, t, "stm32f7-adc_common", STM32F7_ADC_COMMON_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));

}

static void stm32f7_adc_common_realize(DeviceState *dev, Error **errp) {
	return;

}

static void stm32f7_adc_common_reset_enter(Object *obj, ResetType type) {
	STM32F7ADC_COMMON *t = STM32F7_ADC_COMMON(obj);
	stm32f7_adc_common_register_reset(t);
}

static const VMStateDescription stm32f7_adc_common_vmstate = {
	.name = "stm32f7-adc_common",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CSR, STM32F7ADC_COMMON),
		VMSTATE_UINT32(CCR, STM32F7ADC_COMMON),
		VMSTATE_UINT32(CDR, STM32F7ADC_COMMON),

		VMSTATE_END_OF_LIST()
	}
};

static void stm32f7_adc_common_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);
	dc->vmsd = &stm32f7_adc_common_vmstate;
	dc->realize = stm32f7_adc_common_realize;
	rc->phases.enter = stm32f7_adc_common_reset_enter;

}

static const TypeInfo stm32f7_adc_common_info = {
	.name = TYPE_STM32F7_ADC_COMMON,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32F7ADC_COMMON),
	.instance_init = stm32f7_adc_common_init,
	.class_init = stm32f7_adc_common_class_init,
};

static void stm32f7_adc_common_register_types(void) {
	type_register_static(&stm32f7_adc_common_info);
}

type_init(stm32f7_adc_common_register_types);
#include "qemu/osdep.h"
#include "qemu/log.h"
#include "qemu/bitops.h"
#include "hw/sysbus.h"
#include "hw/irq.h"
#include "migration/vmstate.h"
#include "hw/registerfields.h"
#include "hw/resettable.h"
#include "hw/misc/stm32f7-syscfg.h"
#include "hw/qdev-properties.h"
#include "exec/cpu-common.h"

#define STM32F7_SYSCFG_SIZE				0x400

REG32(MEMRM, 0x0)
	FIELD(MEMRM, MEM_MODE, 0, 3)
	FIELD(MEMRM, FB_MODE, 8, 1)
	FIELD(MEMRM, SWP_FMC, 10, 2)
REG32(PMC, 0x4)
	FIELD(PMC, MII_RMII_SEL, 23, 1)
	FIELD(PMC, ADC1DC2, 16, 1)
	FIELD(PMC, ADC2DC2, 17, 1)
	FIELD(PMC, ADC3DC2, 18, 1)
REG32(EXTICR1, 0x8)
	FIELD(EXTICR1, EXTI3, 12, 4)
	FIELD(EXTICR1, EXTI2, 8, 4)
	FIELD(EXTICR1, EXTI1, 4, 4)
	FIELD(EXTICR1, EXTI0, 0, 4)
REG32(EXTICR2, 0xc)
	FIELD(EXTICR2, EXTI7, 12, 4)
	FIELD(EXTICR2, EXTI6, 8, 4)
	FIELD(EXTICR2, EXTI5, 4, 4)
	FIELD(EXTICR2, EXTI4, 0, 4)
REG32(EXTICR3, 0x10)
	FIELD(EXTICR3, EXTI11, 12, 4)
	FIELD(EXTICR3, EXTI10, 8, 4)
	FIELD(EXTICR3, EXTI9, 4, 4)
	FIELD(EXTICR3, EXTI8, 0, 4)
REG32(EXTICR4, 0x14)
	FIELD(EXTICR4, EXTI15, 12, 4)
	FIELD(EXTICR4, EXTI14, 8, 4)
	FIELD(EXTICR4, EXTI13, 4, 4)
	FIELD(EXTICR4, EXTI12, 0, 4)
REG32(CMPCR, 0x20)
	FIELD(CMPCR, READY, 8, 1)
	FIELD(CMPCR, CMP_PD, 0, 1)

static void stm32f7_syscfg_register_reset(STM32F7SYSCFG *t) {
	t->MEMRM = 0x00000000;
	t->PMC = 0x00000000;
	t->EXTICR1 = 0x00000000;
	t->EXTICR2 = 0x00000000;
	t->EXTICR3 = 0x00000000;
	t->EXTICR4 = 0x00000000;
	t->CMPCR = 0x00000000;
}

static uint64_t stm32f7_syscfg_read(void *opaque, hwaddr offset, unsigned size) {
	STM32F7SYSCFG *t = STM32F7_SYSCFG(opaque);
	uint64_t ret;

	switch (offset) {
		case A_MEMRM:
			ret = t->MEMRM;
			break;
		case A_PMC:
			ret = t->PMC;
			break;
		case A_EXTICR1:
			ret = t->EXTICR1;
			break;
		case A_EXTICR2:
			ret = t->EXTICR2;
			break;
		case A_EXTICR3:
			ret = t->EXTICR3;
			break;
		case A_EXTICR4:
			ret = t->EXTICR4;
			break;
		case A_CMPCR:
			ret = t->CMPCR;
			break;
		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F7 SYSCFG read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32f7_syscfg_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32F7SYSCFG *t = STM32F7_SYSCFG(opaque);
	switch (offset) {
		case A_MEMRM:
			t->MEMRM = value;
			break;
		case A_PMC:
			t->PMC = value;
			break;
		case A_EXTICR1:
			t->EXTICR1 = value;
			break;
		case A_EXTICR2:
			t->EXTICR2 = value;
			break;
		case A_EXTICR3:
			t->EXTICR3 = value;
			break;
		case A_EXTICR4:
			t->EXTICR4 = value;
			break;
		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F7 SYSCFG write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32f7_syscfg_ops = {
	.read = stm32f7_syscfg_read,
	.write = stm32f7_syscfg_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32f7_syscfg_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32F7SYSCFG *t = STM32F7_SYSCFG(obj);

	memory_region_init_io(&(t->iomem), obj, &stm32f7_syscfg_ops, t, "stm32f7-syscfg", STM32F7_SYSCFG_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
}

static void stm32f7_syscfg_realize(DeviceState *dev, Error **errp) {
	return;
}

static void stm32f7_syscfg_reset_enter(Object *obj, ResetType type) {
	STM32F7SYSCFG *t = STM32F7_SYSCFG(obj);

	stm32f7_syscfg_register_reset(t);
}

static const VMStateDescription stm32f7_syscfg_vmstate = {
	.name = "stm32f7-syscfg",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(MEMRM, STM32F7SYSCFG),
		VMSTATE_UINT32(PMC, STM32F7SYSCFG),
		VMSTATE_UINT32(EXTICR1, STM32F7SYSCFG),
		VMSTATE_UINT32(EXTICR2, STM32F7SYSCFG),
		VMSTATE_UINT32(EXTICR3, STM32F7SYSCFG),
		VMSTATE_UINT32(EXTICR4, STM32F7SYSCFG),
		VMSTATE_UINT32(CMPCR, STM32F7SYSCFG),
		VMSTATE_END_OF_LIST()
	}
};

static void stm32f7_syscfg_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);

	dc->vmsd = &stm32f7_syscfg_vmstate;
	dc->realize = stm32f7_syscfg_realize;
	rc->phases.enter = stm32f7_syscfg_reset_enter;
}

static const TypeInfo stm32f7_syscfg_info = {
	.name = TYPE_STM32F7_SYSCFG,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32F7SYSCFG),
	.instance_init = stm32f7_syscfg_init,
	.class_init = stm32f7_syscfg_class_init,
};

static void stm32f7_syscfg_register_types(void) {
	type_register_static(&stm32f7_syscfg_info);
}

type_init(stm32f7_syscfg_register_types);
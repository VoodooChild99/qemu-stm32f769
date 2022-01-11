/*
 * STM32F7XX USART
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
#include "hw/char/stm32f7-usart.h"
#include "hw/qdev-properties-system.h"
#include "exec/cpu-common.h"

#define STM32F7_USART_SIZE				0x400

REG32(CR1, 0x0)
	FIELD(CR1, M1, 28, 1)
	FIELD(CR1, EOBIE, 27, 1)
	FIELD(CR1, RTOIE, 26, 1)
	FIELD(CR1, DEAT4, 25, 1)
	FIELD(CR1, DEAT3, 24, 1)
	FIELD(CR1, DEAT2, 23, 1)
	FIELD(CR1, DEAT1, 22, 1)
	FIELD(CR1, DEAT0, 21, 1)
	FIELD(CR1, DEDT4, 20, 1)
	FIELD(CR1, DEDT3, 19, 1)
	FIELD(CR1, DEDT2, 18, 1)
	FIELD(CR1, DEDT1, 17, 1)
	FIELD(CR1, DEDT0, 16, 1)
	FIELD(CR1, OVER8, 15, 1)
	FIELD(CR1, CMIE, 14, 1)
	FIELD(CR1, MME, 13, 1)
	FIELD(CR1, M0, 12, 1)
	FIELD(CR1, WAKE, 11, 1)
	FIELD(CR1, PCE, 10, 1)
	FIELD(CR1, PS, 9, 1)
	FIELD(CR1, PEIE, 8, 1)
	FIELD(CR1, TXEIE, 7, 1)
	FIELD(CR1, TCIE, 6, 1)
	FIELD(CR1, RXNEIE, 5, 1)
	FIELD(CR1, IDLEIE, 4, 1)
	FIELD(CR1, TE, 3, 1)
	FIELD(CR1, RE, 2, 1)
	FIELD(CR1, UESM, 1, 1)
	FIELD(CR1, UE, 0, 1)
REG32(CR2, 0x4)
	FIELD(CR2, ADD4_7, 28, 4)
	FIELD(CR2, ADD0_3, 24, 4)
	FIELD(CR2, RTOEN, 23, 1)
	FIELD(CR2, ABRMOD1, 22, 1)
	FIELD(CR2, ABRMOD0, 21, 1)
	FIELD(CR2, ABREN, 20, 1)
	FIELD(CR2, MSBFIRST, 19, 1)
	FIELD(CR2, TAINV, 18, 1)
	FIELD(CR2, TXINV, 17, 1)
	FIELD(CR2, RXINV, 16, 1)
	FIELD(CR2, SWAP, 15, 1)
	FIELD(CR2, LINEN, 14, 1)
	FIELD(CR2, STOP, 12, 2)
	FIELD(CR2, CLKEN, 11, 1)
	FIELD(CR2, CPOL, 10, 1)
	FIELD(CR2, CPHA, 9, 1)
	FIELD(CR2, LBCL, 8, 1)
	FIELD(CR2, LBDIE, 6, 1)
	FIELD(CR2, LBDL, 5, 1)
	FIELD(CR2, ADDM7, 4, 1)
REG32(CR3, 0x8)
	FIELD(CR3, TCBGTIE, 24, 1)
	FIELD(CR3, UCESM, 23, 1)
	FIELD(CR3, WUFIE, 22, 1)
	FIELD(CR3, WUS, 20, 2)
	FIELD(CR3, SCARCNT, 17, 3)
	FIELD(CR3, DEP, 15, 1)
	FIELD(CR3, DEM, 14, 1)
	FIELD(CR3, DDRE, 13, 1)
	FIELD(CR3, OVRDIS, 12, 1)
	FIELD(CR3, ONEBIT, 11, 1)
	FIELD(CR3, CTSIE, 10, 1)
	FIELD(CR3, CTSE, 9, 1)
	FIELD(CR3, RTSE, 8, 1)
	FIELD(CR3, DMAT, 7, 1)
	FIELD(CR3, DMAR, 6, 1)
	FIELD(CR3, SCEN, 5, 1)
	FIELD(CR3, NACK, 4, 1)
	FIELD(CR3, HDSEL, 3, 1)
	FIELD(CR3, IRLP, 2, 1)
	FIELD(CR3, IREN, 1, 1)
	FIELD(CR3, EIE, 0, 1)
REG32(BRR, 0xc)
	FIELD(BRR, DIV_MANTISSA, 4, 12)
	FIELD(BRR, DIV_FRACTION, 0, 4)
REG32(GTPR, 0x10)
	FIELD(GTPR, GT, 8, 8)
	FIELD(GTPR, PSC, 0, 8)
REG32(RTOR, 0x14)
	FIELD(RTOR, BLEN, 24, 8)
	FIELD(RTOR, RTO, 0, 24)
REG32(RQR, 0x18)
	FIELD(RQR, TXFRQ, 4, 1)
	FIELD(RQR, RXFRQ, 3, 1)
	FIELD(RQR, MMRQ, 2, 1)
	FIELD(RQR, SBKRQ, 1, 1)
	FIELD(RQR, ABRRQ, 0, 1)
REG32(ISR, 0x1c)
	FIELD(ISR, TCBGT, 25, 1)
	FIELD(ISR, REACK, 22, 1)
	FIELD(ISR, TEACK, 21, 1)
	FIELD(ISR, WUF, 20, 1)
	FIELD(ISR, RWU, 19, 1)
	FIELD(ISR, SBKF, 18, 1)
	FIELD(ISR, CMF, 17, 1)
	FIELD(ISR, BUSY, 16, 1)
	FIELD(ISR, ABRF, 15, 1)
	FIELD(ISR, ABRE, 14, 1)
	FIELD(ISR, EOBF, 12, 1)
	FIELD(ISR, RTOF, 11, 1)
	FIELD(ISR, CTS, 10, 1)
	FIELD(ISR, CTSIF, 9, 1)
	FIELD(ISR, LBDF, 8, 1)
	FIELD(ISR, TXE, 7, 1)
	FIELD(ISR, TC, 6, 1)
	FIELD(ISR, RXNE, 5, 1)
	FIELD(ISR, IDLE, 4, 1)
	FIELD(ISR, ORE, 3, 1)
	FIELD(ISR, NF, 2, 1)
	FIELD(ISR, FE, 1, 1)
	FIELD(ISR, PE, 0, 1)
REG32(ICR, 0x20)
	FIELD(ICR, WUCF, 20, 1)
	FIELD(ICR, CMCF, 17, 1)
	FIELD(ICR, EOBCF, 12, 1)
	FIELD(ICR, RTOCF, 11, 1)
	FIELD(ICR, CTSCF, 9, 1)
	FIELD(ICR, LBDCF, 8, 1)
	FIELD(ICR, TCBGTCF, 7, 1)
	FIELD(ICR, TCCF, 6, 1)
	FIELD(ICR, IDLECF, 4, 1)
	FIELD(ICR, ORECF, 3, 1)
	FIELD(ICR, NCF, 2, 1)
	FIELD(ICR, FECF, 1, 1)
	FIELD(ICR, PECF, 0, 1)
REG32(RDR, 0x24)
	FIELD(RDR, RDR, 0, 9)
REG32(TDR, 0x28)
	FIELD(TDR, TDR, 0, 9)

static void stm32f7_usart_fire_irq(STM32F7USART *t) {
	int conditions = ((t->CR1 & R_CR1_CMIE_MASK) && (t->ISR & R_ISR_CMF_MASK)) ||
					 ((t->CR1 & R_CR1_EOBIE_MASK) && (t->ISR & R_ISR_EOBF_MASK)) ||
					 ((t->CR1 & R_CR1_RTOIE_MASK) && (t->ISR & R_ISR_RTOF_MASK)) ||
					 ((t->CR1 & R_CR1_TXEIE_MASK) && (t->ISR & R_ISR_TXE_MASK)) ||
					 ((t->CR1 & R_CR1_TCIE_MASK) && (t->ISR & R_ISR_TC_MASK)) ||
					 ((t->CR1 & R_CR1_RXNEIE_MASK) && (t->ISR & R_ISR_RXNE_MASK)) ||
					 ((t->CR1 & R_CR1_IDLEIE_MASK) && (t->ISR & R_ISR_IDLE_MASK)) ||
					 ((t->CR1 & (R_CR1_RXNEIE_MASK | R_CR1_PEIE_MASK | R_CR1_TXEIE_LENGTH | R_CR1_IDLEIE_LENGTH)) && (t->ISR & R_ISR_ORE_MASK)) ||
					 ((t->CR1 & (R_CR1_RXNEIE_MASK | R_CR1_PEIE_MASK | R_CR1_TXEIE_LENGTH | R_CR1_IDLEIE_LENGTH)) && (t->ISR & R_ISR_FE_MASK)) ||
					 ((t->CR1 & R_CR1_PEIE_MASK) && (t->ISR & R_ISR_PE_MASK)) ||
					 ((t->CR2 & R_CR2_LBDIE_MASK) && (t->ISR & R_ISR_LBDF_MASK)) || 
					 ((t->CR3 & R_CR3_TCBGTIE_MASK) && (t->ISR & R_ISR_TCBGT_MASK)) ||
					 ((t->CR3 & R_CR3_WUFIE_MASK) && (t->ISR & R_ISR_WUF_MASK)) ||
					 ((t->CR3 & R_CR3_CTSIE_MASK) && (t->ISR & R_ISR_CTSIF_MASK));

	qemu_set_irq(t->irq[0], conditions);
}

static int stm32f7_usart_can_receive(void *opaque) {
	STM32F7USART *t = STM32F7_USART(opaque);

	if ((t->CR1 & (R_CR1_RE_MASK | R_CR1_UE_MASK)) == (R_CR1_RE_MASK | R_CR1_UE_MASK)) {
		return 1;
	}

	return 0;
}

static void stm32f7_usart_receive(void *opaque, const uint8_t *buf, int size) {
	STM32F7USART *t = STM32F7_USART(opaque);

	t->ISR |= R_ISR_BUSY_MASK;
	t->RDR = *buf;
	t->ISR &= (~R_ISR_BUSY_MASK);
	t->ISR |= (R_ISR_RXNE_MASK | R_ISR_IDLE_MASK);
	if (t->RDR == ((t->CR2 & (R_CR2_ADD0_3_MASK | R_CR2_ADD4_7_MASK)) >> R_CR2_ADD0_3_SHIFT)) {
		t->ISR |= R_ISR_CMF_MASK;
	}
	stm32f7_usart_fire_irq(t);
}

// static void stm32f7_usart_update_params(STM32F7USART *t) {

// }

static gboolean stm32f7_usart_transmit(
	void *do_not_use, GIOCondition cond, void *opaque
) {
	STM32F7USART *t = STM32F7_USART(opaque);
	int ret;

	t->watch_tag = 0;

	if (!(t->CR1 & R_CR1_TE_MASK) || !(t->CR1 & R_CR1_UE_MASK)) {
		return FALSE;
	}

	t->ISR &= (~(R_ISR_TXE_MASK | R_ISR_TC_MASK));
	stm32f7_usart_fire_irq(t);

	ret = qemu_chr_fe_write(&(t->chr), (uint8_t*)&(t->TDR), 1);
    if (ret <= 0) {
        t->watch_tag = qemu_chr_fe_add_watch(&(t->chr), G_IO_OUT | G_IO_HUP,
                                             stm32f7_usart_transmit, t);
        if (!t->watch_tag) {
            /* Most common reason to be here is "no chardev backend":
             * just insta-drain the buffer, so the serial output
             * goes into a void, rather than blocking the guest.
             */
            goto buffer_drained;
        }

        return FALSE;
    }

buffer_drained:
    t->ISR |= (R_ISR_TXE_MASK | R_ISR_TC_MASK);
	stm32f7_usart_fire_irq(t);

    return FALSE;
}

static void stm32f7_usart_register_reset(STM32F7USART *t) {
	t->CR1 = 0x00000000;
	t->CR2 = 0x00000000;
	t->CR3 = 0x00000000;
	t->BRR = 0x00000000;
	t->GTPR = 0x00000000;
	t->RTOR = 0x00000000;
	t->RQR = 0x00000000;
	t->ISR = 0x000000c0;
	t->ICR = 0x00000000;
	t->RDR = 0x00000000;
	t->TDR = 0x00000000;
}

static uint64_t stm32f7_usart_read(void *opaque, hwaddr offset, unsigned size) {
	STM32F7USART *t = STM32F7_USART(opaque);
	uint64_t ret;

	switch (offset) {
		case A_CR1:
			ret = t->CR1;
			break;
		case A_CR2:
			ret = t->CR2;
			break;
		case A_CR3:
			ret = t->CR3;
			break;
		case A_BRR:
			ret = t->BRR;
			break;
		case A_GTPR:
			ret = t->GTPR;
			break;
		case A_RTOR:
			ret = t->RTOR;
			break;
		case A_ISR:
			ret = t->ISR;
			break;
		case A_RDR:
			ret = t->RDR;
			t->ISR &= (~R_ISR_RXNE_MASK);
			qemu_chr_fe_accept_input(&(t->chr));
			stm32f7_usart_fire_irq(t);
			break;
		case A_TDR:
			ret = t->TDR;
			break;
		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F7 USART read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32f7_usart_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32F7USART *t = STM32F7_USART(opaque);
	switch (offset) {
		case A_CR1:
			t->ISR = (t->ISR & (~(R_ISR_REACK_MASK | R_ISR_TEACK_MASK))) | (((value & (R_CR1_RE_MASK | R_CR1_TE_MASK)) >> R_CR1_RE_SHIFT) << R_ISR_TEACK_SHIFT);
			t->CR1 = value;
			break;
		case A_CR2:
			t->CR2 = value;
			break;
		case A_CR3:
			t->CR3 = value;
			break;
		case A_BRR:
			t->BRR = value;
			break;
		case A_GTPR:
			t->GTPR = value;
			break;
		case A_RTOR:
			t->RTOR = value;
			break;
		case A_RQR:
			t->RQR = value;
			break;
		case A_ICR:
			if (value & R_ICR_WUCF_MASK) {
				t->ISR &= (~R_ISR_WUF_MASK);
			}
			if (value & R_ICR_CMCF_MASK) {
				t->ISR &= (~R_ISR_CMF_MASK);
			}
			if (value & R_ICR_EOBCF_MASK) {
				t->ISR &= (~R_ISR_EOBF_MASK);
			}
			if (value & R_ICR_RTOCF_MASK) {
				t->ISR &= (~R_ISR_RTOF_MASK);
			}
			if (value & R_ICR_CTSCF_MASK) {
				t->ISR &= (~R_ISR_CTSIF_MASK);
			}
			if (value & R_ICR_LBDCF_MASK) {
				t->ISR &= (~R_ISR_LBDF_MASK);
			}
			if (value & R_ICR_TCBGTCF_MASK) {
				t->ISR &= (~R_ISR_TCBGT_MASK);
			}
			if (value & R_ICR_TCCF_MASK) {
				t->ISR &= (~R_ISR_TC_MASK);
			}
			if (value & R_ICR_IDLECF_MASK) {
				t->ISR &= (~R_ISR_IDLE_MASK);
			}
			if (value & R_ICR_ORECF_MASK) {
				t->ISR &= (~R_ISR_ORE_MASK);
			}
			if (value & R_ICR_NCF_MASK) {
				t->ISR &= (~R_ISR_NF_MASK);
			}
			if (value & R_ICR_FECF_MASK) {
				t->ISR &= (~R_ISR_FE_MASK);
			}
			if (value & R_ICR_PECF_MASK) {
				t->ISR &= (~R_ISR_PE_MASK);
			}
			stm32f7_usart_fire_irq(t);
			break;
		case A_TDR:
			t->TDR = value;
			stm32f7_usart_transmit(NULL, G_IO_OUT, t);
			break;
		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F7 USART write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32f7_usart_ops = {
	.read = stm32f7_usart_read,
	.write = stm32f7_usart_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32f7_usart_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32F7USART *t = STM32F7_USART(obj);

	memory_region_init_io(&(t->iomem), obj, &stm32f7_usart_ops, t, "stm32f7-usart", STM32F7_USART_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0 ; i < 1 ; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}
}

static void stm32f7_usart_realize(DeviceState *dev, Error **errp) {
	STM32F7USART *t = STM32F7_USART(dev);

	qemu_chr_fe_set_handlers(
		&(t->chr), stm32f7_usart_can_receive, stm32f7_usart_receive,
		NULL, NULL, t, NULL, true
	);
}

static void stm32f7_usart_reset_enter(Object *obj, ResetType type) {
	STM32F7USART *t = STM32F7_USART(obj);

	stm32f7_usart_register_reset(t);
}

static const VMStateDescription stm32f7_usart_vmstate = {
	.name = "stm32f7-usart",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CR1, STM32F7USART),
		VMSTATE_UINT32(CR2, STM32F7USART),
		VMSTATE_UINT32(CR3, STM32F7USART),
		VMSTATE_UINT32(BRR, STM32F7USART),
		VMSTATE_UINT32(GTPR, STM32F7USART),
		VMSTATE_UINT32(RTOR, STM32F7USART),
		VMSTATE_UINT32(RQR, STM32F7USART),
		VMSTATE_UINT32(ISR, STM32F7USART),
		VMSTATE_UINT32(ICR, STM32F7USART),
		VMSTATE_UINT32(RDR, STM32F7USART),
		VMSTATE_UINT32(TDR, STM32F7USART),
		VMSTATE_END_OF_LIST()
	}
};

static Property stm32f7_usart_properties[] = {
	DEFINE_PROP_CHR("chardev", STM32F7USART, chr),
	DEFINE_PROP_END_OF_LIST()
};

static void stm32f7_usart_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);

	dc->vmsd = &stm32f7_usart_vmstate;
	dc->realize = stm32f7_usart_realize;
	rc->phases.enter = stm32f7_usart_reset_enter;
	device_class_set_props(dc, stm32f7_usart_properties);
}

static const TypeInfo stm32f7_usart_info = {
	.name = TYPE_STM32F7_USART,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32F7USART),
	.instance_init = stm32f7_usart_init,
	.class_init = stm32f7_usart_class_init,
};

static void stm32f7_usart_register_types(void) {
	type_register_static(&stm32f7_usart_info);
}

type_init(stm32f7_usart_register_types);
/*
 * STM32F7XX DMA
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
#include "hw/dma/stm32f7-dma.h"
#include "hw/qdev-properties.h"
#include "exec/cpu-common.h"

#define STM32F7_DMA_SIZE				0x400

REG32(LISR, 0x0)
	FIELD(LISR, TCIF3, 27, 1)
	FIELD(LISR, HTIF3, 26, 1)
	FIELD(LISR, TEIF3, 25, 1)
	FIELD(LISR, DMEIF3, 24, 1)
	FIELD(LISR, FEIF3, 22, 1)
	FIELD(LISR, TCIF2, 21, 1)
	FIELD(LISR, HTIF2, 20, 1)
	FIELD(LISR, TEIF2, 19, 1)
	FIELD(LISR, DMEIF2, 18, 1)
	FIELD(LISR, FEIF2, 16, 1)
	FIELD(LISR, TCIF1, 11, 1)
	FIELD(LISR, HTIF1, 10, 1)
	FIELD(LISR, TEIF1, 9, 1)
	FIELD(LISR, DMEIF1, 8, 1)
	FIELD(LISR, FEIF1, 6, 1)
	FIELD(LISR, TCIF0, 5, 1)
	FIELD(LISR, HTIF0, 4, 1)
	FIELD(LISR, TEIF0, 3, 1)
	FIELD(LISR, DMEIF0, 2, 1)
	FIELD(LISR, FEIF0, 0, 1)
REG32(HISR, 0x4)
	FIELD(HISR, TCIF7, 27, 1)
	FIELD(HISR, HTIF7, 26, 1)
	FIELD(HISR, TEIF7, 25, 1)
	FIELD(HISR, DMEIF7, 24, 1)
	FIELD(HISR, FEIF7, 22, 1)
	FIELD(HISR, TCIF6, 21, 1)
	FIELD(HISR, HTIF6, 20, 1)
	FIELD(HISR, TEIF6, 19, 1)
	FIELD(HISR, DMEIF6, 18, 1)
	FIELD(HISR, FEIF6, 16, 1)
	FIELD(HISR, TCIF5, 11, 1)
	FIELD(HISR, HTIF5, 10, 1)
	FIELD(HISR, TEIF5, 9, 1)
	FIELD(HISR, DMEIF5, 8, 1)
	FIELD(HISR, FEIF5, 6, 1)
	FIELD(HISR, TCIF4, 5, 1)
	FIELD(HISR, HTIF4, 4, 1)
	FIELD(HISR, TEIF4, 3, 1)
	FIELD(HISR, DMEIF4, 2, 1)
	FIELD(HISR, FEIF4, 0, 1)
REG32(LIFCR, 0x8)
	FIELD(LIFCR, CTCIF3, 27, 1)
	FIELD(LIFCR, CHTIF3, 26, 1)
	FIELD(LIFCR, CTEIF3, 25, 1)
	FIELD(LIFCR, CDMEIF3, 24, 1)
	FIELD(LIFCR, CFEIF3, 22, 1)
	FIELD(LIFCR, CTCIF2, 21, 1)
	FIELD(LIFCR, CHTIF2, 20, 1)
	FIELD(LIFCR, CTEIF2, 19, 1)
	FIELD(LIFCR, CDMEIF2, 18, 1)
	FIELD(LIFCR, CFEIF2, 16, 1)
	FIELD(LIFCR, CTCIF1, 11, 1)
	FIELD(LIFCR, CHTIF1, 10, 1)
	FIELD(LIFCR, CTEIF1, 9, 1)
	FIELD(LIFCR, CDMEIF1, 8, 1)
	FIELD(LIFCR, CFEIF1, 6, 1)
	FIELD(LIFCR, CTCIF0, 5, 1)
	FIELD(LIFCR, CHTIF0, 4, 1)
	FIELD(LIFCR, CTEIF0, 3, 1)
	FIELD(LIFCR, CDMEIF0, 2, 1)
	FIELD(LIFCR, CFEIF0, 0, 1)
REG32(HIFCR, 0xc)
	FIELD(HIFCR, CTCIF7, 27, 1)
	FIELD(HIFCR, CHTIF7, 26, 1)
	FIELD(HIFCR, CTEIF7, 25, 1)
	FIELD(HIFCR, CDMEIF7, 24, 1)
	FIELD(HIFCR, CFEIF7, 22, 1)
	FIELD(HIFCR, CTCIF6, 21, 1)
	FIELD(HIFCR, CHTIF6, 20, 1)
	FIELD(HIFCR, CTEIF6, 19, 1)
	FIELD(HIFCR, CDMEIF6, 18, 1)
	FIELD(HIFCR, CFEIF6, 16, 1)
	FIELD(HIFCR, CTCIF5, 11, 1)
	FIELD(HIFCR, CHTIF5, 10, 1)
	FIELD(HIFCR, CTEIF5, 9, 1)
	FIELD(HIFCR, CDMEIF5, 8, 1)
	FIELD(HIFCR, CFEIF5, 6, 1)
	FIELD(HIFCR, CTCIF4, 5, 1)
	FIELD(HIFCR, CHTIF4, 4, 1)
	FIELD(HIFCR, CTEIF4, 3, 1)
	FIELD(HIFCR, CDMEIF4, 2, 1)
	FIELD(HIFCR, CFEIF4, 0, 1)
REG32(S0CR, 0x10)
	FIELD(S0CR, CHSEL, 25, 4)
	FIELD(S0CR, MBURST, 23, 2)
	FIELD(S0CR, PBURST, 21, 2)
	FIELD(S0CR, CT, 19, 1)
	FIELD(S0CR, DBM, 18, 1)
	FIELD(S0CR, PL, 16, 2)
	FIELD(S0CR, PINCOS, 15, 1)
	FIELD(S0CR, MSIZE, 13, 2)
	FIELD(S0CR, PSIZE, 11, 2)
	FIELD(S0CR, MINC, 10, 1)
	FIELD(S0CR, PINC, 9, 1)
	FIELD(S0CR, CIRC, 8, 1)
	FIELD(S0CR, DIR, 6, 2)
	FIELD(S0CR, PFCTRL, 5, 1)
	FIELD(S0CR, TCIE, 4, 1)
	FIELD(S0CR, HTIE, 3, 1)
	FIELD(S0CR, TEIE, 2, 1)
	FIELD(S0CR, DMEIE, 1, 1)
	FIELD(S0CR, EN, 0, 1)
REG32(S0NDTR, 0x14)
	FIELD(S0NDTR, NDT, 0, 16)
REG32(S0PAR, 0x18)
	FIELD(S0PAR, PA, 0, 32)
REG32(S0M0AR, 0x1c)
	FIELD(S0M0AR, M0A, 0, 32)
REG32(S0M1AR, 0x20)
	FIELD(S0M1AR, M1A, 0, 32)
REG32(S0FCR, 0x24)
	FIELD(S0FCR, FEIE, 7, 1)
	FIELD(S0FCR, FS, 3, 3)
	FIELD(S0FCR, DMDIS, 2, 1)
	FIELD(S0FCR, FTH, 0, 2)
REG32(S1CR, 0x28)
	FIELD(S1CR, CHSEL, 25, 4)
	FIELD(S1CR, MBURST, 23, 2)
	FIELD(S1CR, PBURST, 21, 2)
	FIELD(S1CR, ACK, 20, 1)
	FIELD(S1CR, CT, 19, 1)
	FIELD(S1CR, DBM, 18, 1)
	FIELD(S1CR, PL, 16, 2)
	FIELD(S1CR, PINCOS, 15, 1)
	FIELD(S1CR, MSIZE, 13, 2)
	FIELD(S1CR, PSIZE, 11, 2)
	FIELD(S1CR, MINC, 10, 1)
	FIELD(S1CR, PINC, 9, 1)
	FIELD(S1CR, CIRC, 8, 1)
	FIELD(S1CR, DIR, 6, 2)
	FIELD(S1CR, PFCTRL, 5, 1)
	FIELD(S1CR, TCIE, 4, 1)
	FIELD(S1CR, HTIE, 3, 1)
	FIELD(S1CR, TEIE, 2, 1)
	FIELD(S1CR, DMEIE, 1, 1)
	FIELD(S1CR, EN, 0, 1)
REG32(S1NDTR, 0x2c)
	FIELD(S1NDTR, NDT, 0, 16)
REG32(S1PAR, 0x30)
	FIELD(S1PAR, PA, 0, 32)
REG32(S1M0AR, 0x34)
	FIELD(S1M0AR, M0A, 0, 32)
REG32(S1M1AR, 0x38)
	FIELD(S1M1AR, M1A, 0, 32)
REG32(S1FCR, 0x3c)
	FIELD(S1FCR, FEIE, 7, 1)
	FIELD(S1FCR, FS, 3, 3)
	FIELD(S1FCR, DMDIS, 2, 1)
	FIELD(S1FCR, FTH, 0, 2)
REG32(S2CR, 0x40)
	FIELD(S2CR, CHSEL, 25, 4)
	FIELD(S2CR, MBURST, 23, 2)
	FIELD(S2CR, PBURST, 21, 2)
	FIELD(S2CR, ACK, 20, 1)
	FIELD(S2CR, CT, 19, 1)
	FIELD(S2CR, DBM, 18, 1)
	FIELD(S2CR, PL, 16, 2)
	FIELD(S2CR, PINCOS, 15, 1)
	FIELD(S2CR, MSIZE, 13, 2)
	FIELD(S2CR, PSIZE, 11, 2)
	FIELD(S2CR, MINC, 10, 1)
	FIELD(S2CR, PINC, 9, 1)
	FIELD(S2CR, CIRC, 8, 1)
	FIELD(S2CR, DIR, 6, 2)
	FIELD(S2CR, PFCTRL, 5, 1)
	FIELD(S2CR, TCIE, 4, 1)
	FIELD(S2CR, HTIE, 3, 1)
	FIELD(S2CR, TEIE, 2, 1)
	FIELD(S2CR, DMEIE, 1, 1)
	FIELD(S2CR, EN, 0, 1)
REG32(S2NDTR, 0x44)
	FIELD(S2NDTR, NDT, 0, 16)
REG32(S2PAR, 0x48)
	FIELD(S2PAR, PA, 0, 32)
REG32(S2M0AR, 0x4c)
	FIELD(S2M0AR, M0A, 0, 32)
REG32(S2M1AR, 0x50)
	FIELD(S2M1AR, M1A, 0, 32)
REG32(S2FCR, 0x54)
	FIELD(S2FCR, FEIE, 7, 1)
	FIELD(S2FCR, FS, 3, 3)
	FIELD(S2FCR, DMDIS, 2, 1)
	FIELD(S2FCR, FTH, 0, 2)
REG32(S3CR, 0x58)
	FIELD(S3CR, CHSEL, 25, 4)
	FIELD(S3CR, MBURST, 23, 2)
	FIELD(S3CR, PBURST, 21, 2)
	FIELD(S3CR, ACK, 20, 1)
	FIELD(S3CR, CT, 19, 1)
	FIELD(S3CR, DBM, 18, 1)
	FIELD(S3CR, PL, 16, 2)
	FIELD(S3CR, PINCOS, 15, 1)
	FIELD(S3CR, MSIZE, 13, 2)
	FIELD(S3CR, PSIZE, 11, 2)
	FIELD(S3CR, MINC, 10, 1)
	FIELD(S3CR, PINC, 9, 1)
	FIELD(S3CR, CIRC, 8, 1)
	FIELD(S3CR, DIR, 6, 2)
	FIELD(S3CR, PFCTRL, 5, 1)
	FIELD(S3CR, TCIE, 4, 1)
	FIELD(S3CR, HTIE, 3, 1)
	FIELD(S3CR, TEIE, 2, 1)
	FIELD(S3CR, DMEIE, 1, 1)
	FIELD(S3CR, EN, 0, 1)
REG32(S3NDTR, 0x5c)
	FIELD(S3NDTR, NDT, 0, 16)
REG32(S3PAR, 0x60)
	FIELD(S3PAR, PA, 0, 32)
REG32(S3M0AR, 0x64)
	FIELD(S3M0AR, M0A, 0, 32)
REG32(S3M1AR, 0x68)
	FIELD(S3M1AR, M1A, 0, 32)
REG32(S3FCR, 0x6c)
	FIELD(S3FCR, FEIE, 7, 1)
	FIELD(S3FCR, FS, 3, 3)
	FIELD(S3FCR, DMDIS, 2, 1)
	FIELD(S3FCR, FTH, 0, 2)
REG32(S4CR, 0x70)
	FIELD(S4CR, CHSEL, 25, 4)
	FIELD(S4CR, MBURST, 23, 2)
	FIELD(S4CR, PBURST, 21, 2)
	FIELD(S4CR, ACK, 20, 1)
	FIELD(S4CR, CT, 19, 1)
	FIELD(S4CR, DBM, 18, 1)
	FIELD(S4CR, PL, 16, 2)
	FIELD(S4CR, PINCOS, 15, 1)
	FIELD(S4CR, MSIZE, 13, 2)
	FIELD(S4CR, PSIZE, 11, 2)
	FIELD(S4CR, MINC, 10, 1)
	FIELD(S4CR, PINC, 9, 1)
	FIELD(S4CR, CIRC, 8, 1)
	FIELD(S4CR, DIR, 6, 2)
	FIELD(S4CR, PFCTRL, 5, 1)
	FIELD(S4CR, TCIE, 4, 1)
	FIELD(S4CR, HTIE, 3, 1)
	FIELD(S4CR, TEIE, 2, 1)
	FIELD(S4CR, DMEIE, 1, 1)
	FIELD(S4CR, EN, 0, 1)
REG32(S4NDTR, 0x74)
	FIELD(S4NDTR, NDT, 0, 16)
REG32(S4PAR, 0x78)
	FIELD(S4PAR, PA, 0, 32)
REG32(S4M0AR, 0x7c)
	FIELD(S4M0AR, M0A, 0, 32)
REG32(S4M1AR, 0x80)
	FIELD(S4M1AR, M1A, 0, 32)
REG32(S4FCR, 0x84)
	FIELD(S4FCR, FEIE, 7, 1)
	FIELD(S4FCR, FS, 3, 3)
	FIELD(S4FCR, DMDIS, 2, 1)
	FIELD(S4FCR, FTH, 0, 2)
REG32(S5CR, 0x88)
	FIELD(S5CR, CHSEL, 25, 4)
	FIELD(S5CR, MBURST, 23, 2)
	FIELD(S5CR, PBURST, 21, 2)
	FIELD(S5CR, ACK, 20, 1)
	FIELD(S5CR, CT, 19, 1)
	FIELD(S5CR, DBM, 18, 1)
	FIELD(S5CR, PL, 16, 2)
	FIELD(S5CR, PINCOS, 15, 1)
	FIELD(S5CR, MSIZE, 13, 2)
	FIELD(S5CR, PSIZE, 11, 2)
	FIELD(S5CR, MINC, 10, 1)
	FIELD(S5CR, PINC, 9, 1)
	FIELD(S5CR, CIRC, 8, 1)
	FIELD(S5CR, DIR, 6, 2)
	FIELD(S5CR, PFCTRL, 5, 1)
	FIELD(S5CR, TCIE, 4, 1)
	FIELD(S5CR, HTIE, 3, 1)
	FIELD(S5CR, TEIE, 2, 1)
	FIELD(S5CR, DMEIE, 1, 1)
	FIELD(S5CR, EN, 0, 1)
REG32(S5NDTR, 0x8c)
	FIELD(S5NDTR, NDT, 0, 16)
REG32(S5PAR, 0x90)
	FIELD(S5PAR, PA, 0, 32)
REG32(S5M0AR, 0x94)
	FIELD(S5M0AR, M0A, 0, 32)
REG32(S5M1AR, 0x98)
	FIELD(S5M1AR, M1A, 0, 32)
REG32(S5FCR, 0x9c)
	FIELD(S5FCR, FEIE, 7, 1)
	FIELD(S5FCR, FS, 3, 3)
	FIELD(S5FCR, DMDIS, 2, 1)
	FIELD(S5FCR, FTH, 0, 2)
REG32(S6CR, 0xa0)
	FIELD(S6CR, CHSEL, 25, 4)
	FIELD(S6CR, MBURST, 23, 2)
	FIELD(S6CR, PBURST, 21, 2)
	FIELD(S6CR, ACK, 20, 1)
	FIELD(S6CR, CT, 19, 1)
	FIELD(S6CR, DBM, 18, 1)
	FIELD(S6CR, PL, 16, 2)
	FIELD(S6CR, PINCOS, 15, 1)
	FIELD(S6CR, MSIZE, 13, 2)
	FIELD(S6CR, PSIZE, 11, 2)
	FIELD(S6CR, MINC, 10, 1)
	FIELD(S6CR, PINC, 9, 1)
	FIELD(S6CR, CIRC, 8, 1)
	FIELD(S6CR, DIR, 6, 2)
	FIELD(S6CR, PFCTRL, 5, 1)
	FIELD(S6CR, TCIE, 4, 1)
	FIELD(S6CR, HTIE, 3, 1)
	FIELD(S6CR, TEIE, 2, 1)
	FIELD(S6CR, DMEIE, 1, 1)
	FIELD(S6CR, EN, 0, 1)
REG32(S6NDTR, 0xa4)
	FIELD(S6NDTR, NDT, 0, 16)
REG32(S6PAR, 0xa8)
	FIELD(S6PAR, PA, 0, 32)
REG32(S6M0AR, 0xac)
	FIELD(S6M0AR, M0A, 0, 32)
REG32(S6M1AR, 0xb0)
	FIELD(S6M1AR, M1A, 0, 32)
REG32(S6FCR, 0xb4)
	FIELD(S6FCR, FEIE, 7, 1)
	FIELD(S6FCR, FS, 3, 3)
	FIELD(S6FCR, DMDIS, 2, 1)
	FIELD(S6FCR, FTH, 0, 2)
REG32(S7CR, 0xb8)
	FIELD(S7CR, CHSEL, 25, 4)
	FIELD(S7CR, MBURST, 23, 2)
	FIELD(S7CR, PBURST, 21, 2)
	FIELD(S7CR, ACK, 20, 1)
	FIELD(S7CR, CT, 19, 1)
	FIELD(S7CR, DBM, 18, 1)
	FIELD(S7CR, PL, 16, 2)
	FIELD(S7CR, PINCOS, 15, 1)
	FIELD(S7CR, MSIZE, 13, 2)
	FIELD(S7CR, PSIZE, 11, 2)
	FIELD(S7CR, MINC, 10, 1)
	FIELD(S7CR, PINC, 9, 1)
	FIELD(S7CR, CIRC, 8, 1)
	FIELD(S7CR, DIR, 6, 2)
	FIELD(S7CR, PFCTRL, 5, 1)
	FIELD(S7CR, TCIE, 4, 1)
	FIELD(S7CR, HTIE, 3, 1)
	FIELD(S7CR, TEIE, 2, 1)
	FIELD(S7CR, DMEIE, 1, 1)
	FIELD(S7CR, EN, 0, 1)
REG32(S7NDTR, 0xbc)
	FIELD(S7NDTR, NDT, 0, 16)
REG32(S7PAR, 0xc0)
	FIELD(S7PAR, PA, 0, 32)
REG32(S7M0AR, 0xc4)
	FIELD(S7M0AR, M0A, 0, 32)
REG32(S7M1AR, 0xc8)
	FIELD(S7M1AR, M1A, 0, 32)
REG32(S7FCR, 0xcc)
	FIELD(S7FCR, FEIE, 7, 1)
	FIELD(S7FCR, FS, 3, 3)
	FIELD(S7FCR, DMDIS, 2, 1)
	FIELD(S7FCR, FTH, 0, 2)

static void stm32f7_dma_register_reset(STM32F7DMA *t) {
	t->LISR = 0x00000000;
	t->HISR = 0x00000000;
	t->LIFCR = 0x00000000;
	t->HIFCR = 0x00000000;
	t->S0CR = 0x00000000;
	t->S0NDTR = 0x00000000;
	t->S0PAR = 0x00000000;
	t->S0M0AR = 0x00000000;
	t->S0M1AR = 0x00000000;
	t->S0FCR = 0x00000021;
	t->S1CR = 0x00000000;
	t->S1NDTR = 0x00000000;
	t->S1PAR = 0x00000000;
	t->S1M0AR = 0x00000000;
	t->S1M1AR = 0x00000000;
	t->S1FCR = 0x00000021;
	t->S2CR = 0x00000000;
	t->S2NDTR = 0x00000000;
	t->S2PAR = 0x00000000;
	t->S2M0AR = 0x00000000;
	t->S2M1AR = 0x00000000;
	t->S2FCR = 0x00000021;
	t->S3CR = 0x00000000;
	t->S3NDTR = 0x00000000;
	t->S3PAR = 0x00000000;
	t->S3M0AR = 0x00000000;
	t->S3M1AR = 0x00000000;
	t->S3FCR = 0x00000021;
	t->S4CR = 0x00000000;
	t->S4NDTR = 0x00000000;
	t->S4PAR = 0x00000000;
	t->S4M0AR = 0x00000000;
	t->S4M1AR = 0x00000000;
	t->S4FCR = 0x00000021;
	t->S5CR = 0x00000000;
	t->S5NDTR = 0x00000000;
	t->S5PAR = 0x00000000;
	t->S5M0AR = 0x00000000;
	t->S5M1AR = 0x00000000;
	t->S5FCR = 0x00000021;
	t->S6CR = 0x00000000;
	t->S6NDTR = 0x00000000;
	t->S6PAR = 0x00000000;
	t->S6M0AR = 0x00000000;
	t->S6M1AR = 0x00000000;
	t->S6FCR = 0x00000021;
	t->S7CR = 0x00000000;
	t->S7NDTR = 0x00000000;
	t->S7PAR = 0x00000000;
	t->S7M0AR = 0x00000000;
	t->S7M1AR = 0x00000000;
	t->S7FCR = 0x00000021;
}

static uint64_t stm32f7_dma_read(void *opaque, hwaddr offset, unsigned size) {
	STM32F7DMA *t = STM32F7_DMA(opaque);
	uint64_t ret;

	switch (offset) {
		case A_LISR:
			ret = t->LISR;
			break;
		case A_HISR:
			ret = t->HISR;
			break;
		case A_LIFCR:
			ret = t->LIFCR;
			break;
		case A_HIFCR:
			ret = t->HIFCR;
			break;
		case A_S0CR:
			ret = t->S0CR;
			break;
		case A_S0NDTR:
			ret = t->S0NDTR;
			break;
		case A_S0PAR:
			ret = t->S0PAR;
			break;
		case A_S0M0AR:
			ret = t->S0M0AR;
			break;
		case A_S0M1AR:
			ret = t->S0M1AR;
			break;
		case A_S0FCR:
			ret = t->S0FCR;
			break;
		case A_S1CR:
			ret = t->S1CR;
			break;
		case A_S1NDTR:
			ret = t->S1NDTR;
			break;
		case A_S1PAR:
			ret = t->S1PAR;
			break;
		case A_S1M0AR:
			ret = t->S1M0AR;
			break;
		case A_S1M1AR:
			ret = t->S1M1AR;
			break;
		case A_S1FCR:
			ret = t->S1FCR;
			break;
		case A_S2CR:
			ret = t->S2CR;
			break;
		case A_S2NDTR:
			ret = t->S2NDTR;
			break;
		case A_S2PAR:
			ret = t->S2PAR;
			break;
		case A_S2M0AR:
			ret = t->S2M0AR;
			break;
		case A_S2M1AR:
			ret = t->S2M1AR;
			break;
		case A_S2FCR:
			ret = t->S2FCR;
			break;
		case A_S3CR:
			ret = t->S3CR;
			break;
		case A_S3NDTR:
			ret = t->S3NDTR;
			break;
		case A_S3PAR:
			ret = t->S3PAR;
			break;
		case A_S3M0AR:
			ret = t->S3M0AR;
			break;
		case A_S3M1AR:
			ret = t->S3M1AR;
			break;
		case A_S3FCR:
			ret = t->S3FCR;
			break;
		case A_S4CR:
			ret = t->S4CR;
			break;
		case A_S4NDTR:
			ret = t->S4NDTR;
			break;
		case A_S4PAR:
			ret = t->S4PAR;
			break;
		case A_S4M0AR:
			ret = t->S4M0AR;
			break;
		case A_S4M1AR:
			ret = t->S4M1AR;
			break;
		case A_S4FCR:
			ret = t->S4FCR;
			break;
		case A_S5CR:
			ret = t->S5CR;
			break;
		case A_S5NDTR:
			ret = t->S5NDTR;
			break;
		case A_S5PAR:
			ret = t->S5PAR;
			break;
		case A_S5M0AR:
			ret = t->S5M0AR;
			break;
		case A_S5M1AR:
			ret = t->S5M1AR;
			break;
		case A_S5FCR:
			ret = t->S5FCR;
			break;
		case A_S6CR:
			ret = t->S6CR;
			break;
		case A_S6NDTR:
			ret = t->S6NDTR;
			break;
		case A_S6PAR:
			ret = t->S6PAR;
			break;
		case A_S6M0AR:
			ret = t->S6M0AR;
			break;
		case A_S6M1AR:
			ret = t->S6M1AR;
			break;
		case A_S6FCR:
			ret = t->S6FCR;
			break;
		case A_S7CR:
			ret = t->S7CR;
			break;
		case A_S7NDTR:
			ret = t->S7NDTR;
			break;
		case A_S7PAR:
			ret = t->S7PAR;
			break;
		case A_S7M0AR:
			ret = t->S7M0AR;
			break;
		case A_S7M1AR:
			ret = t->S7M1AR;
			break;
		case A_S7FCR:
			ret = t->S7FCR;
			break;
		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F7 DMA read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32f7_dma_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32F7DMA *t = STM32F7_DMA(opaque);
	switch (offset) {
		case A_LIFCR:
			t->LIFCR = value;
			break;
		case A_HIFCR:
			t->HIFCR = value;
			break;
		case A_S0CR:
			t->S0CR = value;
			break;
		case A_S0NDTR:
			t->S0NDTR = value;
			break;
		case A_S0PAR:
			t->S0PAR = value;
			break;
		case A_S0M0AR:
			t->S0M0AR = value;
			break;
		case A_S0M1AR:
			t->S0M1AR = value;
			break;
		case A_S0FCR:
			t->S0FCR = value;
			break;
		case A_S1CR:
			t->S1CR = value;
			break;
		case A_S1NDTR:
			t->S1NDTR = value;
			break;
		case A_S1PAR:
			t->S1PAR = value;
			break;
		case A_S1M0AR:
			t->S1M0AR = value;
			break;
		case A_S1M1AR:
			t->S1M1AR = value;
			break;
		case A_S1FCR:
			t->S1FCR = value;
			break;
		case A_S2CR:
			t->S2CR = value;
			break;
		case A_S2NDTR:
			t->S2NDTR = value;
			break;
		case A_S2PAR:
			t->S2PAR = value;
			break;
		case A_S2M0AR:
			t->S2M0AR = value;
			break;
		case A_S2M1AR:
			t->S2M1AR = value;
			break;
		case A_S2FCR:
			t->S2FCR = value;
			break;
		case A_S3CR:
			t->S3CR = value;
			break;
		case A_S3NDTR:
			t->S3NDTR = value;
			break;
		case A_S3PAR:
			t->S3PAR = value;
			break;
		case A_S3M0AR:
			t->S3M0AR = value;
			break;
		case A_S3M1AR:
			t->S3M1AR = value;
			break;
		case A_S3FCR:
			t->S3FCR = value;
			break;
		case A_S4CR:
			t->S4CR = value;
			break;
		case A_S4NDTR:
			t->S4NDTR = value;
			break;
		case A_S4PAR:
			t->S4PAR = value;
			break;
		case A_S4M0AR:
			t->S4M0AR = value;
			break;
		case A_S4M1AR:
			t->S4M1AR = value;
			break;
		case A_S4FCR:
			t->S4FCR = value;
			break;
		case A_S5CR:
			t->S5CR = value;
			break;
		case A_S5NDTR:
			t->S5NDTR = value;
			break;
		case A_S5PAR:
			t->S5PAR = value;
			break;
		case A_S5M0AR:
			t->S5M0AR = value;
			break;
		case A_S5M1AR:
			t->S5M1AR = value;
			break;
		case A_S5FCR:
			t->S5FCR = value;
			break;
		case A_S6CR:
			t->S6CR = value;
			break;
		case A_S6NDTR:
			t->S6NDTR = value;
			break;
		case A_S6PAR:
			t->S6PAR = value;
			break;
		case A_S6M0AR:
			t->S6M0AR = value;
			break;
		case A_S6M1AR:
			t->S6M1AR = value;
			break;
		case A_S6FCR:
			t->S6FCR = value;
			break;
		case A_S7CR:
			t->S7CR = value;
			break;
		case A_S7NDTR:
			t->S7NDTR = value;
			break;
		case A_S7PAR:
			t->S7PAR = value;
			break;
		case A_S7M0AR:
			t->S7M0AR = value;
			break;
		case A_S7M1AR:
			t->S7M1AR = value;
			break;
		case A_S7FCR:
			t->S7FCR = value;
			break;
		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F7 DMA write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32f7_dma_ops = {
	.read = stm32f7_dma_read,
	.write = stm32f7_dma_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32f7_dma_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32F7DMA *t = STM32F7_DMA(obj);

	memory_region_init_io(&(t->iomem), obj, &stm32f7_dma_ops, t, "stm32f7-dma", STM32F7_DMA_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0 ; i < 8 ; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
		sysbus_init_irq(sbd, &(t->irq[i]));
		sysbus_init_irq(sbd, &(t->irq[i]));
		sysbus_init_irq(sbd, &(t->irq[i]));
		sysbus_init_irq(sbd, &(t->irq[i]));
		sysbus_init_irq(sbd, &(t->irq[i]));
		sysbus_init_irq(sbd, &(t->irq[i]));
		sysbus_init_irq(sbd, &(t->irq[i]));
	}
}

static void stm32f7_dma_realize(DeviceState *dev, Error **errp) {
	return;
}

static void stm32f7_dma_reset_enter(Object *obj, ResetType type) {
	STM32F7DMA *t = STM32F7_DMA(obj);

	stm32f7_dma_register_reset(t);
}

static const VMStateDescription stm32f7_dma_vmstate = {
	.name = "stm32f7-dma",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(LISR, STM32F7DMA),
		VMSTATE_UINT32(HISR, STM32F7DMA),
		VMSTATE_UINT32(LIFCR, STM32F7DMA),
		VMSTATE_UINT32(HIFCR, STM32F7DMA),
		VMSTATE_UINT32(S0CR, STM32F7DMA),
		VMSTATE_UINT32(S0NDTR, STM32F7DMA),
		VMSTATE_UINT32(S0PAR, STM32F7DMA),
		VMSTATE_UINT32(S0M0AR, STM32F7DMA),
		VMSTATE_UINT32(S0M1AR, STM32F7DMA),
		VMSTATE_UINT32(S0FCR, STM32F7DMA),
		VMSTATE_UINT32(S1CR, STM32F7DMA),
		VMSTATE_UINT32(S1NDTR, STM32F7DMA),
		VMSTATE_UINT32(S1PAR, STM32F7DMA),
		VMSTATE_UINT32(S1M0AR, STM32F7DMA),
		VMSTATE_UINT32(S1M1AR, STM32F7DMA),
		VMSTATE_UINT32(S1FCR, STM32F7DMA),
		VMSTATE_UINT32(S2CR, STM32F7DMA),
		VMSTATE_UINT32(S2NDTR, STM32F7DMA),
		VMSTATE_UINT32(S2PAR, STM32F7DMA),
		VMSTATE_UINT32(S2M0AR, STM32F7DMA),
		VMSTATE_UINT32(S2M1AR, STM32F7DMA),
		VMSTATE_UINT32(S2FCR, STM32F7DMA),
		VMSTATE_UINT32(S3CR, STM32F7DMA),
		VMSTATE_UINT32(S3NDTR, STM32F7DMA),
		VMSTATE_UINT32(S3PAR, STM32F7DMA),
		VMSTATE_UINT32(S3M0AR, STM32F7DMA),
		VMSTATE_UINT32(S3M1AR, STM32F7DMA),
		VMSTATE_UINT32(S3FCR, STM32F7DMA),
		VMSTATE_UINT32(S4CR, STM32F7DMA),
		VMSTATE_UINT32(S4NDTR, STM32F7DMA),
		VMSTATE_UINT32(S4PAR, STM32F7DMA),
		VMSTATE_UINT32(S4M0AR, STM32F7DMA),
		VMSTATE_UINT32(S4M1AR, STM32F7DMA),
		VMSTATE_UINT32(S4FCR, STM32F7DMA),
		VMSTATE_UINT32(S5CR, STM32F7DMA),
		VMSTATE_UINT32(S5NDTR, STM32F7DMA),
		VMSTATE_UINT32(S5PAR, STM32F7DMA),
		VMSTATE_UINT32(S5M0AR, STM32F7DMA),
		VMSTATE_UINT32(S5M1AR, STM32F7DMA),
		VMSTATE_UINT32(S5FCR, STM32F7DMA),
		VMSTATE_UINT32(S6CR, STM32F7DMA),
		VMSTATE_UINT32(S6NDTR, STM32F7DMA),
		VMSTATE_UINT32(S6PAR, STM32F7DMA),
		VMSTATE_UINT32(S6M0AR, STM32F7DMA),
		VMSTATE_UINT32(S6M1AR, STM32F7DMA),
		VMSTATE_UINT32(S6FCR, STM32F7DMA),
		VMSTATE_UINT32(S7CR, STM32F7DMA),
		VMSTATE_UINT32(S7NDTR, STM32F7DMA),
		VMSTATE_UINT32(S7PAR, STM32F7DMA),
		VMSTATE_UINT32(S7M0AR, STM32F7DMA),
		VMSTATE_UINT32(S7M1AR, STM32F7DMA),
		VMSTATE_UINT32(S7FCR, STM32F7DMA),
		VMSTATE_END_OF_LIST()
	}
};

static void stm32f7_dma_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);

	dc->vmsd = &stm32f7_dma_vmstate;
	dc->realize = stm32f7_dma_realize;
	rc->phases.enter = stm32f7_dma_reset_enter;
}

static const TypeInfo stm32f7_dma_info = {
	.name = TYPE_STM32F7_DMA,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32F7DMA),
	.instance_init = stm32f7_dma_init,
	.class_init = stm32f7_dma_class_init,
};

static void stm32f7_dma_register_types(void) {
	type_register_static(&stm32f7_dma_info);
}

type_init(stm32f7_dma_register_types);
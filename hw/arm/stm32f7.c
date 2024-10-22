/*
 * STM32F7XX Series Board Emulation
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
#include "qemu/units.h"
#include "hw/sysbus.h"
#include "qapi/error.h"
#include "hw/arm/boot.h"
#include "hw/arm/armv7m.h"
#include "hw/misc/stm32f7-rcc.h"
#include "hw/misc/stm32f7-flash.h"
#include "hw/misc/stm32f7-pwr.h"
#include "hw/misc/stm32f7-exti.h"
#include "hw/misc/stm32f7-adc.h"
#include "hw/misc/stm32f7-adc_common.h"
#include "hw/gpio/stm32f7-gpioa.h"
#include "hw/gpio/stm32f7-gpiod.h"
#include "hw/char/stm32f7-usart.h"
#include "hw/timer/stm32f7-tim3.h"
#include "hw/dma/stm32f7-dma.h"
#include "hw/misc/stm32f7-fmc.h"
#include "hw/misc/stm32f7-rng.h"
#include "hw/net/stm32f7-eth.h"
#include "hw/misc/stm32f7-syscfg.h"
#include "hw/boards.h"
#include "exec/address-spaces.h"
#include "hw/misc/unimp.h"
#include "hw/clock.h"
#include "hw/qdev-clock.h"
#include "qom/object.h"
#include "qemu/bitops.h"
#include "hw/qdev-properties-system.h"
#include "sysemu/sysemu.h"

#define TYPE_STM32F7_MACHINE               "stm32f7"
#define TYPE_STM32F769IDISCOVERY_MACHINE   MACHINE_TYPE_NAME("stm32f769idiscovery")

OBJECT_DECLARE_TYPE(STM32F7MachineState, STM32F7MachineClass, STM32F7_MACHINE)

/* Max frequency 216MHz */
#define SYSCLK_FREQ     216000000

/* This works for all STM32 boards I've met, don't know how universal it will be */
#define STM32_MAGIC_VTOR_ADDR   0x08000000

typedef enum STM32F7Type {
    STM32F769IDISCOVERY = 0,
} STM32F7Type;

typedef enum MEM_TYPE {
    MEM_RAM = 0,
    MEM_ROM,
    MEM_ALIAS
} MEM_TYPE;

typedef struct MEMInfo {
    const char *name;
    uint32_t base;
    uint32_t size;
    MEM_TYPE type;
    uint32_t alias_to; 
} MEMInfo;

typedef void (*stm32f7_board_init_mem_fn)(MachineState *);
typedef void (*stm32f7_board_init_periph_fn)(MachineState *);

struct STM32F7MachineClass {
    MachineClass parent;
    STM32F7Type board_type;
    uint32_t sysclk_frq;
    uint32_t num_irq;
    uint32_t num_mem_info;
    const MEMInfo *meminfo;
    uint32_t flash_size;

    /* Some Methods */
    stm32f7_board_init_mem_fn custome_mem_init_fn;
    stm32f7_board_init_periph_fn custome_periph_init_fn;
};

struct STM32F7MachineState {
    MachineState parent;

    ARMv7MState armv7m;

    MemoryRegion *mem;

    Clock *sysclk;
    Clock *refclk;
};

static const MEMInfo stm32f769nidiscovery_meminfo[] = { {
        .name = "itcm-ram",
        .base = 0x00000000,
        .size = 0x4000,
        .type = MEM_RAM
    }, {
        .name = "system-memory",
        .base = 0x00100000,
        .size = 0xEDC0,
        .type = MEM_ROM
    }, {
        .name = "flash-itcm",
        .base = 0x00200000,
        .size = 0x00200000,
        .type = MEM_ROM
    }, {
        .name = "flash-axim",
        .base = 0x08000000,
        // in case
        .size = 0x00800000,
        .type = MEM_ROM
    }, {
        .name = "option-bytes",
        .base = 0x1fff0000,
        .size = 0x20,
        .type = MEM_ROM
    }, {
        // merge dtcm-ram, sram1 and sram2
        .name = "sram",
        .base = 0x20000000,
        // let's go, QEMU!!!
        .size = 0x20000000,
        .type = MEM_RAM,
    }, {
        .name = "sdram",
        .base = 0xC0000000,
        .size = 0x1000000,
        .type = MEM_RAM
    }
};

static void stm32f769nidiscovery_custome_mem_init(MachineState *machine) {
    return;
}

typedef struct STM32F769NIDISCOVERYUsartInfo {
    const char *name;
    uint32_t base;
    uint32_t irq_line;
} STM32F769NIDISCOVERYUsartInfo;

static const STM32F769NIDISCOVERYUsartInfo stm32f769nidiscovery_usart_info[] = {
    {
        .name = "USART1",
        .base = 0x40011000,
        .irq_line = 37
    },{
        .name = "USART2",
        .base = 0x40004400,
        .irq_line = 38
    }, {
        .name = "USART3",
        .base = 0x40004800,
        .irq_line = 39
    }, {
        .name = "UART4",
        .base = 0x40004c00,
        .irq_line = 52
    }, {
        .name = "UART5",
        .base = 0x40005000,
        .irq_line = 53
    }, {
        .name = "UART7",
        .base = 0x40007800,
        .irq_line = 82
    }, {
        .name = "UART8",
        .base = 0x40007c00,
        .irq_line = 83
    }, {
        .name = "USART6",
        .base = 0x40011400,
        .irq_line = 71
    }
};

typedef struct STM32F769NIDISCOVERYGPIODInfo {
    const char *name;
    uint32_t base;
} STM32F769NIDISCOVERYGPIODInfo;

static const STM32F769NIDISCOVERYGPIODInfo stm32f769nidiscovery_gpiod_info[] = {
    {
        .name = "GPIOC",
        .base = 0x40020800
    }, {
        .name = "GPIOD",
        .base = 0x40020C00
    }, {
        .name = "GPIOE",
        .base = 0x40021000
    }, {
        .name = "GPIOF",
        .base = 0x40021400
    }, {
        .name = "GPIOG",
        .base = 0x40021800
    }, {
        .name = "GPIOH",
        .base = 0x40021C00
    }, {
        .name = "GPIOI",
        .base = 0x40022000
    }, {
        .name = "GPIOJ",
        .base = 0x40022400
    }, {
        .name = "GPIOK",
        .base = 0x40022800
    }
};

typedef struct STM32F769NIDISCOVERYDMAInfo {
    const char *name;
    uint32_t base;
    uint32_t irq_lines[8];
} STM32F769NIDISCOVERYDMAInfo;

static const STM32F769NIDISCOVERYDMAInfo stm32f769nidiscovery_dma_info[] = {
    {
        .name = "DMA1",
        .base = 0x40026000,
        .irq_lines = {11, 12, 13, 14, 15, 16, 17, 47}
    }, {
        .name = "DMA2",
        .base = 0x40026400,
        .irq_lines = {56, 57, 58, 59, 60, 68, 69, 70}
    }
};

#define STM32F769NIDISCOVERY_RCC_BASE        0x40023800
#define STM32F769NIDISCOVERY_FLASH_BASE      0x40023C00
#define STM32F769NIDISCOVERY_PWR_BASE        0x40007000
#define STM32F769NIDISCOVERY_GPIOA_BASE      0x40020000
#define STM32F769NIDISCOVERY_TIM3_BASE       0x40000400
#define STM32F769NIDISCOVERY_FMC_BASE        0xA0000000
#define STM32F769NIDISCOVERY_RNG_BASE        0x50060800
#define STM32F769NIDISCOVERY_ETH_BASE        0x40028000
#define STM32F769NIDISCOVERY_SYSCFG_BASE     0x40013800

static void stm32f769nidiscovery_custome_periph_init(MachineState *machine) {
    STM32F7MachineState *sms = STM32F7_MACHINE(machine);

    /* RCC */
    STM32F7RCC *rcc = g_new(STM32F7RCC, 1);
    object_initialize_child(OBJECT(sms), "RCC", rcc, TYPE_STM32F7_RCC);
    sysbus_realize(SYS_BUS_DEVICE(rcc), &error_fatal);
    sysbus_connect_irq(
        SYS_BUS_DEVICE(rcc),
        0,
        qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 5)
    );
    sysbus_mmio_map(SYS_BUS_DEVICE(rcc), 0, STM32F769NIDISCOVERY_RCC_BASE);

    /* Flash */
    STM32F7FLASH *flash = g_new(STM32F7FLASH, 1);
    object_initialize_child(OBJECT(sms), "FLASH", flash, TYPE_STM32F7_FLASH);
    sysbus_realize(SYS_BUS_DEVICE(flash), &error_fatal);
    sysbus_connect_irq(
        SYS_BUS_DEVICE(flash),
        0,
        qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 4)
    );
    sysbus_mmio_map(SYS_BUS_DEVICE(flash), 0, STM32F769NIDISCOVERY_FLASH_BASE);

    /* PWR */
    STM32F7PWR *pwr = g_new(STM32F7PWR, 1);
    object_initialize_child(OBJECT(sms), "PWR", pwr, TYPE_STM32F7_PWR);
    sysbus_realize(SYS_BUS_DEVICE(pwr), &error_fatal);
    sysbus_mmio_map(SYS_BUS_DEVICE(pwr), 0, STM32F769NIDISCOVERY_PWR_BASE);

    /* GPIOA */
    STM32F7GPIOA *gpioa = g_new(STM32F7GPIOA, 1);
    object_initialize_child(OBJECT(sms), "GPIOA", gpioa, TYPE_STM32F7_GPIOA);
    sysbus_realize(SYS_BUS_DEVICE(gpioa), &error_fatal);
    sysbus_mmio_map(SYS_BUS_DEVICE(gpioa), 0, STM32F769NIDISCOVERY_GPIOA_BASE);

    /* GPIOD */
    int gpiod_num = (
        sizeof(stm32f769nidiscovery_gpiod_info) / sizeof(stm32f769nidiscovery_gpiod_info[0])
    );
    const STM32F769NIDISCOVERYGPIODInfo *p_gpiod;
    STM32F7GPIOD *gpiod;
    for(int i = 0; i < gpiod_num; ++i) {
        p_gpiod = &(stm32f769nidiscovery_gpiod_info[i]);
        gpiod = g_new(STM32F7GPIOD, 1);
        object_initialize_child(OBJECT(sms), p_gpiod->name, gpiod, TYPE_STM32F7_GPIOD);
        sysbus_realize(SYS_BUS_DEVICE(gpiod), &error_fatal);
        sysbus_mmio_map(SYS_BUS_DEVICE(gpiod), 0, p_gpiod->base);
    }

    /* USART */
    int usart_num = (
        sizeof(stm32f769nidiscovery_usart_info) / sizeof(stm32f769nidiscovery_usart_info[0])
    );
    const STM32F769NIDISCOVERYUsartInfo *p_usart;
    STM32F7USART *usart;
    for(int i = 0; i < usart_num; ++i) {
        p_usart = &(stm32f769nidiscovery_usart_info[i]);
        usart = g_new(STM32F7USART, 1);
        object_initialize_child(OBJECT(sms), p_usart->name, usart, TYPE_STM32F7_USART);
        qdev_prop_set_chr(DEVICE(usart), "chardev", serial_hd(i));
        sysbus_realize(SYS_BUS_DEVICE(usart), &error_fatal);
        sysbus_connect_irq(
            SYS_BUS_DEVICE(usart),
            0,
            qdev_get_gpio_in(DEVICE(&(sms->armv7m)), p_usart->irq_line)
        );
        sysbus_mmio_map(SYS_BUS_DEVICE(usart), 0, p_usart->base);
    }

    /* DMA */
    int dma_num = (
        sizeof(stm32f769nidiscovery_dma_info) / sizeof(stm32f769nidiscovery_dma_info[0])
    );
    const STM32F769NIDISCOVERYDMAInfo *p_dma;
    STM32F7DMA *dma;
    for(int i = 0; i < dma_num; ++i) {
        p_dma = &(stm32f769nidiscovery_dma_info[i]);
        dma = g_new(STM32F7DMA, 1);
        object_initialize_child(OBJECT(sms), p_dma->name, dma, TYPE_STM32F7_DMA);
        sysbus_realize(SYS_BUS_DEVICE(dma), &error_fatal);
        for (int j = 0; j < 8; ++j) {
            sysbus_connect_irq(
                SYS_BUS_DEVICE(dma),
                j,
                qdev_get_gpio_in(DEVICE(&(sms->armv7m)), p_dma->irq_lines[j])
            );
        }
        sysbus_mmio_map(SYS_BUS_DEVICE(dma), 0, p_dma->base);
    }

    /* TIM3 */
    STM32F7TIM3 *tim3 = g_new(STM32F7TIM3, 1);
    object_initialize_child(OBJECT(sms), "TIM3", tim3, TYPE_STM32F7_TIM3);
    sysbus_realize(SYS_BUS_DEVICE(tim3), &error_fatal);
    sysbus_connect_irq(
        SYS_BUS_DEVICE(tim3),
        0,
        qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 29)
    );
    sysbus_mmio_map(SYS_BUS_DEVICE(tim3), 0, STM32F769NIDISCOVERY_TIM3_BASE);

    /* TIM6 */
    STM32F7TIM3 *tim6 = g_new(STM32F7TIM3, 1);
    object_initialize_child(OBJECT(sms), "TIM6", tim6, TYPE_STM32F7_TIM3);
    sysbus_realize(SYS_BUS_DEVICE(tim6), &error_fatal);
    sysbus_connect_irq(
        SYS_BUS_DEVICE(tim6),
        0,
        qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 54)
    );
    sysbus_mmio_map(SYS_BUS_DEVICE(tim6), 0, 0x40001000);

    /* TIM1 */
    STM32F7TIM3 *tim1 = g_new(STM32F7TIM3, 1);
    object_initialize_child(OBJECT(sms), "TIM1", tim1, TYPE_STM32F7_TIM3);
    sysbus_realize(SYS_BUS_DEVICE(tim1), &error_fatal);
    sysbus_connect_irq(
        SYS_BUS_DEVICE(tim1),
        0,
        qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 24)
    );
    sysbus_mmio_map(SYS_BUS_DEVICE(tim1), 0, 0x40010000);

    /* TIM10 */
    STM32F7TIM3 *tim10 = g_new(STM32F7TIM3, 1);
    object_initialize_child(OBJECT(sms), "TIM10", tim10, TYPE_STM32F7_TIM3);
    sysbus_realize(SYS_BUS_DEVICE(tim10), &error_fatal);
    sysbus_mmio_map(SYS_BUS_DEVICE(tim10), 0, 0x40014400);

    /* TIM11 */
    STM32F7TIM3 *tim11 = g_new(STM32F7TIM3, 1);
    object_initialize_child(OBJECT(sms), "TIM11", tim11, TYPE_STM32F7_TIM3);
    sysbus_realize(SYS_BUS_DEVICE(tim11), &error_fatal);
    sysbus_mmio_map(SYS_BUS_DEVICE(tim11), 0, 0x40014800);

    /* TIM12 */
    STM32F7TIM3 *tim12 = g_new(STM32F7TIM3, 1);
    object_initialize_child(OBJECT(sms), "TIM12", tim12, TYPE_STM32F7_TIM3);
    sysbus_realize(SYS_BUS_DEVICE(tim12), &error_fatal);
    sysbus_mmio_map(SYS_BUS_DEVICE(tim12), 0, 0x40001800);

    /* FMC */
    STM32F7FMC *fmc = g_new(STM32F7FMC, 1);
    object_initialize_child(OBJECT(sms), "FMC", fmc, TYPE_STM32F7_FMC);
    sysbus_realize(SYS_BUS_DEVICE(fmc), &error_fatal);
    sysbus_connect_irq(
        SYS_BUS_DEVICE(fmc),
        0,
        qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 48)
    );
    sysbus_mmio_map(SYS_BUS_DEVICE(fmc), 0, STM32F769NIDISCOVERY_FMC_BASE);

    /* RNG */
    STM32F7RNG *rng = g_new(STM32F7RNG, 1);
    object_initialize_child(OBJECT(sms), "RNG", rng, TYPE_STM32F7_RNG);
    sysbus_realize(SYS_BUS_DEVICE(rng), &error_fatal);
    sysbus_connect_irq(
        SYS_BUS_DEVICE(rng),
        0,
        qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 80)
    );
    sysbus_mmio_map(SYS_BUS_DEVICE(rng), 0, STM32F769NIDISCOVERY_RNG_BASE);

    /* ETH */
    qemu_check_nic_model(&nd_table[0], "stm32f7");
    DeviceState *eth = qdev_new(TYPE_STM32F7_ETH);
    qdev_set_nic_properties(eth, &nd_table[0]);
    sysbus_realize(SYS_BUS_DEVICE(eth), &error_fatal);
    sysbus_connect_irq(
        SYS_BUS_DEVICE(eth),
        0,
        qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 61)
    );
    sysbus_mmio_map(SYS_BUS_DEVICE(eth), 0, STM32F769NIDISCOVERY_ETH_BASE);

    /* SYSCFG */
    STM32F7SYSCFG *syscfg = g_new(STM32F7SYSCFG, 1);
    object_initialize_child(OBJECT(sms), "SYSCFG", syscfg, TYPE_STM32F7_SYSCFG);
    sysbus_realize(SYS_BUS_DEVICE(syscfg), &error_fatal);
    sysbus_mmio_map(SYS_BUS_DEVICE(syscfg), 0, STM32F769NIDISCOVERY_SYSCFG_BASE);

    /* EXTI */
    STM32F7EXTI *exti = g_new(STM32F7EXTI, 1);
    object_initialize_child(OBJECT(sms), "EXTI", exti, TYPE_STM32F7_EXTI);
    sysbus_realize(SYS_BUS_DEVICE(exti), &error_fatal);
    sysbus_mmio_map(SYS_BUS_DEVICE(exti), 0, 0x40013C00);
    const int exti_irqs[] = {2, 6, 7, 8, 9, 10, 23, 40, 1};
    for (int i = 0; i < sizeof(exti_irqs) / sizeof(*exti_irqs); ++i) {
        sysbus_connect_irq(
            SYS_BUS_DEVICE(exti),
            i,
            qdev_get_gpio_in(DEVICE(&(sms->armv7m)), exti_irqs[i])
        );
    }

    STM32F7ADC *p0 = g_new(STM32F7ADC, 1);
	object_initialize_child(OBJECT(sms), "ADC1", p0, TYPE_STM32F7_ADC);
	sysbus_realize(SYS_BUS_DEVICE(p0), &error_fatal);
	sysbus_connect_irq(SYS_BUS_DEVICE(p0), 0, qdev_get_gpio_in(DEVICE(&(sms->armv7m)), 18));
	sysbus_mmio_map(SYS_BUS_DEVICE(p0), 0, 0x40012000);

	STM32F7ADC *p1 = g_new(STM32F7ADC, 1);
	object_initialize_child(OBJECT(sms), "ADC2", p1, TYPE_STM32F7_ADC);
	sysbus_realize(SYS_BUS_DEVICE(p1), &error_fatal);
	sysbus_mmio_map(SYS_BUS_DEVICE(p1), 0, 0x40012100);

	STM32F7ADC *p2 = g_new(STM32F7ADC, 1);
	object_initialize_child(OBJECT(sms), "ADC3", p2, TYPE_STM32F7_ADC);
	sysbus_realize(SYS_BUS_DEVICE(p2), &error_fatal);
	sysbus_mmio_map(SYS_BUS_DEVICE(p2), 0, 0x40012200);

	STM32F7ADC_COMMON *p3 = g_new(STM32F7ADC_COMMON, 1);
	object_initialize_child(OBJECT(sms), "ADC_Common", p3, TYPE_STM32F7_ADC_COMMON);
	sysbus_realize(SYS_BUS_DEVICE(p3), &error_fatal);
	sysbus_mmio_map(SYS_BUS_DEVICE(p3), 0, 0x40012300);
}

static void stm32f7_common_init(MachineState *machine) {
    STM32F7MachineState *sms = STM32F7_MACHINE(machine);
    STM32F7MachineClass *smc = STM32F7_MACHINE_GET_CLASS(machine);
    MemoryRegion *sysmem = get_system_memory();
    const MEMInfo *p;

    sms->mem = g_new(MemoryRegion, smc->num_mem_info);

    sms->sysclk = clock_new(OBJECT(sms), "SYSCLK");
    clock_set_hz(sms->sysclk, SYSCLK_FREQ);

    // FUCK I'M STUPID
    sms->refclk = clock_new(OBJECT(sms), "REFCLK");
    clock_set_hz(sms->refclk, SYSCLK_FREQ);

    // initialize memory region
    for (int i = 0; i < smc->num_mem_info; ++i) {
        p = &(smc->meminfo[i]);
        if (p->type == MEM_RAM) {
            memory_region_init_ram(
                &(sms->mem[i]), NULL, p->name, p->size, &error_fatal
            );
        } else if (p->type == MEM_ROM) {
            memory_region_init_rom(
                &(sms->mem[i]), NULL, p->name, p->size, &error_fatal
            );
        } else if (p->type == MEM_ALIAS) {
            memory_region_init_alias(
                &(sms->mem[i]), NULL, p->name, &(sms->mem[p->alias_to]),
                p->base, p->size
            );
        } else {
            printf("stm32f7_common_init: unrecognized memory type, abort\n");
            abort();
        }
        memory_region_add_subregion(sysmem, p->base, &(sms->mem[i]));
    }

    // custome memory setup
    smc->custome_mem_init_fn(machine);

    object_initialize_child(OBJECT(sms), "armv7m", &(sms->armv7m), TYPE_ARMV7M);
    qdev_prop_set_uint32(
        DEVICE(&(sms->armv7m)),"num-irq", smc->num_irq
    );
    qdev_prop_set_string(DEVICE(&(sms->armv7m)), "cpu-type", machine->cpu_type);
    // Cortex-M7 does not support bitbanding
    qdev_prop_set_bit(DEVICE(&(sms->armv7m)), "enable-bitband", false);
    qdev_connect_clock_in(DEVICE(&(sms->armv7m)), "cpuclk", sms->sysclk);
    qdev_connect_clock_in(DEVICE(&(sms->armv7m)), "refclk", sms->refclk);
    // STM32 hack
    qdev_prop_set_uint32(
        DEVICE(&(sms->armv7m)), "init-nsvtor", STM32_MAGIC_VTOR_ADDR
    );
    object_property_set_link(
        OBJECT(&sms->armv7m), "memory", OBJECT(sysmem), &error_abort
    );
    sysbus_realize(SYS_BUS_DEVICE(&sms->armv7m), &error_fatal);

    // intialize peripherals
    // TODO:

    // custome peripheral setup
    smc->custome_periph_init_fn(machine);

    armv7m_load_kernel(
        ARM_CPU(first_cpu), machine->kernel_filename, smc->flash_size
    );
}

static void stm32f7_class_init(ObjectClass *oc, void *data) {
    MachineClass *mc = MACHINE_CLASS(oc);

    mc->init = stm32f7_common_init;
    mc->max_cpus = 1;
    mc->default_cpus = 1;
    mc->default_cpu_type = ARM_CPU_TYPE_NAME("cortex-m7");
}

static void stm32f769idiscovery_class_init(ObjectClass *oc, void *data) {
    MachineClass *mc = MACHINE_CLASS(oc);
    STM32F7MachineClass *smc = STM32F7_MACHINE_CLASS(oc);

    mc->desc = "STM32F769I-Discovery";
    smc->board_type = STM32F769IDISCOVERY;
    smc->sysclk_frq = SYSCLK_FREQ;
    smc->num_irq = 110;
    smc->custome_mem_init_fn = stm32f769nidiscovery_custome_mem_init;
    smc->custome_periph_init_fn = stm32f769nidiscovery_custome_periph_init;
    smc->meminfo = stm32f769nidiscovery_meminfo;
    smc->num_mem_info = (
        sizeof(stm32f769nidiscovery_meminfo) / sizeof(stm32f769nidiscovery_meminfo[0])
    );
    smc->flash_size = 0x800000;
}

static const TypeInfo stm32f7_info = {
    .name = TYPE_STM32F7_MACHINE,
    .parent = TYPE_MACHINE,
    .abstract = true,
    .instance_size = sizeof(STM32F7MachineState),
    .class_size = sizeof(STM32F7MachineClass),
    .class_init = stm32f7_class_init
};

static const TypeInfo stm32f769idiscovery_info = {
    .name = TYPE_STM32F769IDISCOVERY_MACHINE,
    .parent = TYPE_STM32F7_MACHINE,
    .class_init = stm32f769idiscovery_class_init
};

static void stm32f7_machine_init(void) {
    type_register_static(&stm32f7_info);
    type_register_static(&stm32f769idiscovery_info);
}

type_init(stm32f7_machine_init);
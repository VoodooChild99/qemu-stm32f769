/*
 * STM32F7XX RCC
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
#include "hw/misc/stm32f7-rcc.h"
#include "hw/qdev-properties.h"
#include "exec/cpu-common.h"

#define STM32F7_RCC_SIZE				0x400

REG32(CR, 0x0)
    FIELD(CR, PLLSAIRDY, 29, 1)
    FIELD(CR, PLLSAION, 28, 1)
	FIELD(CR, PLLI2SRDY, 27, 1)
	FIELD(CR, PLLI2SON, 26, 1)
	FIELD(CR, PLLRDY, 25, 1)
	FIELD(CR, PLLON, 24, 1)
	FIELD(CR, CSSON, 19, 1)
	FIELD(CR, HSEBYP, 18, 1)
	FIELD(CR, HSERDY, 17, 1)
	FIELD(CR, HSEON, 16, 1)
	FIELD(CR, HSICAL, 8, 8)
	FIELD(CR, HSITRIM, 3, 5)
	FIELD(CR, HSIRDY, 1, 1)
	FIELD(CR, HSION, 0, 1)
REG32(PLLCFGR, 0x4)
	FIELD(PLLCFGR, PLLQ3, 27, 1)
	FIELD(PLLCFGR, PLLQ2, 26, 1)
	FIELD(PLLCFGR, PLLQ1, 25, 1)
	FIELD(PLLCFGR, PLLQ0, 24, 1)
	FIELD(PLLCFGR, PLLSRC, 22, 1)
	FIELD(PLLCFGR, PLLP1, 17, 1)
	FIELD(PLLCFGR, PLLP0, 16, 1)
	FIELD(PLLCFGR, PLLN8, 14, 1)
	FIELD(PLLCFGR, PLLN7, 13, 1)
	FIELD(PLLCFGR, PLLN6, 12, 1)
	FIELD(PLLCFGR, PLLN5, 11, 1)
	FIELD(PLLCFGR, PLLN4, 10, 1)
	FIELD(PLLCFGR, PLLN3, 9, 1)
	FIELD(PLLCFGR, PLLN2, 8, 1)
	FIELD(PLLCFGR, PLLN1, 7, 1)
	FIELD(PLLCFGR, PLLN0, 6, 1)
	FIELD(PLLCFGR, PLLM5, 5, 1)
	FIELD(PLLCFGR, PLLM4, 4, 1)
	FIELD(PLLCFGR, PLLM3, 3, 1)
	FIELD(PLLCFGR, PLLM2, 2, 1)
	FIELD(PLLCFGR, PLLM1, 1, 1)
	FIELD(PLLCFGR, PLLM0, 0, 1)
REG32(CFGR, 0x8)
	FIELD(CFGR, MCO2, 30, 2)
	FIELD(CFGR, MCO2PRE, 27, 3)
	FIELD(CFGR, MCO1PRE, 24, 3)
	FIELD(CFGR, I2SSRC, 23, 1)
	FIELD(CFGR, MCO1, 21, 2)
	FIELD(CFGR, RTCPRE, 16, 5)
	FIELD(CFGR, PPRE2, 13, 3)
	FIELD(CFGR, PPRE1, 10, 3)
	FIELD(CFGR, HPRE, 4, 4)
	FIELD(CFGR, SWS1, 3, 1)
	FIELD(CFGR, SWS0, 2, 1)
	FIELD(CFGR, SW1, 1, 1)
	FIELD(CFGR, SW0, 0, 1)
REG32(CIR, 0xc)
	FIELD(CIR, CSSC, 23, 1)
	FIELD(CIR, PLLSAIRDYC, 22, 1)
	FIELD(CIR, PLLI2SRDYC, 21, 1)
	FIELD(CIR, PLLRDYC, 20, 1)
	FIELD(CIR, HSERDYC, 19, 1)
	FIELD(CIR, HSIRDYC, 18, 1)
	FIELD(CIR, LSERDYC, 17, 1)
	FIELD(CIR, LSIRDYC, 16, 1)
	FIELD(CIR, PLLSAIRDYIE, 14, 1)
	FIELD(CIR, PLLI2SRDYIE, 13, 1)
	FIELD(CIR, PLLRDYIE, 12, 1)
	FIELD(CIR, HSERDYIE, 11, 1)
	FIELD(CIR, HSIRDYIE, 10, 1)
	FIELD(CIR, LSERDYIE, 9, 1)
	FIELD(CIR, LSIRDYIE, 8, 1)
	FIELD(CIR, CSSF, 7, 1)
	FIELD(CIR, PLLSAIRDYF, 6, 1)
	FIELD(CIR, PLLI2SRDYF, 5, 1)
	FIELD(CIR, PLLRDYF, 4, 1)
	FIELD(CIR, HSERDYF, 3, 1)
	FIELD(CIR, HSIRDYF, 2, 1)
	FIELD(CIR, LSERDYF, 1, 1)
	FIELD(CIR, LSIRDYF, 0, 1)
REG32(AHB1RSTR, 0x10)
	FIELD(AHB1RSTR, OTGHSRST, 29, 1)
	FIELD(AHB1RSTR, ETHMACRST, 25, 1)
	FIELD(AHB1RSTR, DMA2DRST, 23, 1)
	FIELD(AHB1RSTR, DMA2RST, 22, 1)
	FIELD(AHB1RSTR, DMA1RST, 21, 1)
	FIELD(AHB1RSTR, CRCRST, 12, 1)
	FIELD(AHB1RSTR, GPIOKRST, 10, 1)
	FIELD(AHB1RSTR, GPIOJRST, 9, 1)
	FIELD(AHB1RSTR, GPIOIRST, 8, 1)
	FIELD(AHB1RSTR, GPIOHRST, 7, 1)
	FIELD(AHB1RSTR, GPIOGRST, 6, 1)
	FIELD(AHB1RSTR, GPIOFRST, 5, 1)
	FIELD(AHB1RSTR, GPIOERST, 4, 1)
	FIELD(AHB1RSTR, GPIODRST, 3, 1)
	FIELD(AHB1RSTR, GPIOCRST, 2, 1)
	FIELD(AHB1RSTR, GPIOBRST, 1, 1)
	FIELD(AHB1RSTR, GPIOARST, 0, 1)
REG32(AHB2RSTR, 0x14)
	FIELD(AHB2RSTR, OTGFSRST, 7, 1)
	FIELD(AHB2RSTR, RNGRST, 6, 1)
	FIELD(AHB2RSTR, HSAHRST, 5, 1)
	FIELD(AHB2RSTR, CRYPRST, 4, 1)
	FIELD(AHB2RSTR, DCMIRST, 0, 1)
REG32(AHB3RSTR, 0x18)
	FIELD(AHB3RSTR, FMCRST, 0, 1)
	FIELD(AHB3RSTR, QSPIRST, 1, 1)
REG32(APB1RSTR, 0x20)
	FIELD(APB1RSTR, TIM2RST, 0, 1)
	FIELD(APB1RSTR, TIM3RST, 1, 1)
	FIELD(APB1RSTR, TIM4RST, 2, 1)
	FIELD(APB1RSTR, TIM5RST, 3, 1)
	FIELD(APB1RSTR, TIM6RST, 4, 1)
	FIELD(APB1RSTR, TIM7RST, 5, 1)
	FIELD(APB1RSTR, TIM12RST, 6, 1)
	FIELD(APB1RSTR, TIM13RST, 7, 1)
	FIELD(APB1RSTR, TIM14RST, 8, 1)
	FIELD(APB1RSTR, WWDGRST, 11, 1)
	FIELD(APB1RSTR, SPI2RST, 14, 1)
	FIELD(APB1RSTR, SPI3RST, 15, 1)
	FIELD(APB1RSTR, UART2RST, 17, 1)
	FIELD(APB1RSTR, UART3RST, 18, 1)
	FIELD(APB1RSTR, UART4RST, 19, 1)
	FIELD(APB1RSTR, UART5RST, 20, 1)
	FIELD(APB1RSTR, I2C1RST, 21, 1)
	FIELD(APB1RSTR, I2C2RST, 22, 1)
	FIELD(APB1RSTR, I2C3RST, 23, 1)
	FIELD(APB1RSTR, CAN1RST, 25, 1)
	FIELD(APB1RSTR, CAN2RST, 26, 1)
	FIELD(APB1RSTR, PWRRST, 28, 1)
	FIELD(APB1RSTR, DACRST, 29, 1)
	FIELD(APB1RSTR, UART7RST, 30, 1)
	FIELD(APB1RSTR, UART8RST, 31, 1)
	FIELD(APB1RSTR, SPDIFRXRST, 16, 1)
	FIELD(APB1RSTR, CECRST, 27, 1)
	FIELD(APB1RSTR, LPTIM1RST, 9, 1)
	FIELD(APB1RSTR, I2C4RST, 24, 1)
REG32(APB2RSTR, 0x24)
	FIELD(APB2RSTR, TIM1RST, 0, 1)
	FIELD(APB2RSTR, TIM8RST, 1, 1)
	FIELD(APB2RSTR, USART1RST, 4, 1)
	FIELD(APB2RSTR, USART6RST, 5, 1)
	FIELD(APB2RSTR, ADCRST, 8, 1)
	FIELD(APB2RSTR, SPI1RST, 12, 1)
	FIELD(APB2RSTR, SPI4RST, 13, 1)
	FIELD(APB2RSTR, SYSCFGRST, 14, 1)
	FIELD(APB2RSTR, TIM9RST, 16, 1)
	FIELD(APB2RSTR, TIM10RST, 17, 1)
	FIELD(APB2RSTR, TIM11RST, 18, 1)
	FIELD(APB2RSTR, SPI5RST, 20, 1)
	FIELD(APB2RSTR, SPI6RST, 21, 1)
	FIELD(APB2RSTR, SAI1RST, 22, 1)
	FIELD(APB2RSTR, LTDCRST, 26, 1)
	FIELD(APB2RSTR, SAI2RST, 23, 1)
	FIELD(APB2RSTR, SDMMC1RST, 11, 1)
REG32(AHB1ENR, 0x30)
	FIELD(AHB1ENR, OTGHSULPIEN, 30, 1)
	FIELD(AHB1ENR, OTGHSEN, 29, 1)
	FIELD(AHB1ENR, ETHMACPTPEN, 28, 1)
	FIELD(AHB1ENR, ETHMACRXEN, 27, 1)
	FIELD(AHB1ENR, ETHMACTXEN, 26, 1)
	FIELD(AHB1ENR, ETHMACEN, 25, 1)
	FIELD(AHB1ENR, DMA2DEN, 23, 1)
	FIELD(AHB1ENR, DMA2EN, 22, 1)
	FIELD(AHB1ENR, DMA1EN, 21, 1)
	FIELD(AHB1ENR, CCMDATARAMEN, 20, 1)
	FIELD(AHB1ENR, BKPSRAMEN, 18, 1)
	FIELD(AHB1ENR, CRCEN, 12, 1)
	FIELD(AHB1ENR, GPIOKEN, 10, 1)
	FIELD(AHB1ENR, GPIOJEN, 9, 1)
	FIELD(AHB1ENR, GPIOIEN, 8, 1)
	FIELD(AHB1ENR, GPIOHEN, 7, 1)
	FIELD(AHB1ENR, GPIOGEN, 6, 1)
	FIELD(AHB1ENR, GPIOFEN, 5, 1)
	FIELD(AHB1ENR, GPIOEEN, 4, 1)
	FIELD(AHB1ENR, GPIODEN, 3, 1)
	FIELD(AHB1ENR, GPIOCEN, 2, 1)
	FIELD(AHB1ENR, GPIOBEN, 1, 1)
	FIELD(AHB1ENR, GPIOAEN, 0, 1)
REG32(AHB2ENR, 0x34)
	FIELD(AHB2ENR, OTGFSEN, 7, 1)
	FIELD(AHB2ENR, RNGEN, 6, 1)
	FIELD(AHB2ENR, HASHEN, 5, 1)
	FIELD(AHB2ENR, CRYPEN, 4, 1)
	FIELD(AHB2ENR, DCMIEN, 0, 1)
REG32(AHB3ENR, 0x38)
	FIELD(AHB3ENR, FMCEN, 0, 1)
	FIELD(AHB3ENR, QSPIEN, 1, 1)
REG32(APB1ENR, 0x40)
	FIELD(APB1ENR, TIM2EN, 0, 1)
	FIELD(APB1ENR, TIM3EN, 1, 1)
	FIELD(APB1ENR, TIM4EN, 2, 1)
	FIELD(APB1ENR, TIM5EN, 3, 1)
	FIELD(APB1ENR, TIM6EN, 4, 1)
	FIELD(APB1ENR, TIM7EN, 5, 1)
	FIELD(APB1ENR, TIM12EN, 6, 1)
	FIELD(APB1ENR, TIM13EN, 7, 1)
	FIELD(APB1ENR, TIM14EN, 8, 1)
	FIELD(APB1ENR, WWDGEN, 11, 1)
	FIELD(APB1ENR, SPI2EN, 14, 1)
	FIELD(APB1ENR, SPI3EN, 15, 1)
	FIELD(APB1ENR, USART2EN, 17, 1)
	FIELD(APB1ENR, USART3EN, 18, 1)
	FIELD(APB1ENR, UART4EN, 19, 1)
	FIELD(APB1ENR, UART5EN, 20, 1)
	FIELD(APB1ENR, I2C1EN, 21, 1)
	FIELD(APB1ENR, I2C2EN, 22, 1)
	FIELD(APB1ENR, I2C3EN, 23, 1)
	FIELD(APB1ENR, CAN1EN, 25, 1)
	FIELD(APB1ENR, CAN2EN, 26, 1)
	FIELD(APB1ENR, PWREN, 28, 1)
	FIELD(APB1ENR, DACEN, 29, 1)
	FIELD(APB1ENR, UART7ENR, 30, 1)
	FIELD(APB1ENR, UART8ENR, 31, 1)
	FIELD(APB1ENR, SPDIFRXEN, 16, 1)
	FIELD(APB1ENR, CECEN, 27, 1)
	FIELD(APB1ENR, LPTMI1EN, 9, 1)
	FIELD(APB1ENR, I2C4EN, 24, 1)
REG32(APB2ENR, 0x44)
	FIELD(APB2ENR, TIM1EN, 0, 1)
	FIELD(APB2ENR, TIM8EN, 1, 1)
	FIELD(APB2ENR, USART1EN, 4, 1)
	FIELD(APB2ENR, USART6EN, 5, 1)
	FIELD(APB2ENR, ADC1EN, 8, 1)
	FIELD(APB2ENR, ADC2EN, 9, 1)
	FIELD(APB2ENR, ADC3EN, 10, 1)
	FIELD(APB2ENR, SPI1EN, 12, 1)
	FIELD(APB2ENR, SPI4ENR, 13, 1)
	FIELD(APB2ENR, SYSCFGEN, 14, 1)
	FIELD(APB2ENR, TIM9EN, 16, 1)
	FIELD(APB2ENR, TIM10EN, 17, 1)
	FIELD(APB2ENR, TIM11EN, 18, 1)
	FIELD(APB2ENR, SPI5ENR, 20, 1)
	FIELD(APB2ENR, SPI6ENR, 21, 1)
	FIELD(APB2ENR, SAI1EN, 22, 1)
	FIELD(APB2ENR, LTDCEN, 26, 1)
	FIELD(APB2ENR, SAI2EN, 23, 1)
	FIELD(APB2ENR, SDMMC1EN, 11, 1)
REG32(AHB1LPENR, 0x50)
	FIELD(AHB1LPENR, GPIOALPEN, 0, 1)
	FIELD(AHB1LPENR, GPIOBLPEN, 1, 1)
	FIELD(AHB1LPENR, GPIOCLPEN, 2, 1)
	FIELD(AHB1LPENR, GPIODLPEN, 3, 1)
	FIELD(AHB1LPENR, GPIOELPEN, 4, 1)
	FIELD(AHB1LPENR, GPIOFLPEN, 5, 1)
	FIELD(AHB1LPENR, GPIOGLPEN, 6, 1)
	FIELD(AHB1LPENR, GPIOHLPEN, 7, 1)
	FIELD(AHB1LPENR, GPIOILPEN, 8, 1)
	FIELD(AHB1LPENR, GPIOJLPEN, 9, 1)
	FIELD(AHB1LPENR, GPIOKLPEN, 10, 1)
	FIELD(AHB1LPENR, CRCLPEN, 12, 1)
	FIELD(AHB1LPENR, FLITFLPEN, 15, 1)
	FIELD(AHB1LPENR, SRAM1LPEN, 16, 1)
	FIELD(AHB1LPENR, SRAM2LPEN, 17, 1)
	FIELD(AHB1LPENR, BKPSRAMLPEN, 18, 1)
	FIELD(AHB1LPENR, SRAM3LPEN, 19, 1)
	FIELD(AHB1LPENR, DMA1LPEN, 21, 1)
	FIELD(AHB1LPENR, DMA2LPEN, 22, 1)
	FIELD(AHB1LPENR, DMA2DLPEN, 23, 1)
	FIELD(AHB1LPENR, ETHMACLPEN, 25, 1)
	FIELD(AHB1LPENR, ETHMACTXLPEN, 26, 1)
	FIELD(AHB1LPENR, ETHMACRXLPEN, 27, 1)
	FIELD(AHB1LPENR, ETHMACPTPLPEN, 28, 1)
	FIELD(AHB1LPENR, OTGHSLPEN, 29, 1)
	FIELD(AHB1LPENR, OTGHSULPILPEN, 30, 1)
REG32(AHB2LPENR, 0x54)
	FIELD(AHB2LPENR, OTGFSLPEN, 7, 1)
	FIELD(AHB2LPENR, RNGLPEN, 6, 1)
	FIELD(AHB2LPENR, HASHLPEN, 5, 1)
	FIELD(AHB2LPENR, CRYPLPEN, 4, 1)
	FIELD(AHB2LPENR, DCMILPEN, 0, 1)
REG32(AHB3LPENR, 0x58)
	FIELD(AHB3LPENR, FMCLPEN, 0, 1)
	FIELD(AHB3LPENR, QSPILPEN, 1, 1)
REG32(APB1LPENR, 0x60)
	FIELD(APB1LPENR, TIM2LPEN, 0, 1)
	FIELD(APB1LPENR, TIM3LPEN, 1, 1)
	FIELD(APB1LPENR, TIM4LPEN, 2, 1)
	FIELD(APB1LPENR, TIM5LPEN, 3, 1)
	FIELD(APB1LPENR, TIM6LPEN, 4, 1)
	FIELD(APB1LPENR, TIM7LPEN, 5, 1)
	FIELD(APB1LPENR, TIM12LPEN, 6, 1)
	FIELD(APB1LPENR, TIM13LPEN, 7, 1)
	FIELD(APB1LPENR, TIM14LPEN, 8, 1)
	FIELD(APB1LPENR, WWDGLPEN, 11, 1)
	FIELD(APB1LPENR, SPI2LPEN, 14, 1)
	FIELD(APB1LPENR, SPI3LPEN, 15, 1)
	FIELD(APB1LPENR, USART2LPEN, 17, 1)
	FIELD(APB1LPENR, USART3LPEN, 18, 1)
	FIELD(APB1LPENR, UART4LPEN, 19, 1)
	FIELD(APB1LPENR, UART5LPEN, 20, 1)
	FIELD(APB1LPENR, I2C1LPEN, 21, 1)
	FIELD(APB1LPENR, I2C2LPEN, 22, 1)
	FIELD(APB1LPENR, I2C3LPEN, 23, 1)
	FIELD(APB1LPENR, CAN1LPEN, 25, 1)
	FIELD(APB1LPENR, CAN2LPEN, 26, 1)
	FIELD(APB1LPENR, PWRLPEN, 28, 1)
	FIELD(APB1LPENR, DACLPEN, 29, 1)
	FIELD(APB1LPENR, UART7LPEN, 30, 1)
	FIELD(APB1LPENR, UART8LPEN, 31, 1)
	FIELD(APB1LPENR, SPDIFRXLPEN, 16, 1)
	FIELD(APB1LPENR, CECLPEN, 27, 1)
	FIELD(APB1LPENR, LPTIM1LPEN, 9, 1)
	FIELD(APB1LPENR, I2C4LPEN, 24, 1)
REG32(APB2LPENR, 0x64)
	FIELD(APB2LPENR, TIM1LPEN, 0, 1)
	FIELD(APB2LPENR, TIM8LPEN, 1, 1)
	FIELD(APB2LPENR, USART1LPEN, 4, 1)
	FIELD(APB2LPENR, USART6LPEN, 5, 1)
	FIELD(APB2LPENR, ADC1LPEN, 8, 1)
	FIELD(APB2LPENR, ADC2LPEN, 9, 1)
	FIELD(APB2LPENR, ADC3LPEN, 10, 1)
	FIELD(APB2LPENR, SPI1LPEN, 12, 1)
	FIELD(APB2LPENR, SPI4LPEN, 13, 1)
	FIELD(APB2LPENR, SYSCFGLPEN, 14, 1)
	FIELD(APB2LPENR, TIM9LPEN, 16, 1)
	FIELD(APB2LPENR, TIM10LPEN, 17, 1)
	FIELD(APB2LPENR, TIM11LPEN, 18, 1)
	FIELD(APB2LPENR, SPI5LPEN, 20, 1)
	FIELD(APB2LPENR, SPI6LPEN, 21, 1)
	FIELD(APB2LPENR, SAI1LPEN, 22, 1)
	FIELD(APB2LPENR, LTDCLPEN, 26, 1)
	FIELD(APB2LPENR, SAI2LPEN, 23, 1)
	FIELD(APB2LPENR, SDMMC1LPEN, 11, 1)
REG32(BDCR, 0x70)
	FIELD(BDCR, BDRST, 16, 1)
	FIELD(BDCR, RTCEN, 15, 1)
	FIELD(BDCR, RTCSEL1, 9, 1)
	FIELD(BDCR, RTCSEL0, 8, 1)
	FIELD(BDCR, LSEBYP, 2, 1)
	FIELD(BDCR, LSERDY, 1, 1)
	FIELD(BDCR, LSEON, 0, 1)
REG32(CSR, 0x74)
	FIELD(CSR, LPWRRSTF, 31, 1)
	FIELD(CSR, WWDGRSTF, 30, 1)
	FIELD(CSR, WDGRSTF, 29, 1)
	FIELD(CSR, SFTRSTF, 28, 1)
	FIELD(CSR, PORRSTF, 27, 1)
	FIELD(CSR, PADRSTF, 26, 1)
	FIELD(CSR, BORRSTF, 25, 1)
	FIELD(CSR, RMVF, 24, 1)
	FIELD(CSR, LSIRDY, 1, 1)
	FIELD(CSR, LSION, 0, 1)
REG32(SSCGR, 0x80)
	FIELD(SSCGR, SSCGEN, 31, 1)
	FIELD(SSCGR, SPREADSEL, 30, 1)
	FIELD(SSCGR, INCSTEP, 13, 15)
	FIELD(SSCGR, MODPER, 0, 13)
REG32(PLLI2SCFGR, 0x84)
	FIELD(PLLI2SCFGR, PLLI2SR, 28, 3)
	FIELD(PLLI2SCFGR, PLLI2SQ, 24, 4)
	FIELD(PLLI2SCFGR, PLLI2SN, 6, 9)
REG32(PLLSAICFGR, 0x88)
	FIELD(PLLSAICFGR, PLLSAIN, 6, 9)
	FIELD(PLLSAICFGR, PLLSAIP, 16, 2)
	FIELD(PLLSAICFGR, PLLSAIQ, 24, 4)
	FIELD(PLLSAICFGR, PLLSAIR, 28, 3)
REG32(DKCFGR1, 0x8c)
	FIELD(DKCFGR1, PLLI2SDIV, 0, 5)
	FIELD(DKCFGR1, PLLSAIDIVQ, 8, 5)
	FIELD(DKCFGR1, PLLSAIDIVR, 16, 2)
	FIELD(DKCFGR1, SAI1SEL, 20, 2)
	FIELD(DKCFGR1, SAI2SEL, 22, 2)
	FIELD(DKCFGR1, TIMPRE, 24, 1)
REG32(DKCFGR2, 0x90)
	FIELD(DKCFGR2, USART1SEL, 0, 2)
	FIELD(DKCFGR2, USART2SEL, 2, 2)
	FIELD(DKCFGR2, USART3SEL, 4, 2)
	FIELD(DKCFGR2, UART4SEL, 6, 2)
	FIELD(DKCFGR2, UART5SEL, 8, 2)
	FIELD(DKCFGR2, USART6SEL, 10, 2)
	FIELD(DKCFGR2, UART7SEL, 12, 2)
	FIELD(DKCFGR2, UART8SEL, 14, 2)
	FIELD(DKCFGR2, I2C1SEL, 16, 2)
	FIELD(DKCFGR2, I2C2SEL, 18, 2)
	FIELD(DKCFGR2, I2C3SEL, 20, 2)
	FIELD(DKCFGR2, I2C4SEL, 22, 2)
	FIELD(DKCFGR2, LPTIM1SEL, 24, 2)
	FIELD(DKCFGR2, CECSEL, 26, 1)
	FIELD(DKCFGR2, CK48MSEL, 27, 1)
	FIELD(DKCFGR2, SDMMCSEL, 28, 1)

static void stm32f7_rcc_register_reset(STM32F7RCC *t) {
	t->CR = 0x00000083;
	t->PLLCFGR = 0x24003010;
	t->CFGR = 0x00000000;
	t->CIR = 0x00000000;
	t->AHB1RSTR = 0x00000000;
	t->AHB2RSTR = 0x00000000;
	t->AHB3RSTR = 0x00000000;
	t->APB1RSTR = 0x00000000;
	t->APB2RSTR = 0x00000000;
	t->AHB1ENR = 0x00100000;
	t->AHB2ENR = 0x00000000;
	t->AHB3ENR = 0x00000000;
	t->APB1ENR = 0x00000000;
	t->APB2ENR = 0x00000000;
	t->AHB1LPENR = 0x7e6791ff;
	t->AHB2LPENR = 0x000000f1;
	t->AHB3LPENR = 0x00000001;
	t->APB1LPENR = 0x36fec9ff;
	t->APB2LPENR = 0x00075f33;
	t->BDCR = 0x00000000;
	t->CSR = 0x0e000000;
	t->SSCGR = 0x00000000;
	t->PLLI2SCFGR = 0x20003000;
	t->PLLSAICFGR = 0x20003000;
	t->DKCFGR1 = 0x20003000;
	t->DKCFGR2 = 0x20003000;
}

static uint64_t stm32f7_rcc_read(void *opaque, hwaddr offset, unsigned size) {
	STM32F7RCC *t = STM32F7_RCC(opaque);
	uint64_t ret;

	switch (offset) {
		case A_CR:
			ret = t->CR;
			break;
		case A_PLLCFGR:
			ret = t->PLLCFGR;
			break;
		case A_CFGR:
			ret = t->CFGR;
			break;
		case A_CIR:
			ret = t->CIR;
			break;
		case A_AHB1RSTR:
			ret = t->AHB1RSTR;
			break;
		case A_AHB2RSTR:
			ret = t->AHB2RSTR;
			break;
		case A_AHB3RSTR:
			ret = t->AHB3RSTR;
			break;
		case A_APB1RSTR:
			ret = t->APB1RSTR;
			break;
		case A_APB2RSTR:
			ret = t->APB2RSTR;
			break;
		case A_AHB1ENR:
			ret = t->AHB1ENR;
			break;
		case A_AHB2ENR:
			ret = t->AHB2ENR;
			break;
		case A_AHB3ENR:
			ret = t->AHB3ENR;
			break;
		case A_APB1ENR:
			ret = t->APB1ENR;
			break;
		case A_APB2ENR:
			ret = t->APB2ENR;
			break;
		case A_AHB1LPENR:
			ret = t->AHB1LPENR;
			break;
		case A_AHB2LPENR:
			ret = t->AHB2LPENR;
			break;
		case A_AHB3LPENR:
			ret = t->AHB3LPENR;
			break;
		case A_APB1LPENR:
			ret = t->APB1LPENR;
			break;
		case A_APB2LPENR:
			ret = t->APB2LPENR;
			break;
		case A_BDCR:
			ret = t->BDCR;
			break;
		case A_CSR:
			ret = t->CSR;
			break;
		case A_SSCGR:
			ret = t->SSCGR;
			break;
		case A_PLLI2SCFGR:
			ret = t->PLLI2SCFGR;
			break;
		case A_PLLSAICFGR:
			ret = t->PLLSAICFGR;
			break;
		case A_DKCFGR1:
			ret = t->DKCFGR1;
			break;
		case A_DKCFGR2:
			ret = t->DKCFGR2;
			break;
		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F7 RCC read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32f7_rcc_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32F7RCC *t = STM32F7_RCC(opaque);
	switch (offset) {
		case A_CR:
            if (value & R_CR_HSION_MASK) {
                value |= R_CR_HSIRDY_MASK;
            }
            if (value & R_CR_HSEON_MASK) {
                value |= R_CR_HSERDY_MASK;
            }
            if (value & R_CR_PLLON_MASK) {
                value |= R_CR_PLLRDY_MASK;
            }
            if (value & R_CR_PLLI2SON_MASK) {
                value |= R_CR_PLLI2SRDY_MASK;
            }
            if (value & R_CR_PLLSAION_MASK) {
                value |= R_CR_PLLSAIRDY_MASK;
            }
            t->CR = value;
			break;
		case A_PLLCFGR:
			t->PLLCFGR = value;
			break;
		case A_CFGR:
			value |= ((value & (R_CFGR_SW0_MASK | R_CFGR_SW1_MASK)) << 2);
			t->CFGR = value;
			break;
		case A_CIR:
			t->CIR = value;
			break;
		case A_AHB1RSTR:
			t->AHB1RSTR = value;
			break;
		case A_AHB2RSTR:
			t->AHB2RSTR = value;
			break;
		case A_AHB3RSTR:
			t->AHB3RSTR = value;
			break;
		case A_APB1RSTR:
			t->APB1RSTR = value;
			break;
		case A_APB2RSTR:
			t->APB2RSTR = value;
			break;
		case A_AHB1ENR:
			t->AHB1ENR = value;
			break;
		case A_AHB2ENR:
			t->AHB2ENR = value;
			break;
		case A_AHB3ENR:
			t->AHB3ENR = value;
			break;
		case A_APB1ENR:
			t->APB1ENR = value;
			break;
		case A_APB2ENR:
			t->APB2ENR = value;
			break;
		case A_AHB1LPENR:
			t->AHB1LPENR = value;
			break;
		case A_AHB2LPENR:
			t->AHB2LPENR = value;
			break;
		case A_AHB3LPENR:
			t->AHB3LPENR = value;
			break;
		case A_APB1LPENR:
			t->APB1LPENR = value;
			break;
		case A_APB2LPENR:
			t->APB2LPENR = value;
			break;
		case A_BDCR:
			t->BDCR = value;
			break;
		case A_CSR:
			if (value & R_CSR_LSION_MASK) {
				value |= R_CSR_LSIRDY_MASK;
			}
			t->CSR = value;
			break;
		case A_SSCGR:
			t->SSCGR = value;
			break;
		case A_PLLI2SCFGR:
			t->PLLI2SCFGR = value;
			break;
		case A_PLLSAICFGR:
			t->PLLSAICFGR = value;
			break;
		case A_DKCFGR1:
			t->DKCFGR1 = value;
			break;
		case A_DKCFGR2:
			t->DKCFGR2 = value;
			break;
		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F7 RCC write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32f7_rcc_ops = {
	.read = stm32f7_rcc_read,
	.write = stm32f7_rcc_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static void stm32f7_rcc_init(Object *obj) {
	SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
	STM32F7RCC *t = STM32F7_RCC(obj);

	memory_region_init_io(&(t->iomem), obj, &stm32f7_rcc_ops, t, "stm32f7-rcc", STM32F7_RCC_SIZE);
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0 ; i < 1 ; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}
}

static void stm32f7_rcc_realize(DeviceState *dev, Error **errp) {
	return;
}

static void stm32f7_rcc_reset_enter(Object *obj, ResetType type) {
	STM32F7RCC *t = STM32F7_RCC(obj);

	stm32f7_rcc_register_reset(t);
}

static const VMStateDescription stm32f7_rcc_vmstate = {
	.name = "stm32f7-rcc",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(CR, STM32F7RCC),
		VMSTATE_UINT32(PLLCFGR, STM32F7RCC),
		VMSTATE_UINT32(CFGR, STM32F7RCC),
		VMSTATE_UINT32(CIR, STM32F7RCC),
		VMSTATE_UINT32(AHB1RSTR, STM32F7RCC),
		VMSTATE_UINT32(AHB2RSTR, STM32F7RCC),
		VMSTATE_UINT32(AHB3RSTR, STM32F7RCC),
		VMSTATE_UINT32(APB1RSTR, STM32F7RCC),
		VMSTATE_UINT32(APB2RSTR, STM32F7RCC),
		VMSTATE_UINT32(AHB1ENR, STM32F7RCC),
		VMSTATE_UINT32(AHB2ENR, STM32F7RCC),
		VMSTATE_UINT32(AHB3ENR, STM32F7RCC),
		VMSTATE_UINT32(APB1ENR, STM32F7RCC),
		VMSTATE_UINT32(APB2ENR, STM32F7RCC),
		VMSTATE_UINT32(AHB1LPENR, STM32F7RCC),
		VMSTATE_UINT32(AHB2LPENR, STM32F7RCC),
		VMSTATE_UINT32(AHB3LPENR, STM32F7RCC),
		VMSTATE_UINT32(APB1LPENR, STM32F7RCC),
		VMSTATE_UINT32(APB2LPENR, STM32F7RCC),
		VMSTATE_UINT32(BDCR, STM32F7RCC),
		VMSTATE_UINT32(CSR, STM32F7RCC),
		VMSTATE_UINT32(SSCGR, STM32F7RCC),
		VMSTATE_UINT32(PLLI2SCFGR, STM32F7RCC),
		VMSTATE_UINT32(PLLSAICFGR, STM32F7RCC),
		VMSTATE_UINT32(DKCFGR1, STM32F7RCC),
		VMSTATE_UINT32(DKCFGR2, STM32F7RCC),
		VMSTATE_END_OF_LIST()
	}
};

static void stm32f7_rcc_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);

	dc->vmsd = &stm32f7_rcc_vmstate;
	dc->realize = stm32f7_rcc_realize;
	rc->phases.enter = stm32f7_rcc_reset_enter;
}

static const TypeInfo stm32f7_rcc_info = {
	.name = TYPE_STM32F7_RCC,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32F7RCC),
	.instance_init = stm32f7_rcc_init,
	.class_init = stm32f7_rcc_class_init,
};

static void stm32f7_rcc_register_types(void) {
	type_register_static(&stm32f7_rcc_info);
}

type_init(stm32f7_rcc_register_types);
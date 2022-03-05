#ifndef __STM32F7_ETH_H__
#define __STM32F7_ETH_H__

#include "hw/sysbus.h"
#include "qom/object.h"
#include "net/net.h"
#include "net/eth.h"

#define TYPE_STM32F7_ETH "stm32f7-eth"
OBJECT_DECLARE_SIMPLE_TYPE(STM32F7ETH, STM32F7_ETH)

struct STM32F7ETH {
	/* parent */
	SysBusDevice parent;

	/* memory mapping */
	MemoryRegion iomem;

	/* irqs */
	qemu_irq irq[2];

    /* Backends */
    NICState *nic;
    NICConf conf;

	/* Timer (to poll) */
	QEMUTimer *timer;

	/* additional states */
	uint8_t rxstate;
	uint8_t txstate;
	uint32_t cur_rx_descriptor;
	uint32_t cur_tx_descriptor;

	/*registers*/
	uint32_t MACCR;
	uint32_t MACFFR;
	uint32_t MACHTHR;
	uint32_t MACHTLR;
	uint32_t MACMIIAR;
	uint32_t MACMIIDR;
	uint32_t MACFCR;
	uint32_t MACVLANTR;
	uint32_t MACPMTCSR;
	uint32_t MACDBGR;
	uint32_t MACSR;
	uint32_t MACIMR;
	uint32_t MACA0HR;
	uint32_t MACA0LR;
	uint32_t MACA1HR;
	uint32_t MACA1LR;
	uint32_t MACA2HR;
	uint32_t MACA2LR;
	uint32_t MACA3HR;
	uint32_t MACA3LR;
	uint32_t MACRWUFFER;
	uint32_t MMCCR;
	uint32_t MMCRIR;
	uint32_t MMCTIR;
	uint32_t MMCRIMR;
	uint32_t MMCTIMR;
	uint32_t MMCTGFSCCR;
	uint32_t MMCTGFMSCCR;
	uint32_t MMCTGFCR;
	uint32_t MMCRFCECR;
	uint32_t MMCRFAECR;
	uint32_t MMCRGUFCR;
	uint32_t PTPTSCR;
	uint32_t PTPSSIR;
	uint32_t PTPTSHR;
	uint32_t PTPTSLR;
	uint32_t PTPTSHUR;
	uint32_t PTPTSLUR;
	uint32_t PTPTSAR;
	uint32_t PTPTTHR;
	uint32_t PTPTTLR;
	uint32_t PTPTSSR;
	uint32_t PTPPPSCR;
	uint32_t DMABMR;
	uint32_t DMATPDR;
	uint32_t DMARPDR;
	uint32_t DMARDLAR;
	uint32_t DMATDLAR;
	uint32_t DMASR;
	uint32_t DMAOMR;
	uint32_t DMAIER;
	uint32_t DMAMFBOCR;
	uint32_t DMARSWTR;
	uint32_t DMACHTDR;
	uint32_t DMACHRDR;
	uint32_t DMACHTBAR;
	uint32_t DMACHRBAR;
};

#endif
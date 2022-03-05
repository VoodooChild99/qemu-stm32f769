#include "qemu/osdep.h"
#include "qemu/log.h"
#include "qemu/bitops.h"
#include "hw/sysbus.h"
#include "hw/irq.h"
#include "migration/vmstate.h"
#include "hw/registerfields.h"
#include "hw/resettable.h"
#include "hw/net/stm32f7-eth.h"
#include "hw/qdev-properties.h"
#include "exec/cpu-common.h"
#include "sysemu/dma.h"

#define STM32F7_ETH_SIZE				0x1400

REG32(MACCR, 0x0)
	FIELD(MACCR, RE, 2, 1)
	FIELD(MACCR, TE, 3, 1)
	FIELD(MACCR, DC, 4, 1)
	FIELD(MACCR, BL, 5, 2)
	FIELD(MACCR, APCS, 7, 1)
	FIELD(MACCR, RD, 9, 1)
	FIELD(MACCR, IPCO, 10, 1)
	FIELD(MACCR, DM, 11, 1)
	FIELD(MACCR, LM, 12, 1)
	FIELD(MACCR, ROD, 13, 1)
	FIELD(MACCR, FES, 14, 1)
	FIELD(MACCR, CSD, 16, 1)
	FIELD(MACCR, IFG, 17, 3)
	FIELD(MACCR, JD, 22, 1)
	FIELD(MACCR, WD, 23, 1)
	FIELD(MACCR, CSTF, 25, 1)
REG32(MACFFR, 0x4)
	FIELD(MACFFR, PM, 0, 1)
	FIELD(MACFFR, HU, 1, 1)
	FIELD(MACFFR, HM, 2, 1)
	FIELD(MACFFR, DAIF, 3, 1)
	FIELD(MACFFR, RAM, 4, 1)
	FIELD(MACFFR, BFD, 5, 1)
	FIELD(MACFFR, PCF, 6, 1)
	FIELD(MACFFR, SAIF, 7, 1)
	FIELD(MACFFR, SAF, 8, 1)
	FIELD(MACFFR, HPF, 9, 1)
	FIELD(MACFFR, RA, 31, 1)
REG32(MACHTHR, 0x8)
	FIELD(MACHTHR, HTH, 0, 32)
REG32(MACHTLR, 0xc)
	FIELD(MACHTLR, HTL, 0, 32)
REG32(MACMIIAR, 0x10)
	FIELD(MACMIIAR, MB, 0, 1)
	FIELD(MACMIIAR, MW, 1, 1)
	FIELD(MACMIIAR, CR, 2, 3)
	FIELD(MACMIIAR, MR, 6, 5)
	FIELD(MACMIIAR, PA, 11, 5)
REG32(MACMIIDR, 0x14)
	FIELD(MACMIIDR, TD, 0, 16)
REG32(MACFCR, 0x18)
	FIELD(MACFCR, FCB, 0, 1)
	FIELD(MACFCR, TFCE, 1, 1)
	FIELD(MACFCR, RFCE, 2, 1)
	FIELD(MACFCR, UPFD, 3, 1)
	FIELD(MACFCR, PLT, 4, 2)
	FIELD(MACFCR, ZQPD, 7, 1)
	FIELD(MACFCR, PT, 16, 16)
REG32(MACVLANTR, 0x1c)
	FIELD(MACVLANTR, VLANTI, 0, 16)
	FIELD(MACVLANTR, VLANTC, 16, 1)
REG32(MACPMTCSR, 0x2c)
	FIELD(MACPMTCSR, PD, 0, 1)
	FIELD(MACPMTCSR, MPE, 1, 1)
	FIELD(MACPMTCSR, WFE, 2, 1)
	FIELD(MACPMTCSR, MPR, 5, 1)
	FIELD(MACPMTCSR, WFR, 6, 1)
	FIELD(MACPMTCSR, GU, 9, 1)
	FIELD(MACPMTCSR, WFFRPR, 31, 1)
REG32(MACDBGR, 0x34)
	FIELD(MACDBGR, CR, 0, 1)
	FIELD(MACDBGR, CSR, 1, 1)
	FIELD(MACDBGR, ROR, 2, 1)
	FIELD(MACDBGR, MCF, 3, 1)
	FIELD(MACDBGR, MCP, 4, 1)
	FIELD(MACDBGR, MCFHP, 5, 1)
REG32(MACSR, 0x38)
	FIELD(MACSR, PMTS, 3, 1)
	FIELD(MACSR, MMCS, 4, 1)
	FIELD(MACSR, MMCRS, 5, 1)
	FIELD(MACSR, MMCTS, 6, 1)
	FIELD(MACSR, TSTS, 9, 1)
REG32(MACIMR, 0x3c)
	FIELD(MACIMR, PMTIM, 3, 1)
	FIELD(MACIMR, TSTIM, 9, 1)
REG32(MACA0HR, 0x40)
	FIELD(MACA0HR, MACA0H, 0, 16)
	FIELD(MACA0HR, MO, 31, 1)
REG32(MACA0LR, 0x44)
	FIELD(MACA0LR, MACA0L, 0, 32)
REG32(MACA1HR, 0x48)
	FIELD(MACA1HR, MACA1H, 0, 16)
	FIELD(MACA1HR, MBC, 24, 6)
	FIELD(MACA1HR, SA, 30, 1)
	FIELD(MACA1HR, AE, 31, 1)
REG32(MACA1LR, 0x4c)
	FIELD(MACA1LR, MACA1LR, 0, 32)
REG32(MACA2HR, 0x50)
	FIELD(MACA2HR, MAC2AH, 0, 16)
	FIELD(MACA2HR, MBC, 24, 6)
	FIELD(MACA2HR, SA, 30, 1)
	FIELD(MACA2HR, AE, 31, 1)
REG32(MACA2LR, 0x54)
	FIELD(MACA2LR, MACA2L, 0, 31)
REG32(MACA3HR, 0x58)
	FIELD(MACA3HR, MACA3H, 0, 16)
	FIELD(MACA3HR, MBC, 24, 6)
	FIELD(MACA3HR, SA, 30, 1)
	FIELD(MACA3HR, AE, 31, 1)
REG32(MACA3LR, 0x5c)
	FIELD(MACA3LR, MBCA3L, 0, 32)
REG32(MACRWUFFER, 0x60)
REG32(MMCCR, 0x100)
	FIELD(MMCCR, CR, 0, 1)
	FIELD(MMCCR, CSR, 1, 1)
	FIELD(MMCCR, ROR, 2, 1)
	FIELD(MMCCR, MCF, 3, 1)
	FIELD(MMCCR, MCP, 4, 1)
	FIELD(MMCCR, MCFHP, 5, 1)
REG32(MMCRIR, 0x104)
	FIELD(MMCRIR, RFCES, 5, 1)
	FIELD(MMCRIR, RFAES, 6, 1)
	FIELD(MMCRIR, RGUFS, 17, 1)
REG32(MMCTIR, 0x108)
	FIELD(MMCTIR, TGFSCS, 14, 1)
	FIELD(MMCTIR, TGFMSCS, 15, 1)
	FIELD(MMCTIR, TGFS, 21, 1)
REG32(MMCRIMR, 0x10c)
	FIELD(MMCRIMR, RFCEM, 5, 1)
	FIELD(MMCRIMR, RFAEM, 6, 1)
	FIELD(MMCRIMR, RGUFM, 17, 1)
REG32(MMCTIMR, 0x110)
	FIELD(MMCTIMR, TGFSCM, 14, 1)
	FIELD(MMCTIMR, TGFMSCM, 15, 1)
	FIELD(MMCTIMR, TGFM, 16, 1)
REG32(MMCTGFSCCR, 0x14c)
	FIELD(MMCTGFSCCR, TGFSCC, 0, 32)
REG32(MMCTGFMSCCR, 0x150)
	FIELD(MMCTGFMSCCR, TGFMSCC, 0, 32)
REG32(MMCTGFCR, 0x168)
	FIELD(MMCTGFCR, TGFC, 0, 32)
REG32(MMCRFCECR, 0x194)
	FIELD(MMCRFCECR, RFCFC, 0, 32)
REG32(MMCRFAECR, 0x198)
	FIELD(MMCRFAECR, RFAEC, 0, 32)
REG32(MMCRGUFCR, 0x1c4)
	FIELD(MMCRGUFCR, RGUFC, 0, 32)
REG32(PTPTSCR, 0x700)
	FIELD(PTPTSCR, TSE, 0, 1)
	FIELD(PTPTSCR, TSFCU, 1, 1)
	FIELD(PTPTSCR, TSPTPPSV2E, 10, 1)
	FIELD(PTPTSCR, TSSPTPOEFE, 11, 1)
	FIELD(PTPTSCR, TSSIPV6FE, 12, 1)
	FIELD(PTPTSCR, TSSIPV4FE, 13, 1)
	FIELD(PTPTSCR, TSSEME, 14, 1)
	FIELD(PTPTSCR, TSSMRME, 15, 1)
	FIELD(PTPTSCR, TSCNT, 16, 2)
	FIELD(PTPTSCR, TSPFFMAE, 18, 1)
	FIELD(PTPTSCR, TSSTI, 2, 1)
	FIELD(PTPTSCR, TSSTU, 3, 1)
	FIELD(PTPTSCR, TSITE, 4, 1)
	FIELD(PTPTSCR, TTSARU, 5, 1)
	FIELD(PTPTSCR, TSSARFE, 8, 1)
	FIELD(PTPTSCR, TSSSR, 9, 1)
REG32(PTPSSIR, 0x704)
	FIELD(PTPSSIR, STSSI, 0, 8)
REG32(PTPTSHR, 0x708)
	FIELD(PTPTSHR, STS, 0, 32)
REG32(PTPTSLR, 0x70c)
	FIELD(PTPTSLR, STSS, 0, 31)
	FIELD(PTPTSLR, STPNS, 31, 1)
REG32(PTPTSHUR, 0x710)
	FIELD(PTPTSHUR, TSUS, 0, 32)
REG32(PTPTSLUR, 0x714)
	FIELD(PTPTSLUR, TSUSS, 0, 31)
	FIELD(PTPTSLUR, TSUPNS, 31, 1)
REG32(PTPTSAR, 0x718)
	FIELD(PTPTSAR, TSA, 0, 32)
REG32(PTPTTHR, 0x71c)
	FIELD(PTPTTHR, TTSH, 0, 32)
REG32(PTPTTLR, 0x720)
	FIELD(PTPTTLR, TTSL, 0, 32)
REG32(PTPTSSR, 0x728)
	FIELD(PTPTSSR, TSSO, 0, 1)
	FIELD(PTPTSSR, TSTTR, 1, 1)
REG32(PTPPPSCR, 0x72c)
	FIELD(PTPPPSCR, TSSO, 0, 1)
	FIELD(PTPPPSCR, TSTTR, 1, 1)
REG32(DMABMR, 0x1000)
	FIELD(DMABMR, SR, 0, 1)
	FIELD(DMABMR, DA, 1, 1)
	FIELD(DMABMR, DSL, 2, 5)
	FIELD(DMABMR, EDFE, 7, 1)
	FIELD(DMABMR, PBL, 8, 6)
	FIELD(DMABMR, RTPR, 14, 2)
	FIELD(DMABMR, FB, 16, 1)
	FIELD(DMABMR, RDP, 17, 6)
	FIELD(DMABMR, USP, 23, 1)
	FIELD(DMABMR, FPM, 24, 1)
	FIELD(DMABMR, AAB, 25, 1)
	FIELD(DMABMR, MB, 26, 1)
REG32(DMATPDR, 0x1004)
	FIELD(DMATPDR, TPD, 0, 32)
REG32(DMARPDR, 0x1008)
	FIELD(DMARPDR, RPD, 0, 32)
REG32(DMARDLAR, 0x100c)
	FIELD(DMARDLAR, SRL, 0, 32)
REG32(DMATDLAR, 0x1010)
	FIELD(DMATDLAR, STL, 0, 32)
REG32(DMASR, 0x1014)
	FIELD(DMASR, TS, 0, 1)
	FIELD(DMASR, TPSS, 1, 1)
	FIELD(DMASR, TBUS, 2, 1)
	FIELD(DMASR, TJTS, 3, 1)
	FIELD(DMASR, ROS, 4, 1)
	FIELD(DMASR, TUS, 5, 1)
	FIELD(DMASR, RS, 6, 1)
	FIELD(DMASR, RBUS, 7, 1)
	FIELD(DMASR, RPSS, 8, 1)
	FIELD(DMASR, PWTS, 9, 1)
	FIELD(DMASR, ETS, 10, 1)
	FIELD(DMASR, FBES, 13, 1)
	FIELD(DMASR, ERS, 14, 1)
	FIELD(DMASR, AIS, 15, 1)
	FIELD(DMASR, NIS, 16, 1)
	FIELD(DMASR, RPS, 17, 3)
	FIELD(DMASR, TPS, 20, 3)
	FIELD(DMASR, EBS, 23, 3)
	FIELD(DMASR, MMCS, 27, 1)
	FIELD(DMASR, PMTS, 28, 1)
	FIELD(DMASR, TSTS, 29, 1)
REG32(DMAOMR, 0x1018)
	FIELD(DMAOMR, SR, 1, 1)
	FIELD(DMAOMR, OSF, 2, 1)
	FIELD(DMAOMR, RTC, 3, 2)
	FIELD(DMAOMR, FUGF, 6, 1)
	FIELD(DMAOMR, FEF, 7, 1)
	FIELD(DMAOMR, ST, 13, 1)
	FIELD(DMAOMR, TTC, 14, 3)
	FIELD(DMAOMR, FTF, 20, 1)
	FIELD(DMAOMR, TSF, 21, 1)
	FIELD(DMAOMR, DFRF, 24, 1)
	FIELD(DMAOMR, RSF, 25, 1)
	FIELD(DMAOMR, DTCEFD, 26, 1)
REG32(DMAIER, 0x101c)
	FIELD(DMAIER, TIE, 0, 1)
	FIELD(DMAIER, TPSIE, 1, 1)
	FIELD(DMAIER, TBUIE, 2, 1)
	FIELD(DMAIER, TJTIE, 3, 1)
	FIELD(DMAIER, ROIE, 4, 1)
	FIELD(DMAIER, TUIE, 5, 1)
	FIELD(DMAIER, RIE, 6, 1)
	FIELD(DMAIER, RBUIE, 7, 1)
	FIELD(DMAIER, RPSIE, 8, 1)
	FIELD(DMAIER, RWTIE, 9, 1)
	FIELD(DMAIER, ETIE, 10, 1)
	FIELD(DMAIER, FBEIE, 13, 1)
	FIELD(DMAIER, ERIE, 14, 1)
	FIELD(DMAIER, AISE, 15, 1)
	FIELD(DMAIER, NISE, 16, 1)
REG32(DMAMFBOCR, 0x1020)
	FIELD(DMAMFBOCR, MFC, 0, 16)
	FIELD(DMAMFBOCR, OMFC, 16, 1)
	FIELD(DMAMFBOCR, MFA, 17, 11)
	FIELD(DMAMFBOCR, OFOC, 28, 1)
REG32(DMARSWTR, 0x1024)
	FIELD(DMARSWTR, RSWTC, 0, 8)
REG32(DMACHTDR, 0x1048)
	FIELD(DMACHTDR, HTDAP, 0, 32)
REG32(DMACHRDR, 0x104c)
	FIELD(DMACHRDR, HRDAP, 0, 32)
REG32(DMACHTBAR, 0x1050)
	FIELD(DMACHTBAR, HTBAP, 0, 32)
REG32(DMACHRBAR, 0x1054)
	FIELD(DMACHRBAR, HRBAP, 0, 32)

static void stm32f7_eth_register_reset(STM32F7ETH *t) {
	t->MACCR = 0x00008000;
	t->MACFFR = 0x00000000;
	t->MACHTHR = 0x00000000;
	t->MACHTLR = 0x00000000;
	t->MACMIIAR = 0x00000000;
	t->MACMIIDR = 0x00000026;
	t->MACFCR = 0x00000000;
	t->MACVLANTR = 0x00000000;
	t->MACPMTCSR = 0x00000000;
	t->MACDBGR = 0x00000000;
	t->MACSR = 0x00000000;
	t->MACIMR = 0x00000000;
	t->MACA0HR = 0x0010ffff;
	t->MACA0LR = 0xffffffff;
	t->MACA1HR = 0x0000ffff;
	t->MACA1LR = 0xffffffff;
	t->MACA2HR = 0x0000ffff;
	t->MACA2LR = 0xffffffff;
	t->MACA3HR = 0x0000ffff;
	t->MACA3LR = 0xffffffff;
	t->MACRWUFFER = 0xffffffff;
	t->MMCCR = 0x00000000;
	t->MMCRIR = 0x00000000;
	t->MMCTIR = 0x00000000;
	t->MMCRIMR = 0x00000000;
	t->MMCTIMR = 0x00000000;
	t->MMCTGFSCCR = 0x00000000;
	t->MMCTGFMSCCR = 0x00000000;
	t->MMCTGFCR = 0x00000000;
	t->MMCRFCECR = 0x00000000;
	t->MMCRFAECR = 0x00000000;
	t->MMCRGUFCR = 0x00000000;
	t->PTPTSCR = 0x00002000;
	t->PTPSSIR = 0x00000000;
	t->PTPTSHR = 0x00000000;
	t->PTPTSLR = 0x00000000;
	t->PTPTSHUR = 0x00000000;
	t->PTPTSLUR = 0x00000000;
	t->PTPTSAR = 0x00000000;
	t->PTPTTHR = 0x00000000;
	t->PTPTTLR = 0x00000000;
	t->PTPTSSR = 0x00000000;
	t->PTPPPSCR = 0x00000000;
	t->DMABMR = 0x00002101;
	t->DMATPDR = 0x00000000;
	t->DMARPDR = 0x00000000;
	t->DMARDLAR = 0x00000000;
	t->DMATDLAR = 0x00000000;
	t->DMASR = 0x00000000;
	t->DMAOMR = 0x00000000;
	t->DMAIER = 0x00000000;
	t->DMAMFBOCR = 0x00000000;
	t->DMARSWTR = 0x00000000;
	t->DMACHTDR = 0x00000000;
	t->DMACHRDR = 0x00000000;
	t->DMACHTBAR = 0x00000000;
	t->DMACHRBAR = 0x00000000;
	t->rxstate = 0;
	t->txstate = 0;
}

typedef struct {
	uint32_t   Status;           /*!< Status */
	uint32_t   ControlBufferSize;     /*!< Control and Buffer1, Buffer2 lengths */
	uint32_t   Buffer1Addr;           /*!< Buffer1 address pointer */
	uint32_t   Buffer2NextDescAddr;   /*!< Buffer2 or next descriptor address pointer */
	/*!< Enhanced Ethernet DMA PTP Descriptors */
	uint32_t   ExtendedStatus;        /*!< Extended status for PTP receive descriptor */
	uint32_t   Reserved1;             /*!< Reserved */
	uint32_t   TimeStampLow;          /*!< Time Stamp Low value for transmit and receive */
	uint32_t   TimeStampHigh;         /*!< Time Stamp High value for transmit and receive */
} ETH_DMADescTypeDef;

static void stm32f7_eth_update(STM32F7ETH *t) {
	uint32_t sr = t->DMASR;
	uint32_t ier = t->DMAIER;
	uint32_t irq_flag = 0;

	if (((sr & R_DMASR_TS_MASK)		&& (ier & R_DMAIER_TIE_MASK))	||
		((sr & R_DMASR_TBUS_MASK)	&& (ier & R_DMAIER_TBUIE_MASK))	|| 
		((sr & R_DMASR_RS_MASK)		&& (ier & R_DMAIER_RIE_MASK))	|| 
		((sr & R_DMASR_ERS_MASK)	&& (ier & R_DMAIER_ERIE_MASK))) {
		t->DMASR |= R_DMASR_NIS_MASK;
		if (ier & R_DMAIER_NISE_MASK) {
			irq_flag = 1;
		}
	}

	if (((sr & R_DMASR_TPSS_MASK)	&& (ier & R_DMAIER_TPSIE_MASK))	||
		((sr & R_DMASR_TJTS_MASK)	&& (ier & R_DMAIER_TJTIE_MASK))	||
		((sr & R_DMASR_ROS_MASK)	&& (ier & R_DMAIER_ROIE_MASK))	||
		((sr & R_DMASR_TUS_MASK)	&& (ier & R_DMAIER_TUIE_MASK))	||
		((sr & R_DMASR_RBUS_MASK)	&& (ier & R_DMAIER_RBUIE_MASK))	||
		((sr & R_DMASR_RPSS_MASK)	&& (ier & R_DMAIER_RPSIE_MASK))	||
		((sr & R_DMASR_ETS_MASK)	&& (ier & R_DMAIER_ETIE_MASK))	||
		((sr & R_DMASR_FBES_MASK)	&& (ier & R_DMAIER_FBEIE_MASK))) {
		t->DMASR |= R_DMASR_AIS_MASK;
		if (ier & R_DMAIER_AISE_MASK) {
			irq_flag = 1;
		}
	}

	qemu_set_irq(t->irq[0], irq_flag);
}

static bool stm32f7_eth_can_receive(NetClientState* nc) {
   return true;
}

static ssize_t stm32f7_eth_receive(NetClientState *nc, const uint8_t *buf, size_t size) {
	STM32F7ETH *t = qemu_get_nic_opaque(nc);
	ETH_DMADescTypeDef rx_desc;
	uint32_t len_to_receive = size;
	uint32_t buffer_len;
	uint32_t next_status;
	uint32_t init_rx_desc_addr = t->cur_rx_descriptor;

	if (t->rxstate != 1) {
		return -1;
	}
	// uint32_t overflow_flag = 0;

	do {
		cpu_physical_memory_read(t->cur_rx_descriptor, &rx_desc, sizeof(ETH_DMADescTypeDef));
		buffer_len = rx_desc.ControlBufferSize & 0x00001fff;
		if (buffer_len < len_to_receive) {
			cpu_physical_memory_write(rx_desc.Buffer1Addr, buf, buffer_len);
			// receive unfinished
			len_to_receive -= buffer_len;
			cpu_physical_memory_read(rx_desc.Buffer2NextDescAddr, &next_status, sizeof(next_status));
			if (next_status & 0x80000000) {
				// own next
				// unset own
				rx_desc.Status &= (~(0x80000000));
				// unset last
				rx_desc.Status &= (~(0x00000100));
				if (len_to_receive + buffer_len == size) {
					// if first, set first
					rx_desc.Status |= 0x00000200;
				} else {
					// else, unset first
					rx_desc.Status &= (~(0x00000200));
				}
				cpu_physical_memory_write(t->cur_rx_descriptor, &(rx_desc.Status), sizeof(rx_desc.Status));
				if (rx_desc.Buffer2NextDescAddr == init_rx_desc_addr) {
					// seems that there are no more spaces, overflow!
					// overflow_flag = 1;
					printf("should not happen");
				}
				t->cur_rx_descriptor = rx_desc.Buffer2NextDescAddr;
				continue;
			} else {
				// dont own next
				rx_desc.Status |= 0x00004000;
				rx_desc.Status &= (~(0x80000000));
				// unset last
				rx_desc.Status &= (~(0x00000100));
				cpu_physical_memory_write(t->cur_rx_descriptor, &(rx_desc.Status), sizeof(rx_desc.Status));
				t->DMASR |= R_DMASR_RBUS_MASK;
				stm32f7_eth_update(t);
				return -1;
			}
		} else {
			cpu_physical_memory_write(rx_desc.Buffer1Addr, buf, len_to_receive);
			// receive finished
			// write timestamp
			// update status
			// set size
			rx_desc.Status &= (~(0x3fff));
			rx_desc.Status |= (((size + 4) & 0x3fff) << 16);
			// unset error
			rx_desc.Status &= (~(0x00004000));
			// unset own
			rx_desc.Status &= (~(0x80000000));
			// set last
			rx_desc.Status |= 0x00000100;
			if (len_to_receive == size) {
				// set first too
				rx_desc.Status |= 0x00000200;
			} else {
				// unset first
				rx_desc.Status &= (~(0x00000200));
			}
			len_to_receive = 0;
			cpu_physical_memory_write(t->cur_rx_descriptor, &(rx_desc.Status), sizeof(rx_desc.Status));
			t->cur_rx_descriptor = rx_desc.Buffer2NextDescAddr;
			t->DMASR |= R_DMASR_RS_MASK;
			stm32f7_eth_update(t);
			break;
		}
	} while(true);
	return size;
}

static void stm32f7_eth_timer_callback(void *opaque);

static void stm32f7_eth_send(STM32F7ETH *t) {
	uint32_t init_tx_desc_addr = t->cur_tx_descriptor;
	ETH_DMADescTypeDef tx_desc;
	int frame_len = 0;
	uint32_t start_tx_desc_addr = 0;
	uint32_t end_tx_desc_addr = 0;
	uint8_t *buf;
	uint8_t *trans_buf;

	if (t->timer) {
		timer_free(t->timer);
		t->timer = NULL;
	}

	if (t->txstate) {
		do {
			cpu_physical_memory_read(t->cur_tx_descriptor, &tx_desc, sizeof(tx_desc));
			if (tx_desc.Status & 0x80000000) {
				if ((tx_desc.Status & 0x30000000) == 0x30000000) {
					// last and first
					frame_len += tx_desc.ControlBufferSize & 0x1fff;
					end_tx_desc_addr = t->cur_tx_descriptor;
					start_tx_desc_addr = t->cur_tx_descriptor;
					break;
				} else if ((tx_desc.Status & 0x30000000) == 0x20000000) {
					// last
					frame_len += tx_desc.ControlBufferSize & 0x1fff;
					end_tx_desc_addr = t->cur_tx_descriptor;
					break;
				} else if ((tx_desc.Status & 0x30000000) == 0x10000000) {
					// first
					frame_len += tx_desc.ControlBufferSize & 0x1fff;
					start_tx_desc_addr = t->cur_tx_descriptor;
				} else {
					// inter
					frame_len += tx_desc.ControlBufferSize & 0x1fff;
				}
			}
			t->cur_tx_descriptor = tx_desc.Buffer2NextDescAddr;
		} while (tx_desc.Buffer2NextDescAddr != init_tx_desc_addr);

		if (start_tx_desc_addr && end_tx_desc_addr && frame_len) {
			assert(frame_len > 14);
			buf = g_malloc(frame_len);
			trans_buf = buf;
			init_tx_desc_addr = start_tx_desc_addr;
			t->cur_tx_descriptor = start_tx_desc_addr;
			do {
				cpu_physical_memory_read(t->cur_tx_descriptor, &tx_desc, sizeof(tx_desc));
				if (tx_desc.Status & 0x80000000) {
					// transfer it
					cpu_physical_memory_read(tx_desc.Buffer1Addr, trans_buf, (tx_desc.ControlBufferSize & 0x1ffff));
					frame_len -= (tx_desc.ControlBufferSize & 0x1ffff);
					if (frame_len <=0) {
						// done!
						trans_buf += (tx_desc.ControlBufferSize & 0x1ffff);
						assert(tx_desc.Status & 0x20000000);
						break;
					} else {
						trans_buf += (tx_desc.ControlBufferSize & 0x1ffff);
					}
					tx_desc.Status &= (~(0x80000000));
					cpu_physical_memory_write(t->cur_tx_descriptor, &(tx_desc.Status), sizeof(tx_desc.Status));
					t->cur_tx_descriptor = tx_desc.Buffer2NextDescAddr;
				} else {
					// dont own
					t->DMASR |= R_DMASR_TBUS_MASK;
					t->txstate = 0;
					stm32f7_eth_update(t);
					free(buf);
					return;
				}
			} while (tx_desc.Buffer2NextDescAddr != init_tx_desc_addr);
			assert(frame_len <= 0);
			frame_len = trans_buf - buf;
			qemu_send_packet(qemu_get_queue(t->nic), buf, frame_len);
			free(buf);
			tx_desc.Status &= (~(0x80000000));
			cpu_physical_memory_write(t->cur_tx_descriptor, &(tx_desc.Status), sizeof(tx_desc.Status));
			if (tx_desc.Status & 0x40000000) {
				// interrupt
				t->DMASR |= R_DMASR_TS_MASK;
				stm32f7_eth_update(t);
			}
			t->cur_tx_descriptor = tx_desc.Buffer2NextDescAddr;
		}
	}

	if (!(t->timer)) {
		t->timer = timer_new_ns(QEMU_CLOCK_VIRTUAL, stm32f7_eth_timer_callback, t);
	}
	timer_mod(t->timer, qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL) + 10);
}

static uint64_t stm32f7_eth_read(void *opaque, hwaddr offset, unsigned size) {
	STM32F7ETH *t = STM32F7_ETH(opaque);
	uint64_t ret;

	switch (offset) {
		case A_MACCR:
			ret = t->MACCR;
			break;
		case A_MACFFR:
			ret = t->MACFFR;
			break;
		case A_MACHTHR:
			ret = t->MACHTHR;
			break;
		case A_MACHTLR:
			ret = t->MACHTLR;
			break;
		case A_MACMIIAR:
			ret = t->MACMIIAR;
			break;
		case A_MACMIIDR:
			ret = t->MACMIIDR;
			break;
		case A_MACFCR:
			ret = t->MACFCR;
			break;
		case A_MACVLANTR:
			ret = t->MACVLANTR;
			break;
		case A_MACPMTCSR:
			ret = t->MACPMTCSR;
			break;
		case A_MACDBGR:
			ret = t->MACDBGR;
			break;
		case A_MACSR:
			ret = t->MACSR;
			break;
		case A_MACIMR:
			ret = t->MACIMR;
			break;
		case A_MACA0HR:
			ret = ((t->conf.macaddr.a[5] << 8) | t->conf.macaddr.a[4]);
			break;
		case A_MACA0LR:
			ret = (t->conf.macaddr.a[3] << 24) | (t->conf.macaddr.a[2] << 16) | (t->conf.macaddr.a[1] << 8) | (t->conf.macaddr.a[0]);
			break;
		case A_MACA1HR:
			ret = t->MACA1HR;
			break;
		case A_MACA1LR:
			ret = t->MACA1LR;
			break;
		case A_MACA2HR:
			ret = t->MACA2HR;
			break;
		case A_MACA2LR:
			ret = t->MACA2LR;
			break;
		case A_MACA3HR:
			ret = t->MACA3HR;
			break;
		case A_MACA3LR:
			ret = t->MACA3LR;
			break;
		case A_MACRWUFFER:
			ret = t->MACRWUFFER;
			break;
		case A_MMCCR:
			ret = t->MMCCR;
			break;
		case A_MMCRIR:
			ret = t->MMCRIR;
			break;
		case A_MMCTIR:
			ret = t->MMCTIR;
			break;
		case A_MMCRIMR:
			ret = t->MMCRIMR;
			break;
		case A_MMCTIMR:
			ret = t->MMCTIMR;
			break;
		case A_MMCTGFSCCR:
			ret = t->MMCTGFSCCR;
			break;
		case A_MMCTGFMSCCR:
			ret = t->MMCTGFMSCCR;
			break;
		case A_MMCTGFCR:
			ret = t->MMCTGFCR;
			break;
		case A_MMCRFCECR:
			ret = t->MMCRFCECR;
			break;
		case A_MMCRFAECR:
			ret = t->MMCRFAECR;
			break;
		case A_MMCRGUFCR:
			ret = t->MMCRGUFCR;
			break;
		case A_PTPTSCR:
			ret = t->PTPTSCR;
			break;
		case A_PTPSSIR:
			ret = t->PTPSSIR;
			break;
		case A_PTPTSHR:
			ret = t->PTPTSHR;
			break;
		case A_PTPTSLR:
			ret = t->PTPTSLR;
			break;
		case A_PTPTSHUR:
			ret = t->PTPTSHUR;
			break;
		case A_PTPTSLUR:
			ret = t->PTPTSLUR;
			break;
		case A_PTPTSAR:
			ret = t->PTPTSAR;
			break;
		case A_PTPTTHR:
			ret = t->PTPTTHR;
			break;
		case A_PTPTTLR:
			ret = t->PTPTTLR;
			break;
		case A_PTPTSSR:
			ret = t->PTPTSSR;
			break;
		case A_PTPPPSCR:
			ret = t->PTPPPSCR;
			break;
		case A_DMABMR:
			ret = t->DMABMR;
			break;
		case A_DMATPDR:
			ret = t->DMATPDR;
			break;
		case A_DMARPDR:
			ret = t->DMARPDR;
			break;
		case A_DMARDLAR:
			ret = t->DMARDLAR;
			break;
		case A_DMATDLAR:
			ret = t->DMATDLAR;
			break;
		case A_DMASR:
			ret = t->DMASR;
			break;
		case A_DMAOMR:
			ret = t->DMAOMR;
			break;
		case A_DMAIER:
			ret = t->DMAIER;
			break;
		case A_DMAMFBOCR:
			ret = t->DMAMFBOCR;
			break;
		case A_DMARSWTR:
			ret = t->DMARSWTR;
			break;
		case A_DMACHTDR:
			ret = t->DMACHTDR;
			break;
		case A_DMACHRDR:
			ret = t->DMACHRDR;
			break;
		case A_DMACHTBAR:
			ret = t->DMACHTBAR;
			break;
		case A_DMACHRBAR:
			ret = t->DMACHRBAR;
			break;
		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F7 ETH read: bad offset %x\n", (int)offset);
			ret = 0;
			break;
	}
	return ret;
}

static void stm32f7_eth_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
	STM32F7ETH *t = STM32F7_ETH(opaque);
	switch (offset) {
		case A_MACCR:
			if (value & R_MACCR_RE_MASK) {
				if (t->DMARDLAR) {
					t->MACCR = value;
				}
			} else if (value & R_MACCR_TE_MASK) {
				if (t->DMATDLAR) {
					t->MACCR = value;
				}
			} else {
				t->MACCR = value;
				t->rxstate = 0;
				t->txstate = 0;
				if (t->timer) {
					timer_free(t->timer);
					t->timer = NULL;
				}
			}
			break;
		case A_MACFFR:
			t->MACFFR = value;
			break;
		case A_MACHTHR:
			t->MACHTHR = value;
			break;
		case A_MACHTLR:
			t->MACHTLR = value;
			break;
		case A_MACMIIAR:
			t->MACMIIAR = (value & (~R_MACMIIAR_MB_MASK));
			break;
		case A_MACMIIDR:
			// t->MACMIIDR = value;
			break;
		case A_MACFCR:
			t->MACFCR = value;
			break;
		case A_MACVLANTR:
			t->MACVLANTR = value;
			break;
		case A_MACPMTCSR:
			t->MACPMTCSR = value;
			break;
		case A_MACSR:
			t->MACSR = value;
			break;
		case A_MACIMR:
			t->MACIMR = value;
			break;
		case A_MACA0HR:
			// t->MACA0HR = value;
			break;
		case A_MACA0LR:
			// t->MACA0LR = value;
			break;
		case A_MACA1HR:
			// t->MACA1HR = value;
			break;
		case A_MACA1LR:
			// t->MACA1LR = value;
			break;
		case A_MACA2HR:
			// t->MACA2HR = value;
			break;
		case A_MACA2LR:
			// t->MACA2LR = value;
			break;
		case A_MACA3HR:
			// t->MACA3HR = value;
			break;
		case A_MACA3LR:
			// t->MACA3LR = value;
			break;
		case A_MACRWUFFER:
			t->MACRWUFFER = value;
			break;
		case A_MMCCR:
			t->MMCCR = value;
			break;
		case A_MMCRIR:
			t->MMCRIR = value;
			break;
		case A_MMCRIMR:
			t->MMCRIMR = value;
			break;
		case A_MMCTIMR:
			t->MMCTIMR = value;
			break;
		case A_PTPTSCR:
			t->PTPTSCR = value;
			break;
		case A_PTPSSIR:
			t->PTPSSIR = value;
			break;
		case A_PTPTSHUR:
			t->PTPTSHUR = value;
			break;
		case A_PTPTSLUR:
			t->PTPTSLUR = value;
			break;
		case A_PTPTSAR:
			t->PTPTSAR = value;
			break;
		case A_PTPTTHR:
			t->PTPTTHR = value;
			break;
		case A_PTPTTLR:
			t->PTPTTLR = value;
			break;
		case A_DMABMR:
            if (value & R_DMABMR_SR_MASK) {
                stm32f7_eth_register_reset(t);
                t->DMABMR &= (~R_DMABMR_SR_MASK);
            } else {
                t->DMABMR = value;
            }
			break;
		case A_DMATPDR:
			// t->DMATPDR = value;
			// dma transmit start
			t->txstate = 1;
			t->DMASR &= (~R_DMASR_TBUS_MASK);
			stm32f7_eth_update(t);
			stm32f7_eth_send(t);
			break;
		case A_DMARPDR:
			t->DMARPDR = value;
			break;
		case A_DMARDLAR:
			if (!(t->MACCR & R_MACCR_RE_MASK)) {
				t->DMARDLAR = value;
				t->cur_rx_descriptor = value;
			}
			break;
		case A_DMATDLAR:
			if (!(t->MACCR & R_MACCR_TE_MASK)) {
				t->DMATDLAR = value;
				t->cur_tx_descriptor = value;
			}
			break;
		case A_DMASR:
			t->DMASR = (value & 0xfffe0000) | ((t->DMASR & (~value)) & 0x00001ffff);
			stm32f7_eth_update(t);
			break;
		case A_DMAOMR:
			t->DMAOMR = value;
			if (value & R_DMAOMR_SR_MASK) {
				t->rxstate = 1;
			} else if (value & R_DMAOMR_ST_MASK) {
				t->txstate = 1;
				if (!(t->timer)) {
					t->timer = timer_new_ns(QEMU_CLOCK_VIRTUAL, stm32f7_eth_timer_callback, t);
				}
				timer_mod(t->timer, qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL) + 10);
			} else {
				t->rxstate = 0;
				t->txstate = 0;
				if (t->timer) {
					timer_free(t->timer);
					t->timer = NULL;
				}
			}
			break;
		case A_DMAIER:
			t->DMAIER = value;
			stm32f7_eth_update(t);
			break;
		case A_DMAMFBOCR:
			t->DMAMFBOCR = value;
			break;
		case A_DMARSWTR:
			t->DMARSWTR = value;
			break;
		default:
			qemu_log_mask(LOG_GUEST_ERROR, "STM32F7 ETH write: bad offset %x\n", (int)offset);
			break;
	}
}

static const MemoryRegionOps stm32f7_eth_ops = {
	.read = stm32f7_eth_read,
	.write = stm32f7_eth_write,
	.endianness = DEVICE_LITTLE_ENDIAN
};

static NetClientInfo net_stm32f7_eth_info = {
    .type = NET_CLIENT_DRIVER_NIC,
    .size = sizeof(NICState),
    .can_receive = stm32f7_eth_can_receive,
    .receive = stm32f7_eth_receive,
};

static void stm32f7_eth_init(Object *obj) {
	return;
}

static void stm32f7_eth_timer_callback(void *opaque) {
	STM32F7ETH *eth = (STM32F7ETH*)opaque;
	ETH_DMADescTypeDef tx_desc;
	if (eth->cur_tx_descriptor && eth->txstate) {
		cpu_physical_memory_read(eth->cur_tx_descriptor, &tx_desc, sizeof(tx_desc));
		if (!(tx_desc.Status & 0x80000000)) {
			eth->DMASR |= R_DMASR_TBUS_MASK;
			eth->txstate = 0;
			stm32f7_eth_update(eth);
			// wait for poll command, dont update timer
			return;
		} else {
			// owned, try to send this
			stm32f7_eth_send(eth);
			// as long as no error happended, the timer is restarted
		}
	}
}

static void stm32f7_eth_realize(DeviceState *dev, Error **errp) {
    SysBusDevice *sbd = SYS_BUS_DEVICE(dev);
	STM32F7ETH *t = STM32F7_ETH(dev);

	memory_region_init_io(
        &(t->iomem), OBJECT(t), &stm32f7_eth_ops, t,
        "stm32f7-eth", STM32F7_ETH_SIZE
    );
	sysbus_init_mmio(sbd, &(t->iomem));
	for (int i = 0 ; i < 2 ; ++i) {
		sysbus_init_irq(sbd, &(t->irq[i]));
	}

    qemu_macaddr_default_if_unset(&t->conf.macaddr);
    t->nic = qemu_new_nic(
        &net_stm32f7_eth_info, &t->conf,
        object_get_typename(OBJECT(dev)), dev->id, t
    );
    qemu_format_nic_info_str(qemu_get_queue(t->nic), t->conf.macaddr.a);
	// create a timer
	t->timer = timer_new_ns(QEMU_CLOCK_VIRTUAL, stm32f7_eth_timer_callback, t);
}

static void stm32f7_eth_reset_enter(Object *obj, ResetType type) {
	STM32F7ETH *t = STM32F7_ETH(obj);

	stm32f7_eth_register_reset(t);
}

static const VMStateDescription stm32f7_eth_vmstate = {
	.name = "stm32f7-eth",
	.version_id = 0,
	.minimum_version_id = 0,
	.fields = (VMStateField[]) {
		VMSTATE_UINT32(MACCR, STM32F7ETH),
		VMSTATE_UINT32(MACFFR, STM32F7ETH),
		VMSTATE_UINT32(MACHTHR, STM32F7ETH),
		VMSTATE_UINT32(MACHTLR, STM32F7ETH),
		VMSTATE_UINT32(MACMIIAR, STM32F7ETH),
		VMSTATE_UINT32(MACMIIDR, STM32F7ETH),
		VMSTATE_UINT32(MACFCR, STM32F7ETH),
		VMSTATE_UINT32(MACVLANTR, STM32F7ETH),
		VMSTATE_UINT32(MACPMTCSR, STM32F7ETH),
		VMSTATE_UINT32(MACDBGR, STM32F7ETH),
		VMSTATE_UINT32(MACSR, STM32F7ETH),
		VMSTATE_UINT32(MACIMR, STM32F7ETH),
		VMSTATE_UINT32(MACA0HR, STM32F7ETH),
		VMSTATE_UINT32(MACA0LR, STM32F7ETH),
		VMSTATE_UINT32(MACA1HR, STM32F7ETH),
		VMSTATE_UINT32(MACA1LR, STM32F7ETH),
		VMSTATE_UINT32(MACA2HR, STM32F7ETH),
		VMSTATE_UINT32(MACA2LR, STM32F7ETH),
		VMSTATE_UINT32(MACA3HR, STM32F7ETH),
		VMSTATE_UINT32(MACA3LR, STM32F7ETH),
		VMSTATE_UINT32(MACRWUFFER, STM32F7ETH),
		VMSTATE_UINT32(MMCCR, STM32F7ETH),
		VMSTATE_UINT32(MMCRIR, STM32F7ETH),
		VMSTATE_UINT32(MMCTIR, STM32F7ETH),
		VMSTATE_UINT32(MMCRIMR, STM32F7ETH),
		VMSTATE_UINT32(MMCTIMR, STM32F7ETH),
		VMSTATE_UINT32(MMCTGFSCCR, STM32F7ETH),
		VMSTATE_UINT32(MMCTGFMSCCR, STM32F7ETH),
		VMSTATE_UINT32(MMCTGFCR, STM32F7ETH),
		VMSTATE_UINT32(MMCRFCECR, STM32F7ETH),
		VMSTATE_UINT32(MMCRFAECR, STM32F7ETH),
		VMSTATE_UINT32(MMCRGUFCR, STM32F7ETH),
		VMSTATE_UINT32(PTPTSCR, STM32F7ETH),
		VMSTATE_UINT32(PTPSSIR, STM32F7ETH),
		VMSTATE_UINT32(PTPTSHR, STM32F7ETH),
		VMSTATE_UINT32(PTPTSLR, STM32F7ETH),
		VMSTATE_UINT32(PTPTSHUR, STM32F7ETH),
		VMSTATE_UINT32(PTPTSLUR, STM32F7ETH),
		VMSTATE_UINT32(PTPTSAR, STM32F7ETH),
		VMSTATE_UINT32(PTPTTHR, STM32F7ETH),
		VMSTATE_UINT32(PTPTTLR, STM32F7ETH),
		VMSTATE_UINT32(PTPTSSR, STM32F7ETH),
		VMSTATE_UINT32(PTPPPSCR, STM32F7ETH),
		VMSTATE_UINT32(DMABMR, STM32F7ETH),
		VMSTATE_UINT32(DMATPDR, STM32F7ETH),
		VMSTATE_UINT32(DMARPDR, STM32F7ETH),
		VMSTATE_UINT32(DMARDLAR, STM32F7ETH),
		VMSTATE_UINT32(DMATDLAR, STM32F7ETH),
		VMSTATE_UINT32(DMASR, STM32F7ETH),
		VMSTATE_UINT32(DMAOMR, STM32F7ETH),
		VMSTATE_UINT32(DMAIER, STM32F7ETH),
		VMSTATE_UINT32(DMAMFBOCR, STM32F7ETH),
		VMSTATE_UINT32(DMARSWTR, STM32F7ETH),
		VMSTATE_UINT32(DMACHTDR, STM32F7ETH),
		VMSTATE_UINT32(DMACHRDR, STM32F7ETH),
		VMSTATE_UINT32(DMACHTBAR, STM32F7ETH),
		VMSTATE_UINT32(DMACHRBAR, STM32F7ETH),
		VMSTATE_END_OF_LIST()
	}
};

static Property stm32f7_eth_properties[] = {
    DEFINE_NIC_PROPERTIES(STM32F7ETH, conf),
    DEFINE_PROP_END_OF_LIST(),
};

static void stm32f7_eth_class_init(ObjectClass *oc, void *data) {
	DeviceClass *dc = DEVICE_CLASS(oc);
	ResettableClass *rc = RESETTABLE_CLASS(oc);

	dc->vmsd = &stm32f7_eth_vmstate;
	dc->realize = stm32f7_eth_realize;
	rc->phases.enter = stm32f7_eth_reset_enter;

    device_class_set_props(dc, stm32f7_eth_properties);
}

static const TypeInfo stm32f7_eth_info = {
	.name = TYPE_STM32F7_ETH,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(STM32F7ETH),
	.instance_init = stm32f7_eth_init,
	.class_init = stm32f7_eth_class_init
};

static void stm32f7_eth_register_types(void) {
	type_register_static(&stm32f7_eth_info);
}

type_init(stm32f7_eth_register_types);
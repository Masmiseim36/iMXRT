/*
 * The Clear BSD License
 * Copyright 2017 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 * that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


 /* This is a Serial NOR Configuration Block definition for Adesto's EcoXiP 
  * flash (ATXP032). Among other parameters it configues the system to operate
  * with:
  * - Octal-SPI
  * - Double Data Rate (DDR)
  * - SCLK = 133MHz
  */

#include "flexspi_flash.h"
#if defined XIP_BOOT_OCTASPI	// Is defined in the iMXRT CPU Support package depended on the selected placement

/*******************************************************************************
 * Code
 ******************************************************************************/
#if defined(__CC_ARM) || defined(__GNUC__) || defined __SES_ARM || defined __CROSSWORKS_ARM
	__attribute__((section(".boot_hdr.conf")))
#elif defined(__ICCARM__)
	#pragma section="app_image"
	#pragma location=".boot_hdr.conf"
	__root
#else
	#error "Unknown Compiler"
#endif

#define DUMMY_CYCLES 18U	/* Number of dummy cycles after Read Command for Adesto-Flash */
#define CTRL_REG_BYTE3_VAL (((DUMMY_CYCLES - 8U) >> 1U) | 0x10U)

const flexspi_nor_config_t FlashBootHeader =
{
	.memConfig =
	{
		.tag                  = FLEXSPI_CFG_BLK_TAG,
		.version              = FLEXSPI_CFG_BLK_VERSION,
		.readSampleClkSrc     = kFlexSPIReadSampleClk_ExternalInputFromDqsPad,
		.csHoldTime           = 3U,
		.csSetupTime          = 3U,
		.columnAddressWidth   = 0U,
		.deviceModeCfgEnable  = 1U,
		// Sequence for changing device mode. In this sequence we write to status/control regs 2-3.
		// This will switch EcoXiP to Octal-DDR mode and modify the number of dummy cycles used by it.
		.deviceModeSeq        = {.seqId=7, .seqNum=1}, // index/size Status/Control Registers sequence
		.deviceModeArg        = 0x88U | (CTRL_REG_BYTE3_VAL << 8U), // values to be written to status/control regs 2-3
		// Enable DDR mode, Safe configuration
		.controllerMiscOption = (1U << kFlexSpiMiscOffset_DdrModeEnable) | (1U << kFlexSpiMiscOffset_SafeConfigFreqEnable),
		.deviceType           = kFlexSpiDeviceType_SerialNOR, // serial NOR
		.sflashPadType        = kSerialFlash_8Pads,
		.serialClkFreq        = kFlexSpiSerialClk_133MHz,
		.lutCustomSeqEnable   = 0, // Use pre-defined LUT sequence index and number
		.sflashA1Size         = 8 * 1024U * 1024U,
		.dataValidTime        = {[0] = 20}, //2ns from DQS to data
		.busyOffset           = 0U, // busy bit in bit 0
		.busyBitPolarity      = 0U, // busy bit is 1 when device is busy
		.lookupTable =
		{
			// (0) Read
			FLEXSPI_LUT_SEQ (CMD_SDR,   FLEXSPI_8PAD, 0x0B,              RADDR_DDR, FLEXSPI_8PAD, 0x20),
			FLEXSPI_LUT_SEQ (DUMMY_DDR, FLEXSPI_8PAD,(DUMMY_CYCLES*2+1), READ_DDR,  FLEXSPI_8PAD, 0x80),
			0,
			0,

			// (1) Read Status (byte 1)
			FLEXSPI_LUT_SEQ (CMD_SDR,   FLEXSPI_8PAD, 0x05, DUMMY_DDR, FLEXSPI_8PAD, 0x10),
			FLEXSPI_LUT_SEQ (READ_DDR,  FLEXSPI_8PAD, 0x01, STOP,      FLEXSPI_1PAD, 0x0),
			0,
			0,

			// (2) 
			0,
			0,
			0,
			0,

			// (3) Write Enable
			FLEXSPI_LUT_SEQ (CMD_SDR,   FLEXSPI_1PAD, 0x06, STOP,      FLEXSPI_1PAD, 0x0),
			0,
			0,
			0,

			// (4) Enter OPI witjh DDR
			FLEXSPI_LUT_SEQ (CMD_SDR,   FLEXSPI_1PAD, 0x71, CMD_SDR,   FLEXSPI_1PAD, 0x03),
			FLEXSPI_LUT_SEQ (WRITE_SDR, FLEXSPI_1PAD, 0x88, STOP,      FLEXSPI_1PAD, 0x0),
			0,
			0,

			// (5) Block Erase 4K
			FLEXSPI_LUT_SEQ (CMD_SDR,   FLEXSPI_4PAD, 0x20, RADDR_DDR, FLEXSPI_4PAD, 32),
			0,
			0,
			0,

			// (6) 
			0,
			0,
			0,
			0,

			// (7) Write Status/Control Registers (this specifc sequence will writes 2 bytes to status/control regs 2-3)
			FLEXSPI_LUT_SEQ (CMD_SDR,   FLEXSPI_1PAD, 0x71, CMD_SDR,   FLEXSPI_1PAD, 0x02),
			FLEXSPI_LUT_SEQ (WRITE_SDR, FLEXSPI_1PAD, 0x02, STOP,      FLEXSPI_1PAD, 0x0),
			0,
			0,

			// (8) Erase Block 32K
			FLEXSPI_LUT_SEQ (CMD_SDR,   FLEXSPI_4PAD, 0x52, RADDR_DDR, FLEXSPI_4PAD, 32),
			0,
			0,
			0,

			// (9) Page Program
			FLEXSPI_LUT_SEQ (CMD_SDR,   FLEXSPI_8PAD, 0x02, RADDR_DDR, FLEXSPI_8PAD, 32),
			FLEXSPI_LUT_SEQ (WRITE_SDR, FLEXSPI_8PAD, 128,  STOP,      FLEXSPI_1PAD, 0),
			0,
			0,

			// (10) 
			0,
			0,
			0,
			0,

			// (11) Chip Erase
			FLEXSPI_LUT_SEQ (CMD_SDR,   FLEXSPI_4PAD, 0x60,  STOP,     FLEXSPI_1PAD, 0),
			0,
			0,
			0,

			// (12) 
			0,
			0,
			0,
			0,

			// (13) 
			0,
			0,
			0,
			0,

			// (14) 
			0,
			0,
			0,
			0,

			// (15) Read JEDEC-ID
			FLEXSPI_LUT_SEQ (CMD_SDR,   FLEXSPI_1PAD, 0x9F, READ_SDR,  FLEXSPI_1PAD, 24),
			FLEXSPI_LUT_SEQ (STOP,      FLEXSPI_1PAD, 0,    STOP,      FLEXSPI_1PAD, 0),
			0,
			0
		},
	},
	.pageSize           = 256U,
	.sectorSize         = 4U * 1024U, // 4K - that's actually a block not a sector (has to match erase size)
	.ipcmdSerialClkFreq = 1, // 30MHz
	.blockSize          = 4U * 1024U,
	.isUniformBlockSize = true,
};

#endif	// #if defined XIP_BOOT_OCTASPI

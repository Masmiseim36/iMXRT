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

#include "flexspi_flash.h"
#if defined XIP_BOOT_QSPI	/* Is defined in the iMXRT CPU Support package depended on the selected placement */

/*******************************************************************************
 * Code
 ******************************************************************************/
#if defined(__CC_ARM) || defined(__GNUC__) || defined __SES_ARM || defined __CROSSWORKS_ARM
	__attribute__((section(".boot_hdr.conf")))
#elif defined(__ICCARM__)
	#pragma section="app_image"
	#pragma location=".conf"
	__root
#else
	#error "Unknown Compiler"
#endif

//
// QSPI boot header
//
const flexspi_nor_config_t FlashBootHeader =
{
	.memConfig =
	{
		.tag              = FLEXSPI_CFG_BLK_TAG,
		.version          = FLEXSPI_CFG_BLK_VERSION,
		.readSampleClkSrc = kFlexSPIReadSampleClk_LoopbackFromDqsPad,
		.csHoldTime       = 3u,
		.csSetupTime      = 3u,
		// Enable DDR mode, Wordaddassable, Safe configuration, Differential clock
		.sflashPadType    = kSerialFlash_4Pads,
		.serialClkFreq    = kFlexSpiSerialClk_100MHz,
		.sflashA1Size     = 16u * 1024u * 1024u,
		.lookupTable =
		{
			// (0) Read Array
			FLEXSPI_LUT_SEQ (CMD_SDR,   FLEXSPI_1PAD, 0xEB, RADDR_SDR, FLEXSPI_4PAD, 0x18),
			FLEXSPI_LUT_SEQ (DUMMY_SDR, FLEXSPI_4PAD, 0x06, READ_SDR,  FLEXSPI_4PAD, 0x04),
			0,
			0,

			// (1) Read Status
			FLEXSPI_LUT_SEQ (CMD_SDR,   FLEXSPI_1PAD, 0x05, RADDR_SDR, FLEXSPI_1PAD, 0x04),
			0,
			0,
			0,

			// (2) 
			0,
			0,
			0,
			0,

			// (3) Write Enable  
			FLEXSPI_LUT_SEQ (CMD_SDR,   FLEXSPI_1PAD, 0x06, STOP,      FLEXSPI_1PAD, 0),
			0,
			0,
			0,

			// (4) Write Status/Control Registers
			FLEXSPI_LUT_SEQ (CMD_SDR,   FLEXSPI_1PAD, 0x01, WRITE_SDR, FLEXSPI_1PAD, 0x04),
			0,
			0,
			0,
		},
	},
	.pageSize           = 256u,
	.sectorSize         = 4u * 1024u,
	.blockSize          = 256u * 1024u,
	.isUniformBlockSize = false,
	};

	#endif // XIP_BOOT_QSPI

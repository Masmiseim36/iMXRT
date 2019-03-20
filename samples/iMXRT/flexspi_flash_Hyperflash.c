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
#if defined XIP_BOOT_HEADER_ENABLE && XIP_BOOT_HEADER_ENABLE != 0 && defined XIP_BOOT_HYPERFLASH

/*******************************************************************************
 * Code
 ******************************************************************************/
//
// HyperFlash boot header
//
__attribute__((section(".boot_hdr.conf"))) const flexspi_nor_config_t FlashBootHeader =
{
	.memConfig =
	{
		.tag					= FLEXSPI_CFG_BLK_TAG,
		.version				= FLEXSPI_CFG_BLK_VERSION,
		.readSampleClkSrc		= kFlexSPIReadSampleClk_ExternalInputFromDqsPad,
		.csHoldTime				= 3U,
		.csSetupTime			= 3U,
		.columnAddressWidth		= 3U,
		.controllerMiscOption	= (1 << kFlexSpiMiscOffset_DdrModeEnable) | (1 << kFlexSpiMiscOffset_WordAddressableEnable) | (1 << kFlexSpiMiscOffset_SafeConfigFreqEnable) | (1u << kFlexSpiMiscOffset_DiffClkEnable),
//		.deviceType				= kFlexSpiDeviceType_SerialNOR,
		.sflashPadType			= kSerialFlash_8Pads,
		.serialClkFreq			= kFlexSpiSerialClk_133MHz,
		.sflashA1Size			= 64 * 1024 * 1024,
		.dataValidTime			= {16, 16},
//		.busyOffset				= 15,            //busy bit offset, valid range : 0-31
//		.busyBitPolarity		= 1,        //1 – busy bit is 0 if device is busy
		.lookupTable =
		{
			// Configure LUT for read
			FLEXSPI_LUT_SEQ(CMD_DDR,   FLEXSPI_8PAD, 0xA0, RADDR_DDR, FLEXSPI_8PAD, 0x18),
			FLEXSPI_LUT_SEQ(CADDR_DDR, FLEXSPI_8PAD, 0x10, DUMMY_DDR, FLEXSPI_8PAD, 0x06),
			FLEXSPI_LUT_SEQ(READ_DDR,  FLEXSPI_8PAD, 0x04, STOP,      FLEXSPI_1PAD, 0x0),
		},
	},
	.pageSize				= 512,
	.sectorSize				= 256 * 1024,
	.blockSize				= 256 * 1024,
	.isUniformBlockSize		= true,
};

#endif	// defined XIP_BOOT_HEADER_ENABLE && XIP_BOOT_HEADER_ENABLE != 0 && defined XIP_BOOT_HYPERFLASH

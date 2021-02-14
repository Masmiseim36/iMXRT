/** Loader for iMXRT-Family
Copyright (C) 2019-2021  Markus Klein
https://github.com/Masmiseim36/iMXRT

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE. */


#include "libmem_driver_xSPI.h"
#include "libmem_Tools.h"
#include "fsl_device_registers.h"
#include "fsl_flexspi.h"
#include "fsl_common.h"
#include "fsl_clock.h"
#include "DebugPrint.h"

static libmem_driver_paged_write_ctrlblk_t paged_write_ctrlblk;


enum LUT_CommandOffsets
{
	LUT_ReadArray       = 0,
	LUT_ReadStatus      = 1,
	LUT_ReadJEDEC_ID    = 2,
	LUT_WriteEnable     = 3,
	LUT_ProgramPage     = 4,

	LUT_EraseSector     = 5,	// BLOCK_ERASE_4K
	LUT_EraseChip       = 6,

	LUT_EnterQPI        = 7,
	LUT_ReturnSPI       = 7,
	LUT_EnterQPI_Atmel  = 10,
	LUT_EnterQPI_ISSI   = 12,

	LUT_WriteStatusReg_Adesto    = 8,
	LUT_WriteConfigReg_Macronix  = 9,
	LUT_WriteConfigReg_Winbond   = 11,
	LUT_WriteConfigReg1_Macronix = 13,
};

static flexspi_device_config_t deviceconfig =
{
	.flexspiRootClk       = 0, // SPI root clock (will be set up later)
	.isSck2Enabled        = false,
	.flashSize            = BOARD_FLASH_SIZE / 1024, // expressed in KByte, will be reset after read the JEDEC Information
	.CSIntervalUnit       = kFLEXSPI_CsIntervalUnit1SckCycle,
	.CSInterval           = 2,
	.CSHoldTime           = 3,
	.CSSetupTime          = 3,
	.dataValidTime        = 0,
	.columnspace          = 0, // we don't use colums
	.enableWordAddress    = 0,
	.AWRSeqIndex          = LUT_ProgramPage,
	.AWRSeqNumber         = 1,
	.ARDSeqIndex          = LUT_ReadArray,
	.ARDSeqNumber         = 1,
	.AHBWriteWaitUnit     = kFLEXSPI_AhbWriteWaitUnit2AhbCycle,
	.AHBWriteWaitInterval = 0,
	.enableWriteMask      = false,
};

enum DummyCycles
{
	DummyCycles_Adesto   = 18,	// Number of dummy cycles after Read Command for Adesto-Flash
	DummyCycles_Macronix = 0x29//12,	// Number of dummy cycles after Read Command for Macronix-Flash - Set 0x04 To the Config
};

static const uint32_t LUT_SPI_Generic[CUSTOM_LUT_LENGTH] =
{
	// (0) Read Array --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,        kFLEXSPI_1PAD, 0x0B, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, 32),
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DUMMY_SDR,  kFLEXSPI_1PAD, 8,    kFLEXSPI_Command_READ_SDR,  kFLEXSPI_1PAD, 128),
//	(kFLEXSPI_Command_JUMP_ON_CS, kFLEXSPI_1PAD, 0,    kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (1) Read Status (byte 1) --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_1PAD, 0x05,  kFLEXSPI_Command_READ_SDR,  kFLEXSPI_1PAD, 1),
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (2) Read ID --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_1PAD, 0x9F, kFLEXSPI_Command_READ_SDR,  kFLEXSPI_1PAD, 24),
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_STOP,        kFLEXSPI_1PAD, 0,    kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (3) Write Enable --> compare @LUT_CommandOffsets    
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_1PAD, 0x06, kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (4) Page Program --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_1PAD, 0x02, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, 32),
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_WRITE_SDR,   kFLEXSPI_1PAD, 128,  kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0), 
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (5) Sector/Block Erase 4K --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_1PAD, 0x20, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, 32),
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (6) Chip Erase --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_1PAD, 0x60, kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (7) Enter QPI mode --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_1PAD, 0x38, kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (8) Write Status/Control Registers (Adesto) --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_1PAD, 0x71, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, 8),
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_WRITE_SDR,   kFLEXSPI_1PAD, 0x01, kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (9) Write Configuration Register 2 (Macronix) --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_1PAD, 0x72, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, 32),
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_WRITE_SDR,   kFLEXSPI_1PAD, 0x01, kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (10) Enter QPI mode - Atmel --> compare @LUT_CommandOffsets
	 FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_1PAD, 0x31, kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (11) Write Status Register-2  (Winbond) --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_1PAD, 0x31, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, 8),
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_WRITE_SDR,   kFLEXSPI_1PAD, 0x01, kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (12) Enter QPI mode - ISSI --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_1PAD, 0x38, kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (13) Write Configuration Register 1 (Macronix) --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_1PAD, 0x01,kFLEXSPI_Command_WRITE_SDR,  kFLEXSPI_1PAD, 1),
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four
};

static const uint32_t LUT_QuadSPI[CUSTOM_LUT_LENGTH] =
{
	// (0) Read Array --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_4PAD, 0x0B, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_4PAD, 32),
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DUMMY_SDR,   kFLEXSPI_4PAD, DummyCycles_Adesto, kFLEXSPI_Command_READ_SDR,  kFLEXSPI_4PAD, 128),
//	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_JUMP_ON_CS,  kFLEXSPI_4PAD, 0,
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (1) Read Status (byte 1) --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_4PAD, 0x05, kFLEXSPI_Command_DUMMY_SDR, kFLEXSPI_4PAD, 4),
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_READ_SDR,    kFLEXSPI_4PAD, 0x01, kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),
//	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_1PAD, 0x05, kFLEXSPI_Command_READ_SDR,  kFLEXSPI_1PAD, 4),
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (2) free
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (3) Write Enable --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_4PAD, 0x06, kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (4) Page Program --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_4PAD, 0x02, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_4PAD, 32),
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_WRITE_SDR,   kFLEXSPI_4PAD, 128,  kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (5) Block Erase 4K --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_4PAD, 0x20, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_4PAD, 32),
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (6) Chip Erase --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_4PAD, 0x60, kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (7) Return to Standard SPI Mode --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_4PAD, 0xFF, kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (8) Write Status/Control Registers --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_4PAD, 0x71, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_4PAD, 8),
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_WRITE_SDR,   kFLEXSPI_4PAD, 1,    kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four
};

// LUT for ISSI
static const uint32_t LUT_QuadSPI_ISSI[CUSTOM_LUT_LENGTH] =
{
	// (0) Read Array --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,       kFLEXSPI_1PAD, 0xEB, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_4PAD, 0x18),
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DUMMY_SDR, kFLEXSPI_4PAD, 0x06, kFLEXSPI_Command_READ_SDR,  kFLEXSPI_4PAD, 0x04),
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four
	
	// (1) Read Status --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,       kFLEXSPI_1PAD, 0x05, kFLEXSPI_Command_READ_SDR,  kFLEXSPI_1PAD, 0x04),
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (2) free
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (3) Write Enable --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,       kFLEXSPI_1PAD, 0x06, kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (4) Page Program --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,       kFLEXSPI_1PAD, 0x32, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, 0x18),
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_WRITE_SDR, kFLEXSPI_4PAD, 0x04, kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (5) Block Erase 4K / Sector Erase --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,       kFLEXSPI_1PAD, 0xD7, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, 0x18),
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (6) Chip Erase --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,       kFLEXSPI_1PAD, 0xC7, kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),	
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (7) Return to Standard SPI Mode --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,       kFLEXSPI_4PAD, 0xF5, kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four
	
	// (8) Write Status/Control Registers --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,       kFLEXSPI_1PAD, 0x01, kFLEXSPI_Command_WRITE_SDR, kFLEXSPI_1PAD, 0x04),
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four
};

// LUT for Atmel / Winbond
static const uint32_t LUT_QuadSPI_3[CUSTOM_LUT_LENGTH] =
{
	// (0) Read Array --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,       kFLEXSPI_1PAD, 0xEB, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_4PAD, 0x18),
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DUMMY_SDR, kFLEXSPI_4PAD, 0x06, kFLEXSPI_Command_READ_SDR,  kFLEXSPI_4PAD, 0x04),
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four
	
	// (1) Read Status --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,       kFLEXSPI_1PAD, 0x05, kFLEXSPI_Command_READ_SDR,  kFLEXSPI_1PAD, 0x04),
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (2) free
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (3) Write Enable --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,       kFLEXSPI_1PAD, 0x06, kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (4) Page Program --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,       kFLEXSPI_1PAD, 0x32, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, 0x18),
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_WRITE_SDR, kFLEXSPI_4PAD, 0x04, kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (5) Block Erase 4K / Sector Erase --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,       kFLEXSPI_1PAD, 0x20, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, 0x18),
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (6) Chip Erase --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,       kFLEXSPI_1PAD, 0xC7, kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),	
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (7) Return to Standard SPI Mode --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,       kFLEXSPI_4PAD, 0xF5, kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four
	
	// (8) Write Status/Control Registers --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,       kFLEXSPI_1PAD, 0x01, kFLEXSPI_Command_WRITE_SDR, kFLEXSPI_1PAD, 0x04),
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four
};

static const uint32_t LUT_OctaSPI_Adesto[CUSTOM_LUT_LENGTH] =
{
	// (0) Read Array --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,          kFLEXSPI_8PAD, 0x0B, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_8PAD, 32),
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DUMMY_SDR,    kFLEXSPI_8PAD, DummyCycles_Adesto, kFLEXSPI_Command_READ_SDR,  kFLEXSPI_8PAD, 128),
//	[2]  = FLEXSPI_LUT_SEQ (kFLEXSPI_Command_JUMP_ON_CS,   kFLEXSPI_8PAD, 0,    kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (1) Read Status (byte 1) --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,          kFLEXSPI_8PAD, 0x05, kFLEXSPI_Command_DUMMY_SDR, kFLEXSPI_8PAD, 4),
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_READ_SDR,     kFLEXSPI_8PAD, 1,    kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (2) free
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (3) Write Enable --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0x06,  kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (4) Page Program --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0x02,  kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_8PAD, 32),
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_WRITE_SDR,   kFLEXSPI_8PAD, 128,   kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (5) Block Erase 4K --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0x20,  kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_8PAD, 32),
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (6) Chip Erase --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0x60,  kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (7) Return to Standard SPI Mode --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0xFF,  kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (8) Write Status/Control Registers --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0x71,  kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_8PAD, 8),
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_WRITE_SDR,   kFLEXSPI_8PAD, 1,     kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four
};

static const uint32_t LUT_QuadSPI_DDR_Adesto[CUSTOM_LUT_LENGTH] =
{
	// (0) Read Array --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_4PAD, 0x0B,  kFLEXSPI_Command_RADDR_DDR, kFLEXSPI_4PAD, 32),
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DUMMY_DDR,   kFLEXSPI_4PAD, DummyCycles_Adesto*2+1, kFLEXSPI_Command_READ_DDR,  kFLEXSPI_4PAD, 128),
//	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_JUMP_ON_CS,  kFLEXSPI_4PAD, 0,     kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (1) Read Status (byte 1) --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_4PAD, 0x05,  kFLEXSPI_Command_DUMMY_DDR, kFLEXSPI_4PAD, 9),
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_READ_DDR,    kFLEXSPI_4PAD, 1,     kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (2) free
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (3) Write Enable --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_4PAD, 0x06,  kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (4) Page Program --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_4PAD, 0x02,  kFLEXSPI_Command_RADDR_DDR, kFLEXSPI_4PAD, 32),
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_WRITE_DDR,   kFLEXSPI_4PAD, 128,   kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (5) Block Erase 4K --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_4PAD, 0x20,  kFLEXSPI_Command_RADDR_DDR, kFLEXSPI_4PAD, 32),
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (6) Chip Erase --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_4PAD, 0x60,  kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (7) Return to Standard SPI Mode --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_4PAD, 0xFF,  kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (8) Write Status/Control Registers --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_4PAD, 0x71,  kFLEXSPI_Command_RADDR_DDR, kFLEXSPI_4PAD, 8),
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_WRITE_DDR,   kFLEXSPI_4PAD, 1,     kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four
};


static const uint32_t LUT_OctaSPI_DDR_Adesto[CUSTOM_LUT_LENGTH] =
{
	// (0) Read Array --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0x0B, kFLEXSPI_Command_RADDR_DDR, kFLEXSPI_8PAD, 32),
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DUMMY_DDR,   kFLEXSPI_8PAD,   45, kFLEXSPI_Command_READ_DDR,  kFLEXSPI_8PAD, 128),
//	[2]  = FLEXSPI_LUT_SEQ (kFLEXSPI_Command_JUMP_ON_CS,  kFLEXSPI_8PAD, 0,    kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (1) Read Status (byte 1) --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0x05, kFLEXSPI_Command_DUMMY_DDR, kFLEXSPI_8PAD, 16),
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_READ_DDR,    kFLEXSPI_8PAD, 1,    kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (2) free
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (3) Write Enable --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0x06, kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (4) Page Program --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0x02, kFLEXSPI_Command_RADDR_DDR, kFLEXSPI_8PAD, 32),
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_WRITE_DDR,   kFLEXSPI_8PAD, 128,  kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (5) Block Erase 4K --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0x20, kFLEXSPI_Command_RADDR_DDR, kFLEXSPI_8PAD, 32),
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (6) Chip Erase --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0x60, kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (7) Return to Standard SPI Mode --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0xFF, kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (8) Write Status/Control Registers --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0x71, kFLEXSPI_Command_RADDR_DDR, kFLEXSPI_8PAD, 8),
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_WRITE_DDR,   kFLEXSPI_8PAD, 1,    kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (5) Write Status/Control Registers
//	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0x71, kFLEXSPI_Command_WRITE_DDR, kFLEXSPI_8PAD, 4),
};


// LookUp Table for Macronix MX25UM Octa-Flash devices in Octa configuration with DDR
// Reading seems to be broken
static const uint32_t LUT_OctaSPI_DDR_Macronix[CUSTOM_LUT_LENGTH] =
{
	// (0) Read Array	- Octa IO DT Read --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,         kFLEXSPI_8PAD, 0xEE, kFLEXSPI_Command_DDR,         kFLEXSPI_8PAD, 0x11),
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_RADDR_DDR,   kFLEXSPI_8PAD, 32,   kFLEXSPI_Command_DUMMY_DDR,   kFLEXSPI_8PAD, DummyCycles_Macronix),
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_READ_DDR,    kFLEXSPI_8PAD, 128,  kFLEXSPI_Command_STOP,        kFLEXSPI_1PAD, 0),
	0,	// Dummy to fill a block of four

	// (1) Read Status (byte 1) --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,         kFLEXSPI_8PAD, 0x05, kFLEXSPI_Command_DDR,         kFLEXSPI_8PAD, 0xFA),
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,         kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR,         kFLEXSPI_8PAD, 0x00),
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,         kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR,         kFLEXSPI_8PAD, 0x00),
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DUMMY_DDR,   kFLEXSPI_8PAD,   40, kFLEXSPI_Command_READ_DDR,    kFLEXSPI_8PAD, 1),

	// (2) free
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (3) Write Enable --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,         kFLEXSPI_8PAD, 0x06, kFLEXSPI_Command_DDR,         kFLEXSPI_8PAD, 0xF9),
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (4) Page Program --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,         kFLEXSPI_8PAD, 0x12, kFLEXSPI_Command_DDR,         kFLEXSPI_8PAD, 0xED),
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_RADDR_DDR,   kFLEXSPI_8PAD, 32,   kFLEXSPI_Command_WRITE_DDR,   kFLEXSPI_8PAD, 4),
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (5) Block Erase 4K --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,         kFLEXSPI_8PAD, 0x21, kFLEXSPI_Command_DDR,         kFLEXSPI_8PAD, 0xDE),
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_RADDR_DDR,   kFLEXSPI_8PAD, 32,   kFLEXSPI_Command_STOP,        kFLEXSPI_1PAD, 0),
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (6) Chip Erase --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0x60, kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0x9F),
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (7) free
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (8) Write Status/Control Registers --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_1PAD, 0x01, kFLEXSPI_Command_WRITE_SDR,   kFLEXSPI_1PAD, 0x04),
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (9) Write Configuration Register 2 --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_1PAD, 0x72, kFLEXSPI_Command_RADDR_SDR,   kFLEXSPI_1PAD, 32),
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_WRITE_SDR,   kFLEXSPI_1PAD, 1,    kFLEXSPI_Command_STOP,        kFLEXSPI_1PAD, 0),
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four
};
// LookUp Table for Macronix MX25UM Octa-Flash devices in Octa configuration without DDR
static const uint32_t LUT_OctaSPI_Macronix[CUSTOM_LUT_LENGTH] =
{
	// (0) Read Array	- Octa Read --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0xEC, kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0x13),
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_RADDR_SDR,   kFLEXSPI_8PAD, 32,   kFLEXSPI_Command_DUMMY_SDR,   kFLEXSPI_8PAD, DummyCycles_Macronix),
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_READ_SDR,    kFLEXSPI_8PAD, 256,  kFLEXSPI_Command_STOP,        kFLEXSPI_1PAD, 0),
	0,	// Dummy to fill a block of four

	// (1) Read Status (byte 1) --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0x05, kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0xFA),
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0x00),
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0x00),
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DUMMY_SDR,   kFLEXSPI_8PAD,   40, kFLEXSPI_Command_READ_SDR,    kFLEXSPI_8PAD, 1),

	// (2) free
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (3) Write Enable --> compare @LUT_CommandOffsets  
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0x06, kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0xF9),
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (4) Page Program --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0x12, kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0xED),
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_RADDR_SDR,   kFLEXSPI_8PAD, 32,   kFLEXSPI_Command_WRITE_SDR,   kFLEXSPI_8PAD, 4),
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (5) Block Erase 4K --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0x21, kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0xDE),
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_RADDR_SDR,   kFLEXSPI_8PAD, 32,   kFLEXSPI_Command_STOP,        kFLEXSPI_1PAD, 0),
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (6) Chip Erase --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0x60, kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0x9F),
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (7) free
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (8) Write Status/Control Registers --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_1PAD, 0x01, kFLEXSPI_Command_WRITE_SDR,   kFLEXSPI_1PAD, 0x04),
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (9) Write Configuration Register 2 --> compare @LUT_CommandOffsets
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_1PAD, 0x72, kFLEXSPI_Command_RADDR_SDR,   kFLEXSPI_1PAD, 32),
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_WRITE_SDR,   kFLEXSPI_1PAD, 1,    kFLEXSPI_Command_STOP,        kFLEXSPI_1PAD, 0),
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four
};


// Define the structure of the Flash (Sector Count and Size)
static libmem_geometry_t geometry[] =
{ // count - size
	{0x2000, 0x1000},
	{0, 0} 
};

static status_t ReadJEDEC           (FLEXSPI_Type *base, struct DeviceInfo *Info);
static status_t EnterQuadSPIMode    (FLEXSPI_Type *base, uint32_t baseAddr, enum LUT_CommandOffsets LutOffset);
static status_t WriteEnable         (FLEXSPI_Type *base, uint32_t baseAddr);
static status_t WaitBusBusy         (FLEXSPI_Type *base);
static status_t UnprotectAll_Adesto (FLEXSPI_Type *base);
static status_t WriteRegister       (FLEXSPI_Type *base, uint32_t Address, uint8_t value, enum LUT_CommandOffsets cmd);
static status_t EraseChip           (FLEXSPI_Type *base);
static int EraseSector              (libmem_driver_handle_t *h, libmem_sector_info_t *si);
static int ProgramPage              (libmem_driver_handle_t *h, uint8_t *dest_addr, const uint8_t *src_addr);


static int libmem_ProgramPage (libmem_driver_handle_t *h, uint8_t *dest, const uint8_t *src, size_t size);
static int libmem_EraseSector (libmem_driver_handle_t *h, uint8_t *start, size_t size, uint8_t **erase_start, size_t *erase_size);
static int libmem_Flush       (libmem_driver_handle_t *h);
static int libmem_Read        (libmem_driver_handle_t *h, uint8_t *dest, const uint8_t *src, size_t size);
static uint32_t libmem_CRC32  (libmem_driver_handle_t *h, const uint8_t *start, size_t size, uint32_t crc);


static const libmem_driver_functions_t DriverFunctions =
{
	libmem_ProgramPage,
	0,
	libmem_EraseSector,
	0,
	0,
	libmem_Flush
};

static const libmem_ext_driver_functions_t DriverFunctions_Extended =
{
	0,
	libmem_Read,
	libmem_CRC32
};


LibmemStatus_t Init_AdestoXiP (FLEXSPI_Type *base, enum eMemoryType MemType, const uint32_t **lut)
{
	DebugPrint ("Found Adesto Flash\r\n");
	// Unlock all sectors
	UnprotectAll_Adesto (base);

	// Switch the configuration according to the configured memory-type
	// Compare Status Register Byte 2 in the Adesto Datasheet
	uint8_t Status = 0;
	switch (MemType)
	{
		case MemType_OctaSPI_DDR:
			// Enter Octal-Mode with DDR.
			Status = 0x88;
			*lut = LUT_OctaSPI_DDR_Adesto;
			break;
		case MemType_OctaSPI:
			Status = 0x08;
			*lut = LUT_OctaSPI_Adesto;
			break;
		case MemType_QuadSPI_DDR:
			Status = 0x84;
			*lut = LUT_QuadSPI_DDR_Adesto;
			break;
		case MemType_QuadSPI:
			Status = 0x04;
			*lut = LUT_QuadSPI;
			break;
		case MemType_SPI:
			// don't update the LUT, we are already in SPI-mode
			Status = 0x00;
			break;
		default:
			return LibmemStaus_Error;
	}

	// send write-enable 
	status_t stat = WriteEnable (base, 0);
	if (stat != kStatus_Success)
		return LibmemStaus_Error;

	// Write to status/control register 3 to set the dummy clock cycles, compare table "Dummy Clock cycles and Maximum Operating Frequency"
//	stat = WriteRegister (base, 3, 5, LUT_WriteStatusReg_Adesto); // 5 --> 18 dummy cycles
//	if (stat != kStatus_Success)
//		return LibmemStaus_Error;

	// Write to status/control register 2 to switch to chosen memory-Type
	stat = WriteRegister (base, 2, Status, LUT_WriteStatusReg_Adesto);
	if (stat != kStatus_Success)
		return LibmemStaus_Error;

	stat = EnterQuadSPIMode (base, 0, LUT_EnterQPI);
	if (stat != kStatus_Success)
		return LibmemStaus_Error;

	return LibmemStaus_Success;
}

LibmemStatus_t Init_Macronix (FLEXSPI_Type *base, enum eMemoryType MemType, const uint32_t **lut)
{
	DebugPrint ("Found Macronix Flash\r\n");
	uint8_t Status = 0;
	switch (MemType)
	{
		case MemType_OctaSPI_DDR:
			// Enter Octal-Mode with DDR.
			// DDR is not working. Fall back to SDR
			Status = 2;
			*lut = LUT_OctaSPI_DDR_Macronix;
			break;
		case MemType_OctaSPI:
			Status = 0x1U;
			*lut = LUT_OctaSPI_Macronix;
			break;
		case MemType_SPI:
			// don't update the LUT, we are already in SPI-mode
			Status = 0x0U;
			break;
		default:
			return LibmemStaus_Error;
	}

	// SET Dummy Cycles
	// send write-enable 
	status_t stat = WriteEnable (base, 0);
	if (stat != kStatus_Success)
		return LibmemStaus_Error;
	// Write to status/control register 2 to set the Dummy-Cycles
	stat = WriteRegister (base, 0x300U, 8U, LUT_WriteConfigReg_Macronix);	// 8 --> 12 Cycles at 133 MHz
	if (stat != kStatus_Success)
		return LibmemStaus_Error;
/*
	// Set Output impedance
	// send write-enable 
	stat = WriteEnable (base, 0);
	if (stat != kStatus_Success)
		return LibmemStaus_Error;
	// Write to status/control register 1 to switch to chosen memory-Type
	stat = WriteRegister (base, 0, 2, LUT_WriteConfigReg1_Macronix);	// 2 --> 52 Ohm. Compare "Output Driver Strength Table in the Reference manual"
	if (stat != kStatus_Success)
		return LibmemStaus_Error;*/

	// Switch to OSPI-Mode
	// send write-enable 
	stat = WriteEnable (base, 0);
	if (stat != kStatus_Success)
		return LibmemStaus_Error;
	// Write to status/control register 2 to switch to chosen memory-Type
	stat = WriteRegister (base, 0x0U, Status, LUT_WriteConfigReg_Macronix);
	if (stat != kStatus_Success)
		return LibmemStaus_Error;

	return LibmemStaus_Success;
}

/** Libmem_InitializeDriver_xSPI:
Initialize the FlexSPI Interface for using as a SPI-Interface
@param FlashHandle The handle which should be initialized
@param base The Flex-SPI-base to use
@param MemType The Type of SPI-Interface to use if possible
@return LibmemStatus_t LibmemStaus_Success if the operation was successfully */
LibmemStatus_t Libmem_InitializeDriver_xSPI (FLEXSPI_Type *base, enum eMemoryType MemType)
{
	#if (defined MIMXRT633S_SERIES) || defined (MIMXRT685S_cm33_SERIES) || defined (MIMXRT595S_cm33_SERIES)
		uint32_t src = 2;		// Use AUX0_PLL as clock source for the FlexSPI
		uint32_t FlexSPI_ClockDiv = 4;	// with a divider of four
		if (CLKCTL0->FLEXSPIFCLKSEL != CLKCTL0_FLEXSPIFCLKSEL_SEL(src) || (CLKCTL0->FLEXSPIFCLKDIV & CLKCTL0_FLEXSPIFCLKDIV_DIV_MASK) != (FlexSPI_ClockDiv - 1))
		{
			CLKCTL0->PSCCTL0_CLR = CLKCTL0_PSCCTL0_CLR_FLEXSPI_OTFAD_CLK_MASK;	// Disable clock before changing clock source
			CLKCTL0->FLEXSPIFCLKSEL = CLKCTL0_FLEXSPIFCLKSEL_SEL(src);			// Update flexspi clock.
			CLKCTL0->FLEXSPIFCLKDIV |= CLKCTL0_FLEXSPIFCLKDIV_RESET_MASK;		// Reset the divider counter
			CLKCTL0->FLEXSPIFCLKDIV = CLKCTL0_FLEXSPIFCLKDIV_DIV(FlexSPI_ClockDiv - 1);
			while ((CLKCTL0->FLEXSPIFCLKDIV) & CLKCTL0_FLEXSPIFCLKDIV_REQFLAG_MASK)
				;
			CLKCTL0->PSCCTL0_SET = CLKCTL0_PSCCTL0_SET_FLEXSPI_OTFAD_CLK_MASK;	// Enable FLEXSPI clock again
		}
		uint32_t FlexSPI_Clock_Hz = CLOCK_GetFlexspiClkFreq ();
		uint32_t SourceClock_Hz = FlexSPI_Clock_Hz * FlexSPI_ClockDiv;
	#elif ((defined MIMXRT1011_SERIES) || (defined MIMXRT1015_SERIES) || (defined MIMXRT1021_SERIES) || (defined MIMXRT1024_SERIES) || \
		   (defined MIMXRT1051_SERIES) || (defined MIMXRT1052_SERIES) || (defined MIMXRT1061_SERIES) || (defined MIMXRT1062_SERIES) || \
		   (defined MIMXRT1064_SERIES))
		const clock_usb_pll_config_t g_ccmConfigUsbPll = {.loopDivider = 0U, .src=0};
		CLOCK_InitUsb1Pll(&g_ccmConfigUsbPll);

		// Set up source clock
		CLOCK_InitSysPfd (kCLOCK_Pfd2, 24);        // Set PLL2 PFD2 clock 360 MHZ.
		uint32_t SourceClock_Hz = CLOCK_GetSysPfdFreq (kCLOCK_Pfd2);

		uint32_t FlexSPI_ClockDiv = 3;
		uint32_t FlexSPI_Clock_Hz = SourceClock_Hz / FlexSPI_ClockDiv;
		clock_div_t FlexSPIDiv = kCLOCK_FlexspiDiv;
		switch ((uint32_t)base)
		{
			case FLEXSPI_BASE:
				CLOCK_EnableClock(kCLOCK_FlexSpi);
				CLOCK_SetMux     (kCLOCK_FlexspiMux, 0x3); // Choose PLL3 PFD0 clock as flexspi source clock.
				FlexSPIDiv = kCLOCK_FlexspiDiv;
				break;
			#ifdef FLEXSPI2
			case FLEXSPI2_BASE:
				CLOCK_EnableClock(kCLOCK_FlexSpi2);
				CLOCK_SetMux     (kCLOCK_Flexspi2Mux, 0x3); // Choose PLL3 PFD0 clock as flexspi source clock.
				FlexSPIDiv = kCLOCK_Flexspi2Div;
				break;
			#endif
			default:
				break;
		}
		CLOCK_SetDiv (FlexSPIDiv, FlexSPI_ClockDiv-1);	// flexspi clock 120M.
	#elif (defined MIMXRT1171_SERIES)     || (defined MIMXRT1172_SERIES)     || (defined MIMXRT1173_cm7_SERIES) || (defined MIMXRT1173_cm4_SERIES) || \
		  (defined MIMXRT1175_cm7_SERIES) || (defined MIMXRT1175_cm4_SERIES) || (defined MIMXRT1176_cm7_SERIES) || (defined MIMXRT1176_cm4_SERIES)
/*		clock_root_config_t rootCfg = {false, 0, 0, 7, 2-1};	// Configure FlexSPI using PLL3Out (-> 480 MHz) divided by 2
		CLOCK_SetRootClock (kCLOCK_Root_Flexspi1, &rootCfg);
		CLOCK_ControlGate (kCLOCK_Root_Flexspi1, CCM_LPCG_DIRECT_ON_MASK);*/

//		CLOCK_SetRootClockMux (kCLOCK_Root_Flexspi1, 4);	// Choose SysPll3Pfd0 clock as flexspi source clock. 396M
//		CLOCK_SetRootClockDiv (kCLOCK_Root_Flexspi1, 5);	// flexspi clock 133M

		clock_root_t FlexSPIClock = kCLOCK_Root_Flexspi1;
		clock_lpcg_t FlexSPIClockGate = kCLOCK_Flexspi1;
		switch (reinterpret_cast<uint32_t>(base))
		{
			case FLEXSPI_BASE:
				FlexSPIClock = kCLOCK_Root_Flexspi1;
				FlexSPIClockGate = kCLOCK_Flexspi1;
				break;
			case FLEXSPI2_BASE:
				FlexSPIClock = kCLOCK_Root_Flexspi2;
				FlexSPIClockGate = kCLOCK_Flexspi2;
				break;
			default:
				return LibmemStaus_InvalidDevice;
		}
//		CLOCK_ControlGate (FlexSPIClockGate, kCLOCK_Off);	// The module clock must be disabled during clock switch in order to avoid glitch
		CLOCK_SetRootClockDiv (FlexSPIClock, 4); // --> 396 MHz / 4 = ~100 MHz
		CLOCK_SetRootClockMux (FlexSPIClock, 6); // ClockSource_SysPll2Pfd2 --> 396 MHz
//		CLOCK_SetRootClockDiv (FlexSPIClock, 2); // 12 NHz
//		CLOCK_SetRootClockMux (FlexSPIClock, 0); // OscRc48MDiv2 --> 24 MHz
		CLOCK_ControlGate (FlexSPIClockGate, kCLOCK_On);

		uint32_t FlexSPI_Clock_Hz = CLOCK_GetRootClockFreq (FlexSPIClock);
//		uint32_t SourceClock_Hz   = FlexSPI_Clock_Hz;
//		uint32_t BusClock_Hz      = CLOCK_GetRootClockFreq (kCLOCK_Root_Bus);
	#else
		#error "unknon controller family"
	#endif
	deviceconfig.flexspiRootClk = FlexSPI_Clock_Hz;

	// Get FLEXSPI default settings and configure the FlexSPI.
	flexspi_config_t config {};
	FLEXSPI_GetDefaultConfig (&config);

	// Need to set the combination-enable option. This options combines 8 data lines from FlexSPI channel A with
	// 4 data lines from FlexSPI channel B to form an 8-line bus for octal. On this SoC this is the only way to enable octal.
	config.enableCombination              = (MemType == MemType_OctaSPI_DDR || MemType == MemType_OctaSPI);	// Only true when using Octa-Mode
	config.ahbConfig.enableAHBPrefetch    = true;	// Enable AHB prefetching
	config.ahbConfig.enableAHBBufferable  = true;
	config.ahbConfig.enableReadAddressOpt = true;
	config.ahbConfig.enableAHBCachable    = true;
	config.rxSampleClock                  = kFLEXSPI_ReadSampleClkLoopbackFromDqsPad;// To achieve high speeds - always use DQS

	FLEXSPI_Init           (base, &config);
	FLEXSPI_SetFlashConfig (base, &deviceconfig, kFLEXSPI_PortA1);        // Configure flash settings according to serial flash feature.
	FLEXSPI_UpdateLUT      (base, 0, LUT_SPI_Generic, CUSTOM_LUT_LENGTH); // Update LUT table
	FLEXSPI_SoftwareReset  (base);                                        // Do software reset.


	// Get the JEDEC Informations
	struct DeviceInfo Info {};
	status_t status = ReadJEDEC (base, &Info);
	if (status != kStatus_Success || Info.ManufactureID == ManufactureID_UNDEF)
	{
		DebugPrint ("JEDEC read Error\r\n");
		return LibmemStaus_InvalidDevice;
	}

	// Check for the Manufacture-ID and adapt the Configuration
	const uint32_t *lut = NULL;
	if (Info.ManufactureID == ManufactureID_AdestoTechnologies)
	{
		LibmemStatus_t res = Init_AdestoXiP (base, MemType, &lut);
		if (res != LibmemStaus_Success)
			return res;
	}
	else if (Info.ManufactureID == ManufactureID_Atmel)
	{
		DebugPrint ("Found Atmel Flash\r\n");
		if (Info.Type == 0xA8)
		{
			LibmemStatus_t res = Init_AdestoXiP (base, MemType, &lut);
			if (res != LibmemStaus_Success)
				return res;
		}
		else
		{
			status_t stat = EnterQuadSPIMode (base, 0, LUT_EnterQPI_Atmel);
			if (stat != kStatus_Success)
				return LibmemStaus_Error;
			lut = LUT_QuadSPI_3;
		}
	}
	else if (Info.ManufactureID == ManufactureID_Nexcom)	// Winbond
	{
		DebugPrint ("Found Nexcom Flash\r\n");
		// send write-enable 
		status_t stat = WriteEnable (base, 0);
		if (stat != kStatus_Success)
			return LibmemStaus_Error;

		// Write to status/control register 2 to switch to enter quad-Mode
		stat = WriteRegister (base, 0, 2, LUT_WriteConfigReg_Winbond);
		if (stat != kStatus_Success)
			return LibmemStaus_Error;

		lut = LUT_QuadSPI_3;
	}
	else if (Info.ManufactureID == ManufactureID_Macronix)
	{
		LibmemStatus_t res = Init_Macronix (base, MemType, &lut);
		if (res != LibmemStaus_Success)
			return res;
	}
	else if (Info.ManufactureID == ManufactureID_Lucent)
	{
		DebugPrint ("Found Lucent Flash\r\n");
		status_t stat = EnterQuadSPIMode (base, 0, LUT_EnterQPI_ISSI);
		if (stat != kStatus_Success)
			return LibmemStaus_Error;

		lut = LUT_QuadSPI_ISSI;
	}
	else
	{
		DebugPrint ("unknown Flash-memory\r\n");
		return LibmemStaus_InvalidDevice;
	}

	// Use the size information from the JEDEC-Information to configure the Interface
	uint32_t FlashSize = CalculateCapacity_KBytes (Info.Capacity);
	deviceconfig.flashSize = FlashSize;
	geometry[0].count = FlashSize / (4096 / 1024);
	FlashSize *= 1024;	// Convert kBytes to bytes
	FLEXSPI_SetFlashConfig (base, &deviceconfig, kFLEXSPI_PortA1);	// Configure flash settings according to serial flash feature.

	// Reconfigure the Interface according to the gathered Flash-Information and configuration
	if (MemType == MemType_OctaSPI_DDR || MemType == MemType_QuadSPI_DDR)
	{
		#if defined FSL_FEATURE_SOC_CCM_ANALOG_COUNT && FSL_FEATURE_SOC_CCM_ANALOG_COUNT > 0
			CLOCK_SetDiv (FlexSPIDiv, FlexSPI_ClockDiv-1);
		#else
			CLOCK_SetRootClockDiv (FlexSPIClock, 2); // --> 396 MHz / 2 = ~200 MHz
			deviceconfig.flexspiRootClk = CLOCK_GetRootClockFreq (FlexSPIClock);
		#endif
	}
	else
	{
		#if defined FSL_FEATURE_SOC_CCM_ANALOG_COUNT && FSL_FEATURE_SOC_CCM_ANALOG_COUNT > 0
		#else
//			CLOCK_SetRootClockDiv (FlexSPIClock, 8);
			deviceconfig.flexspiRootClk = CLOCK_GetRootClockFreq (FlexSPIClock);
		#endif
	}

	if (lut != NULL)
		// Update the LUT
		FLEXSPI_UpdateLUT (base, 0, lut, CUSTOM_LUT_LENGTH);

	FLEXSPI_SoftwareReset (base);
	status_t stat = WaitBusBusy (base);
	if (stat != kStatus_Success)
		return LibmemStaus_Error;


	static uint8_t write_buffer[QSPIFLASH_PAGE_SIZE];
	libmem_driver_handle_t *FlashHandle = LibmemDriver::GetDriver ();
//	libmem_register_driver (FlashHandle, libmem_GetBaseAddress(base), FlashSize, geometry, 0, &DriverFunctions, &DriverFunctions_Extended);
	libmem_register_driver (FlashHandle, libmem_GetBaseAddress(base), FlashSize, geometry, 0, &DriverFunctions, NULL);
	FlashHandle->user_data = (uint32_t)base;

	uint8_t *AliasAddress = libmem_GetAliasBaseAddress (base);
	if (AliasAddress != nullptr)
	{
		FlashHandle = LibmemDriver::GetDriver ();
		libmem_register_driver (FlashHandle, AliasAddress, FlashSize, geometry, 0, &DriverFunctions, NULL);
		FlashHandle->user_data = (uint32_t)base;
		DebugPrint ("### Add Driver for Alias\r\n");
	}
	return static_cast<LibmemStatus_t>(libmem_driver_paged_write_init (&paged_write_ctrlblk, write_buffer, QSPIFLASH_PAGE_SIZE, ProgramPage, 4, 0));
}


static status_t UnprotectAll_Adesto (FLEXSPI_Type *base)
{
	status_t stat = WriteEnable (base, 0);
	if (stat != kStatus_Success)
		return stat;

	// write 0 to status/control register 1 - this will unprotect all sectors
	stat = WriteRegister (base, 1, 0, LUT_WriteStatusReg_Adesto);
	if (stat != kStatus_Success)
		return stat;

	return WaitBusBusy (base);
}

/** ReadJEDEC
Read the JEDEC Device informations
@param base The Flex-SPI-base to use
@param Info The read Device informations from the flash memory
@return status_t Status of the Operation - kStatus_Success when successfully */
static status_t ReadJEDEC (FLEXSPI_Type *base, struct DeviceInfo *Info)
{
	uint8_t Identification[16] = { 0U };

	flexspi_transfer_t flashXfer;
	flashXfer.deviceAddress = 0;
	flashXfer.port          = kFLEXSPI_PortA1;
	flashXfer.cmdType       = kFLEXSPI_Read;
	flashXfer.SeqNumber     = 1;
	flashXfer.seqIndex      = LUT_ReadJEDEC_ID;
	flashXfer.data          = (uint32_t *)Identification;
	flashXfer.dataSize      = sizeof(Identification);

	status_t status = FLEXSPI_TransferBlocking (base, &flashXfer);
	if (status != kStatus_Success)
		return status;

	if (Identification[0] == 0 || Identification[0] == 0xFF)
		return kStatus_Fail;	// got no ID-Code: No Flash available

	int i=0;
	for (; i<8; i++)
	{
		if (Identification[i] != ManufactureID_NEXT_MARKER)
			break;
	}

	Info->ManufactureID = (enum SerialFlash_ManufactureID)(((i+1)<<8) | Identification[i]);
	Info->Type          = Identification[i+1];
	Info->Capacity      = (enum Capacity)Identification[i+2];
	switch (Info->ManufactureID)
	{
		case ManufactureID_AdestoTechnologies:
			switch (Info->Type & 0x1F)
			{
				case 0x06:
					Info->Capacity = Capacity_16MBit;
					break;
				case 0x07:
					Info->Capacity = Capacity_32MBit;
					break;
				case 0x08:
					Info->Capacity = Capacity_64MBit;
					break;
				case 0x09:
					Info->Capacity = Capacity_128MBit;
					break;
			}
			break;
		case ManufactureID_Macronix:
			Info->Capacity = (enum Capacity)(Identification[i+2] & 0x1F);
			break;
		case ManufactureID_Atmel:
			switch (Info->Type)
			{
				case  0x89:	// AT25SF128A
					Info->Capacity = Capacity_128MBit;
					break;
				case  0xA8:	// ATXP064
					Info->Capacity = Capacity_64MBit;
					break;
			}
			break;
		default:
			break;
	}

	return status;
}

/** WriteEnable:
Send write-enable command
@param base The Flex-SPI-base to use
@param baseAddr The base-address of the command
@return static status_t */
static status_t WriteEnable (FLEXSPI_Type *base, uint32_t baseAddr)
{
	flexspi_transfer_t flashXfer;

	// Write enable
	flashXfer.deviceAddress = baseAddr;
	flashXfer.port          = kFLEXSPI_PortA1;
	flashXfer.cmdType       = kFLEXSPI_Command;
	flashXfer.SeqNumber     = 1;
	flashXfer.seqIndex      = LUT_WriteEnable;
	status_t status = FLEXSPI_TransferBlocking (base, &flashXfer);

	return status;
}

static status_t EnterQuadSPIMode (FLEXSPI_Type *base, uint32_t baseAddr, enum LUT_CommandOffsets LutOffset)
{
	flexspi_transfer_t flashXfer;

	// Write enable
	flashXfer.deviceAddress = baseAddr;
	flashXfer.port          = kFLEXSPI_PortA1;
	flashXfer.cmdType       = kFLEXSPI_Command;
	flashXfer.SeqNumber     = 1;
	flashXfer.seqIndex      = LutOffset;
	status_t status = FLEXSPI_TransferBlocking (base, &flashXfer);

	return status;
}


/** WaitBusBusy:
Wait until the Write/erase operation is finished and the Flash is not busy anymore
@param base The Flex-SPI-base to use
@return status_t kStatus_Success if the operation was successfully */
static status_t WaitBusBusy (FLEXSPI_Type *base)
{
	// Wait status ready.
	bool isBusy;
	uint32_t readValue;
	status_t status;

	flexspi_transfer_t flashXfer;
	flashXfer.deviceAddress = 0;
	flashXfer.port          = kFLEXSPI_PortA1;
	flashXfer.cmdType       = kFLEXSPI_Read;
	flashXfer.SeqNumber     = 1;
	flashXfer.seqIndex      = LUT_ReadStatus;
	flashXfer.data          = &readValue;
	flashXfer.dataSize      = 1;

	do
	{
		status = FLEXSPI_TransferBlocking (base, &flashXfer);

		if (status != kStatus_Success)
			return status;

		isBusy = (readValue & 1U);
	}
	while (isBusy);

	return status;
}


static status_t WriteRegister (FLEXSPI_Type *base, uint32_t Address, uint8_t value, enum LUT_CommandOffsets cmd)
{
	uint32_t Buffer = value;

	flexspi_transfer_t flashXfer;
	flashXfer.port          = kFLEXSPI_PortA1;
	flashXfer.cmdType       = kFLEXSPI_Write;
	flashXfer.SeqNumber     = 1;
	flashXfer.seqIndex      = cmd;
	flashXfer.deviceAddress = Address;
	flashXfer.data          = &Buffer;
	flashXfer.dataSize      = 1;
	return FLEXSPI_TransferBlocking (base, &flashXfer);
}

[[maybe_unused]] static status_t ReadStatusRegister (FLEXSPI_Type *base, uint8_t Address, uint8_t *value)
{
	flexspi_transfer_t flashXfer;
	flashXfer.port          = kFLEXSPI_PortA1;
	flashXfer.cmdType       = kFLEXSPI_Read;
	flashXfer.SeqNumber     = 1;
	flashXfer.seqIndex      = LUT_ReadStatus;
	flashXfer.deviceAddress = Address;
	flashXfer.data          = (uint32_t *)value;
	flashXfer.dataSize      = 1;
	return FLEXSPI_TransferBlocking (base, &flashXfer);
}


/** EraseChip:
Erase the whole-Flash-memory
@param base The FlexSPI-Interface where the Flash is located which should be erased
@return static status_t Status of the Operation - kStatus_Success when successfully */
[[maybe_unused]] static status_t EraseChip (FLEXSPI_Type *base)
{
	DebugPrintf ("EraseChip\r\n");

	status_t stat = WriteEnable (base, 0);
	if (stat != kStatus_Success)
		return stat;

	flexspi_transfer_t flashXfer;
	flashXfer.port          = kFLEXSPI_PortA1;
	flashXfer.SeqNumber     = 1;
	flashXfer.cmdType       = kFLEXSPI_Command;
	flashXfer.seqIndex      = LUT_EraseChip;
	flashXfer.deviceAddress = 0;
	//flashXfer.data = &flash_status;
	//flashXfer.dataSize = 1;
	stat = FLEXSPI_TransferBlocking (base, &flashXfer);
	if (stat != kStatus_Success)
		return stat;

	return WaitBusBusy (base);
}

/** EraseSector:
Erase a sector of the Flash-Memory
@param h Handle to the Flash-Driver
@param si Information about the sector which should be erased
@return static int LibmemStaus_Success when the erase operation was successfully, otherwise LibmemStaus_Error */
static int EraseSector (libmem_driver_handle_t *h, libmem_sector_info_t *si)
{
	FLEXSPI_Type *base = (FLEXSPI_Type *)h->user_data;
	uint32_t SectorAddr = libmem_CalculateOffset (h, si->start);
	if (SectorAddr == UINT32_MAX)
		return LibmemStaus_Error;

	DebugPrintf ("EraseSector at 0x%x, size: %d\r\n", SectorAddr, si->size);

	status_t status = WriteEnable (base, SectorAddr);
	if (status != kStatus_Success)
		return LibmemStaus_Error;

	flexspi_transfer_t flashXfer;
	flashXfer.deviceAddress = SectorAddr;
	flashXfer.port          = kFLEXSPI_PortA1;
	flashXfer.cmdType       = kFLEXSPI_Command;
	flashXfer.SeqNumber     = 1;
	flashXfer.seqIndex      = LUT_EraseSector;
	status = FLEXSPI_TransferBlocking (base, &flashXfer);

	if (status != kStatus_Success)
		return LibmemStaus_Error;

	status = WaitBusBusy (base);
	if (status != kStatus_Success)
		return LibmemStaus_Error;

	return LibmemStaus_Success;
}

/** ProgramPage:
Write Data to a Flash-Page
@param h Handle to the Flash-Driver
@param Destination Address to write the Data to. This Address is in the Address-Range of the Controller
@param Source Address of the Array with the Data to write
@return static int LibmemStaus_Success when the write operation was successfully, otherwise LibmemStaus_Error */
static int ProgramPage (libmem_driver_handle_t *h, uint8_t *dest_addr, const uint8_t *src_addr)
{
	FLEXSPI_Type *base = (FLEXSPI_Type *)h->user_data;
	uint32_t DeviceAddress = libmem_CalculateOffset (h, dest_addr);
	if (DeviceAddress == UINT32_MAX)
		return LibmemStaus_Error;

	DebugPrintf ("ProgramPage at 0x%X\r\n", DeviceAddress);

	// Write enable
	status_t status = WriteEnable (base, DeviceAddress);
	if (status != kStatus_Success)
		return LibmemStaus_Error;

	// Prepare page program command
	flexspi_transfer_t flashXfer;
	flashXfer.deviceAddress = DeviceAddress;
	flashXfer.port          = kFLEXSPI_PortA1;
	flashXfer.cmdType       = kFLEXSPI_Write;
	flashXfer.SeqNumber     = 1;
	flashXfer.seqIndex      = LUT_ProgramPage;
	flashXfer.data          = (uint32_t *)src_addr;
	flashXfer.dataSize      = QSPIFLASH_PAGE_SIZE;
	status = FLEXSPI_TransferBlocking (base, &flashXfer);
	if (status != kStatus_Success)
		return LibmemStaus_Error;

	status = WaitBusBusy (base);
	if (status != kStatus_Success)
		return LibmemStaus_Error;

	return LibmemStaus_Success;
}

/** libmem_ProgramPage:
The LIBMEM driver's write function.
@param h    A pointer to the handle of the LIBMEM driver.
@param dest A pointer to the memory address in memory range handled by driver to write data to.
@param src  pointer to the memory address to read data from.
@param size The number of bytes to write.
@return int The LIBMEM status result */
static int libmem_ProgramPage (libmem_driver_handle_t *h, uint8_t *dest, const uint8_t *src, size_t size)
{
	return libmem_driver_paged_write (h, dest, src, size, &paged_write_ctrlblk);
}

/** libmem_EraseSector:
The LIBMEM driver's erase function
@param h           A pointer to the handle of the LIBMEM driver.
@param start       A pointer to the initial memory address in memory range handled by driver to erase.
@param size        The number of bytes to erase.
@param erase_start A pointer to a location in memory to store a pointer to the start of the memory range that has actually been erased or NULL if not required.
@param erase_size  A pointer to a location in memory to store the size in bytes of the memory range that has actually been erased or NULL if not required.
@return int        The LIBMEM status result */
static int libmem_EraseSector (libmem_driver_handle_t *h, uint8_t *start, size_t size, uint8_t **erase_start, size_t *erase_size)
{
	return libmem_foreach_sector_in_range (h, start, size, EraseSector, erase_start, erase_size);
}

/** libmem_Flush:
The LIBMEM driver's flush function.
@param h    A pointer to the handle of the LIBMEM driver. 
@return int The LIBMEM status result */
static int libmem_Flush (libmem_driver_handle_t *h)
{
	int res = libmem_driver_paged_write_flush (h, &paged_write_ctrlblk);
	return res;
}

/** libmem_Read:
The LIBMEM driver's read extended function.
@param h    A pointer to the handle of the LIBMEM driver.
@param dest A pointer to the initial memory address to write data to.
@param src  A pointer to the initial memory address in the memory range handled by the driver to read data from.
@param size The number of bytes to write.
@return int The LIBMEM status result */
static int libmem_Read (libmem_driver_handle_t *h, uint8_t *dest, const uint8_t *src, size_t size)
{
	(void)h;

	DebugPrintf ("Read at 0x%x, size: %d\r\n", src, size);
	if (size)
		memcpy (dest, src, size);
	return LibmemStaus_Success;
}

/** libmem_CRC32:
The LIBMEM driver's crc32 extended function.
@param h     A pointer to the handle of the LIBMEM driver.
@param start A pointer to the start of the address range.
@param size  The size of the address range in bytes.
@param crc   The initial CRC-32 value.
@return uint32_t The computed CRC-32 value. */
static uint32_t libmem_CRC32 (libmem_driver_handle_t *h, const uint8_t *start, size_t size, uint32_t crc)
{
	(void)h;
	crc = libmem_crc32_direct (start, size, crc);
	DebugPrintf ("Calculate CRC from 0x%X, size 0x%X, calculated CRC: 0x%X\r\n", start, size, crc);
	return crc;
}



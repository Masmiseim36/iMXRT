/** Loader for iMXRT-Family
Copyright (C) 2019-2024 Markus Klein
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

#ifndef FLEXSPI_ISSI_H_
#define FLEXSPI_ISSI_H_

#include "libmem_driver_FlexSPI.h"
#include "FlexSPI_Helper.h"
#include <array>


namespace ISSI
{
	LibmemStatus_t Initialize (FlexSPI_Helper &flexSPI, MemoryType memType, DeviceInfo &info, flexspi_config_t &config, flexspi_device_config_t &deviceConfig);
	status_t TryDetect  (FlexSPI_Helper &flexSPI, DeviceInfo &Info);

	enum class Command
	{
		ReadArray       =  0,
		ReadStatus      =  1,
		ReadJEDEC_ID    =  2,
		WriteEnable     =  3,
		ProgramPage     =  4,
		EraseSector     =  5,
		EraseChip       =  6,
		// SPI
		Return2SPI      =  7,
		WriteStatus     =  8,
		EnterQpiMode    =  9,

		// OctaSPI
		ReadConfigVolatile     =  7,
		ReadConfigNonVolatile  =  8,
		WriteConfigVolatile    =  9,
		WriteConfigNonVolatile = 10,
		WriteConfigNonVolatile_SPI = 11
	};

	// Number of dummy cycles after Read Command for ISSI-Flash
	constexpr uint32_t DummyCycles   = 18;	// Compare "Table 6.8 Maximum Clock Frequencies – SDR and DDR Read Starting at 4-Byte Address" in the Datasheet

	// LUT for ISSI Quad SPI with 24-Bit Addressing
	constexpr FlexSPI_LUT LUT_QuadSPI
	{
		// (0) Read Array --> compare @Command
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,       kFLEXSPI_1PAD, 0xEB, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_4PAD, 24),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DUMMY_SDR, kFLEXSPI_4PAD, 0x06, kFLEXSPI_Command_READ_SDR,  kFLEXSPI_4PAD, 0x04),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
	
		// (1) Read Status --> compare @Command
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,       kFLEXSPI_1PAD, 0x05, kFLEXSPI_Command_READ_SDR,  kFLEXSPI_1PAD, 0x04),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four

		// (2) Read JEDEC ID --> compare @Command
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,       kFLEXSPI_1PAD, 0x9F, kFLEXSPI_Command_READ_SDR,  kFLEXSPI_1PAD, 24),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four

		// (3) Write Enable --> compare @Command
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,       kFLEXSPI_1PAD, 0x06, kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four

		// (4) Page Program --> compare @Command
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,       kFLEXSPI_1PAD, 0x32, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, 24),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_WRITE_SDR, kFLEXSPI_4PAD, 0x04, kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four

		// (5) Sector Erase 4k --> compare @Command
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,       kFLEXSPI_1PAD, 0xD7, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, 24),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four

		// (6) Chip Erase --> compare @Command
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,       kFLEXSPI_1PAD, 0xC7, kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),	
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four

		// (7) Return to Standard SPI Mode --> compare @Command
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,       kFLEXSPI_4PAD, 0xF5, kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
	
		// (8) Write Status Registers --> compare @Command
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,       kFLEXSPI_4PAD, 0x01, kFLEXSPI_Command_WRITE_SDR, kFLEXSPI_4PAD, 0x04),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four

		// (9) Enter QPI mode --> compare @Command
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_1PAD, 0x38, kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four

		// (10) empty
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four

		// (11) Write Configuration volatile - SPI
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_1PAD, 0x81, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, 24),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_WRITE_SDR,   kFLEXSPI_1PAD, 8,    kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four

		// (12) Read Configuration volatile - SPI
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_1PAD, 0x85, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, 24),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DUMMY_SDR,   kFLEXSPI_1PAD, DummyCycles,  kFLEXSPI_Command_READ_SDR,    kFLEXSPI_1PAD, 8),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
	};

	// LUT for ISSI Quad SPI with 32-Bit Addressing
	constexpr FlexSPI_LUT LUT_QuadSPI_32Bit
	{
		// (0) Read Array --> compare @Command
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,       kFLEXSPI_1PAD, 0xEC, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_4PAD, 32),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DUMMY_SDR, kFLEXSPI_4PAD, 0x06, kFLEXSPI_Command_READ_SDR,  kFLEXSPI_4PAD, 0x04),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
	
		// (1) Read Status --> compare @Command
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,       kFLEXSPI_1PAD, 0x05, kFLEXSPI_Command_READ_SDR,  kFLEXSPI_1PAD, 0x04),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four

		// (2) Read JEDEC ID --> compare @Command
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,       kFLEXSPI_1PAD, 0x9F, kFLEXSPI_Command_READ_SDR,  kFLEXSPI_1PAD, 24),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four

		// (3) Write Enable --> compare @Command
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,       kFLEXSPI_1PAD, 0x06, kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four

		// (4) Page Program --> compare @Command
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,       kFLEXSPI_4PAD, 0x12, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_4PAD, 32),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_WRITE_SDR, kFLEXSPI_4PAD, 0x04, kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four

		// (5) Block Erase 4K / Sector Erase --> compare @Command
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,       kFLEXSPI_4PAD, 0x21, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_4PAD, 32),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four

		// (6) Chip Erase --> compare @Command
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,       kFLEXSPI_4PAD, 0x60, kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),	
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four

		// (7) Return to Standard SPI Mode --> compare @Command
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,       kFLEXSPI_4PAD, 0xF5, kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
	
		// (8) Write Status Registers --> compare @Command
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,       kFLEXSPI_4PAD, 0x01, kFLEXSPI_Command_WRITE_SDR, kFLEXSPI_4PAD, 0x04),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four

		// (9) Enter QPI mode --> compare @Command
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,       kFLEXSPI_1PAD, 0x38, kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
	};

	//// Number of dummy cycles after Read Command for ISSI-Flash
	constexpr uint32_t DummyCycles_Octa   = 16;	// Compare "Table 6.8 Maximum Clock Frequencies – SDR and DDR Read Starting at 4-Byte Address" in the Datasheet

	// LUT for ISSI Octa SPI with 32-Bit Addressing
	constexpr FlexSPI_LUT LUT_OctaSPI_DDR
	{
		// (0) Read Array --> compare @Command
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0xFD, kFLEXSPI_Command_RADDR_DDR, kFLEXSPI_8PAD, 32),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DUMMY_RWDS_DDR, kFLEXSPI_8PAD, DummyCycles_Octa, kFLEXSPI_Command_READ_DDR, kFLEXSPI_8PAD, 128),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four

		// (1) Read Status --> compare @Command
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0x05, kFLEXSPI_Command_DUMMY_DDR, kFLEXSPI_8PAD,  DummyCycles_Octa),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_READ_DDR,    kFLEXSPI_8PAD, 1,    kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four

		// (2) Read JEDEC ID --> compare @Command
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0x9F, kFLEXSPI_Command_DUMMY_RWDS_DDR, kFLEXSPI_8PAD, 4),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_READ_DDR,    kFLEXSPI_8PAD, 0x04, kFLEXSPI_Command_STOP,        kFLEXSPI_8PAD, 0x0),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four

		// (3) Write Enable --> compare @Command
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0x06, kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four

		// (4) Page Program --> compare @Command
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0x8E, kFLEXSPI_Command_RADDR_DDR, kFLEXSPI_8PAD, 32),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_WRITE_DDR,   kFLEXSPI_8PAD, 128,  kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four

		// (5) Sector erase 4K --> compare @Command
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0x21, kFLEXSPI_Command_RADDR_DDR, kFLEXSPI_8PAD, 32),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four

		// (6) Chip Erase --> compare @Command
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0x60, kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four

		// (7) Read Configuration volatile
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0x85, kFLEXSPI_Command_RADDR_DDR, kFLEXSPI_8PAD, 32),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DUMMY_DDR,   kFLEXSPI_8PAD,  DummyCycles_Octa,  kFLEXSPI_Command_READ_DDR, kFLEXSPI_8PAD, 1),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four

		// (8) Read Configuration non-volatile
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0xB5, kFLEXSPI_Command_RADDR_DDR,   kFLEXSPI_8PAD, 32),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DUMMY_DDR,   kFLEXSPI_8PAD, DummyCycles_Octa, kFLEXSPI_Command_READ_DDR, kFLEXSPI_8PAD, 8),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four

		// (9) Write Configuration volatile
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0x81, kFLEXSPI_Command_RADDR_DDR, kFLEXSPI_8PAD, 32),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_WRITE_SDR,   kFLEXSPI_8PAD, 8,    kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four

		// (10) Write Configuration non-volatile
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0xB1, kFLEXSPI_Command_RADDR_DDR, kFLEXSPI_8PAD, 32),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_WRITE_SDR,   kFLEXSPI_8PAD, 4,    kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four

		// (11) Write Configuration volatile - SPI
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_1PAD, 0x81, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, 24),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_WRITE_SDR,   kFLEXSPI_1PAD, 8,    kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four

		// (12) Read Configuration volatile - SPI
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_1PAD, 0x85, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, 24),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DUMMY_SDR,   kFLEXSPI_1PAD, DummyCycles_Octa, kFLEXSPI_Command_READ_SDR, kFLEXSPI_1PAD, 8),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four

		// (13) Read Status - SPI
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_1PAD, 0x05, kFLEXSPI_Command_READ_SDR,  kFLEXSPI_1PAD, 0x04),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
	};
}


#endif // FLEXSPI_ISSI_H_
/** Loader for iMXRT-Family
Copyright (C) 2019-2025 Markus Klein
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

#ifndef FLEXSPI_Micron_H_
#define FLEXSPI_Micron_H_

#include "libmem_driver_FlexSPI.h"
#include "FlexSPI_Helper.h"
#include <array>


namespace Micron
{
	LibmemStatus_t Initialize (FlexSPI_Helper &flexSPI, MemoryType memType, DeviceInfo &info, flexspi_config_t &config, flexspi_device_config_t &deviceConfig);
	status_t TryDetect        (FlexSPI_Helper &flexSPI, DeviceInfo &Info);

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

	//// Number of dummy cycles after Read Command for Micron-Flash
	constexpr uint32_t DummyCycles_Octa   = 16;	// Compare "Table 6.8 Maximum Clock Frequencies – SDR and DDR Read Starting at 4-Byte Address" in the Datasheet

	// LUT for ISSI Octa SPI with 32-Bit Addressing
	constexpr FlexSPI_LUT LUT_OctaSPI_DDR
	{
		// (0) DDR OCTAL I/O FAST READ --> compare @Command
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,            kFLEXSPI_8PAD, 0xFD, kFLEXSPI_Command_RADDR_DDR, kFLEXSPI_8PAD, 32),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DUMMY_RWDS_DDR, kFLEXSPI_8PAD, DummyCycles_Octa, kFLEXSPI_Command_READ_DDR, kFLEXSPI_8PAD, 128),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four

		// (1) Read Status --> compare @Command
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0x05, kFLEXSPI_Command_DUMMY_DDR, kFLEXSPI_8PAD,  DummyCycles_Octa),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_READ_DDR,    kFLEXSPI_8PAD, 1,    kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four

		// (2) Read JEDEC ID --> compare @Command
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0x9F, kFLEXSPI_Command_DUMMY_DDR, kFLEXSPI_8PAD, 16),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_READ_DDR,    kFLEXSPI_8PAD, 0x04, kFLEXSPI_Command_STOP,      kFLEXSPI_8PAD, 0x0),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four

		// (3) Write Enable --> compare @Command
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0x06, kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four

		// (4) Page Program (OCTAL INPUT FAST PROGRAM) --> compare @Command
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
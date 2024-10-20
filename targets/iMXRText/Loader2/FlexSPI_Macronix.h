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

#ifndef FLEXSPI_MACRONIX_H_
#define FLEXSPI_MACRONIX_H_

#include "libmem_driver_xSPI.h"
#include "FlexSPI_Helper.h"
#include <array>

namespace Macronix
{
	LibmemStatus_t Initialize (FlexSPI_Helper &flexSPI, MemoryType memType, DeviceInfo &Info, flexspi_config_t &config, flexspi_device_config_t &DeviceConfig);
	status_t TryDetect        (FlexSPI_Helper &flexSPI, DeviceInfo &Info);

	enum class Command
	{
		ReadArray           =  0,
		ReadStatus          =  1,
		ReadJEDEC_ID        =  2,
		WriteEnable         =  3,
		ProgramPage         =  4,
		EraseSector         =  5,
		EraseChip           =  6,
		ReadConfiguration   =  7,
		WriteStatus         =  8,
		WriteConfiguration2 =  9, // Octa SPI only
		ReadConfiguration2  = 10, // Octa SPI only

		EnterFourByteMode   = 11, // SPI only
	};
	
	// LUT for Macronix SPI - 24 Bit Addressing
	constexpr FlexSPI_LUT LUT_SPI
	{
		// (0) Read Array --> compare @Command
		// Read with normal read
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,       kFLEXSPI_1PAD, 0x03, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, 24),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_READ_SDR,  kFLEXSPI_1PAD, 128,  kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),
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
		// normal page program
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,       kFLEXSPI_1PAD, 0x02, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, 24),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_WRITE_SDR, kFLEXSPI_1PAD, 128,  kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four

		// (5) Sector Erase 4K --> compare @Command
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,       kFLEXSPI_1PAD, 0x20, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, 24),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four

		// (6) Chip Erase --> compare @Command
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,       kFLEXSPI_1PAD, 0xC7, kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),	
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four

		// (7) Read configuration register
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,       kFLEXSPI_1PAD, 0x15, kFLEXSPI_Command_READ_SDR,  kFLEXSPI_1PAD, 0x04),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
		
		// (8) Write status and configuration registers --> compare @Command
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,       kFLEXSPI_1PAD, 0x01, kFLEXSPI_Command_WRITE_SDR, kFLEXSPI_1PAD, 0x04),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four

		// (9) Write Configuration Register 2 --> compare @Command
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,       kFLEXSPI_1PAD, 0x72, kFLEXSPI_Command_RADDR_SDR,   kFLEXSPI_1PAD, 32),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_WRITE_SDR, kFLEXSPI_1PAD, 0x01, kFLEXSPI_Command_STOP,        kFLEXSPI_1PAD, 0),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four

		// (10) read Configuration Register 2
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,       kFLEXSPI_1PAD, 0x71, kFLEXSPI_Command_READ_SDR,  kFLEXSPI_1PAD, 0x04),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four

		// (11) Enter 4 Byte Mode --> compare @Command
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,       kFLEXSPI_1PAD, 0xB7, kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),	
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
	};

	// LUT for Macronix Quad-SPI - 24 Bit Addressing
	constexpr FlexSPI_LUT LUT_QuadSPI
	{
		// (0) Read Array --> compare @Command
		// Read with 4READ
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,       kFLEXSPI_1PAD, 0xEB, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_4PAD, 24),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DUMMY_SDR, kFLEXSPI_4PAD,    6, kFLEXSPI_Command_READ_SDR,  kFLEXSPI_4PAD, 0x08),
		// Read with QREAD
	//	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,       kFLEXSPI_1PAD, 0x6B, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, 24),
	//	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DUMMY_SDR, kFLEXSPI_1PAD, 0x08, kFLEXSPI_Command_READ_SDR,  kFLEXSPI_4PAD, 128),
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
		// quad page program (4pp)
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,       kFLEXSPI_1PAD, 0x38, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_4PAD, 24),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_WRITE_SDR, kFLEXSPI_4PAD, 255,  kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four

		// (5) Sector Erase 4K --> compare @Command
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,       kFLEXSPI_1PAD, 0x20, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, 24),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four

		// (6) Chip Erase --> compare @Command
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,       kFLEXSPI_1PAD, 0xC7, kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),	
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four

		// (7) Read configuration register
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,       kFLEXSPI_1PAD, 0x15, kFLEXSPI_Command_READ_SDR,  kFLEXSPI_4PAD, 0x04),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
		
		// (8) Write status and configuration registers --> compare @Command
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,       kFLEXSPI_1PAD, 0x01, kFLEXSPI_Command_WRITE_SDR, kFLEXSPI_4PAD, 0x04),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
	};

	// LUT for Macronix Quad-SPI - 24 Bit Addressing
	constexpr FlexSPI_LUT LUT_QuadSPI_32Bit
	{
		// (0) Read Array --> compare @Command
		// Read with 4READ
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,       kFLEXSPI_1PAD, 0xEB, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_4PAD, 0x20),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DUMMY_SDR, kFLEXSPI_4PAD,    6, kFLEXSPI_Command_READ_SDR,  kFLEXSPI_4PAD, 0x08),
		// Read with QREAD
	//	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,       kFLEXSPI_1PAD, 0x6B, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, 0x20,
	//	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DUMMY_SDR, kFLEXSPI_1PAD, 0x08, kFLEXSPI_Command_READ_SDR,  kFLEXSPI_4PAD, 128),
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
		// quad page program (4pp)
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,       kFLEXSPI_1PAD, 0x38, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_4PAD, 0x20),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_WRITE_SDR, kFLEXSPI_4PAD, 255,  kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four

		// (5) Sector Erase 4K --> compare @Command
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,       kFLEXSPI_1PAD, 0x20, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, 0x20),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four

		// (6) Chip Erase --> compare @Command
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,       kFLEXSPI_1PAD, 0xC7, kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),	
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four

		// (7) Read configuration register
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,       kFLEXSPI_1PAD, 0x15, kFLEXSPI_Command_READ_SDR,  kFLEXSPI_4PAD, 0x04),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
		
		// (8) Write status and configuration registers --> compare @Command
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,       kFLEXSPI_1PAD, 0x01, kFLEXSPI_Command_WRITE_SDR, kFLEXSPI_4PAD, 0x04),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
	};

	// Number of dummy cycles after Read Command for Macronix-Flash
	constexpr uint32_t DummyCycles   = 12;	// Compare "Dummy Cycle and Frequency Table (MHz)" in the datasheet

	// LookUp Table for Macronix MX25UM-Family Octa-Flash devices in Octa configuration with DDR
	constexpr FlexSPI_LUT LUT_OctaSPI_DDR
	{
		// (0) Read Array	- Octa IO DT Read --> compare @Command
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,         kFLEXSPI_8PAD, 0xEE, kFLEXSPI_Command_DDR,         kFLEXSPI_8PAD, 0x11),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_RADDR_DDR,   kFLEXSPI_8PAD, 32,   kFLEXSPI_Command_DUMMY_DDR,   kFLEXSPI_8PAD, (DummyCycles * 2)+1),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_READ_DDR,    kFLEXSPI_8PAD, 128,  kFLEXSPI_Command_STOP,        kFLEXSPI_1PAD, 0),
		0,	// Dummy to fill a block of four

		// (1) Read Status (byte 1) --> compare @Command
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,         kFLEXSPI_8PAD, 0x05, kFLEXSPI_Command_DDR,         kFLEXSPI_8PAD, 0xFA),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_RADDR_DDR,   kFLEXSPI_8PAD, 32,   kFLEXSPI_Command_DUMMY_DDR,   kFLEXSPI_8PAD, (DummyCycles * 2)+1),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_READ_DDR,    kFLEXSPI_8PAD, 4,    kFLEXSPI_Command_STOP,        kFLEXSPI_1PAD, 0x0),
		0,

		// (2) Read JEDEC ID --> compare @Command
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,         kFLEXSPI_8PAD, 0x9F, kFLEXSPI_Command_DDR,         kFLEXSPI_8PAD, 0x60),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_RADDR_DDR,   kFLEXSPI_8PAD, 0x20, kFLEXSPI_Command_DUMMY_DDR,   kFLEXSPI_8PAD, 0x16),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_READ_DDR,    kFLEXSPI_8PAD, 0x04, kFLEXSPI_Command_STOP,        kFLEXSPI_1PAD, 0x0),
		0,	// Dummy to fill a block of four

		// (3) Write Enable --> compare @Command
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,         kFLEXSPI_8PAD, 0x06, kFLEXSPI_Command_DDR,         kFLEXSPI_8PAD, 0xF9),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four

		// (4) Page Program --> compare @Command
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,         kFLEXSPI_8PAD, 0x12, kFLEXSPI_Command_DDR,         kFLEXSPI_8PAD, 0xED),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_RADDR_DDR,   kFLEXSPI_8PAD, 32,   kFLEXSPI_Command_WRITE_DDR,   kFLEXSPI_8PAD, 4),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four

		// (5) Sector Erase 4K --> compare @Command
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,         kFLEXSPI_8PAD, 0x21, kFLEXSPI_Command_DDR,         kFLEXSPI_8PAD, 0xDE),	// Sector Erase (4K)
//		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,         kFLEXSPI_8PAD, 0xDC, kFLEXSPI_Command_DDR,         kFLEXSPI_8PAD, 0x23),	// Block Erase (64K)
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_RADDR_DDR,   kFLEXSPI_8PAD, 32,   kFLEXSPI_Command_STOP,        kFLEXSPI_1PAD, 0),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four

		// (6) Chip Erase --> compare @Command
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,         kFLEXSPI_8PAD, 0x60, kFLEXSPI_Command_DDR,         kFLEXSPI_8PAD, 0x9F),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four

		// (7) read Configuration Register 1
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,         kFLEXSPI_8PAD, 0x15, kFLEXSPI_Command_DDR,         kFLEXSPI_8PAD, 0xEA),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_RADDR_DDR,   kFLEXSPI_8PAD, 32,   kFLEXSPI_Command_DUMMY_RWDS_DDR, kFLEXSPI_8PAD, DummyCycles),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_READ_DDR,    kFLEXSPI_8PAD, 2,    kFLEXSPI_Command_STOP,        kFLEXSPI_1PAD, 0),
		0,	// Dummy to fill a block of four

		// (8) Write Status/Control Registers --> compare @Command
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,         kFLEXSPI_8PAD, 0x01, kFLEXSPI_Command_WRITE_SDR,   kFLEXSPI_8PAD, 0x04),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four

		// (9) Write Configuration Register 2 --> compare @Command
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,         kFLEXSPI_8PAD, 0x72, kFLEXSPI_Command_DDR,         kFLEXSPI_8PAD, 0x8D),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_RADDR_DDR,   kFLEXSPI_8PAD, 32,   kFLEXSPI_Command_WRITE_SDR,   kFLEXSPI_8PAD, 2),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four

		// (10) read Configuration Register 2
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,         kFLEXSPI_8PAD, 0x71, kFLEXSPI_Command_DDR,         kFLEXSPI_8PAD, 0x8E),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_RADDR_DDR,   kFLEXSPI_8PAD, 32,   kFLEXSPI_Command_DUMMY_RWDS_DDR, kFLEXSPI_8PAD, DummyCycles),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_READ_DDR,    kFLEXSPI_8PAD, 2,    kFLEXSPI_Command_STOP,        kFLEXSPI_1PAD, 0),
		0,	// Dummy to fill a block of four
	};
	// LookUp Table for Macronix MX25UM Octa-Flash devices in Octa configuration without DDR
	constexpr FlexSPI_LUT LUT_OctaSPI
	{
		// (0) Read Array	- Octa Read --> compare @Command
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0xEC, kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0x13),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_RADDR_SDR,   kFLEXSPI_8PAD, 32,   kFLEXSPI_Command_DUMMY_SDR,   kFLEXSPI_8PAD, DummyCycles),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_READ_SDR,    kFLEXSPI_8PAD, 128,  kFLEXSPI_Command_STOP,        kFLEXSPI_1PAD, 0),
		0,	// Dummy to fill a block of four

		// (1) Read Status (byte 1) --> compare @Command
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0x05, kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0xFA),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_RADDR_SDR,   kFLEXSPI_8PAD, 32,   kFLEXSPI_Command_DUMMY_SDR,   kFLEXSPI_8PAD, DummyCycles),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_READ_SDR,    kFLEXSPI_8PAD, 1,    kFLEXSPI_Command_STOP,        kFLEXSPI_1PAD, 0x0),
		0,

		// (2) Read JEDEC ID --> compare @Command
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0x9F, kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0x60),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_RADDR_SDR,   kFLEXSPI_8PAD, 0x20, kFLEXSPI_Command_DUMMY_SDR,   kFLEXSPI_8PAD, 0x16),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_READ_SDR,    kFLEXSPI_8PAD, 0x04, kFLEXSPI_Command_STOP,        kFLEXSPI_1PAD, 0x0),
		0,	// Dummy to fill a block of four

		// (3) Write Enable --> compare @Command
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0x06, kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0xF9),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four

		// (4) Page Program --> compare @Command
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0x12, kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0xED),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_RADDR_SDR,   kFLEXSPI_8PAD, 32,   kFLEXSPI_Command_WRITE_SDR,   kFLEXSPI_8PAD, 4),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four

		// (5) Sector Erase 4K --> compare @Command
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0x21, kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0xDE),	// Sector Erase (4K)
//		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0xDC, kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0x23),	// Block Erase (64K)
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_RADDR_SDR,   kFLEXSPI_8PAD, 32,   kFLEXSPI_Command_STOP,        kFLEXSPI_1PAD, 0),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four

		// (6) Chip Erase --> compare @Command
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0x60, kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0x9F),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four

		// (7) read Configuration Register 1
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0x15, kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0xEA),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_RADDR_SDR,   kFLEXSPI_8PAD, 32,   kFLEXSPI_Command_DUMMY_SDR,   kFLEXSPI_8PAD, DummyCycles),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_READ_SDR,    kFLEXSPI_8PAD, 2,    kFLEXSPI_Command_STOP,        kFLEXSPI_1PAD, 0),
		0,	// Dummy to fill a block of four

		// (8) Write Status/Control Registers --> compare @Command
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_1PAD, 0x01, kFLEXSPI_Command_WRITE_SDR,   kFLEXSPI_8PAD, 0x04),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four

		// (9) Write Configuration Register 2 --> compare @Command
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0x72, kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0x8D),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_RADDR_SDR,   kFLEXSPI_8PAD, 32,   kFLEXSPI_Command_WRITE_SDR,   kFLEXSPI_8PAD, 2),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four

		// (10) read Configuration Register 2
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0x71, kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0x8E),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_RADDR_SDR,   kFLEXSPI_8PAD, 32,   kFLEXSPI_Command_DUMMY_SDR, kFLEXSPI_8PAD, DummyCycles),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_READ_SDR,    kFLEXSPI_8PAD, 2,    kFLEXSPI_Command_STOP,        kFLEXSPI_1PAD, 0),
		0,	// Dummy to fill a block of four
	};
}

#endif // FLEXSPI_MACRONIX_H_

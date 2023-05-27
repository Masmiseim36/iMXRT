/** Loader for iMXRT-Family
Copyright (C) 2019-2023 Markus Klein
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

#ifndef FLKEXSPI_ADESTO_H_
#define FLKEXSPI_ADESTO_H_

#include "libmem_driver_xSPI.h"
#include "FlexSPI_Helper.h"
#include <array>

namespace Adesto
{
	LibmemStatus_t Initialize (FlexSPI_Helper &flexSPI, MemoryType MemType, DeviceInfo &Info, flexspi_config_t &config, flexspi_device_config_t &DeviceConfig);

	constexpr uint32_t DummyCycles   = 18;	// Number of dummy cycles after Read Command
	
	constexpr FlexSPI_LUT LUT_OctaSPI
	{
		// (0) Read Array --> compare @LUT_CommandOffsets
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,          kFLEXSPI_8PAD, 0x0B, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_8PAD, 32),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DUMMY_SDR,    kFLEXSPI_8PAD, DummyCycles, kFLEXSPI_Command_READ_SDR,  kFLEXSPI_8PAD, 128),
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

		// (9) Block Erase 32K --> compare @LUT_CommandOffsets
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0x52,  kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_8PAD, 32),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four

		// (10) Block Erase 64K --> compare @LUT_CommandOffsets
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0xD8,  kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_8PAD, 32),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four

		// (12) free
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four

		// (13) free
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four

		// (14) free
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four

		// (15) free
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
	};

	constexpr FlexSPI_LUT LUT_QuadSPI_DDR
	{
		// (0) Read Array --> compare @LUT_CommandOffsets
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_4PAD, 0x0B,  kFLEXSPI_Command_RADDR_DDR, kFLEXSPI_4PAD, 32),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DUMMY_DDR,   kFLEXSPI_4PAD, DummyCycles*2+1, kFLEXSPI_Command_READ_DDR,  kFLEXSPI_4PAD, 128),
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

		// (9) Block Erase 32K --> compare @LUT_CommandOffsets
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_4PAD, 0x52,  kFLEXSPI_Command_RADDR_DDR, kFLEXSPI_4PAD, 32),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four

		// (10) Block Erase 64K --> compare @LUT_CommandOffsets
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_4PAD, 0xD8,  kFLEXSPI_Command_RADDR_DDR, kFLEXSPI_4PAD, 32),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
	};


	constexpr FlexSPI_LUT LUT_OctaSPI_DDR
	{
		// (0) Read Array --> compare @LUT_CommandOffsets
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0x0B, kFLEXSPI_Command_RADDR_DDR, kFLEXSPI_8PAD, 32),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DUMMY_DDR,   kFLEXSPI_8PAD, (DummyCycles*2+1), kFLEXSPI_Command_READ_DDR,  kFLEXSPI_8PAD, 128),
	//	[2]  = FLEXSPI_LUT_SEQ (kFLEXSPI_Command_JUMP_ON_CS,  kFLEXSPI_8PAD, 0,    kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four

		// (1) Read Status (byte 1) --> compare @LUT_CommandOffsets
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0x05, kFLEXSPI_Command_DUMMY_DDR, kFLEXSPI_8PAD, 8),
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

		// (9) Block Erase 32K --> compare @LUT_CommandOffsets
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0x52, kFLEXSPI_Command_RADDR_DDR, kFLEXSPI_8PAD, 32),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four

		// (10) Block Erase 64K --> compare @LUT_CommandOffsets
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0xD8, kFLEXSPI_Command_RADDR_DDR, kFLEXSPI_8PAD, 32),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
	};
	
	static constexpr FlexSPI_LUT LUT_QuadSPI
	{
		// (0) Read Array --> compare @LUT_CommandOffsets
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_4PAD, 0x0B, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_4PAD, 32),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DUMMY_SDR,   kFLEXSPI_4PAD, DummyCycles, kFLEXSPI_Command_READ_SDR,  kFLEXSPI_4PAD, 128),
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
}


#endif // FLKEXSPI_ADESTO_H_
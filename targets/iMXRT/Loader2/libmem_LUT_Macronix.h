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

#ifndef XSPI_LUT_MACRONIX_H_
#define XSPI_LUT_MACRONIX_H_

#include "libmem_driver_xSPI.h"
#include "fsl_flexspi.h"
#include <array>

namespace Macronix
{
	LibmemStatus_t Initialize (FlexSPI_Helper &flexSPI, MemoryType MemType, DeviceInfo &Info);

	constexpr uint32_t DummyCycles   = 0x29;//12;	// Number of dummy cycles after Read Command for Macronix-Flash - Set 0x04 To the Config
	
	// LUT for Macronix Quad SPI
	constexpr std::array <uint32_t, 64> LUT_QuadSPI
	{
		// (0) Read Array --> compare @LUT_CommandOffsets
		// Read with 4READ
	//	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,       kFLEXSPI_1PAD, 0xEB, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_4PAD, 24),
	//	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DUMMY_SDR, kFLEXSPI_1PAD, 0x06, kFLEXSPI_Command_READ_SDR,  kFLEXSPI_4PAD, 0x04),
		// Read with QREAD
	//	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,       kFLEXSPI_1PAD, 0x6B, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, 24),
	//	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DUMMY_SDR, kFLEXSPI_1PAD, 0x08, kFLEXSPI_Command_READ_SDR,  kFLEXSPI_4PAD, 128),
		// Read with normal read
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,       kFLEXSPI_1PAD, 0x03, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, 24),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_READ_SDR,  kFLEXSPI_1PAD, 128,  kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),
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
		// (quad page program)
	//	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,       kFLEXSPI_1PAD, 0x38, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_4PAD, 24),
	//	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_WRITE_SDR, kFLEXSPI_4PAD, 128,  kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),
		// normal page program
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,       kFLEXSPI_1PAD, 0x02, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, 24),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_WRITE_SDR, kFLEXSPI_1PAD, 128,  kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four

		// (5) Block Erase 4K / Sector Erase --> compare @LUT_CommandOffsets
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,       kFLEXSPI_1PAD, 0x20, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, 24),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four

		// (6) Chip Erase --> compare @LUT_CommandOffsets
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,       kFLEXSPI_1PAD, 0xC7, kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),	
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four

		// (7) free
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
		
		// (8) Write Status/Control Registers --> compare @LUT_CommandOffsets
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,       kFLEXSPI_1PAD, 0x01, kFLEXSPI_Command_WRITE_SDR, kFLEXSPI_1PAD, 0x04),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
	};


	// LookUp Table for Macronix MX25UM Octa-Flash devices in Octa configuration with DDR
	// Reading seems to be broken
	constexpr std::array <uint32_t, 64> LUT_OctaSPI_DDR
	{
		// (0) Read Array	- Octa IO DT Read --> compare @LUT_CommandOffsets
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,         kFLEXSPI_8PAD, 0xEE, kFLEXSPI_Command_DDR,         kFLEXSPI_8PAD, 0x11),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_RADDR_DDR,   kFLEXSPI_8PAD, 32,   kFLEXSPI_Command_DUMMY_DDR,   kFLEXSPI_8PAD, DummyCycles),
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
	constexpr std::array <uint32_t, 64> LUT_OctaSPI
	{
		// (0) Read Array	- Octa Read --> compare @LUT_CommandOffsets
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0xEC, kFLEXSPI_Command_SDR,         kFLEXSPI_8PAD, 0x13),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_RADDR_SDR,   kFLEXSPI_8PAD, 32,   kFLEXSPI_Command_DUMMY_SDR,   kFLEXSPI_8PAD, DummyCycles),
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
}

#endif // XSPI_LUT_MACRONIX_H_
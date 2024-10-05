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

#ifndef FLEXSPI_SPANSION_H_
#define FLEXSPI_SPANSION_H_

#include "libmem_driver_xSPI.h"
#include "FlexSPI_Helper.h"
#include <array>

namespace Spansion
{
	enum class Command
	{
		ReadData    = 0,
		WriteData   = 1,
		ReadStatus  = 2,
		WriteEnable = 4,
		EraseSector = 6,
		PageProgram = 10,
		EraseChip   = 12
	};

	/*! LUT_HyperFlash:  */
	constexpr FlexSPI_LUT LUT_HyperFlash
	{
		// (0) Read Data --> compare @FlashCommands
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0xA0, kFLEXSPI_Command_RADDR_DDR, kFLEXSPI_8PAD, 0x18),
//		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_CADDR_DDR, kFLEXSPI_8PAD, 0x10, kFLEXSPI_Command_READ_DDR,  kFLEXSPI_8PAD, 0x04),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_CADDR_DDR, kFLEXSPI_8PAD, 0x10, kFLEXSPI_Command_DUMMY_RWDS_DDR, kFLEXSPI_8PAD, 0x0B),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_READ_DDR,  kFLEXSPI_8PAD, 0x04, kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0x0),
		0,	// Dummy to fill a block of four

		// (1) Write Data --> compare @FlashCommands
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x20, kFLEXSPI_Command_RADDR_DDR, kFLEXSPI_8PAD, 0x18),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_CADDR_DDR, kFLEXSPI_8PAD, 0x10, kFLEXSPI_Command_WRITE_DDR, kFLEXSPI_8PAD, 0x02),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four

		// (2) Read Status --> compare @FlashCommands
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x00),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0xAA), // ADDR 0x555 --> 0x555/8 = 0xAA
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x05), //            --> 0x555%8 = 0x05
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x70), // DATA 0x70
		// (3) continue with Read Status
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0xA0, kFLEXSPI_Command_RADDR_DDR, kFLEXSPI_8PAD, 0x18),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_CADDR_DDR, kFLEXSPI_8PAD, 0x10, kFLEXSPI_Command_DUMMY_RWDS_DDR, kFLEXSPI_8PAD, 0x0B),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_READ_DDR,  kFLEXSPI_8PAD, 0x04, kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0x0),
		0,	// Dummy to fill a block of four

		// (4) Write Enable --> compare @FlashCommands
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x00),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0xAA), // ADDR 0x555 --> 0x555/8 = 0xAA
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x05), //            --> 0x555%8 = 0x05
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0xAA), // DATA 0xAA
		// (5) continue with Write Enable
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x00),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x55), // ADDR 0x2AA --> 0x2AA/8 = 0x55
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x02), //            --> 0x2AA%8 = 0x02
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x55), // Data 0x55

		// (6) Erase Sector --> compare @FlashCommands
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x00),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0xAA), // ADDR 0x555 --> 0x555/8 = 0xAA
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x05), //            --> 0x555%8 = 0x05
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x80), // DATA 0x80
		// (7) continue with Erase Sector
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x00),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0xAA), // ADDR 0x555 --> 0x555/8 = 0xAA
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x05), //            --> 0x555%8 = 0x05
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0xAA), // Data 0x555
		// (8) continue with Erase Sector
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x00),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x55),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x02),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x55),
		// (9) continue with Erase Sector
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_RADDR_DDR, kFLEXSPI_8PAD, 0x18),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_CADDR_DDR, kFLEXSPI_8PAD, 0x10, kFLEXSPI_Command_DDR,  kFLEXSPI_8PAD, 0x00),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x30, kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0x00),
		0,	// Dummy to fill a block of four

		// (10) program page: --> compare @FlashCommands
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x00),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0xAA), // ADDR 0x555 --> 0x555/8 = 0xAA
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x05), //            --> 0x555%8 = 0x05
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0xA0), // DATA 0xA0
		// (11) continue with program page
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_RADDR_DDR, kFLEXSPI_8PAD, 0x18),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_CADDR_DDR, kFLEXSPI_8PAD, 0x10, kFLEXSPI_Command_WRITE_DDR, kFLEXSPI_8PAD, 0x80),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four

		// (12) Erase chip: --> compare @FlashCommands
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x00),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0xAA),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x05),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x80),
		// (13) continue with Erase chip
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x00),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0xAA),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x05),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0xAA),
		// (14) continue with Erase chip
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x00),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x55),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x02),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x55),
		// (15) continue with Erase chip
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x00),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0xAA),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x05),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x10),
	};
}

#endif // FLEXSPI_SPANSION_H_
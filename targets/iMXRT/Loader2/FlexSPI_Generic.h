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

#ifndef XSPI_LUT_GENERIC_H_
#define XSPI_LUT_GENERIC_H_

#include "libmem_Tools.h"
#include <array>

namespace Generic
{
	constexpr FlexSPI_LUT LUT_SPI
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

		// (2) Read JEDEC-ID --> compare @LUT_CommandOffsets
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

		// (7) Enter free
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four

		// (8) Write Status/Control Registers (Adesto) --> compare @LUT_CommandOffsets
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_SDR,         kFLEXSPI_1PAD, 0x71, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, 8),
		FLEXSPI_LUT_SEQ (kFLEXSPI_Command_WRITE_SDR,   kFLEXSPI_1PAD, 0x01, kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0),
		0,	// Dummy to fill a block of four
		0,	// Dummy to fill a block of four
	};
}

#endif // XSPI_LUT_GENERIC_H_

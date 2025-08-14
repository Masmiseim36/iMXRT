/** Loader for iMXRT-Family
Copyright (C) 2019-2026 Markus Klein
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

#ifndef FLEXSPI_WINBOND_H_
#define FLEXSPI_WINBOND_H_

#include "libmem_driver_FlexSPI.h"
#include "FlexSPI_Helper.h"
#include "LutTranslator.h"
#include <array>

namespace Winbond
{
	LibmemStatus_t Initialize (FlexSPI_Helper &flexSPI, MemoryType memType, DeviceInfo &info);

	enum class Command
	{
		ReadArray       =  0,
		ReadStatus1     =  1,
		ReadJEDEC_ID    =  2,
		WriteEnable     =  3,
		ProgramPage     =  4,
		EraseSector     =  5,
		EraseChip       =  6,
		ReadStatus2     =  7,
		WriteStatus1    =  8,
		WriteStatus2    =  9,
		ReadStatus3     = 10,

		Enter4ByteMode  = 11, // 32-bit LUT only
		Exit4ByteMode   = 11, // 24-bit LUT only
	};

	// LUT for Winbond Quad SPI with 24-Bit Addressing
	inline constexpr Lut::Table LUT_QuadSPI_24Bit = Lut::Make
	(
		{
			// (0) Fast Read Quad I/O --> compare @CommandOffsets
			// Read with 4READ
			Lut::Sequence (Lut::Command::Cmd_Sdr,       Lut::Pad::Count1, 0xEB, Lut::Command::RowAddress_Sdr, Lut::Pad::Count4, 24),
			Lut::Sequence (Lut::Command::Dummy_Sdr,     Lut::Pad::Count4,    6, Lut::Command::Read_Sdr,       Lut::Pad::Count4, 0x08),
		},

		{
			// (1) Read Status Register 1 --> compare @CommandOffsets
			Lut::Sequence (Lut::Command::Cmd_Sdr,       Lut::Pad::Count1, 0x05, Lut::Command::Read_Sdr,  Lut::Pad::Count1, 0x04),
		},

		{
			// (2) Read ID --> compare @CommandOffsets
			Lut::Sequence (Lut::Command::Cmd_Sdr,       Lut::Pad::Count1, 0x9F, Lut::Command::Read_Sdr,  Lut::Pad::Count1, 24),
		},

		{
			// (3) Write Enable --> compare @CommandOffsets
			Lut::Sequence (Lut::Command::Cmd_Sdr,       Lut::Pad::Count1, 0x06, Lut::Command::Stop,      Lut::Pad::Count1, 0),
		},

		{
			// (4) Page Program --> compare @CommandOffsets
			Lut::Sequence (Lut::Command::Cmd_Sdr,       Lut::Pad::Count1, 0x32, Lut::Command::RowAddress_Sdr, Lut::Pad::Count1, 24),
			Lut::Sequence (Lut::Command::Write_Sdr,     Lut::Pad::Count4, 255,  Lut::Command::Stop,           Lut::Pad::Count1, 0),
		},

		{
			// (5) Sector Erase (4k) --> compare @CommandOffsets
			Lut::Sequence (Lut::Command::Cmd_Sdr,       Lut::Pad::Count1, 0x20, Lut::Command::RowAddress_Sdr, Lut::Pad::Count1, 24),
		},

		{
			// (6) Chip Erase --> compare @CommandOffsets
			Lut::Sequence (Lut::Command::Cmd_Sdr,       Lut::Pad::Count1, 0xC7, Lut::Command::Stop,      Lut::Pad::Count1, 0),
		},

		{
			// (7) Read Status Register 2 --> compare @CommandOffsets
			Lut::Sequence (Lut::Command::Cmd_Sdr,       Lut::Pad::Count1, 0x35, Lut::Command::Read_Sdr,  Lut::Pad::Count1, 0x04),
		},

		{
			// (8) Write Status Registers 1 --> compare @CommandOffsets
			Lut::Sequence (Lut::Command::Cmd_Sdr,       Lut::Pad::Count1, 0x01, Lut::Command::Write_Sdr, Lut::Pad::Count1, 0x04),
		},

		{
			// (9) Write Status Registers 2 --> compare @CommandOffsets
			Lut::Sequence (Lut::Command::Cmd_Sdr,       Lut::Pad::Count1, 0x31, Lut::Command::Write_Sdr, Lut::Pad::Count1, 0x04),
		},

		{
			// (10) Read Status Register 3 --> compare @CommandOffsets
			Lut::Sequence (Lut::Command::Cmd_Sdr,       Lut::Pad::Count1, 0x15, Lut::Command::Read_Sdr,  Lut::Pad::Count1, 0x04),
		},

		{
			// (11) Exit 4-Byte Address Mode --> compare @CommandOffsets
			Lut::Sequence (Lut::Command::Cmd_Sdr,       Lut::Pad::Count1, 0xE9, Lut::Command::Stop,      Lut::Pad::Count1, 0x00),
		}
	);


	// LUT for Winbond Quad SPI with 32-Bit Addressing
	inline constexpr Lut::Table LUT_QuadSPI_32Bit = Lut::Make
	(
		{
			// (0) Fast Read Quad I/O --> compare @CommandOffsets
			// Read with 4READ
			Lut::Sequence (Lut::Command::Cmd_Sdr,       Lut::Pad::Count1, 0xEB, Lut::Command::RowAddress_Sdr, Lut::Pad::Count4, 32),
			Lut::Sequence (Lut::Command::Dummy_Sdr,     Lut::Pad::Count4,    6, Lut::Command::Read_Sdr,       Lut::Pad::Count4, 0x08),
		},

		{
			// (1) Read Status Register 1 --> compare @CommandOffsets
			Lut::Sequence (Lut::Command::Cmd_Sdr,       Lut::Pad::Count1, 0x05, Lut::Command::Read_Sdr,  Lut::Pad::Count1, 0x04),
		},

		{
			// (2) Read ID --> compare @CommandOffsets
			Lut::Sequence (Lut::Command::Cmd_Sdr,       Lut::Pad::Count1, 0x9F, Lut::Command::Read_Sdr,  Lut::Pad::Count1, 24),
		},

		{
			// (3) Write Enable --> compare @CommandOffsets
			Lut::Sequence (Lut::Command::Cmd_Sdr,       Lut::Pad::Count1, 0x06, Lut::Command::Stop,      Lut::Pad::Count1, 0),
		},

		{
			// (4) Page Program --> compare @CommandOffsets
			Lut::Sequence (Lut::Command::Cmd_Sdr,       Lut::Pad::Count1, 0x34, Lut::Command::RowAddress_Sdr, Lut::Pad::Count1, 32),
			Lut::Sequence (Lut::Command::Write_Sdr,     Lut::Pad::Count4, 255,  Lut::Command::Stop,           Lut::Pad::Count1, 0),
		},

		{
			// (5) Sector Erase (4k) --> compare @CommandOffsets
			Lut::Sequence (Lut::Command::Cmd_Sdr,       Lut::Pad::Count1, 0x21, Lut::Command::RowAddress_Sdr, Lut::Pad::Count1, 32),
		},

		{
			// (6) Chip Erase --> compare @CommandOffsets
			Lut::Sequence (Lut::Command::Cmd_Sdr,       Lut::Pad::Count1, 0xC7, Lut::Command::Stop,      Lut::Pad::Count1, 0),
		},

		{
			// (7) Read Status Register 2 --> compare @CommandOffsets
			Lut::Sequence (Lut::Command::Cmd_Sdr,       Lut::Pad::Count1, 0x35, Lut::Command::Read_Sdr,  Lut::Pad::Count1, 0x04),
		},

		{
			// (8) Write Status Registers 1 --> compare @CommandOffsets
			Lut::Sequence (Lut::Command::Cmd_Sdr,       Lut::Pad::Count1, 0x01, Lut::Command::Write_Sdr, Lut::Pad::Count1, 0x04),
		},

		{
			// (9) Write Status Registers 2 --> compare @CommandOffsets
			Lut::Sequence (Lut::Command::Cmd_Sdr,       Lut::Pad::Count1, 0x31, Lut::Command::Write_Sdr, Lut::Pad::Count1, 0x04),
		},

		{
			// (10) Read Status Register 3 --> compare @CommandOffsets
			Lut::Sequence (Lut::Command::Cmd_Sdr,       Lut::Pad::Count1, 0x15, Lut::Command::Read_Sdr,  Lut::Pad::Count1, 0x04),
		},

		{
			// (11) Enter 4-Byte Address Mode --> compare @CommandOffsets
			Lut::Sequence (Lut::Command::Cmd_Sdr,       Lut::Pad::Count1, 0xB7, Lut::Command::Stop,      Lut::Pad::Count1, 0x00),
		}
	);

}


#endif // FLEXSPI_WINBOND_H_
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

#ifndef FLEXSPI_SPANSION_H_
#define FLEXSPI_SPANSION_H_

#include "libmem_driver_FlexSPI.h"
#include "FlexSPI_Helper.h"
#include "LutTranslator.h"
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
	inline constexpr Lut::Table LUT_HyperFlash = Lut::Make
	(
		{
			// (0) Read Data --> compare @FlashCommands
			Lut::Sequence (Lut::Command::Cmd_Ddr,       Lut::Pad::Count8, 0xA0, Lut::Command::RowAddress_Ddr, Lut::Pad::Count8, 0x18),
			//	Lut::Sequence (Lut::Command::ColumnAddress_Ddr, Lut::Pad::Count8, 0x10, Lut::Command::Read_Ddr,  Lut::Pad::Count8, 0x04),
			Lut::Sequence (Lut::Command::ColumnAddress_Ddr, Lut::Pad::Count8, 0x10, Lut::Command::Dummy_Rwds_Ddr, Lut::Pad::Count8, 0x0B),
			Lut::Sequence (Lut::Command::Read_Ddr,      Lut::Pad::Count8, 0x04, Lut::Command::Stop,      Lut::Pad::Count1, 0x0),
		},
		{
			// (1) Write Data --> compare @FlashCommands
			Lut::Sequence (Lut::Command::Cmd_Ddr,       Lut::Pad::Count8, 0x20, Lut::Command::RowAddress_Ddr, Lut::Pad::Count8, 0x18),
			Lut::Sequence (Lut::Command::ColumnAddress_Ddr, Lut::Pad::Count8, 0x10, Lut::Command::Write_Ddr, Lut::Pad::Count8, 0x02),
		},
		{
			// (2) Read Status --> compare @FlashCommands
			Lut::Sequence (Lut::Command::Cmd_Ddr,       Lut::Pad::Count8, 0x00, Lut::Command::Cmd_Ddr, Lut::Pad::Count8, 0x00),
			Lut::Sequence (Lut::Command::Cmd_Ddr,       Lut::Pad::Count8, 0x00, Lut::Command::Cmd_Ddr, Lut::Pad::Count8, 0xAA), // ADDR 0x555 --> 0x555/8 = 0xAA
			Lut::Sequence (Lut::Command::Cmd_Ddr,       Lut::Pad::Count8, 0x00, Lut::Command::Cmd_Ddr, Lut::Pad::Count8, 0x05), //            --> 0x555%8 = 0x05
			Lut::Sequence (Lut::Command::Cmd_Ddr,       Lut::Pad::Count8, 0x00, Lut::Command::Cmd_Ddr, Lut::Pad::Count8, 0x70), // DATA 0x70
		},
		{
			// (3) continue with Read Status
			Lut::Sequence (Lut::Command::Cmd_Ddr,       Lut::Pad::Count8, 0xA0, Lut::Command::RowAddress_Ddr, Lut::Pad::Count8, 0x18),
			Lut::Sequence (Lut::Command::ColumnAddress_Ddr, Lut::Pad::Count8, 0x10, Lut::Command::Dummy_Rwds_Ddr, Lut::Pad::Count8, 0x0B),
			Lut::Sequence (Lut::Command::Read_Ddr,      Lut::Pad::Count8, 0x04, Lut::Command::Stop, Lut::Pad::Count1, 0x0),
		},
		{
			// (4) Write Enable --> compare @FlashCommands
			Lut::Sequence (Lut::Command::Cmd_Ddr,       Lut::Pad::Count8, 0x00, Lut::Command::Cmd_Ddr, Lut::Pad::Count8, 0x00),
			Lut::Sequence (Lut::Command::Cmd_Ddr,       Lut::Pad::Count8, 0x00, Lut::Command::Cmd_Ddr, Lut::Pad::Count8, 0xAA), // ADDR 0x555 --> 0x555/8 = 0xAA
			Lut::Sequence (Lut::Command::Cmd_Ddr,       Lut::Pad::Count8, 0x00, Lut::Command::Cmd_Ddr, Lut::Pad::Count8, 0x05), //            --> 0x555%8 = 0x05
			Lut::Sequence (Lut::Command::Cmd_Ddr,       Lut::Pad::Count8, 0x00, Lut::Command::Cmd_Ddr, Lut::Pad::Count8, 0xAA), // DATA 0xAA
		},
		{
			// (5) continue with Write Enable
			Lut::Sequence (Lut::Command::Cmd_Ddr,       Lut::Pad::Count8, 0x00, Lut::Command::Cmd_Ddr, Lut::Pad::Count8, 0x00),
			Lut::Sequence (Lut::Command::Cmd_Ddr,       Lut::Pad::Count8, 0x00, Lut::Command::Cmd_Ddr, Lut::Pad::Count8, 0x55), // ADDR 0x2AA --> 0x2AA/8 = 0x55
			Lut::Sequence (Lut::Command::Cmd_Ddr,       Lut::Pad::Count8, 0x00, Lut::Command::Cmd_Ddr, Lut::Pad::Count8, 0x02), //            --> 0x2AA%8 = 0x02
			Lut::Sequence (Lut::Command::Cmd_Ddr,       Lut::Pad::Count8, 0x00, Lut::Command::Cmd_Ddr, Lut::Pad::Count8, 0x55), // Data 0x55
		},
		{
			// (6) Erase Sector --> compare @FlashCommands
			Lut::Sequence (Lut::Command::Cmd_Ddr,       Lut::Pad::Count8, 0x00, Lut::Command::Cmd_Ddr, Lut::Pad::Count8, 0x00),
			Lut::Sequence (Lut::Command::Cmd_Ddr,       Lut::Pad::Count8, 0x00, Lut::Command::Cmd_Ddr, Lut::Pad::Count8, 0xAA), // ADDR 0x555 --> 0x555/8 = 0xAA
			Lut::Sequence (Lut::Command::Cmd_Ddr,       Lut::Pad::Count8, 0x00, Lut::Command::Cmd_Ddr, Lut::Pad::Count8, 0x05), //            --> 0x555%8 = 0x05
			Lut::Sequence (Lut::Command::Cmd_Ddr,       Lut::Pad::Count8, 0x00, Lut::Command::Cmd_Ddr, Lut::Pad::Count8, 0x80), // DATA 0x80
		},
		{
			// (7) continue with Erase Sector
			Lut::Sequence (Lut::Command::Cmd_Ddr,       Lut::Pad::Count8, 0x00, Lut::Command::Cmd_Ddr, Lut::Pad::Count8, 0x00),
			Lut::Sequence (Lut::Command::Cmd_Ddr,       Lut::Pad::Count8, 0x00, Lut::Command::Cmd_Ddr, Lut::Pad::Count8, 0xAA), // ADDR 0x555 --> 0x555/8 = 0xAA
			Lut::Sequence (Lut::Command::Cmd_Ddr,       Lut::Pad::Count8, 0x00, Lut::Command::Cmd_Ddr, Lut::Pad::Count8, 0x05), //            --> 0x555%8 = 0x05
			Lut::Sequence (Lut::Command::Cmd_Ddr,       Lut::Pad::Count8, 0x00, Lut::Command::Cmd_Ddr, Lut::Pad::Count8, 0xAA), // Data 0x555
		},
		{
			// (8) continue with Erase Sector
			Lut::Sequence (Lut::Command::Cmd_Ddr,       Lut::Pad::Count8, 0x00, Lut::Command::Cmd_Ddr, Lut::Pad::Count8, 0x00),
			Lut::Sequence (Lut::Command::Cmd_Ddr,       Lut::Pad::Count8, 0x00, Lut::Command::Cmd_Ddr, Lut::Pad::Count8, 0x55),
			Lut::Sequence (Lut::Command::Cmd_Ddr,       Lut::Pad::Count8, 0x00, Lut::Command::Cmd_Ddr, Lut::Pad::Count8, 0x02),
			Lut::Sequence (Lut::Command::Cmd_Ddr,       Lut::Pad::Count8, 0x00, Lut::Command::Cmd_Ddr, Lut::Pad::Count8, 0x55),
		},
		{
			// (9) continue with Erase Sector
			Lut::Sequence (Lut::Command::Cmd_Ddr,       Lut::Pad::Count8, 0x00, Lut::Command::RowAddress_Ddr, Lut::Pad::Count8, 0x18),
			Lut::Sequence (Lut::Command::ColumnAddress_Ddr, Lut::Pad::Count8, 0x10, Lut::Command::Cmd_Ddr,  Lut::Pad::Count8, 0x00),
			Lut::Sequence (Lut::Command::Cmd_Ddr,       Lut::Pad::Count8, 0x30, Lut::Command::Stop, Lut::Pad::Count1, 0x00),
		},
		{
			// (10) program page: --> compare @FlashCommands
			Lut::Sequence (Lut::Command::Cmd_Ddr,       Lut::Pad::Count8, 0x00, Lut::Command::Cmd_Ddr, Lut::Pad::Count8, 0x00),
			Lut::Sequence (Lut::Command::Cmd_Ddr,       Lut::Pad::Count8, 0x00, Lut::Command::Cmd_Ddr, Lut::Pad::Count8, 0xAA), // ADDR 0x555 --> 0x555/8 = 0xAA
			Lut::Sequence (Lut::Command::Cmd_Ddr,       Lut::Pad::Count8, 0x00, Lut::Command::Cmd_Ddr, Lut::Pad::Count8, 0x05), //            --> 0x555%8 = 0x05
			Lut::Sequence (Lut::Command::Cmd_Ddr,       Lut::Pad::Count8, 0x00, Lut::Command::Cmd_Ddr, Lut::Pad::Count8, 0xA0), // DATA 0xA0
		},
		{
			// (11) continue with program page
			Lut::Sequence (Lut::Command::Cmd_Ddr,       Lut::Pad::Count8, 0x00, Lut::Command::RowAddress_Ddr, Lut::Pad::Count8, 0x18),
			Lut::Sequence (Lut::Command::ColumnAddress_Ddr, Lut::Pad::Count8, 0x10, Lut::Command::Write_Ddr, Lut::Pad::Count8, 0x80),
		},
		{
			// (12) Erase chip: --> compare @FlashCommands
			Lut::Sequence (Lut::Command::Cmd_Ddr,       Lut::Pad::Count8, 0x00, Lut::Command::Cmd_Ddr, Lut::Pad::Count8, 0x00),
			Lut::Sequence (Lut::Command::Cmd_Ddr,       Lut::Pad::Count8, 0x00, Lut::Command::Cmd_Ddr, Lut::Pad::Count8, 0xAA),
			Lut::Sequence (Lut::Command::Cmd_Ddr,       Lut::Pad::Count8, 0x00, Lut::Command::Cmd_Ddr, Lut::Pad::Count8, 0x05),
			Lut::Sequence (Lut::Command::Cmd_Ddr,       Lut::Pad::Count8, 0x00, Lut::Command::Cmd_Ddr, Lut::Pad::Count8, 0x80),
		},
		{
			// (13) continue with Erase chip
			Lut::Sequence (Lut::Command::Cmd_Ddr,       Lut::Pad::Count8, 0x00, Lut::Command::Cmd_Ddr, Lut::Pad::Count8, 0x00),
			Lut::Sequence (Lut::Command::Cmd_Ddr,       Lut::Pad::Count8, 0x00, Lut::Command::Cmd_Ddr, Lut::Pad::Count8, 0xAA),
			Lut::Sequence (Lut::Command::Cmd_Ddr,       Lut::Pad::Count8, 0x00, Lut::Command::Cmd_Ddr, Lut::Pad::Count8, 0x05),
			Lut::Sequence (Lut::Command::Cmd_Ddr,       Lut::Pad::Count8, 0x00, Lut::Command::Cmd_Ddr, Lut::Pad::Count8, 0xAA),
		},
		{
			// (14) continue with Erase chip
			Lut::Sequence (Lut::Command::Cmd_Ddr,       Lut::Pad::Count8, 0x00, Lut::Command::Cmd_Ddr, Lut::Pad::Count8, 0x00),
			Lut::Sequence (Lut::Command::Cmd_Ddr,       Lut::Pad::Count8, 0x00, Lut::Command::Cmd_Ddr, Lut::Pad::Count8, 0x55),
			Lut::Sequence (Lut::Command::Cmd_Ddr,       Lut::Pad::Count8, 0x00, Lut::Command::Cmd_Ddr, Lut::Pad::Count8, 0x02),
			Lut::Sequence (Lut::Command::Cmd_Ddr,       Lut::Pad::Count8, 0x00, Lut::Command::Cmd_Ddr, Lut::Pad::Count8, 0x55),
		},
		{
			// (15) continue with Erase chip
			Lut::Sequence (Lut::Command::Cmd_Ddr,       Lut::Pad::Count8, 0x00, Lut::Command::Cmd_Ddr, Lut::Pad::Count8, 0x00),
			Lut::Sequence (Lut::Command::Cmd_Ddr,       Lut::Pad::Count8, 0x00, Lut::Command::Cmd_Ddr, Lut::Pad::Count8, 0xAA),
			Lut::Sequence (Lut::Command::Cmd_Ddr,       Lut::Pad::Count8, 0x00, Lut::Command::Cmd_Ddr, Lut::Pad::Count8, 0x05),
			Lut::Sequence (Lut::Command::Cmd_Ddr,       Lut::Pad::Count8, 0x00, Lut::Command::Cmd_Ddr, Lut::Pad::Count8, 0x10),
		}
	);
}

#endif // FLEXSPI_SPANSION_H_
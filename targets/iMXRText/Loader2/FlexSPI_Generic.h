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

#ifndef XSPI_LUT_GENERIC_H_
#define XSPI_LUT_GENERIC_H_

#include "FlexSPI_Helper.h"
#include "LutTranslator.h"

namespace Generic
{
	inline constexpr Lut::Table LUT_SPI = Lut::Make
	(
		{
			// (0) Read Array --> compare @LUT_CommandOffsets
			Lut::Sequence (Lut::Command::Cmd_Sdr,    Lut::Pad::Count1, 0x0B, Lut::Command::RowAddress_Sdr, Lut::Pad::Count1, 32),
			Lut::Sequence (Lut::Command::Dummy_Sdr,  Lut::Pad::Count1, 8,    Lut::Command::Read_Sdr,       Lut::Pad::Count1, 128),
//			(kFLEXSPI_Command_JUMP_ON_CS, Lut::Pad::Count1, 0,    Lut::Command::Stop,      Lut::Pad::Count1, 0),
		},

		{
			// (1) Read Status (byte 1) --> compare @LUT_CommandOffsets
			Lut::Sequence (Lut::Command::Cmd_Sdr,         Lut::Pad::Count1, 0x05,  Lut::Command::Read_Sdr,  Lut::Pad::Count1, 1),
		},

		{
			// (2) Read JEDEC-ID --> compare @LUT_CommandOffsets
			Lut::Sequence (Lut::Command::Cmd_Sdr,     Lut::Pad::Count1, 0x9F, Lut::Command::Read_Sdr,  Lut::Pad::Count1, 24),
			Lut::Sequence (Lut::Command::Stop,        Lut::Pad::Count1, 0,    Lut::Command::Stop,      Lut::Pad::Count1, 0),
		},

		{
			// (3) Write Enable --> compare @LUT_CommandOffsets    
			Lut::Sequence (Lut::Command::Cmd_Sdr,     Lut::Pad::Count1, 0x06, Lut::Command::Stop,      Lut::Pad::Count1, 0),
		},

		{
			// (4) Page Program --> compare @LUT_CommandOffsets
			Lut::Sequence (Lut::Command::Cmd_Sdr,     Lut::Pad::Count1, 0x02, Lut::Command::RowAddress_Sdr, Lut::Pad::Count1, 32),
			Lut::Sequence (Lut::Command::Write_Sdr,   Lut::Pad::Count1, 128,  Lut::Command::Stop,           Lut::Pad::Count1, 0), 
		},

		{
			// (5) Sector/Block Erase 4K --> compare @LUT_CommandOffsets
			Lut::Sequence (Lut::Command::Cmd_Sdr,     Lut::Pad::Count1, 0x20, Lut::Command::RowAddress_Sdr, Lut::Pad::Count1, 32),
		},

		{
			// (6) Chip Erase --> compare @LUT_CommandOffsets
			Lut::Sequence (Lut::Command::Cmd_Sdr,     Lut::Pad::Count1, 0x60, Lut::Command::Stop,      Lut::Pad::Count1, 0),
		},

		{
			// (7) unused (keep index for command 8)
			0U,
		},

		{
			// (8) Write Status/Control Registers (Adesto) --> compare @LUT_CommandOffsets
			Lut::Sequence (Lut::Command::Cmd_Sdr,     Lut::Pad::Count1, 0x71, Lut::Command::RowAddress_Sdr, Lut::Pad::Count1, 8),
			Lut::Sequence (Lut::Command::Write_Sdr,   Lut::Pad::Count1, 0x01, Lut::Command::Stop,      Lut::Pad::Count1, 0),
		}
	);

	inline constexpr Lut::Table LUT_Hyperram = Lut::Make
	(
		{
			// (0) Read
			Lut::Sequence (Lut::Command::Cmd_Ddr,           Lut::Pad::Count8, 0xA0, Lut::Command::RowAddress_Ddr, Lut::Pad::Count8, 0x18),
			Lut::Sequence (Lut::Command::ColumnAddress_Ddr, Lut::Pad::Count8, 0x10, Lut::Command::Dummy_Rwds_Ddr, Lut::Pad::Count8, 0x06),
			Lut::Sequence (Lut::Command::Read_Ddr,          Lut::Pad::Count8, 0x04, Lut::Command::Stop,           Lut::Pad::Count1, 0x00),
		},

		{
			// (1) Write Register
			Lut::Sequence (Lut::Command::Cmd_Ddr,           Lut::Pad::Count8, 0x60, Lut::Command::RowAddress_Ddr,  Lut::Pad::Count8, 0x18),
			Lut::Sequence (Lut::Command::ColumnAddress_Ddr, Lut::Pad::Count8, 0x10, Lut::Command::Write_Ddr,       Lut::Pad::Count8, 0x02),
		},

		{
			// (2) Read Register
			Lut::Sequence (Lut::Command::Cmd_Ddr,           Lut::Pad::Count8, 0xE0, Lut::Command::RowAddress_Ddr, Lut::Pad::Count8, 0x18),
			Lut::Sequence (Lut::Command::ColumnAddress_Ddr, Lut::Pad::Count8, 0x10, Lut::Command::Dummy_Rwds_Ddr, Lut::Pad::Count8, 0x06),
			Lut::Sequence (Lut::Command::Read_Ddr,          Lut::Pad::Count8, 0x04, Lut::Command::Stop,           Lut::Pad::Count1, 0x00),
		},

		{ 0U }, // (3) unused
		{ 0U }, // (4) unused
		{ 0U }, // (5) unused
		{ 0U }, // (6) unused
		{ 0U }, // (7) unused
		{ 0U }, // (8) unused

		{
			// (9) Read
			Lut::Sequence (Lut::Command::Cmd_Ddr,           Lut::Pad::Count8, 0x20, Lut::Command::RowAddress_Ddr, Lut::Pad::Count8, 0x18),
			Lut::Sequence (Lut::Command::ColumnAddress_Ddr, Lut::Pad::Count8, 0x10, Lut::Command::Dummy_Rwds_Ddr, Lut::Pad::Count8, 0x06),
			Lut::Sequence (Lut::Command::Write_Ddr,         Lut::Pad::Count8, 0x04, Lut::Command::Stop,           Lut::Pad::Count1, 0x00),
		}
	);

//	#define Lut::Sequence(cmd0, pad0, op0, cmd1, pad1, op1)
}

#endif // XSPI_LUT_GENERIC_H_

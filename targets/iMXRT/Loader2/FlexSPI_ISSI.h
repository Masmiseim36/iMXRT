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

#ifndef FLEXSPI_ISSI_H_
#define FLEXSPI_ISSI_H_

#include "libmem_driver_FlexSPI.h"
#include "FlexSPI_Helper.h"
#include "LutTranslator.h"
#include <array>
#include <cstdint>


namespace ISSI
{
	LibmemStatus_t Initialize (FlexSPI_Helper &flexSPI, MemoryType memType, DeviceInfo &info);
	status_t TryDetect  (FlexSPI_Helper &flexSPI, DeviceInfo &Info);

	enum class Command: uint8_t
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
	inline constexpr Lut::Table LUT_QuadSPI = Lut::Make
	(
		{
			// (0) Read Array --> compare @Command
			Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count1, 0xEB, Lut::Command::RowAddress_Sdr, Lut::Pad::Count4, 24),
			Lut::Sequence (Lut::Command::Dummy_Sdr, Lut::Pad::Count4, 0x06, Lut::Command::Read_Sdr,       Lut::Pad::Count4, 0x04),
		},

		{
			// (1) Read Status --> compare @Command
			Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count1, 0x05, Lut::Command::Read_Sdr,  Lut::Pad::Count1, 0x04),
		},

		{
			// (2) Read JEDEC ID --> compare @Command
			Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count1, 0x9F, Lut::Command::Read_Sdr,  Lut::Pad::Count1, 24),
		},

		{
			// (3) Write Enable --> compare @Command
			Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count1, 0x06, Lut::Command::Stop,      Lut::Pad::Count1, 0),
		},

		{
			// (4) Page Program --> compare @Command
			Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count1, 0x32, Lut::Command::RowAddress_Sdr, Lut::Pad::Count1, 24),
			Lut::Sequence (Lut::Command::Write_Sdr, Lut::Pad::Count4, 0x04, Lut::Command::Stop,           Lut::Pad::Count1, 0),
		},

		{
			// (5) Sector Erase 4k --> compare @Command
			Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count1, 0xD7, Lut::Command::RowAddress_Sdr, Lut::Pad::Count1, 24),
		},

		{
			// (6) Chip Erase --> compare @Command
			Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count1, 0xC7, Lut::Command::Stop,      Lut::Pad::Count1, 0),
		},

		{
			// (7) Return to Standard SPI Mode --> compare @Command
			Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count4, 0xF5, Lut::Command::Stop,      Lut::Pad::Count1, 0),
		},

		{
			// (8) Write Status Registers --> compare @Command
			Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count4, 0x01, Lut::Command::Write_Sdr, Lut::Pad::Count4, 0x04),
		},

		{
			// (9) Enter QPI mode --> compare @Command
			Lut::Sequence (Lut::Command::Cmd_Sdr,     Lut::Pad::Count1, 0x38, Lut::Command::Stop,      Lut::Pad::Count1, 0),
		},

		{ 0U }, // (10) empty

		{
			// (11) Write Configuration volatile - SPI
			Lut::Sequence (Lut::Command::Cmd_Sdr,     Lut::Pad::Count1, 0x81, Lut::Command::RowAddress_Sdr, Lut::Pad::Count1, 24),
			Lut::Sequence (Lut::Command::Write_Sdr,   Lut::Pad::Count1, 8,    Lut::Command::Stop,           Lut::Pad::Count1, 0),
		},

		{
			// (12) Read Configuration volatile - SPI
			Lut::Sequence (Lut::Command::Cmd_Sdr,     Lut::Pad::Count1, 0x85, Lut::Command::RowAddress_Sdr,   Lut::Pad::Count1, 24),
			Lut::Sequence (Lut::Command::Dummy_Sdr,   Lut::Pad::Count1, DummyCycles,  Lut::Command::Read_Sdr, Lut::Pad::Count1, 8),
		}
	);

	// LUT for ISSI Quad SPI with 32-Bit Addressing
	inline constexpr Lut::Table LUT_QuadSPI_32Bit = Lut::Make
	(
		{
			// (0) Read Array --> compare @Command
			Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count1, 0xEC, Lut::Command::RowAddress_Sdr, Lut::Pad::Count4, 32),
			Lut::Sequence (Lut::Command::Dummy_Sdr, Lut::Pad::Count4, 0x06, Lut::Command::Read_Sdr,       Lut::Pad::Count4, 0x04),
		},

		{
			// (1) Read Status --> compare @Command
			Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count1, 0x05, Lut::Command::Read_Sdr,  Lut::Pad::Count1, 0x04),
		},

		{
			// (2) Read JEDEC ID --> compare @Command
			Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count1, 0x9F, Lut::Command::Read_Sdr,  Lut::Pad::Count1, 24),
		},

		{
			// (3) Write Enable --> compare @Command
			Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count1, 0x06, Lut::Command::Stop,      Lut::Pad::Count1, 0),
		},

		{
			// (4) Page Program --> compare @Command
			Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count4, 0x12, Lut::Command::RowAddress_Sdr, Lut::Pad::Count4, 32),
			Lut::Sequence (Lut::Command::Write_Sdr, Lut::Pad::Count4, 0x04, Lut::Command::Stop,      Lut::Pad::Count1, 0),
		},

		{
			// (5) Block Erase 4K / Sector Erase --> compare @Command
			Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count4, 0x21, Lut::Command::RowAddress_Sdr, Lut::Pad::Count4, 32),
		},

		{
			// (6) Chip Erase --> compare @Command
			Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count4, 0x60, Lut::Command::Stop,      Lut::Pad::Count1, 0),
		},

		{
			// (7) Return to Standard SPI Mode --> compare @Command
			Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count4, 0xF5, Lut::Command::Stop,      Lut::Pad::Count1, 0),
		},

		{
			// (8) Write Status Registers --> compare @Command
			Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count4, 0x01, Lut::Command::Write_Sdr, Lut::Pad::Count4, 0x04),
		},

		{
			// (9) Enter QPI mode --> compare @Command
			Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count1, 0x38, Lut::Command::Stop,      Lut::Pad::Count1, 0),
		}
	);

	//// Number of dummy cycles after Read Command for ISSI-Flash
	constexpr uint32_t DummyCycles_Octa   = 16;	// Compare "Table 6.8 Maximum Clock Frequencies – SDR and DDR Read Starting at 4-Byte Address" in the Datasheet

	// LUT for ISSI Octa SPI with 32-Bit Addressing
	inline constexpr Lut::Table LUT_OctaSPI_DDR = Lut::Make
	(
		{
			// (0) Read Array --> compare @Command
			Lut::Sequence (Lut::Command::Cmd_Sdr,        Lut::Pad::Count8, 0xFD, Lut::Command::RowAddress_Ddr, Lut::Pad::Count8, 32),
			Lut::Sequence (Lut::Command::Dummy_Rwds_Ddr, Lut::Pad::Count8, DummyCycles_Octa, Lut::Command::Read_Ddr, Lut::Pad::Count8, 128),
		},

		{
			// (1) Read Status --> compare @Command
			Lut::Sequence (Lut::Command::Cmd_Sdr,     Lut::Pad::Count8, 0x05, Lut::Command::Dummy_Ddr, Lut::Pad::Count8,  DummyCycles_Octa),
			Lut::Sequence (Lut::Command::Read_Ddr,    Lut::Pad::Count8, 1,    Lut::Command::Stop,      Lut::Pad::Count1, 0),
		},

		{
			// (2) Read JEDEC ID --> compare @Command
			Lut::Sequence (Lut::Command::Cmd_Sdr,     Lut::Pad::Count8, 0x9F, Lut::Command::Dummy_Rwds_Ddr, Lut::Pad::Count8, 4),
			Lut::Sequence (Lut::Command::Read_Ddr,    Lut::Pad::Count8, 0x04, Lut::Command::Stop,        Lut::Pad::Count8, 0x0),
		},

		{
			// (3) Write Enable --> compare @Command
			Lut::Sequence (Lut::Command::Cmd_Sdr,     Lut::Pad::Count8, 0x06, Lut::Command::Stop,      Lut::Pad::Count1, 0),
		},

		{
			// (4) Page Program --> compare @Command
			Lut::Sequence (Lut::Command::Cmd_Sdr,     Lut::Pad::Count8, 0x8E, Lut::Command::RowAddress_Ddr, Lut::Pad::Count8, 32),
			Lut::Sequence (Lut::Command::Write_Ddr,   Lut::Pad::Count8, 128,  Lut::Command::Stop,      Lut::Pad::Count1, 0),
		},

		{
			// (5) Sector erase 4K --> compare @Command
			Lut::Sequence (Lut::Command::Cmd_Sdr,     Lut::Pad::Count8, 0x21, Lut::Command::RowAddress_Ddr, Lut::Pad::Count8, 32),
		},

		{
			// (6) Chip Erase --> compare @Command
			Lut::Sequence (Lut::Command::Cmd_Sdr,     Lut::Pad::Count8, 0x60, Lut::Command::Stop,      Lut::Pad::Count1, 0),
		},

		{
			// (7) Read Configuration volatile
			Lut::Sequence (Lut::Command::Cmd_Sdr,     Lut::Pad::Count8, 0x85, Lut::Command::RowAddress_Ddr, Lut::Pad::Count8, 32),
			Lut::Sequence (Lut::Command::Dummy_Ddr,   Lut::Pad::Count8,  DummyCycles_Octa,  Lut::Command::Read_Ddr, Lut::Pad::Count8, 1),
		},

		{
			// (8) Read Configuration non-volatile
			Lut::Sequence (Lut::Command::Cmd_Sdr,     Lut::Pad::Count8, 0xB5, Lut::Command::RowAddress_Ddr,   Lut::Pad::Count8, 32),
			Lut::Sequence (Lut::Command::Dummy_Ddr,   Lut::Pad::Count8, DummyCycles_Octa, Lut::Command::Read_Ddr, Lut::Pad::Count8, 8),
		},

		{
			// (9) Write Configuration volatile
			Lut::Sequence (Lut::Command::Cmd_Sdr,     Lut::Pad::Count8, 0x81, Lut::Command::RowAddress_Ddr, Lut::Pad::Count8, 32),
			Lut::Sequence (Lut::Command::Write_Sdr,   Lut::Pad::Count8, 8,    Lut::Command::Stop,      Lut::Pad::Count1, 0),
		},

		{
			// (10) Write Configuration non-volatile
			Lut::Sequence (Lut::Command::Cmd_Sdr,     Lut::Pad::Count8, 0xB1, Lut::Command::RowAddress_Ddr, Lut::Pad::Count8, 32),
			Lut::Sequence (Lut::Command::Write_Sdr,   Lut::Pad::Count8, 4,    Lut::Command::Stop,      Lut::Pad::Count1, 0),
		},

		{
			// (11) Write Configuration volatile - SPI
			Lut::Sequence (Lut::Command::Cmd_Sdr,     Lut::Pad::Count1, 0x81, Lut::Command::RowAddress_Sdr, Lut::Pad::Count1, 24),
			Lut::Sequence (Lut::Command::Write_Sdr,   Lut::Pad::Count1, 8,    Lut::Command::Stop,      Lut::Pad::Count1, 0),
		},

		{
			// (12) Read Configuration volatile - SPI
			Lut::Sequence (Lut::Command::Cmd_Sdr,     Lut::Pad::Count1, 0x85, Lut::Command::RowAddress_Sdr, Lut::Pad::Count1, 24),
			Lut::Sequence (Lut::Command::Dummy_Sdr,   Lut::Pad::Count1, DummyCycles_Octa, Lut::Command::Read_Sdr, Lut::Pad::Count1, 8),
		},

		{
			// (13) Read Status - SPI
			Lut::Sequence (Lut::Command::Cmd_Sdr,     Lut::Pad::Count1, 0x05, Lut::Command::Read_Sdr,  Lut::Pad::Count1, 0x04),
		}
	);
}


#endif // FLEXSPI_ISSI_H_
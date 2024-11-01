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

#ifndef FLEXSPI_MACRONIX_H_
#define FLEXSPI_MACRONIX_H_

#include "libmem_driver_FlexSPI.h"
#include "FlexSPI_Helper.h"
#include "LutTranslator.h"
#include <array>

namespace Macronix
{
	LibmemStatus_t Initialize (FlexSPI_Helper &flexSPI, MemoryType memType, DeviceInfo &Info);
	status_t TryDetect        (FlexSPI_Helper &flexSPI, DeviceInfo &Info);

	enum class Command: uint8_t
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
	inline constexpr Lut::Table LUT_SPI = Lut::Make
	(
		{
			// (0) Read Array --> compare @Command
			// Read with normal read
			Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count1, 0x03, Lut::Command::RowAddress_Sdr, Lut::Pad::Count1, 24),
			Lut::Sequence (Lut::Command::Read_Sdr,  Lut::Pad::Count1, 128,  Lut::Command::Stop,      Lut::Pad::Count1, 0),
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
			// normal page program
			Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count1, 0x02, Lut::Command::RowAddress_Sdr, Lut::Pad::Count1, 24),
			Lut::Sequence (Lut::Command::Write_Sdr, Lut::Pad::Count1, 128,  Lut::Command::Stop,      Lut::Pad::Count1, 0),
		},

		{
			// (5) Sector Erase 4K --> compare @Command
			Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count1, 0x20, Lut::Command::RowAddress_Sdr, Lut::Pad::Count1, 24),
		},

		{
			// (6) Chip Erase --> compare @Command
			Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count1, 0xC7, Lut::Command::Stop,      Lut::Pad::Count1, 0),
		},

		{
			// (7) Read configuration register
			Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count1, 0x15, Lut::Command::Read_Sdr,  Lut::Pad::Count1, 0x04),
		},

		{
			// (8) Write status and configuration registers --> compare @Command
			Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count1, 0x01, Lut::Command::Write_Sdr, Lut::Pad::Count1, 0x04),
		},

		{
			// (9) Write Configuration Register 2 --> compare @Command
			Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count1, 0x72, Lut::Command::RowAddress_Sdr,   Lut::Pad::Count1, 32),
			Lut::Sequence (Lut::Command::Write_Sdr, Lut::Pad::Count1, 0x01, Lut::Command::Stop,        Lut::Pad::Count1, 0),
		},

		{
			// (10) read Configuration Register 2
			Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count1, 0x71, Lut::Command::Read_Sdr,  Lut::Pad::Count1, 0x04),
		},

		{
			// (11) Enter 4 Byte Mode --> compare @Command
			Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count1, 0xB7, Lut::Command::Stop,      Lut::Pad::Count1, 0),
		}
	);

	// LUT for Macronix Quad-SPI - 24 Bit Addressing
	inline constexpr Lut::Table LUT_QuadSPI = Lut::Make
	(
		{
			// (0) Read Array --> compare @Command
			// Read with 4READ
			Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count1, 0xEB, Lut::Command::RowAddress_Sdr, Lut::Pad::Count4, 24),
			Lut::Sequence (Lut::Command::Dummy_Sdr, Lut::Pad::Count4,    6, Lut::Command::Read_Sdr,  Lut::Pad::Count4, 0x08),
			// Read with QREAD
			//	Lut::Sequence (Lut::Command::Cmd_Sdr,       Lut::Pad::Count1, 0x6B, Lut::Command::RowAddress_Sdr, Lut::Pad::Count1, 24),
			//	Lut::Sequence (Lut::Command::Dummy_Sdr, Lut::Pad::Count1, 0x08, Lut::Command::Read_Sdr,  Lut::Pad::Count4, 128),
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
			// quad page program (4pp)
			Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count1, 0x38, Lut::Command::RowAddress_Sdr, Lut::Pad::Count4, 24),
			Lut::Sequence (Lut::Command::Write_Sdr, Lut::Pad::Count4, 255,  Lut::Command::Stop,      Lut::Pad::Count1, 0),
		},

		{
			// (5) Sector Erase 4K --> compare @Command
			Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count1, 0x20, Lut::Command::RowAddress_Sdr, Lut::Pad::Count1, 24),
		},

		{
			// (6) Chip Erase --> compare @Command
			Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count1, 0xC7, Lut::Command::Stop,      Lut::Pad::Count1, 0),
		},

		{
			// (7) Read configuration register
			Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count1, 0x15, Lut::Command::Read_Sdr,  Lut::Pad::Count4, 0x04),
		},

		{
			// (8) Write status and configuration registers --> compare @Command
			Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count1, 0x01, Lut::Command::Write_Sdr, Lut::Pad::Count4, 0x04),
		}
	);

	// LUT for Macronix Quad-SPI - 24 Bit Addressing
	inline constexpr Lut::Table LUT_QuadSPI_32Bit = Lut::Make
	(
		{
			// (0) Read Array --> compare @Command
			// Read with 4READ
			Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count1, 0xEB, Lut::Command::RowAddress_Sdr, Lut::Pad::Count4, 0x20),
			Lut::Sequence (Lut::Command::Dummy_Sdr, Lut::Pad::Count4,    6, Lut::Command::Read_Sdr,  Lut::Pad::Count4, 0x08),
			// Read with QREAD
			//	Lut::Sequence (Lut::Command::Cmd_Sdr,       Lut::Pad::Count1, 0x6B, Lut::Command::RowAddress_Sdr, Lut::Pad::Count1, 0x20,
			//	Lut::Sequence (Lut::Command::Dummy_Sdr, Lut::Pad::Count1, 0x08, Lut::Command::Read_Sdr,  Lut::Pad::Count4, 128),
		},

		{
			// (1) Read Status --> compare @Command
			Lut::Sequence (Lut::Command::Cmd_Sdr,       Lut::Pad::Count1, 0x05, Lut::Command::Read_Sdr,  Lut::Pad::Count1, 0x04),
		},

		{
			// (2) Read JEDEC ID --> compare @Command
			Lut::Sequence (Lut::Command::Cmd_Sdr,       Lut::Pad::Count1, 0x9F, Lut::Command::Read_Sdr,  Lut::Pad::Count1, 24),
		},

		{
			// (3) Write Enable --> compare @Command
			Lut::Sequence (Lut::Command::Cmd_Sdr,       Lut::Pad::Count1, 0x06, Lut::Command::Stop,      Lut::Pad::Count1, 0),
		},

		{
			// (4) Page Program --> compare @Command
			// quad page program (4pp)
			Lut::Sequence (Lut::Command::Cmd_Sdr,       Lut::Pad::Count1, 0x38, Lut::Command::RowAddress_Sdr, Lut::Pad::Count4, 0x20),
			Lut::Sequence (Lut::Command::Write_Sdr, Lut::Pad::Count4, 255,  Lut::Command::Stop,      Lut::Pad::Count1, 0),
		},

		{
			// (5) Sector Erase 4K --> compare @Command
			Lut::Sequence (Lut::Command::Cmd_Sdr,       Lut::Pad::Count1, 0x20, Lut::Command::RowAddress_Sdr, Lut::Pad::Count1, 0x20),
		},

		{
			// (6) Chip Erase --> compare @Command
			Lut::Sequence (Lut::Command::Cmd_Sdr,       Lut::Pad::Count1, 0xC7, Lut::Command::Stop,      Lut::Pad::Count1, 0),
		},

		{
			// (7) Read configuration register
			Lut::Sequence (Lut::Command::Cmd_Sdr,       Lut::Pad::Count1, 0x15, Lut::Command::Read_Sdr,  Lut::Pad::Count4, 0x04),
		},

		{
			// (8) Write status and configuration registers --> compare @Command
			Lut::Sequence (Lut::Command::Cmd_Sdr,       Lut::Pad::Count1, 0x01, Lut::Command::Write_Sdr, Lut::Pad::Count4, 0x04),
		}
	);

	// Number of dummy cycles after Read Command for Macronix-Flash
	inline constexpr uint32_t DummyCycles   = 12;	// Compare "Dummy Cycle and Frequency Table (MHz)" in the datasheet

	// LookUp Table for Macronix MX25UM-Family Octa-Flash devices in Octa configuration with DDR
	inline constexpr Lut::Table LUT_OctaSPI_DDR = Lut::Make
	(
		{
			// (0) Read Array	- Octa IO DT Read --> compare @Command
			Lut::Sequence (Lut::Command::Cmd_Ddr,        Lut::Pad::Count8, 0xEE, Lut::Command::Cmd_Ddr,     Lut::Pad::Count8, 0x11),
			Lut::Sequence (Lut::Command::RowAddress_Ddr, Lut::Pad::Count8, 32,   Lut::Command::Dummy_Ddr,   Lut::Pad::Count8, (DummyCycles * 2)+1),
			Lut::Sequence (Lut::Command::Read_Ddr,       Lut::Pad::Count8, 128,  Lut::Command::Stop,        Lut::Pad::Count1, 0),
		},

		{
			// (1) Read Status (byte 1) --> compare @Command
			Lut::Sequence (Lut::Command::Cmd_Ddr,        Lut::Pad::Count8, 0x05, Lut::Command::Cmd_Ddr,     Lut::Pad::Count8, 0xFA),
			Lut::Sequence (Lut::Command::RowAddress_Ddr, Lut::Pad::Count8, 32,   Lut::Command::Dummy_Ddr,   Lut::Pad::Count8, (DummyCycles * 2)+1),
			Lut::Sequence (Lut::Command::Read_Ddr,       Lut::Pad::Count8, 4,    Lut::Command::Stop,        Lut::Pad::Count1, 0x0),
		},

		{
			// (2) Read JEDEC ID --> compare @Command
			Lut::Sequence (Lut::Command::Cmd_Ddr,        Lut::Pad::Count8, 0x9F, Lut::Command::Cmd_Ddr,     Lut::Pad::Count8, 0x60),
			Lut::Sequence (Lut::Command::RowAddress_Ddr, Lut::Pad::Count8, 0x20, Lut::Command::Dummy_Ddr,   Lut::Pad::Count8, 7),
			Lut::Sequence (Lut::Command::Read_Sdr,       Lut::Pad::Count8, 0x04, Lut::Command::Stop,        Lut::Pad::Count1, 0x0),
		},

		{
			// (3) Write Enable --> compare @Command
			Lut::Sequence (Lut::Command::Cmd_Ddr,        Lut::Pad::Count8, 0x06, Lut::Command::Cmd_Ddr,     Lut::Pad::Count8, 0xF9),
		},

		{
			// (4) Page Program --> compare @Command
			Lut::Sequence (Lut::Command::Cmd_Ddr,         Lut::Pad::Count8, 0x12, Lut::Command::Cmd_Ddr,     Lut::Pad::Count8, 0xED),
			Lut::Sequence (Lut::Command::RowAddress_Ddr,  Lut::Pad::Count8, 32,   Lut::Command::Write_Ddr,   Lut::Pad::Count8, 4),
		},

		{
			// (5) Sector Erase 4K --> compare @Command
			Lut::Sequence (Lut::Command::Cmd_Ddr,         Lut::Pad::Count8, 0x21, Lut::Command::Cmd_Ddr,     Lut::Pad::Count8, 0xDE),	// Sector Erase (4K)
//			Lut::Sequence (Lut::Command::Cmd_Ddr,         Lut::Pad::Count8, 0xDC, Lut::Command::Cmd_Ddr,     Lut::Pad::Count8, 0x23),	// Block Erase (64K)
			Lut::Sequence (Lut::Command::RowAddress_Ddr,  Lut::Pad::Count8, 32,   Lut::Command::Stop,        Lut::Pad::Count1, 0),
		},

		{
			// (6) Chip Erase --> compare @Command
			Lut::Sequence (Lut::Command::Cmd_Ddr,         Lut::Pad::Count8, 0x60, Lut::Command::Cmd_Ddr,     Lut::Pad::Count8, 0x9F),
		},

		{
			// (7) read Configuration Register 1
			Lut::Sequence (Lut::Command::Cmd_Ddr,         Lut::Pad::Count8, 0x15, Lut::Command::Cmd_Ddr,     Lut::Pad::Count8, 0xEA),
			Lut::Sequence (Lut::Command::RowAddress_Ddr,  Lut::Pad::Count8, 32,   Lut::Command::Dummy_Rwds_Ddr, Lut::Pad::Count8, DummyCycles),
			Lut::Sequence (Lut::Command::Read_Ddr,        Lut::Pad::Count8, 2,    Lut::Command::Stop,        Lut::Pad::Count1, 0),
		},

		{
			// (8) Write Status/Control Registers --> compare @Command
			Lut::Sequence (Lut::Command::Cmd_Ddr,         Lut::Pad::Count8, 0x01, Lut::Command::Write_Sdr,   Lut::Pad::Count8, 0x04),
		},

		{
			// (9) Write Configuration Register 2 --> compare @Command
			Lut::Sequence (Lut::Command::Cmd_Ddr,         Lut::Pad::Count8, 0x72, Lut::Command::Cmd_Ddr,     Lut::Pad::Count8, 0x8D),
			Lut::Sequence (Lut::Command::RowAddress_Ddr,  Lut::Pad::Count8, 32,   Lut::Command::Write_Sdr,   Lut::Pad::Count8, 2),
		},

		{
			// (10) read Configuration Register 2
			Lut::Sequence (Lut::Command::Cmd_Ddr,         Lut::Pad::Count8, 0x71, Lut::Command::Cmd_Ddr,     Lut::Pad::Count8, 0x8E),
			Lut::Sequence (Lut::Command::RowAddress_Ddr,  Lut::Pad::Count8, 32,   Lut::Command::Dummy_Rwds_Ddr, Lut::Pad::Count8, DummyCycles),
			Lut::Sequence (Lut::Command::Read_Ddr,        Lut::Pad::Count8, 2,    Lut::Command::Stop,        Lut::Pad::Count1, 0),
		}
	);
	// LookUp Table for Macronix MX25UM Octa-Flash devices in Octa configuration without DDR
	inline constexpr Lut::Table LUT_OctaSPI = Lut::Make
	(
		{
			// (0) Read Array	- Octa Read --> compare @Command
			Lut::Sequence (Lut::Command::Cmd_Sdr,        Lut::Pad::Count8, 0xEC, Lut::Command::Cmd_Sdr,     Lut::Pad::Count8, 0x13),
			Lut::Sequence (Lut::Command::RowAddress_Sdr, Lut::Pad::Count8, 32,   Lut::Command::Dummy_Sdr,   Lut::Pad::Count8, DummyCycles),
			Lut::Sequence (Lut::Command::Read_Sdr,       Lut::Pad::Count8, 128,  Lut::Command::Stop,        Lut::Pad::Count1, 0),
		},

		{
			// (1) Read Status (byte 1) --> compare @Command
			Lut::Sequence (Lut::Command::Cmd_Sdr,        Lut::Pad::Count8, 0x05, Lut::Command::Cmd_Sdr,     Lut::Pad::Count8, 0xFA),
			Lut::Sequence (Lut::Command::RowAddress_Sdr, Lut::Pad::Count8, 32,   Lut::Command::Dummy_Sdr,   Lut::Pad::Count8, DummyCycles),
			Lut::Sequence (Lut::Command::Read_Sdr,       Lut::Pad::Count8, 1,    Lut::Command::Stop,        Lut::Pad::Count1, 0x0),
		},

		{
			// (2) Read JEDEC ID --> compare @Command
			Lut::Sequence (Lut::Command::Cmd_Sdr,        Lut::Pad::Count8, 0x9F, Lut::Command::Cmd_Sdr,     Lut::Pad::Count8, 0x60),
			Lut::Sequence (Lut::Command::RowAddress_Sdr, Lut::Pad::Count8, 0x20, Lut::Command::Dummy_Sdr,   Lut::Pad::Count8, 0x16),
			Lut::Sequence (Lut::Command::Read_Sdr,       Lut::Pad::Count8, 0x04, Lut::Command::Stop,        Lut::Pad::Count1, 0x0),
		},

		{
			// (3) Write Enable --> compare @Command
			Lut::Sequence (Lut::Command::Cmd_Sdr,        Lut::Pad::Count8, 0x06, Lut::Command::Cmd_Sdr,     Lut::Pad::Count8, 0xF9),
		},

		{
			// (4) Page Program --> compare @Command
			Lut::Sequence (Lut::Command::Cmd_Sdr,        Lut::Pad::Count8, 0x12, Lut::Command::Cmd_Sdr,     Lut::Pad::Count8, 0xED),
			Lut::Sequence (Lut::Command::RowAddress_Sdr, Lut::Pad::Count8, 32,   Lut::Command::Write_Sdr,   Lut::Pad::Count8, 4),
		},

		{
			// (5) Sector Erase 4K --> compare @Command
			Lut::Sequence (Lut::Command::Cmd_Sdr,         Lut::Pad::Count8, 0x21, Lut::Command::Cmd_Sdr,     Lut::Pad::Count8, 0xDE),	// Sector Erase (4K)
			//		Lut::Sequence (Lut::Command::Cmd_Sdr, Lut::Pad::Count8, 0xDC, Lut::Command::Cmd_Sdr,     Lut::Pad::Count8, 0x23),	// Block Erase (64K)
			Lut::Sequence (Lut::Command::RowAddress_Sdr,  Lut::Pad::Count8, 32,   Lut::Command::Stop,        Lut::Pad::Count1, 0),
		},

		{
			// (6) Chip Erase --> compare @Command
			Lut::Sequence (Lut::Command::Cmd_Sdr,         Lut::Pad::Count8, 0x60, Lut::Command::Cmd_Sdr,     Lut::Pad::Count8, 0x9F),
		},

		{
			// (7) read Configuration Register 1
			Lut::Sequence (Lut::Command::Cmd_Sdr,         Lut::Pad::Count8, 0x15, Lut::Command::Cmd_Sdr,     Lut::Pad::Count8, 0xEA),
			Lut::Sequence (Lut::Command::RowAddress_Sdr,  Lut::Pad::Count8, 32,   Lut::Command::Dummy_Sdr,   Lut::Pad::Count8, DummyCycles),
			Lut::Sequence (Lut::Command::Read_Sdr,        Lut::Pad::Count8, 2,    Lut::Command::Stop,        Lut::Pad::Count1, 0),
		},

		{
			// (8) Write Status/Control Registers --> compare @Command
			Lut::Sequence (Lut::Command::Cmd_Sdr,         Lut::Pad::Count1, 0x01, Lut::Command::Write_Sdr,   Lut::Pad::Count8, 0x04),
		},

		{
			// (9) Write Configuration Register 2 --> compare @Command
			Lut::Sequence (Lut::Command::Cmd_Sdr,         Lut::Pad::Count8, 0x72, Lut::Command::Cmd_Sdr,     Lut::Pad::Count8, 0x8D),
			Lut::Sequence (Lut::Command::RowAddress_Sdr,  Lut::Pad::Count8, 32,   Lut::Command::Write_Sdr,   Lut::Pad::Count8, 2),
		},

		{
			// (10) read Configuration Register 2
			Lut::Sequence (Lut::Command::Cmd_Sdr,        Lut::Pad::Count8, 0x71, Lut::Command::Cmd_Sdr,      Lut::Pad::Count8, 0x8E),
			Lut::Sequence (Lut::Command::RowAddress_Sdr, Lut::Pad::Count8, 32,   Lut::Command::Dummy_Sdr,    Lut::Pad::Count8, DummyCycles),
			Lut::Sequence (Lut::Command::Read_Sdr,       Lut::Pad::Count8, 2,    Lut::Command::Stop,         Lut::Pad::Count1, 0),
		}
	);
}

#endif // FLEXSPI_MACRONIX_H_

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

#ifndef FLKEXSPI_ADESTO_H_
#define FLKEXSPI_ADESTO_H_

#include "libmem_driver_FlexSPI.h"
#include "FlexSPI_Helper.h"
#include "LutTranslator.h"
#include "libmem_Tools.h"
#include <array>

namespace Adesto
{
	LibmemStatus_t Initialize (FlexSPI_Helper &flexSPI, MemoryType MemType, DeviceInfo &Info);

	namespace ATXP
	{
		constexpr uint32_t DummyCycles   = 18;	// Number of dummy cycles after Read Command
	
		inline constexpr Lut::Table LUT_OctaSPI = Lut::Make
		(
			{
				// (0) Read Array --> compare @LUT_CommandOffsets
				Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count8, 0x0B,        Lut::Command::RowAddress_Sdr, Lut::Pad::Count8, 32),
				Lut::Sequence (Lut::Command::Dummy_Sdr, Lut::Pad::Count8, DummyCycles, Lut::Command::Read_Sdr,  Lut::Pad::Count8, 128),
				// Lut::Sequence (kFLEXSPI_Command_JUMP_ON_CS,   Lut::Pad::Count8, 0,    Lut::Command::Stop,      Lut::Pad::Count1, 0),
			},

			{
				// (1) Read Status (byte 1) --> compare @LUT_CommandOffsets
				Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count8, 0x05, Lut::Command::Dummy_Sdr, Lut::Pad::Count8, 4),
				Lut::Sequence (Lut::Command::Read_Sdr,  Lut::Pad::Count8, 1,    Lut::Command::Stop,      Lut::Pad::Count1, 0),
			},

			{ 0U }, // (2) free

			{
				// (3) Write Enable --> compare @LUT_CommandOffsets
				Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count8, 0x06,  Lut::Command::Stop,      Lut::Pad::Count1, 0),
			},

			{
				// (4) Page Program --> compare @LUT_CommandOffsets
				Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count8, 0x02,  Lut::Command::RowAddress_Sdr, Lut::Pad::Count8, 32),
				Lut::Sequence (Lut::Command::Write_Sdr, Lut::Pad::Count8, 128,   Lut::Command::Stop,           Lut::Pad::Count1, 0),
			},

			{
				// (5) Block Erase 4K --> compare @LUT_CommandOffsets
				Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count8, 0x20,  Lut::Command::RowAddress_Sdr, Lut::Pad::Count8, 32),
			},

			{
				// (6) Chip Erase --> compare @LUT_CommandOffsets
				Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count8, 0x60,  Lut::Command::Stop,      Lut::Pad::Count1, 0),
			},

			{
				// (7) Return to Standard SPI Mode --> compare @LUT_CommandOffsets
				Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count8, 0xFF,  Lut::Command::Stop,      Lut::Pad::Count1, 0),
			},

			{
				// (8) Write Status/Control Registers --> compare @LUT_CommandOffsets
				Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count8, 0x71,  Lut::Command::RowAddress_Sdr, Lut::Pad::Count8, 8),
				Lut::Sequence (Lut::Command::Write_Sdr, Lut::Pad::Count8, 1,     Lut::Command::Stop,           Lut::Pad::Count1, 0),
			},

			{
				// (9) Block Erase 32K --> compare @LUT_CommandOffsets
				Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count8, 0x52,  Lut::Command::RowAddress_Sdr, Lut::Pad::Count8, 32),
			},

			{
				// (10) Block Erase 64K --> compare @LUT_CommandOffsets
				Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count8, 0xD8,  Lut::Command::RowAddress_Sdr, Lut::Pad::Count8, 32),
			}
		);

		inline constexpr Lut::Table LUT_QuadSPI_DDR = Lut::Make
		(
			{
				// (0) Read Array --> compare @LUT_CommandOffsets
				Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count4, 0x0B,            Lut::Command::RowAddress_Ddr, Lut::Pad::Count4, 32),
				Lut::Sequence (Lut::Command::Dummy_Ddr, Lut::Pad::Count4, DummyCycles*2+1, Lut::Command::Read_Ddr,       Lut::Pad::Count4, 128),
				//	Lut::Sequence (kFLEXSPI_Command_JUMP_ON_CS,  Lut::Pad::Count4, 0,     Lut::Command::Stop,      Lut::Pad::Count1, 0),
			},

			{
				// (1) Read Status (byte 1) --> compare @LUT_CommandOffsets
				Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count4, 0x05,  Lut::Command::Dummy_Ddr, Lut::Pad::Count4, 9),
				Lut::Sequence (Lut::Command::Read_Ddr,  Lut::Pad::Count4, 1,     Lut::Command::Stop,      Lut::Pad::Count1, 0),
			},

			{ 0U }, // (2) free

			{
				// (3) Write Enable --> compare @LUT_CommandOffsets
				Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count4, 0x06,  Lut::Command::Stop,      Lut::Pad::Count1, 0),
			},

			{
				// (4) Page Program --> compare @LUT_CommandOffsets
				Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count4, 0x02,  Lut::Command::RowAddress_Ddr, Lut::Pad::Count4, 32),
				Lut::Sequence (Lut::Command::Write_Ddr, Lut::Pad::Count4, 128,   Lut::Command::Stop,      Lut::Pad::Count1, 0),
			},

			{
				// (5) Block Erase 4K --> compare @LUT_CommandOffsets
				Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count4, 0x20,  Lut::Command::RowAddress_Ddr, Lut::Pad::Count4, 32),
			},

			{
				// (6) Chip Erase --> compare @LUT_CommandOffsets
				Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count4, 0x60,  Lut::Command::Stop,      Lut::Pad::Count1, 0),
			},

			{
				// (7) Return to Standard SPI Mode --> compare @LUT_CommandOffsets
				Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count4, 0xFF,  Lut::Command::Stop,      Lut::Pad::Count1, 0),
			},

			{
				// (8) Write Status/Control Registers --> compare @LUT_CommandOffsets
				Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count4, 0x71,  Lut::Command::RowAddress_Ddr, Lut::Pad::Count4, 8),
				Lut::Sequence (Lut::Command::Write_Ddr, Lut::Pad::Count4, 1,     Lut::Command::Stop,      Lut::Pad::Count1, 0),
			},

			{
				// (9) Block Erase 32K --> compare @LUT_CommandOffsets
				Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count4, 0x52,  Lut::Command::RowAddress_Ddr, Lut::Pad::Count4, 32),
			},

			{
				// (10) Block Erase 64K --> compare @LUT_CommandOffsets
				Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count4, 0xD8,  Lut::Command::RowAddress_Ddr, Lut::Pad::Count4, 32),
			}
		);


		inline constexpr Lut::Table LUT_OctaSPI_DDR = Lut::Make
		(
			{
				// (0) Read Array --> compare @LUT_CommandOffsets
				Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count8, 0x0B, Lut::Command::RowAddress_Ddr, Lut::Pad::Count8, 32),
				Lut::Sequence (Lut::Command::Dummy_Ddr, Lut::Pad::Count8, (DummyCycles*2+1), Lut::Command::Read_Ddr,  Lut::Pad::Count8, 128),
				//	Lut::Sequence (kFLEXSPI_Command_JUMP_ON_CS,  Lut::Pad::Count8, 0,    Lut::Command::Stop,      Lut::Pad::Count1, 0),
			},

			{
				// (1) Read Status (byte 1) --> compare @LUT_CommandOffsets
				Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count8, 0x05, Lut::Command::Dummy_Ddr, Lut::Pad::Count8, 8),
				Lut::Sequence (Lut::Command::Read_Ddr,  Lut::Pad::Count8, 1,    Lut::Command::Stop,      Lut::Pad::Count1, 0),
			},

			{ 0U }, // (2) free

			{
				// (3) Write Enable --> compare @LUT_CommandOffsets
				Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count8, 0x06, Lut::Command::Stop,      Lut::Pad::Count1, 0),
			},

			{
				// (4) Page Program --> compare @LUT_CommandOffsets
				Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count8, 0x02, Lut::Command::RowAddress_Ddr, Lut::Pad::Count8, 32),
				Lut::Sequence (Lut::Command::Write_Ddr, Lut::Pad::Count8, 128,  Lut::Command::Stop,      Lut::Pad::Count1, 0),
			},

			{
				// (5) Block Erase 4K --> compare @LUT_CommandOffsets
				Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count8, 0x20, Lut::Command::RowAddress_Ddr, Lut::Pad::Count8, 32),
			},

			{
				// (6) Chip Erase --> compare @LUT_CommandOffsets
				Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count8, 0x60, Lut::Command::Stop,      Lut::Pad::Count1, 0),
			},

			{
				// (7) Return to Standard SPI Mode --> compare @LUT_CommandOffsets
				Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count8, 0xFF, Lut::Command::Stop,      Lut::Pad::Count1, 0),
			},

			{
				// (8) Write Status/Control Registers --> compare @LUT_CommandOffsets
				Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count8, 0x71, Lut::Command::RowAddress_Ddr, Lut::Pad::Count8, 8),
				Lut::Sequence (Lut::Command::Write_Ddr, Lut::Pad::Count8, 1,    Lut::Command::Stop,      Lut::Pad::Count1, 0),
				//	Lut::Sequence (Lut::Command::Cmd_Sdr, Lut::Pad::Count8, 0x71, Lut::Command::Write_Ddr, Lut::Pad::Count8, 4),
			},

			{
				// (9) Block Erase 32K --> compare @LUT_CommandOffsets
				Lut::Sequence (Lut::Command::Cmd_Sdr,    Lut::Pad::Count8, 0x52, Lut::Command::RowAddress_Ddr, Lut::Pad::Count8, 32),
			},

			{
				// (10) Block Erase 64K --> compare @LUT_CommandOffsets
				Lut::Sequence (Lut::Command::Cmd_Sdr,    Lut::Pad::Count8, 0xD8, Lut::Command::RowAddress_Ddr, Lut::Pad::Count8, 32),
			}
		);
	
		inline constexpr Lut::Table LUT_QuadSPI = Lut::Make
		(
			{
				// (0) Read Array --> compare @LUT_CommandOffsets
				Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count4, 0x0B, Lut::Command::RowAddress_Sdr, Lut::Pad::Count4, 32),
				Lut::Sequence (Lut::Command::Dummy_Sdr, Lut::Pad::Count4, DummyCycles, Lut::Command::Read_Sdr,  Lut::Pad::Count4, 128),
				//	Lut::Sequence (kFLEXSPI_Command_JUMP_ON_CS,  Lut::Pad::Count4, 0,
			},

			{
				// (1) Read Status (byte 1) --> compare @LUT_CommandOffsets
				Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count4, 0x05, Lut::Command::Dummy_Sdr, Lut::Pad::Count4, 4),
				Lut::Sequence (Lut::Command::Read_Sdr,  Lut::Pad::Count4, 0x01, Lut::Command::Stop,      Lut::Pad::Count1, 0),
				//	Lut::Sequence (Lut::Command::Cmd_Sdr, Lut::Pad::Count1, 0x05, Lut::Command::Read_Sdr,  Lut::Pad::Count1, 4),
			},

			{ 0U }, // (2) free

			{
				// (3) Write Enable --> compare @LUT_CommandOffsets
				Lut::Sequence (Lut::Command::Cmd_Sdr,    Lut::Pad::Count4, 0x06, Lut::Command::Stop,      Lut::Pad::Count1, 0),
			},

			{
				// (4) Page Program --> compare @LUT_CommandOffsets
				Lut::Sequence (Lut::Command::Cmd_Sdr,    Lut::Pad::Count4, 0x02, Lut::Command::RowAddress_Sdr, Lut::Pad::Count4, 32),
				Lut::Sequence (Lut::Command::Write_Sdr,  Lut::Pad::Count4, 128,  Lut::Command::Stop,      Lut::Pad::Count1, 0),
			},

			{
				// (5) Block Erase 4K --> compare @LUT_CommandOffsets
				Lut::Sequence (Lut::Command::Cmd_Sdr,    Lut::Pad::Count4, 0x20, Lut::Command::RowAddress_Sdr, Lut::Pad::Count4, 32),
			},

			{
				// (6) Chip Erase --> compare @LUT_CommandOffsets
				Lut::Sequence (Lut::Command::Cmd_Sdr,    Lut::Pad::Count4, 0x60, Lut::Command::Stop,      Lut::Pad::Count1, 0),
			},

			{
				// (7) Return to Standard SPI Mode --> compare @LUT_CommandOffsets
				Lut::Sequence (Lut::Command::Cmd_Sdr,    Lut::Pad::Count4, 0xFF, Lut::Command::Stop,      Lut::Pad::Count1, 0),
			},

			{
				// (8) Write Status/Control Registers --> compare @LUT_CommandOffsets
				Lut::Sequence (Lut::Command::Cmd_Sdr,    Lut::Pad::Count4, 0x71, Lut::Command::RowAddress_Sdr, Lut::Pad::Count4, 8),
				Lut::Sequence (Lut::Command::Write_Sdr,  Lut::Pad::Count4, 1,    Lut::Command::Stop,      Lut::Pad::Count1, 0),
			}
		);
	}


	namespace AT25
	{
		enum class Command
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
		};

		inline constexpr Lut::Table LUT_QuadSPI = Lut::Make
		(
			{
				// (0) Read Array --> compare @AT25::Command
				Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count1, 0xEB, Lut::Command::RowAddress_Sdr, Lut::Pad::Count4, 0x18),
				Lut::Sequence (Lut::Command::Dummy_Sdr, Lut::Pad::Count4, 0x06, Lut::Command::Read_Sdr,  Lut::Pad::Count4, 0x04),
			},

			{
				// (1) Read Status --> compare @AT25::Command
				Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count1, 0x05, Lut::Command::Read_Sdr,  Lut::Pad::Count1, 0x04),
			},

			{
				// (2) Read JEDEC-ID --> compare @AT25::Command
				Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count1, 0x9F, Lut::Command::Read_Sdr,  Lut::Pad::Count1, 24),
				Lut::Sequence (Lut::Command::Stop,      Lut::Pad::Count1, 0,    Lut::Command::Stop,      Lut::Pad::Count1, 0),
			},

			{
				// (3) Write Enable --> compare @AT25::Command
				Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count1, 0x06, Lut::Command::Stop,      Lut::Pad::Count1, 0),
			},

			{
				// (4) Page Program --> compare @AT25::Command
				Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count1, 0x32, Lut::Command::RowAddress_Sdr, Lut::Pad::Count1, 0x18),
				Lut::Sequence (Lut::Command::Write_Sdr, Lut::Pad::Count4, 0x04, Lut::Command::Stop,      Lut::Pad::Count1, 0),
			},

			{
				// (5) Sector Erase 4K --> compare @AT25::Command
				Lut::Sequence (Lut::Command::Cmd_Sdr,  Lut::Pad::Count1, 0x20, Lut::Command::RowAddress_Sdr, Lut::Pad::Count1, 0x18),
			},

			{
				// (6) Chip Erase --> compare @AT25::Command
				Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count1, 0xC7, Lut::Command::Stop,      Lut::Pad::Count1, 0),
			},

			{
				// (7) Return to Standard SPI Mode --> compare @AT25::Command
				Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count4, 0xF5, Lut::Command::Stop,      Lut::Pad::Count1, 0),
			},

			{
				// (8) Write Status/Control Registers --> compare @AT25::Command
				Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count1, 0x01, Lut::Command::Write_Sdr, Lut::Pad::Count1, 0x04),
			},

			{
				// (9) Enter QPI mode --> compare @AT25::Command
				Lut::Sequence (Lut::Command::Cmd_Sdr,   Lut::Pad::Count1, 0x31, Lut::Command::Stop,      Lut::Pad::Count1, 0),
			}
		);
	}
}


#endif // FLKEXSPI_ADESTO_H_
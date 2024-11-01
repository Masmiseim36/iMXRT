/** Loader for iMXRT-Family
Copyright (C) 2026-2026 Markus Klein
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

#ifndef LUT_TRANSLATOR_H_
#define LUT_TRANSLATOR_H_

#include <cstdint>
#include <array>

#include "fsl_device_registers.h"
#if defined FLEXSPI || defined FLEXSPI0 || defined FLEXSPI1
	#include "fsl_flexspi.h"
#endif
#if defined XSPI0 || defined XSPI1 || defined XSPI2
	#include "fsl_xspi.h"
#endif


namespace Lut
{
	#if defined FLEXSPI || defined FLEXSPI0 || defined FLEXSPI1
		inline constexpr size_t Entries = sizeof(FLEXSPI_Type::LUT) / sizeof (FLEXSPI_Type::LUT[0]);
		inline constexpr size_t BlockSize {4};
		inline constexpr size_t BlockCount {Entries / BlockSize};

		/*! Table:
		\brief Type for the LUT information used for different Flash types */
		using Table = std::array <uint32_t, Entries>;

		enum class Command : uint8_t
		{
			Stop				= kFLEXSPI_Command_STOP,
			Cmd_Sdr				= kFLEXSPI_Command_SDR,
			RowAddress_Sdr		= kFLEXSPI_Command_RADDR_SDR,
			ColumnAddress_Sdr	= kFLEXSPI_Command_CADDR_SDR,
			Mode1_Sdr			= kFLEXSPI_Command_MODE1_SDR,
			Mode2_Sdr			= kFLEXSPI_Command_MODE2_SDR,
			Mode4_Sdr			= kFLEXSPI_Command_MODE4_SDR,
			Mode8_Sdr			= kFLEXSPI_Command_MODE8_SDR,
			Write_Sdr			= kFLEXSPI_Command_WRITE_SDR,
			Read_Sdr			= kFLEXSPI_Command_READ_SDR,
			Learn_Sdr			= kFLEXSPI_Command_LEARN_SDR,
			DataSize_Sdr		= kFLEXSPI_Command_DATSZ_SDR,
			Dummy_Sdr			= kFLEXSPI_Command_DUMMY_SDR,
			Dummy_Rwds_Sdr		= kFLEXSPI_Command_DUMMY_RWDS_SDR,

			Cmd_Ddr				= kFLEXSPI_Command_DDR,
			RowAddress_Ddr		= kFLEXSPI_Command_RADDR_DDR,
			ColumnAddress_Ddr	= kFLEXSPI_Command_CADDR_DDR,
			Mode1_Ddr			= kFLEXSPI_Command_MODE1_DDR,
			Mode2_Ddr			= kFLEXSPI_Command_MODE2_DDR,
			Mode4_Ddr			= kFLEXSPI_Command_MODE4_DDR,
			Mode8_Ddr			= kFLEXSPI_Command_MODE8_DDR,
			Write_Ddr			= kFLEXSPI_Command_WRITE_DDR,
			Read_Ddr			= kFLEXSPI_Command_READ_DDR,
			Learn_Ddr			= kFLEXSPI_Command_LEARN_DDR,
			DataSize_Ddr		= kFLEXSPI_Command_DATSZ_DDR,
			Dummy_Ddr			= kFLEXSPI_Command_DUMMY_DDR,
			Dummy_Rwds_Ddr		= kFLEXSPI_Command_DUMMY_RWDS_DDR,
			JumpOnCs			= kFLEXSPI_Command_JUMP_ON_CS,
		};

		enum class Pad : uint8_t
		{
			Count1	= kFLEXSPI_1PAD,
			Count2	= kFLEXSPI_2PAD,
			Count4	= kFLEXSPI_4PAD,
			Count8	= kFLEXSPI_8PAD,
		};

		constexpr uint32_t Sequence (Command cmd0, Pad pad0, uint16_t op0, Command cmd1, Pad pad1, uint16_t op1)
		{
			return FLEXSPI_LUT_SEQ (static_cast<int>(cmd0), static_cast<int>(pad0), op0,
									static_cast<int>(cmd1), static_cast<int>(pad1), op1);
		}
	#endif

	#if defined XSPI0 || defined XSPI1 || defined XSPI2
		inline constexpr size_t Entries = XSPI_LUT_COUNT;
		inline constexpr size_t BlockSize {5};
		inline constexpr size_t BlockCount {Entries / BlockSize};

		/*! Table:
		\brief Type for the LUT information used for different Flash types */
		using Table = std::array <uint32_t, Entries>;

		enum class Command : uint8_t
		{
			Stop				= kXSPI_Command_STOP,
			Cmd_Sdr				= kXSPI_Command_SDR,
			RowAddress_Sdr		= kXSPI_Command_RADDR_SDR,
			Dummy_Sdr			= kXSPI_Command_DUMMY_SDR,
			Mode_Sdr			= kXSPI_Command_MODE_SDR,
			Mode2_Sdr			= kXSPI_Command_MODE2_SDR,
			Mode4_Sdr			= kXSPI_Command_MODE4_SDR,
			Read_Sdr			= kXSPI_Command_READ_SDR,
			Write_Sdr			= kXSPI_Command_WRITE_SDR,
			JumpOnCs			= kXSPI_Command_JMP_ON_CS,

			RowAddress_Ddr		= kXSPI_Command_RADDR_DDR,
			Mode_Ddr			= kXSPI_Command_MODE_DDR,
			Mode2_Ddr			= kXSPI_Command_MODE2_DDR,
			Mode4_Ddr			= kXSPI_Command_MODE4_DDR,
			Read_Ddr			= kXSPI_Command_READ_DDR,
			Write_Ddr			= kXSPI_Command_WRITE_DDR,
			Learn_Ddr			= kXSPI_Command_LEARN_DDR,
			Cmd_Ddr				= kXSPI_Command_DDR,
			ColumnAddress_Sdr	= kXSPI_Command_CADDR_SDR,
			ColumnAddress_Ddr	= kXSPI_Command_CADDR_DDR,
			JumpToSequence		= kXSPI_Command_JUMP_TO_SEQ,
// ToDo: What do we do here? Is this correct?
			Dummy_Ddr      = 0xFF,
			Dummy_Rwds_Ddr = 0xFE,
		};

		enum class Pad : uint8_t
		{
			Count1	= kXSPI_1PAD,
			Count2	= kXSPI_2PAD,
			Count4	= kXSPI_4PAD,
			Count8	= kXSPI_8PAD,
		};

		constexpr uint32_t Sequence (Command cmd0, Pad pad0, uint16_t op0, Command cmd1, Pad pad1, uint16_t op1)
		{
			if (cmd0 == Command::Dummy_Ddr || cmd0 == Command::Dummy_Rwds_Ddr)
			{
				cmd0 = Command::Dummy_Sdr;
				op0 /= 2;
			}
			if (cmd1 == Command::Dummy_Ddr || cmd1 == Command::Dummy_Rwds_Ddr)
			{
				cmd1 = Command::Dummy_Sdr;
				op1 /= 2;
			}
			return XSPI_LUT_SEQ (static_cast<int>(cmd0), static_cast<int>(pad0), op0,
								 static_cast<int>(cmd1), static_cast<int>(pad1), op1);
		}
	#endif


	namespace Detail
	{
		template<std::size_t N>
		constexpr void AppendBlock (Table& result, size_t block, const uint32_t (&a)[N])
		{
			size_t pos {block * BlockSize};
			for (size_t i {0}; i < N; ++i)
				result[pos++] = a[i];
		}
	}

	/*! Make:
	\brief Build a full LUT from per-command blocks.
	Each argument is one command block (brace list of Sequence values).
	Blocks are placed at indices 0, BlockSize, 2*BlockSize, ... Unused entries
	within a block are zero-filled (BlockSize is 4 for FlexSPI, 5 for XSPI/iMXRT700).
	Empty command slots keep their index: pass {0} (or any unused entry). */
	template<std::size_t... Ns>
	constexpr Table Make (const uint32_t (&... arrays)[Ns])
	{
		static_assert (sizeof...(Ns) <= BlockCount, "Too many LUT command blocks.");
		static_assert (((Ns >= 1 && Ns <= BlockSize) && ...), "Each block must have 1..BlockSize entries.");

		Table result{};
		size_t block {0};
		(Detail::AppendBlock (result, block++, arrays), ...);
		return result;
	}
}


#endif // LUT_TRANSLATOR_H_


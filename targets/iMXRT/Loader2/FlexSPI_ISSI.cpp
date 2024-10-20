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

#include "FlexSPI_ISSI.h"
#include "DebugPrint.h"
#include "FlexSPI_Generic.h"

namespace ISSI
{
	// Some Debug Code
	uint32_t CheckRegisters (FlexSPI_Helper &flexSPI)
	{
		uint32_t value[32]{};
		flexspi_transfer_t flashXfer
		{
			0,						// deviceAddress	- Operation device address.
			FlexSPI_Helper::port,	// port				- Operation port
			kFLEXSPI_Read,			// cmdType			- Execution command type.
			static_cast<uint8_t>(Command::ReadConfigNonVolatile),	// seqIndex			- Sequence ID for command.
			1,						// SeqNumber		- Sequence number for command.
			value,					// data				- Data buffer.
			sizeof(value)			// dataSize			- Data size in bytes.
		};
		[[maybe_unused]]status_t stat = FLEXSPI_TransferBlocking (&flexSPI, &flashXfer);

		stat = flexSPI.ReadRegister (0x0, value[0], (LUT_CommandOffsets)Command::ReadConfigVolatile);
		stat = flexSPI.ReadRegister (0x1, value[1], (LUT_CommandOffsets)Command::ReadConfigVolatile);
//		stat = flexSPI.ReadRegister (0x2, value[2], (LUT_CommandOffsets)Command::ReadConfigVolatile);
		stat = flexSPI.ReadRegister (0x3, value[3], (LUT_CommandOffsets)Command::ReadConfigVolatile);
		stat = flexSPI.ReadRegister (0x4, value[4], (LUT_CommandOffsets)Command::ReadConfigVolatile);
		stat = flexSPI.ReadRegister (0x5, value[5], (LUT_CommandOffsets)Command::ReadConfigVolatile);
		stat = flexSPI.ReadRegister (0x6, value[6], (LUT_CommandOffsets)Command::ReadConfigVolatile);
		stat = flexSPI.ReadRegister (0x7, value[7], (LUT_CommandOffsets)Command::ReadConfigVolatile);
		stat = flexSPI.ReadRegister (0xA, value[8], (LUT_CommandOffsets)Command::ReadConfigVolatile);
		stat = flexSPI.ReadRegister (0xB, value[9], (LUT_CommandOffsets)Command::ReadConfigVolatile);

		return value[0];
	}


	static MemoryType tryDetectMemoryType = MemType_Invalid;
	status_t TryDetect  (FlexSPI_Helper &flexSPI, DeviceInfo &info)
	{
		flexSPI.UpdateLUT (LUT_ReadJEDEC_ID*4, LUT_OctaSPI_DDR, 4);
		status_t status = flexSPI.ReadJEDEC (&info);

		if (status != kStatus_Success)
		{
			// We were able to read the JEDEC ID via octaspi-DDR, so we are in this mode
			tryDetectMemoryType = MemType_OctaSPI_DDR;
		}
		return status;
	}


	// Non Volatile Register
	enum class SPIMode // Address 0
	{
		SPI_DQS      = 0xFF,
		SPI          = 0xDF,	// without DQS
		OctalSPI_DQS = 0xE7,
		OctalSPI     = 0xC7		// without DQS
	};

	LibmemStatus_t Initialize (FlexSPI_Helper &flexSPI, [[maybe_unused]] MemoryType memType, DeviceInfo &info, flexspi_config_t &config, [[maybe_unused]]flexspi_device_config_t &deviceConfig)
	{
		DebugPrint ("Found ISSI (Lucent) Flash\r\n");

		// Memory Type + Capacity
		// IS25LP - 2.30V to 3.60V
		// - 0x60'14 - IS25LP080D
		// - 0x60'15 - IS25LP016D
		// - 0x60'16 - IS25LP032D
		// - 0x60'17 - IS25LP064A - IS25LP064D
		// - 0x60'18 - IS25LP128F - IS25LP128
		// - 0x60'1A - IS25LP512MH
		// - 0x60'1B - IS25LP01G
		// - 0x60'20 - IS25LP512MG
		// - 0x60'21 - IS25LP01GG

		// IS25WP - 1.65V to 1.95V
		// - 0x70'12 - IS25WP020D
		// - 0x70'13 - IS25WP040D
		// - 0x70'14 - IS25WP080D
		// - 0x70'15 - IS25WP016D
		// - 0x70'16 - IS25WP032D - IS25WP032
		// - 0x70'17 - IS25WP064D - IS25WP064A - IS25WP064
		// - 0x70'18 - IS25WP128F - IS25WP128
		// - 0x70'1A - IS25WP512MH
		// - 0x70'1B - IS25WP01G
		// - 0x70'20 - IS25WP512MG
		// - 0x70'21 - IS25WP01GG

		// Octa
		// - 0x5A'16 - IS25LX032
		// - 0x5A'17 - IS25LX064
		// - 0x5A'18 - IS25LX128
		// - 0x5A'19 - IS25LX256
		// - 0x5A'1A - IS25LX512M
		// - 0x5A'1B - IS25LX01G
		// - 0x5A'1C - 
		// - 0x5B'16 - IS25WX032
		// - 0x5B'17 - IS25WX064
		// - 0x5B'18 - IS25WX128
		// - 0x5B'19 - IS25WX256
		// - 0x5B'1A - IS25WX512M
		// - 0x5B'1B - IS25WX01G
		// - 0x5B'1C - 

		if (info.Type == 0x60 || info.Type == 0x70)
		{
			// Quad SPI
			if (info.Capacity <= Capacity_128MBit)
				flexSPI.UpdateLUT (ISSI::LUT_QuadSPI); // 24 Bit addressing
			else
				flexSPI.UpdateLUT (ISSI::LUT_QuadSPI_32Bit); // 32 bit Addressing

			status_t stat = flexSPI.SendCommand (0, static_cast<LUT_CommandOffsets>(Command::EnterQpiMode)); // Enter QuadSPI mode
			if (stat != kStatus_Success)
				return LibmemStaus_Error;

			config.rxSampleClock = kFLEXSPI_ReadSampleClkLoopbackFromDqsPad;
		}
		else if (info.Type == 0x5A || info.Type == 0x5B)
		{
			if (tryDetectMemoryType != MemType_OctaSPI_DDR)
			{
				flexSPI.UpdateLUT (ISSI::LUT_QuadSPI);

				// Switch to OSPI-Mode
				// Write to non volatile control register at address one to set the dummy cycles
				// Compare Table 6.5 Nonvolatile Configuration Register in the datasheet
				status_t stat = flexSPI.WriteEnable (0);	// send write-enable 
				if (stat != kStatus_Success)
					return LibmemStaus_Error;
				stat = flexSPI.WriteRegister (1, DummyCycles_Octa, static_cast<LUT_CommandOffsets>(Command::WriteConfigNonVolatile_SPI));
				if (stat != kStatus_Success)
					return LibmemStaus_Error;

				// Write to non volatile control register to switch to chosen interface-Type
				stat = flexSPI.WriteEnable (0);	// send write-enable 
				if (stat != kStatus_Success)
					return LibmemStaus_Error;
				stat = flexSPI.WriteRegister (0, static_cast<uint32_t>(SPIMode::OctalSPI_DQS), static_cast<LUT_CommandOffsets>(Command::WriteConfigNonVolatile_SPI));
				if (stat != kStatus_Success)
					return LibmemStaus_Error;
			}

			// Octa SPI
			flexSPI.UpdateLUT (ISSI::LUT_OctaSPI_DDR);
			config.rxSampleClock = kFLEXSPI_ReadSampleClkExternalInputFromDqsPad;// To achieve high speeds - always use DQS

//			CheckRegisters (flexSPI);
		}

		return LibmemStaus_Success;
	}
}
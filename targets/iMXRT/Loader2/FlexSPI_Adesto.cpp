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

#include "FlexSPI_Adesto.h"
#include "DebugPrint.h"
#include "FlexSPI_Generic.h"

namespace Adesto
{

	static status_t UnprotectAll (FlexSPI_Helper &flexSPI)
	{
		status_t stat = flexSPI.WriteEnable (0);
		if (stat != kStatus_Success)
			return stat;

		// write 0 to status/control register 1 - this will unprotect all sectors
		stat = flexSPI.WriteRegister (1, 0, LUT_WriteStatusReg_Adesto);
		if (stat != kStatus_Success)
			return stat;

		return flexSPI.WaitBusBusy ();
	}


	
	static_assert (MemType_Invalid     == 0, "Invalid MemType definition for MemType_Invalid");
	static_assert (MemType_Hyperflash  == 1, "Invalid MemType definition for MemType_Hyperflash");
	static_assert (MemType_OctaSPI_DDR == 2, "Invalid MemType definition for MemType_OctaSPI_DDR");
	static_assert (MemType_OctaSPI     == 3, "Invalid MemType definition for MemType_OctaSPI");
	static_assert (MemType_QuadSPI_DDR == 4, "Invalid MemType definition for MemType_QuadSPI_DDR");
	static_assert (MemType_QuadSPI     == 5, "Invalid MemType definition for MemType_QuadSPI");
	static_assert (MemType_SPI         == 6, "Invalid MemType definition for MemType_SPI");

	// Compare Status Register Byte 2 in the Adesto Datasheet - chapter "11.2 Status Register Byte 2"
	static constexpr std::array <uint32_t, 7> StatusReg2
	{
		0x00,	// unused
		0x00,	// unused
		0x88,	// Octa SPI with DDR
		0x08,	// Octa SPI without DDR
		0x84,	// Quad SPI with DDR
		0x04,	// Quad SPI without DDR
		0x00,	// SPI
	};

	static constexpr std::array <const FlexSPI_LUT *, 7> LUT
	{
		nullptr,				// unused
		nullptr,				// unused
		&ATXP::LUT_OctaSPI_DDR,	// Octa SPI with DDR
		&ATXP::LUT_OctaSPI,		// Octa SPI without DDR
		&ATXP::LUT_QuadSPI_DDR,	// Quad SPI with DDR
		&ATXP::LUT_QuadSPI,		// Quad SPI without DDR
		&Generic::LUT_SPI,		// SPI
	};

	LibmemStatus_t InitializeOcta (FlexSPI_Helper &flexSPI, const MemoryType memType, DeviceInfo &info, flexspi_config_t &config, [[maybe_unused]]flexspi_device_config_t &deviceConfig)
	{
		(void)info;
		if (memType == MemType_Invalid || memType == MemType_Hyperflash)
			return LibmemStaus_Error;

		// Unlock all sectors
		UnprotectAll (flexSPI);

		// send write-enable 
		status_t stat = flexSPI.WriteEnable (0);
		if (stat != kStatus_Success)
			return LibmemStaus_Error;

		// Write to status/control register 2 to switch to chosen memory-Type
		static constexpr uint8_t CtrlReg_Byte3Value (((ATXP::DummyCycles - 8U) >> 1U) | 0x10U);
		uint32_t value = 0x00U | (static_cast<uint32_t>(StatusReg2[memType]) << 8U) | (static_cast<uint32_t>(CtrlReg_Byte3Value) << 16U);
		stat = flexSPI.WriteRegister (1, value, LUT_WriteStatusReg_Adesto, 3);
		if (stat != kStatus_Success)
			return LibmemStaus_Error;

		flexSPI.UpdateLUT (*LUT[memType]);

		config.rxSampleClock = kFLEXSPI_ReadSampleClkExternalInputFromDqsPad;// To achieve high speeds - always use DQS

/*		deviceConfig.CSInterval    = 5;
		deviceConfig.CSHoldTime    = 2;
		deviceConfig.CSSetupTime   = 4;
		deviceConfig.dataValidTime = 1;
*/
		return LibmemStaus_Success;
	}

	LibmemStatus_t Initialize (FlexSPI_Helper &flexSPI, const MemoryType memType, DeviceInfo &info, flexspi_config_t &config, [[maybe_unused]]flexspi_device_config_t &deviceConfig)
	{
		DebugPrint ("Found Atmel/Adesto/Renesas Flash\r\n");
		if (info.Type == 0xA8 || info.ManufactureID == ManufactureID_AdestoTechnologies)
		{
			// Adjust JEDEC information
			switch (info.Type & 0x1FU)
			{
				case 0x06:
					info.Capacity = Capacity_16MBit;
					break;
				case 0x07:
					info.Capacity = Capacity_32MBit;
					break;
				case 0x08:	// ATXP064 --> 0xA8
					info.Capacity = Capacity_64MBit;
					break;
				case 0x09:
					info.Capacity = Capacity_128MBit;
					break;
			}
			return InitializeOcta (flexSPI, memType, info, config, deviceConfig);
		}
		else
		{
			// Adjust JEDEC information
			switch (info.Type)
			{
				case 0x87: // AT25SF321B
					info.Capacity = Capacity_32MBit;
					break;
				case 0x88:	// AT25QF641B
					info.Capacity = Capacity_64MBit;
					break;
				case 0x89:	// AT25SF128A
					info.Capacity = Capacity_128MBit;
					break;
			}

			flexSPI.UpdateLUT (AT25::LUT_QuadSPI);
			status_t stat = flexSPI.SendCommand (0, static_cast<LUT_CommandOffsets>(AT25::Command::EnterQpiMode)); // Enter QuadSPI mode
			if (stat != kStatus_Success)
				return LibmemStaus_Error;
		}
		return LibmemStaus_Success;
	}
}
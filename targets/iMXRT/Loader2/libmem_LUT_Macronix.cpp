/** Loader for iMXRT-Family
Copyright (C) 2019-2022 Markus Klein
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

#include "libmem_LUT_Macronix.h"
#include "DebugPrint.h"
#include "libmem_LUT_Generic.h"

namespace Macronix
{
	namespace MX25UW
	{
		enum DummyCycles
		{
			DummyCycles_20 = 0,
			DummyCycles_18,
			DummyCycles_16,
			DummyCycles_14,
			DummyCycles_12,
			DummyCycles_10,
			DummyCycles_8,
			DummyCycles_6
		};

		constexpr DummyCycles GetDummyCycles (void)
		{
			switch (Macronix::DummyCycles)
			{
				case 20:
					return DummyCycles_20;
				case 18:
					return DummyCycles_18;
				case 16:
					return DummyCycles_16;
				case 14:
					return DummyCycles_14;
				case 12:
					return DummyCycles_12;
				case 10:
					return DummyCycles_10;
				case 8:
					return DummyCycles_8;
				case 6:
					return DummyCycles_6;
				default:
					static_assert("Invalid dummy cycle configuration");
			}
		}
	}
	status_t TryDetect  (FlexSPI_Helper &flexSPI, DeviceInfo &Info)
	{
		flexSPI.UpdateLUT (LUT_ReadJEDEC_ID*4, LUT_OctaSPI_DDR, 4);

		uint8_t Identification[16] {0U};

		flexspi_transfer_t flashXfer;
		flashXfer.deviceAddress = 0;
		flashXfer.port          = FlexSPI_Helper::port;
		flashXfer.cmdType       = kFLEXSPI_Read;
		flashXfer.SeqNumber     = 1;
		flashXfer.seqIndex      = LUT_ReadJEDEC_ID;
		flashXfer.data          = (uint32_t *)Identification;
		flashXfer.dataSize      = sizeof(Identification);

		status_t status = FLEXSPI_TransferBlocking (&flexSPI, &flashXfer);
		if (status != kStatus_Success)
			return status;

		if (Identification[0] == 0 || Identification[0] == 0xFF)
			return kStatus_Fail;	// got no ID-Code: No Flash available

		int i=0;
		for (; i<8; i++)
		{
			if (Identification[i] != ManufactureID_NEXT_MARKER)
				break;
		}

		Info.ManufactureID = (SerialFlash_ManufactureID)(((i+1)<<8) | Identification[i]);
		Info.Type          = Identification[i+1];
//		Info.Capacity      = (Capacity)Identification[i+3];
		Info.Capacity      = (Capacity)(Identification[i+3] & 0x1F);


		flexSPI.UpdateLUT (LUT_ReadJEDEC_ID*4, Generic::LUT_SPI, 4);

		return status;
	}

	LibmemStatus_t Initialize (FlexSPI_Helper &flexSPI, MemoryType MemType, DeviceInfo &Info, [[maybe_unused]] flexspi_config_t &config,  [[maybe_unused]]flexspi_device_config_t &DeviceConfig)
	{
		(void)Info;
		if (MemType == MemType_Invalid || MemType == MemType_Hyperflash)
			return LibmemStaus_Error;

		DebugPrint ("Found Macronix Flash\r\n");
		if (MemType == MemType_SPI)
		{
			flexSPI.UpdateLUT (0, LUT_SPI);
		}
		else if (MemType == MemType_QuadSPI)
		{
			uint32_t StateReg{};
			status_t stat = flexSPI.ReadStatusRegister (0, StateReg);
			if (stat != kStatus_Success)
				return LibmemStaus_Error;

			enum StatusFlags
			{
				WriteInProgress = 1U << 0U,
				WriteEnable     = 1U << 1U,
				QuadEnable      = 1U << 6U
			};

			if ((StateReg & QuadEnable) == 0)
			{
				StateReg |= QuadEnable;	// Enable Quad Mode (Status Register)
	
				stat = flexSPI.WriteEnable (0);
				if (stat != kStatus_Success)
					return LibmemStaus_Error;

				// Write to status register 1 / control register to enable QuadSPI
				stat = flexSPI.WriteRegister (0U, StateReg, LUT_WriteStatusReg, 2);
				if (stat != kStatus_Success)
					return LibmemStaus_Error;

				flexSPI.UpdateLUT (0, LUT_QuadSPI);
			}
		}
		else
		{
			uint8_t Status = 0;
			const FlexSPI_LUT *lut = nullptr;
			switch (MemType)
			{
				case MemType_OctaSPI_DDR:
					// Enter Octal-Mode with DDR.
					// DDR is not working. Fall back to SDR
					Status = 2;
					lut = &LUT_OctaSPI_DDR;
					break;
				case MemType_OctaSPI:
					Status = 0x1U;
					lut = &LUT_OctaSPI;
					break;
				default:
					return LibmemStaus_Error;
			}

			// Switch to OSPI-Mode
			// Write to status/control register 2 to set Dummy Cycles
			// Compare "Dummy Cycle and Frequency Table (MHz)" in the Datasheet
			status_t stat = flexSPI.WriteEnable (0);	// send write-enable 
			if (stat != kStatus_Success)
				return LibmemStaus_Error;
			stat = flexSPI.WriteRegister (0x0300U, MX25UW::GetDummyCycles(), LUT_WriteConfigReg_Macronix);
			if (stat != kStatus_Success)
				return LibmemStaus_Error;

			// Write to status/control register 2 to switch to chosen interface-Type
			stat = flexSPI.WriteEnable (0);	// send write-enable 
			if (stat != kStatus_Success)
				return LibmemStaus_Error;
			stat = flexSPI.WriteRegister (0x0U, Status, LUT_WriteConfigReg_Macronix);
			if (stat != kStatus_Success)
				return LibmemStaus_Error;

			flexSPI.UpdateLUT (0, *lut);

//			config.ahbConfig.enableReadAddressOpt = false;
			config.rxSampleClock = kFLEXSPI_ReadSampleClkExternalInputFromDqsPad;// To achieve high speeds - always use DQS
		}

		return LibmemStaus_Success;
	}
}
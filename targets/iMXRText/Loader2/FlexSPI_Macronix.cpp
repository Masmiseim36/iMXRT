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

#include "FlexSPI_Macronix.h"
#include "DebugPrint.h"
#include "FlexSPI_Generic.h"

namespace Macronix
{
	namespace MX25UW // also correct for MX25UM
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
	} // namespace MX25UW


	static MemoryType tryDetectMemoryType = MemType_Invalid;
	status_t TryDetect (FlexSPI_Helper &flexSPI, DeviceInfo &info)
	{
		flexSPI.UpdateLUT (LUT_ReadJEDEC_ID*4, LUT_OctaSPI_DDR, 4);
/*		if (kStatus_Success == flexSPI.ReadJEDEC (&info))
		{
			// We were able to read the JEDEC ID via OctaSpi-DDR, so we are in tis mode
			tryDetectMemoryType = MemType_OctaSPI_DDR;
//			info.Capacity = static_cast<Capacity>(info.Capacity & 0x1F);
			flexSPI.UpdateLUT (LUT_ReadJEDEC_ID*4, Generic::LUT_SPI, 4);
			return kStatus_Success;
		}*/

		// SDR Mode for data-transfer only in DDR Mode seems not to work, therefore we need to do it this way:
		uint8_t identification[16] {0U};
		if (kStatus_Success == flexSPI.Read (0, reinterpret_cast<uint32_t *>(identification), sizeof(identification), LUT_ReadJEDEC_ID))
		{
			if (identification[0] != 0 && identification[0] != 0xFF) // Sanity check of the data, first byte must not be zero or 0xFF
			{
				// We were able to read the JEDEC ID via OctaSpi-DDR, so we are in this mode
				tryDetectMemoryType = MemType_OctaSPI_DDR;
				int i=0;
				for (; i<8; i++)
				{
					if (identification[i] != ManufactureID_NEXT_MARKER)
						break;
				}
				info.ManufactureID = (SerialFlash_ManufactureID)(((i+1)<<8) | identification[i]);
				info.Type          = identification[i+1];
				info.Capacity      = (Capacity)(identification[i+3] & 0x1F);
				return kStatus_Success; 
			}
		}

		// DDR mode didn't work, try SDR mode
		flexSPI.UpdateLUT (LUT_ReadJEDEC_ID*4, LUT_OctaSPI, 4);
		if (kStatus_Success == flexSPI.ReadJEDEC (&info))
		{
			// We were able to read the JEDEC ID via OctaSpi (none DDR), so we are in tis mode
			tryDetectMemoryType = MemType_OctaSPI;
//			info.Capacity = static_cast<Capacity>(info.Capacity & 0x1F);
			flexSPI.UpdateLUT (LUT_ReadJEDEC_ID*4, Generic::LUT_SPI, 4); 
			return kStatus_Success;
		}

		return kStatus_Fail;
	}

	// Some Debug Code
	uint32_t CheckRegisters (FlexSPI_Helper &flexSPI)
	{
		uint32_t value[32];
		flexspi_transfer_t flashXfer
		{
			0,							// deviceAddress	- Operation device address.
			FlexSPI_Helper::port,		// port				- Operation port
			kFLEXSPI_Read,				// cmdType			- Execution command type.
			static_cast<uint8_t>(10),	// seqIndex			- Sequence ID for command.
			1,							// SeqNumber		- Sequence number for command.
			value,						// data				- Data buffer.
			sizeof(value)				// dataSize			- Data size in bytes.
		};
		FLEXSPI_TransferBlocking (&flexSPI, &flashXfer);

		flexSPI.ReadRegister (0x000, value[0], (LUT_CommandOffsets)10);
		flexSPI.ReadRegister (0x200, value[1], (LUT_CommandOffsets)10);
		flexSPI.ReadRegister (0x300, value[2], (LUT_CommandOffsets)10);
		flexSPI.ReadRegister (0x400, value[3], (LUT_CommandOffsets)10);
		flexSPI.ReadRegister (0x500, value[4], (LUT_CommandOffsets)10);
		flexSPI.ReadRegister (0x800, value[5], (LUT_CommandOffsets)10);
		flexSPI.ReadRegister (0xC00, value[6], (LUT_CommandOffsets)10);
		flexSPI.ReadRegister (0xD00, value[7], (LUT_CommandOffsets)10);
		flexSPI.ReadRegister (0xE00, value[8], (LUT_CommandOffsets)10);
		flexSPI.ReadRegister (0xF00, value[9], (LUT_CommandOffsets)10);

		return value[0];
	}

	LibmemStatus_t Initialize (FlexSPI_Helper &flexSPI, MemoryType memType, DeviceInfo &info, [[maybe_unused]] flexspi_config_t &config,  [[maybe_unused]]flexspi_device_config_t &deviceConfig)
	{
		if (memType == MemType_Invalid || memType == MemType_Hyperflash)
			return LibmemStaus_Error;

		DebugPrint ("Found Macronix Flash\r\n");

		// Adjust JEDEC information
		info.Capacity = static_cast<Capacity>(info.Capacity & 0x1F);

		if (memType == MemType_SPI)
		{
			flexSPI.UpdateLUT (0, LUT_SPI);
		}
		else if (memType == MemType_QuadSPI)
		{
			flexSPI.UpdateLUT (LUT_SPI);

			// read and check the status register
			uint32_t stateReg{};
			status_t status = flexSPI.ReadStatusRegister (0, stateReg);
			if (status != kStatus_Success)
				return LibmemStaus_Error;

			enum StatusFlags
			{
				WriteInProgress = 1U << 0U,
				WriteEnable     = 1U << 1U,
				QuadEnable      = 1U << 6U
			};

			if ((stateReg & QuadEnable) == 0)
			{
				stateReg |= QuadEnable;	// Enable quad-mode

				status = flexSPI.WriteEnable (0);
				if (status != kStatus_Success)
					return LibmemStaus_Error;

				// Write to status register and control register to enable QuadSPI
				status = flexSPI.WriteRegister (0U, stateReg, LUT_WriteStatusReg, 1);
				if (status != kStatus_Success)
					return LibmemStaus_Error;
			}

			// Check the addressing mode
			const FlexSPI_LUT *lut = &LUT_QuadSPI;
			if (info.Capacity > Capacity_128MBit)	// We can not address this with 24-Bit
			{
				status = flexSPI.SendCommand (0, static_cast<LUT_CommandOffsets>(Command::EnterFourByteMode));
				if (status != kStatus_Success)
					return LibmemStaus_Error;
				lut = &LUT_QuadSPI_32Bit;
			}

			// set the new Look-Up-Table
			flexSPI.UpdateLUT (*lut);
		}
		else /* Octa SPI */
		{
			uint8_t stateReg = 0;
			const FlexSPI_LUT *lut = nullptr;
			switch (memType)
			{
				case MemType_OctaSPI_DDR:
					// Enter Octal-Mode with DDR.
					stateReg = 2;
					lut = &LUT_OctaSPI_DDR;
					break;
				case MemType_OctaSPI:
					stateReg = 1;
					lut = &LUT_OctaSPI;
					break;
				default:
					return LibmemStaus_Error;
			}

			switch (tryDetectMemoryType)
			{
				case MemType_OctaSPI_DDR:
					flexSPI.UpdateLUT (0, LUT_OctaSPI_DDR); // Load the Octa-SPI-DDR LUT if we are already in this mode
					break;
				case MemType_OctaSPI:
					flexSPI.UpdateLUT (0, LUT_OctaSPI);     // Load the Octa-SPI LUT if we are already in this mode
					break;
				default:
					flexSPI.UpdateLUT (LUT_SPI);
			}

			// Switch to OSPI-Mode
			// Write to status/control register 2 to set dummy cycles
			// Compare "Dummy Cycle and Frequency Table (MHz)" in the datasheet
			status_t status = flexSPI.WriteEnable (0);	// send write-enable 
			if (status != kStatus_Success)
				return LibmemStaus_Error;
			status = flexSPI.WriteRegister (0x0300U, MX25UW::GetDummyCycles(), static_cast<LUT_CommandOffsets>(Command::WriteConfiguration2));
			if (status != kStatus_Success)
				return LibmemStaus_Error;

			// Write to status/control register 2 to switch to chosen interface-Type
			status = flexSPI.WriteEnable (0);	// send write-enable 
			if (status != kStatus_Success)
				return LibmemStaus_Error;
			status = flexSPI.WriteRegister (0x0U, stateReg, static_cast<LUT_CommandOffsets>(Command::WriteConfiguration2));
			if (status != kStatus_Success)
				return LibmemStaus_Error;

			flexSPI.UpdateLUT (*lut);

//			config.ahbConfig.enableReadAddressOpt = false;
			config.rxSampleClock = kFLEXSPI_ReadSampleClkExternalInputFromDqsPad;// To achieve high speeds - always use DQS
		}

		return LibmemStaus_Success;
	}
} // namespace Macronix
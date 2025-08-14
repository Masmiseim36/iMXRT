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

#include "FlexSPI_Winbond.h"
#include "DebugPrint.h"
#include "FlexSPI_Generic.h"

namespace Winbond
{
	struct StatusRegister1
	{
		uint32_t Busy                  : 1; // Erase / Write in progress
		uint32_t WriteEnableLatch      : 1;
		uint32_t BlockProtectBits      : 4;
		uint32_t TopBottomProtectBit   : 1;
		uint32_t StatusRegisterProtect : 1;
		uint32_t Reserved              :24;

		status_t Read (FlexSPI_Helper &flexSPI)
		{
			return flexSPI.ReadRegister (0, *this, static_cast<LUT_CommandOffsets>(Command::ReadStatus1));
		}

		operator uint32_t& ()
		{
			return reinterpret_cast<uint32_t&>(*this);
		}
	};

	struct StatusRegister2
	{
		uint32_t StatusRegisterProtect1   : 1;
		uint32_t QuadEnable               : 1;
		uint32_t SfdpLockBit              : 1;
		uint32_t SecurityRegisterLockBits : 3;
		uint32_t ComplementProtect        : 1;
		uint32_t SuspendStatus            : 1;
		uint32_t Reserved                 :24;

		status_t Read (FlexSPI_Helper &flexSPI)
		{
			return flexSPI.ReadRegister (0, *this, static_cast<LUT_CommandOffsets>(Command::ReadStatus2));
		}

		status_t Write (FlexSPI_Helper &flexSPI)
		{
			status_t stat = flexSPI.WriteEnable (0); // send write-enable 
			if (stat != kStatus_Success)
				return LibmemStaus_Error;
			return flexSPI.WriteRegister (0, *this, static_cast<LUT_CommandOffsets>(Command::WriteStatus2));
		}

		operator uint32_t& ()
		{
			return reinterpret_cast<uint32_t&>(*this);
		}
	};

	struct StatusRegister3
	{
		uint32_t CurrentAddressMode    : 1;
		uint32_t PowerUpAddressMode    : 1;
		uint32_t WriteProtectSelection : 1;
		uint32_t Reserved1             : 2;
		uint32_t OutputDriverStrength  : 2;
		uint32_t Reserved              :25;

		status_t Read (FlexSPI_Helper &flexSPI)
		{
			return flexSPI.ReadRegister (0, *this, static_cast<LUT_CommandOffsets>(Command::ReadStatus2));
		}

		operator uint32_t& ()
		{
			return reinterpret_cast<uint32_t&>(*this);
		}
	};
	// Memory Type + Capacity
	// W25Q32JV-IQ/JQ		- 0x40'16
	// W25Q32JW-IQ/FW		- 0x60'16
	// W25Q32JV-IM*/JM*		- 0x70'16
	// W25Q32JW-IM			- 0x80'16

	// W25Q64JV-IQ/JQ		- 0x40'17
	// W25Q64JV-IM/JM		- 0x70'17 - 0x80'17

	// W25Q128JV-IN/IQ/JQ	- 0x40'18
	// W25Q128JW-IQ/JQ		- 0x60'18
	// W25Q128JV-IM*/JM*	- 0x70'18
	// W25Q128JW-IM/JM*		- 0x80'18

	// W25Q256JV-IQ/IN/JQ	- 0x40'19
	// W25Q256JW			- 0x60'19
	// W25Q256JV-IM*/JM*	- 0x70'19
	// W25Q256JW-IM			- 0x80'19

	// W25Q512JV-IQ/IN		- 0x40'20
	// W25Q512NW-IQ/IN		- 0x60'20
	// W25Q512NW-IM			- 0x80'20
	
	LibmemStatus_t Initialize (FlexSPI_Helper &flexSPI, MemoryType memType, DeviceInfo &info, [[maybe_unused]] flexspi_config_t &config, [[maybe_unused]]flexspi_device_config_t &deviceConfig)
	{
		if (memType != MemType_QuadSPI && memType != MemType_SPI)
			return LibmemStaus_Error;

		// Adjust JEDEC information
		switch (static_cast<uint32_t>(info.Capacity))
		{
			case 0x16: info.Capacity = Capacity_32MBit;  break;
			case 0x17: info.Capacity = Capacity_64MBit;  break;
			case 0x18: info.Capacity = Capacity_128MBit; break;
			case 0x19: info.Capacity = Capacity_256MBit; break;
			case 0x20: info.Capacity = Capacity_512MBit; break;
			default:
				return LibmemStaus_Error;
		}
		DebugPrintf ("Found Winbond (Nexcom) Flash with %d KByte\r\n", CalculateCapacity_KBytes(info.Capacity));
		if (info.Capacity <= Capacity_128MBit)
			flexSPI.UpdateLUT (Winbond::LUT_QuadSPI_24Bit);
		else
			flexSPI.UpdateLUT (Winbond::LUT_QuadSPI_32Bit);

		StatusRegister1 statReg1;
		status_t stat = statReg1.Read (flexSPI);

		// Check for SPI / QSPI Mode
		StatusRegister2 statReg2;
		stat = statReg2.Read (flexSPI);
		if (stat != kStatus_Success)
			return LibmemStaus_Error;
		if (statReg2.QuadEnable == 0 && memType == MemType_QuadSPI)
		{
			// Enable Quad Mode
			statReg2.QuadEnable = 1;
			stat = statReg2.Write (flexSPI);
			if (stat != kStatus_Success)
				return LibmemStaus_Error;
		}
		else if (statReg2.QuadEnable == 1 && memType == MemType_SPI)
		{
			// Disable Quad Mode
			statReg2.QuadEnable = 0;
			stat = statReg2.Write (flexSPI);
			if (stat != kStatus_Success)
				return LibmemStaus_Error;
		}

		// Check the Address Mode (3-byte or 4-byte)
		StatusRegister3 statReg3;
		stat = statReg3.Read (flexSPI);
		if (info.Capacity > Capacity_128MBit && statReg3.CurrentAddressMode == 0)
		{
			// Enable 4byte mode
			stat = flexSPI.SendCommand (0, static_cast<LUT_CommandOffsets>(Command::Enter4ByteMode));
			if (stat != kStatus_Success)
				return LibmemStaus_Error;
		}
		if (info.Capacity <= Capacity_128MBit && statReg3.CurrentAddressMode == 1)
		{
			// Enable 3 byte mode
			stat = flexSPI.SendCommand (0, static_cast<LUT_CommandOffsets>(Command::Exit4ByteMode));
			if (stat != kStatus_Success)
				return LibmemStaus_Error;
		}
		stat = statReg3.Read (flexSPI);

		return LibmemStaus_Success;
	}
}
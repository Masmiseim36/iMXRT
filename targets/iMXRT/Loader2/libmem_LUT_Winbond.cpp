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

#include "libmem_LUT_Winbond.h"
#include "DebugPrint.h"
#include "libmem_LUT_Generic.h"

namespace Winbond
{
	LibmemStatus_t Initialize (FlexSPI_Helper &flexSPI, [[maybe_unused]] MemoryType memType, DeviceInfo &info, [[maybe_unused]] flexspi_config_t &config, [[maybe_unused]]flexspi_device_config_t &deviceConfig)
	{
		DebugPrint ("Found Winbond (Nexcom) Flash\r\n");
		if (info.Capacity <= Capacity_128MBit)
			flexSPI.UpdateLUT (Winbond::LUT_QuadSPI_24Bit);
		else
			flexSPI.UpdateLUT (Winbond::LUT_QuadSPI_32Bit);

		status_t stat = flexSPI.WriteEnable (0); // send write-enable 
		if (stat != kStatus_Success)
			return LibmemStaus_Error;

		uint32_t value{};
		stat = flexSPI.ReadRegister (0, value, static_cast<LUT_CommandOffsets>(Command::ReadStatus1));
		DebugPrintf ("Winbond status register 1: 0x: %x", value);
		stat = flexSPI.ReadRegister (0, value, static_cast<LUT_CommandOffsets>(Command::ReadStatus2));
		DebugPrintf ("Winbond status register 2: 0x: %x", value);
		stat = flexSPI.ReadRegister (0, value, static_cast<LUT_CommandOffsets>(Command::ReadStatus3));
		DebugPrintf ("Winbond status register 3: 0x: %x", value);
		// Write to status/control register 2 to switch to enter quad-Mode
//		stat = flexSPI->WriteRegister (0, 2, static_cast<LUT_CommandOffsets>(Command::WriteStatus2));
		if (stat != kStatus_Success)
			return LibmemStaus_Error;

		return LibmemStaus_Success;
	}
}
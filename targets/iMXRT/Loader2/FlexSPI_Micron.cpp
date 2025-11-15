/** Loader for iMXRT-Family
Copyright (C) 2019-2025 Markus Klein
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

#include "FlexSPI_Micron.h"
#include "DebugPrint.h"
#include "FlexSPI_Generic.h"

namespace Micron
{
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
		if (info.ManufactureID == ManufactureID_MicronTechnology) // Lucent ==> Issi
			return kStatus_Success; 
		else
			return kStatus_Fail;
	}


	// Non Volatile Register
	enum class SPIMode // Address 0
	{
		SPI_DQS      = 0xFF,
		SPI          = 0xDF,	// without DQS
		OctalSPI_DQS = 0xE7,
		OctalSPI     = 0xC7		// without DQS
	};

	LibmemStatus_t Initialize (FlexSPI_Helper &flexSPI, [[maybe_unused]] MemoryType memType, [[maybe_unused]] DeviceInfo &info, flexspi_config_t &config, [[maybe_unused]]flexspi_device_config_t &deviceConfig)
	{
		DebugPrint ("Found Micron Flash\r\n");

		// Octa SPI
		flexSPI.UpdateLUT (LUT_OctaSPI_DDR);
		config.rxSampleClock = kFLEXSPI_ReadSampleClkExternalInputFromDqsPad;// To achieve high speeds - always use DQS
		return LibmemStaus_Success;
	}
}
/** Loader for iMXRT-Family
Copyright (C) 2019-2021  Markus Klein
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
	LibmemStatus_t Initialize (FlexSPI_Helper &flexSPI, MemoryType MemType, DeviceInfo &Info)
	{
		(void)Info;
		if (MemType == MemType_Invalid || MemType == MemType_Hyperflash)
			return LibmemStaus_Error;

		DebugPrint ("Found Macronix Flash\r\n");
		if (MemType == MemType_QuadSPI)
		{
	/*
			uint8_t StateReg = 0;
			status_t stat = ReadStatusRegister (base, 0, &StateReg);
			if (stat != kStatus_Success)
				return LibmemStaus_Error; */
	/*
			status_t stat = WriteEnable (base, 0);
			if (stat != kStatus_Success)
				return LibmemStaus_Error;

			// Write to status register 1 to enable QuadSPI
			stat = WriteRegister (base, 0U, 0x40U, LUT_WriteConfigReg1_Macronix);
			if (stat != kStatus_Success)
				return LibmemStaus_Error; */
	/*
			stat = ReadStatusRegister (base, 0, &StateReg);
			if (stat != kStatus_Success)
				return LibmemStaus_Error;*/
			flexSPI.UpdateLUT (0, LUT_QuadSPI);
		}
		else
		{
			uint8_t Status = 0;
			const std::array <uint32_t, 64> *lut = nullptr;
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

			// SET Dummy Cycles
			// send write-enable 
			status_t stat = flexSPI.WriteEnable (0);
			if (stat != kStatus_Success)
				return LibmemStaus_Error;
			// Write to status/control register 2 to set the Dummy-Cycles
			stat = flexSPI.WriteRegister (0x300U, 8U, LUT_WriteConfigReg_Macronix);	// 8 --> 12 Cycles at 133 MHz
			if (stat != kStatus_Success)
				return LibmemStaus_Error;
		/*
			// Set Output impedance
			// send write-enable 
			stat = WriteEnable (base, 0);
			if (stat != kStatus_Success)
				return LibmemStaus_Error;
			// Write to status/control register 1 to switch to chosen memory-Type
			stat = WriteRegister (base, 0, 2, LUT_WriteConfigReg1_Macronix);	// 2 --> 52 Ohm. Compare "Output Driver Strength Table in the Reference manual"
			if (stat != kStatus_Success)
				return LibmemStaus_Error;*/

			// Switch to OSPI-Mode
			// send write-enable 
			stat = flexSPI.WriteEnable (0);
			if (stat != kStatus_Success)
				return LibmemStaus_Error;
			// Write to status/control register 2 to switch to chosen memory-Type
			stat = flexSPI.WriteRegister (0x0U, Status, LUT_WriteConfigReg_Macronix);
			if (stat != kStatus_Success)
				return LibmemStaus_Error;

			flexSPI.UpdateLUT (0, *lut);
		}

		return LibmemStaus_Success;
	}
}
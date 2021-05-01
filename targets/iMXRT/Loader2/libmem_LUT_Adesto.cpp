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

#include "libmem_LUT_Adesto.h"
#include "DebugPrint.h"
#include "libmem_LUT_Generic.h"

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
		nullptr,					// unused
		nullptr,					// unused
		&Adesto::LUT_OctaSPI_DDR,	// Octa SPI with DDR
		&Adesto::LUT_OctaSPI,		// Octa SPI without DDR
		&Adesto::LUT_QuadSPI_DDR,	// Quad SPI with DDR
		&Adesto::LUT_QuadSPI,		// Quad SPI without DDR
		&Generic::LUT_SPI,			// SPI
	};

	static constexpr uint8_t CtrlReg_Byte3Value (((DummyCycles - 8U) >> 1U) | 0x10U);

	LibmemStatus_t Initialize (FlexSPI_Helper &flexSPI, const MemoryType MemType, DeviceInfo &Info, flexspi_config_t &config)
	{
		(void)Info;
		if (MemType == MemType_Invalid || MemType == MemType_Hyperflash)
			return LibmemStaus_Error;

		DebugPrint ("Found Adesto Flash\r\n");
		// Unlock all sectors
		UnprotectAll (flexSPI);

		// send write-enable 
		status_t stat = flexSPI.WriteEnable (0);
		if (stat != kStatus_Success)
			return LibmemStaus_Error;

		// Write to status/control register 2 to switch to chosen memory-Type
		uint32_t Value = 0x00U | (static_cast<uint32_t>(StatusReg2[MemType]) << 8U) | (static_cast<uint32_t>(CtrlReg_Byte3Value) << 16U);
		stat = flexSPI.WriteRegister (1, Value, LUT_WriteStatusReg_Adesto, 3);
		if (stat != kStatus_Success)
			return LibmemStaus_Error;

/*		stat = flexSPI.SendCommand (0, LUT_EnterQPI);
		if (stat != kStatus_Success)
			return LibmemStaus_Error;
*/
		flexSPI.UpdateLUT (0, *LUT[MemType]);

		config.rxSampleClock = kFLEXSPI_ReadSampleClkExternalInputFromDqsPad;// To achieve high speeds - always use DQS

		return LibmemStaus_Success;
	}
}
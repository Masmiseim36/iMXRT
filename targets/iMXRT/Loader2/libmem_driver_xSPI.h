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

#ifndef _LIBMEM_DRIVER_XSPI_H_
#define _LIBMEM_DRIVER_XSPI_H_

#include "libmem.h"
#include "libmem_Tools.h"
#include "fsl_device_registers.h"
#include "fsl_flexspi.h"
#include <array>

enum LUT_CommandOffsets
{
	LUT_ReadArray       = 0,
	LUT_ReadStatus      = 1,
	LUT_ReadJEDEC_ID    = 2,
	LUT_WriteEnable     = 3,
	LUT_ProgramPage     = 4,

	LUT_EraseSector     = 5,	// BLOCK_ERASE_4K
	LUT_EraseChip       = 6,

	LUT_EnterQPI        = 7,
	LUT_ReturnSPI       = 7,
	LUT_EnterQPI_Atmel  = 10,
	LUT_EnterQPI_ISSI   = 12,

	LUT_WriteStatusReg_Adesto    = 8,
	LUT_WriteConfigReg_Macronix  = 9,
	LUT_WriteConfigReg_Winbond   = 11,
	LUT_WriteConfigReg1_Macronix = 13,
};


class FlexSPI_Helper: public FLEXSPI_Type
{
public:
	void UpdateLUT (uint32_t index, const FlexSPI_LUT &lut)
	{
		::FLEXSPI_UpdateLUT (this, index, &lut.front(), lut.size ());
	}

	void SoftwareReset (void)
	{
		::FLEXSPI_SoftwareReset (this);
	}

	status_t WriteRegister (uint32_t Address, uint32_t value, enum LUT_CommandOffsets cmd, size_t size  = 1)
	{
		flexspi_transfer_t flashXfer
		{
			Address,					// deviceAddress	- Operation device address.
			kFLEXSPI_PortA1,			// port				- Operation port
			kFLEXSPI_Write,				// cmdType			- Execution command type.
			static_cast<uint8_t>(cmd),	// seqIndex			- Sequence ID for command.
			1,							// SeqNumber		- Sequence number for command.
			&value,						// data				- Data buffer.
			size						// dataSize			- Data size in bytes.
		};
		return FLEXSPI_TransferBlocking (this, &flashXfer);
	}

	status_t ReadRegister (uint32_t Address, uint32_t &value, enum LUT_CommandOffsets cmd)
	{
		flexspi_transfer_t flashXfer
		{
			Address,					// deviceAddress	- Operation device address.
			kFLEXSPI_PortA1,			// port				- Operation port
			kFLEXSPI_Read,		        // cmdType			- Execution command type.
			static_cast<uint8_t>(cmd),	// seqIndex			- Sequence ID for command.
			1,					        // SeqNumber		- Sequence number for command.
			&value,				        // data				- Data buffer.
			1					        // dataSize			- Data size in bytes.
		};
		return FLEXSPI_TransferBlocking (this, &flashXfer);
	}

	status_t SendCommand (uint32_t Address, enum LUT_CommandOffsets cmd)
	{
		flexspi_transfer_t flashXfer
		{
			Address,					// deviceAddress	- Operation device address.
			kFLEXSPI_PortA1,			// port				- Operation port
			kFLEXSPI_Command,	        // cmdType			- Execution command type.
			static_cast<uint8_t>(cmd),	// seqIndex			- Sequence ID for command.
			1,					        // SeqNumber		- Sequence number for command.
			nullptr,			        // data				- Data buffer.
			0					        // dataSize			- Data size in bytes.
		};
		return FLEXSPI_TransferBlocking (this, &flashXfer);
	}

	/*! WriteEnable:
	\brief Send write-enable command
	\param baseAddr The base-address of the command
	\return status_t */
	status_t WriteEnable (uint32_t baseAddr)
	{
		return this->SendCommand (baseAddr, LUT_WriteEnable);
	}

	/*! WaitBusBusy:
	\brief Wait until the Write/erase operation is finished and the Flash is not busy anymore
	\param base The Flex-SPI-base to use
	\return status_t kStatus_Success if the operation was successfully */
	status_t WaitBusBusy (void)
	{
		// Wait status ready.
		bool isBusy;
		uint32_t readValue;
		status_t status;

		do
		{
			status = this->ReadRegister (0, readValue, LUT_ReadStatus);
			if (status != kStatus_Success)
				return status;

			isBusy = (readValue & 1U);
		}
		while (isBusy);

		return status;
	}
};


LibmemStatus_t Libmem_InitializeDriver_xSPI (FLEXSPI_Type *base, enum MemoryType MemType);

#endif	// _LIBMEM_DRIVER_QCTA_SPI_H_

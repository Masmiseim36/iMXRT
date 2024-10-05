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

#ifndef _LIBMEM_DRIVER_XSPI_H_
#define _LIBMEM_DRIVER_XSPI_H_

#include "libmem_Tools.h"
#include "fsl_device_registers.h"
#include "fsl_flexspi.h"

enum LUT_CommandOffsets
{
	LUT_ReadArray       = 0,
	LUT_ReadStatus      = 1,
	LUT_ReadJEDEC_ID    = 2,
	LUT_WriteEnable     = 3,
	LUT_ProgramPage     = 4,

	LUT_EraseSector     = 5,	// BLOCK_ERASE_4K
	LUT_EraseChip       = 6,

	LUT_WriteStatusReg  = 8,

	LUT_WriteStatusReg_Adesto    = 8,
};


class FlexSPI_Helper: public FLEXSPI_Type
{
public:
	void UpdateLUT (const FlexSPI_LUT &lut)
	{
		::FLEXSPI_UpdateLUT (this, 0, &lut.front(), lut.size());
	}
	void UpdateLUT (uint32_t index, const FlexSPI_LUT &lut)
	{
		::FLEXSPI_UpdateLUT (this, index, &lut.front()+index, lut.size()-index);
	}

	void UpdateLUT (uint32_t index, const FlexSPI_LUT &lut, size_t size)
	{
		::FLEXSPI_UpdateLUT (this, index, &lut.front()+index, size);
	}

	void SoftwareReset (void)
	{
		::FLEXSPI_SoftwareReset (this);
	}

	uint32_t GetBaseAddr (void) const
	{
		return reinterpret_cast<uint32_t>(this);
	}
	
	inline uint8_t *GetAliasBaseAddress ()
	{
		#if defined FlexSPI1_ALIAS_BASE && __CORTEX_M == 4
			if (this->GetBaseAddr () == FLEXSPI1_BASE)
				return reinterpret_cast<uint8_t *>(FlexSPI1_ALIAS_BASE);
		#endif
		#if defined FlexSPI1_AMBA_BASE_NS
			if (this->GetBaseAddr () == FLEXSPI1_BASE)
				return reinterpret_cast<uint8_t *>(FlexSPI1_AMBA_BASE_NS);
		#endif
		#if defined FlexSPI2_AMBA_BASE_NS
			if (this->GetBaseAddr () == FLEXSPI2_BASE)
				return reinterpret_cast<uint8_t *>(FlexSPI2_AMBA_BASE_NS);
		#endif

		return nullptr;
	}

	static constexpr flexspi_port_t port =
	#if (defined MIMXRT633S_SERIES) || defined (MIMXRT685S_cm33_SERIES) || \
		 defined(MIMXRT533S_SERIES) || defined (MIMXRT555S_SERIES) || defined(MIMXRT595S_cm33_SERIES)
		kFLEXSPI_PortB1;
	#else
		kFLEXSPI_PortA1;
	#endif

	status_t WriteRegister (uint32_t Address, uint32_t value, LUT_CommandOffsets cmd, size_t size  = 1)
	{
		flexspi_transfer_t flashXfer
		{
			Address,					// deviceAddress	- Operation device address.
			port,						// port				- Operation port
			kFLEXSPI_Write,				// cmdType			- Execution command type.
			static_cast<uint8_t>(cmd),	// seqIndex			- Sequence ID for command.
			1,							// SeqNumber		- Sequence number for command.
			&value,						// data				- Data buffer.
			size						// dataSize			- Data size in bytes.
		};
		return FLEXSPI_TransferBlocking (this, &flashXfer);
	}

	status_t ReadRegister (uint32_t Address, uint32_t &value, LUT_CommandOffsets cmd, size_t size  = 1)
	{
		flexspi_transfer_t flashXfer
		{
			Address,					// deviceAddress	- Operation device address.
			port,						// port				- Operation port
			kFLEXSPI_Read,				// cmdType			- Execution command type.
			static_cast<uint8_t>(cmd),	// seqIndex			- Sequence ID for command.
			1,							// SeqNumber		- Sequence number for command.
			&value,						// data				- Data buffer.
			size						// dataSize			- Data size in bytes.
		};
		return FLEXSPI_TransferBlocking (this, &flashXfer);
	}

	status_t ReadStatusRegister (uint32_t Address, uint32_t &value)
	{
		flexspi_transfer_t flashXfer
		{
			Address,					// deviceAddress	- Operation device address.
			port,						// port				- Operation port
			kFLEXSPI_Read,				// cmdType			- Execution command type.
			LUT_ReadStatus,				// seqIndex			- Sequence ID for command.
			1,							// SeqNumber		- Sequence number for command.
			&value,						// data				- Data buffer.
			1							// dataSize			- Data size in bytes.
		};
		return FLEXSPI_TransferBlocking (this, &flashXfer);
	}

	status_t SendCommand (uint32_t Address, LUT_CommandOffsets cmd, uint8_t sequenceSize = 1)
	{
		flexspi_transfer_t flashXfer
		{
			Address,					// deviceAddress	- Operation device address.
			port,						// port				- Operation port
			kFLEXSPI_Command,			// cmdType			- Execution command type.
			static_cast<uint8_t>(cmd),	// seqIndex			- Sequence ID for command.
			sequenceSize,				// SeqNumber		- Sequence number for command.
			nullptr,					// data				- Data buffer.
			0							// dataSize			- Data size in bytes.
		};
		return FLEXSPI_TransferBlocking (this, &flashXfer);
	}

	/*! ReadJEDEC
	\brief Read the JEDEC Device informations
	\param base The Flex-SPI-base to use
	\param info The read Device informations from the flash memory
	\return status_t Status of the Operation - kStatus_Success when successfully */
	status_t ReadJEDEC (DeviceInfo *info)
	{
		uint8_t Identification[16] {0U};

		flexspi_transfer_t flashXfer
		{
			0,
			FlexSPI_Helper::port,
			kFLEXSPI_Read,
			LUT_ReadJEDEC_ID,
			1,
			(uint32_t *)Identification,
			sizeof(Identification),
		};

		status_t status = FLEXSPI_TransferBlocking (this, &flashXfer);
		if (status != kStatus_Success)
			return status;

		// Sanity check of the data, first byte must not be zero or 0xFF
		if (Identification[0] == 0 || Identification[0] == 0xFF)
			return kStatus_Fail;	// got no ID-Code: No Flash available
	
		// Check if all data are identical
		size_t Index = sizeof(Identification)/sizeof(Identification[0]);
		while (--Index>0 && Identification[0]==Identification[Index])
			;
		if (Index == 0)
			return kStatus_Fail;	// Data is all identical. Got some transfer error


		int i=0;
		for (; i<8; i++)
		{
			if (Identification[i] != ManufactureID_NEXT_MARKER)
				break;
		}

		info->ManufactureID = static_cast<SerialFlash_ManufactureID>(((i+1)<<8U) | Identification[i]);
		info->Type          = Identification[i+1];
		info->Capacity      = static_cast<Capacity>(Identification[i+2]);

		return status;
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
		bool isBusy{false};
		uint32_t readValue{};
		status_t status{};

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

LibmemStatus_t Libmem_InitializeDriver_xSPI (FlexSPI_Helper *base, enum MemoryType MemType);
inline LibmemStatus_t Libmem_InitializeDriver_xSPI (FLEXSPI_Type *base, enum MemoryType MemType)
{
	return Libmem_InitializeDriver_xSPI (static_cast<FlexSPI_Helper *>(base), MemType);
}

#endif	// _LIBMEM_DRIVER_QCTA_SPI_H_

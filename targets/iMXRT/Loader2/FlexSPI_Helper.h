/** Loader for iMXRT-Family
Copyright (C) 2019-2026 Markus Klein
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


#ifndef FLEX_SPI_HELPER_H_
#define FLEX_SPI_HELPER_H_

#include <cstdint>
#include <array>
#include "fsl_device_registers.h"
#if defined FLEXSPI || defined FLEXSPI0 || defined FLEXSPI1
	#include "fsl_flexspi.h"
#endif
#if defined XSPI0 || defined XSPI1 || defined XSPI2
	#include "fsl_xspi.h"
#endif
#include "pin_mux.h"
#include "DebugPrint.h"
#include "libmem_Tools.h"
#include "LutTranslator.h"

/*! MemoryType:
\brief Supported Memory/Interface types */
enum class MemoryType
{
	Invalid     = 0,
	Hyperflash  = 1,
	OctaSPI_DDR = 2,
	OctaSPI     = 3,
	QuadSPI_DDR = 4,
	QuadSPI     = 5,
	SPI         = 6,
	Hyperram    = 7,
};

static const char *MemoryTypeName[]
{
	"Invalid",
	"Hyperflash",
	"OctaSPI-DDR",
	"OctaSPI",
	"QuadSPI-DDR",
	"QuadSPI",
	"SPI",
	"Hyperram"
};

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


#if defined FLEXSPI || defined FLEXSPI0 || defined FLEXSPI1
	using TrasferConfig         = _flexspi_transfer;
	using NativeCommandType     = flexspi_command_type_t;
	using NativeSpiType         = FLEXSPI_Type;
	const auto UpdateLUT        = FLEXSPI_UpdateLUT;
	const auto SoftwareReset    = FLEXSPI_SoftwareReset;
	const auto TransferBlocking = FLEXSPI_TransferBlocking;
	enum class CommandType
	{
		Command = kFLEXSPI_Command,
		Config  = kFLEXSPI_Config,
		Read    = kFLEXSPI_Read,
		Write   = kFLEXSPI_Write
	};
#endif
#if defined XSPI0 || defined XSPI1 || defined XSPI2
	using TrasferConfig         = _xspi_transfer;
	using NativeCommandType     = xspi_command_type_t;
	using NativeSpiType         = XSPI_Type;
	const auto UpdateLUT        = XSPI_UpdateLUT;
	const auto SoftwareReset    = XSPI_SoftwareReset;
	const auto TransferBlocking = XSPI_TransferBlocking;
	enum class CommandType
	{
		Command = kXSPI_Command,
		Config  = kXSPI_Config,
		Read    = kXSPI_Read,
		Write   = kXSPI_Write
	};
#endif



class Transfer : public TrasferConfig
{
public:
	Transfer (uint32_t address, CommandType command, uint8_t seqIdx, [[maybe_unused]]uint8_t seqNum, const uint32_t *dat, size_t dataSz)
	{
		this-> deviceAddress = address;
		#if defined FLEXSPI || defined FLEXSPI0 || defined FLEXSPI1
			this->port      = Transfer::Port ();
			this->SeqNumber = seqNum;
		#endif
		#if defined XSPI0 || defined XSPI1 || defined XSPI2
			// Not quite sure on how to use it. Check later
			this->targetGroup     = kXSPI_TargetGroup0;
			this->lockArbitration = false;
		#endif
		this->cmdType  = static_cast<NativeCommandType>(command);
		this->seqIndex = seqIdx;
		this->data     = const_cast<uint32_t *>(dat);
		this->dataSize = dataSz;
	}

	#if defined FLEXSPI || defined FLEXSPI0 || defined FLEXSPI1
	static constexpr flexspi_port_t Port (void)
	{
		return 
			// This is a quite dirty hack, as we do not have the possibility to configure the port to use. At least up to now.
			#if (defined MIMXRT633S_SERIES) || defined (MIMXRT685S_cm33_SERIES) || \
				 defined(MIMXRT533S_SERIES) || defined (MIMXRT555S_SERIES) || defined(MIMXRT595S_cm33_SERIES)
				kFLEXSPI_PortB1;
			#else
				kFLEXSPI_PortA1;
			#endif
	}
	#endif
};


class FlexSPI_Helper: public NativeSpiType
{
public:
	void UpdateLUT (const Lut::Table &lut)
	{
		::UpdateLUT (this, 0, &lut.front(), lut.size());
	}
	void UpdateLUT (uint32_t index, const Lut::Table &lut)
	{
		::UpdateLUT (this, index, &lut.front()+index, lut.size()-index);
	}

	void UpdateLUT (uint32_t index, const Lut::Table &lut, size_t size)
	{
		::UpdateLUT (this, index, &lut.front()+index, size);
	}

	void SoftwareReset (void)
	{
		::SoftwareReset (this);
	}

	uint32_t GetBaseAddr (void) const
	{
		return reinterpret_cast<uint32_t>(this);
	}

	uint8_t *GetAmbaAddress (void) const
	{
		switch (this->GetBaseAddr ())
		{
			#ifdef FLEXSPI
			case FLEXSPI_BASE:
				return reinterpret_cast<uint8_t *>(FlexSPI_AMBA_BASE);
			#endif
			#ifdef FLEXSPI0
			case FLEXSPI0_BASE:
//			case FLEXSPI0_BASE_NS:
				return reinterpret_cast<uint8_t *>(FlexSPI0_AMBA_BASE);
			#endif
			#ifdef FLEXSPI1
			case FLEXSPI1_BASE:
//			case FLEXSPI1_BASE_NS:
				return reinterpret_cast<uint8_t *>(FlexSPI1_AMBA_BASE);
			#endif
			#ifdef FLEXSPI2
			case FLEXSPI2_BASE:
//			case FLEXSPI2_BASE_NS:
				return reinterpret_cast<uint8_t *>(FlexSPI2_AMBA_BASE);
			#endif

			#ifdef XSPI0
			case XSPI0_BASE:
			case XSPI0_BASE_NS:
				return reinterpret_cast<uint8_t *>(XSPI0_AMBA_BASE);
			#endif
			#ifdef XSPI1
			case XSPI1_BASE:
			case XSPI1_BASE_NS:
				return reinterpret_cast<uint8_t *>(XSPI1_AMBA_BASE);
			#endif
			#ifdef XSPI2
			case XSPI2_BASE:
			case XSPI2_BASE_NS:
				return reinterpret_cast<uint8_t *>(XSPI2_AMBA_BASE);
			#endif
			default:
				return nullptr;
		}
	}
	
	inline uint8_t *GetAmbaAliasAddress (void) const
	{
		switch (this->GetBaseAddr ())
		{
			#if defined FlexSPI1_ALIAS_BASE && __CORTEX_M == 4
			case FLEXSPI1_BASE:
				return reinterpret_cast<uint8_t *>(FlexSPI1_ALIAS_BASE);
			#endif
			#if defined FlexSPI_AMBA_BASE_NS
			case FLEXSPI_BASE:
			case FLEXSPI_BASE_NS:
				return reinterpret_cast<uint8_t *>(FlexSPI_AMBA_BASE_NS);
			#endif
			#if defined FlexSPI1_AMBA_BASE_NS	
			case FLEXSPI1_BASE:
			case FLEXSPI1_BASE_NS:
				return reinterpret_cast<uint8_t *>(FlexSPI1_AMBA_BASE_NS);
			#endif
			#if defined FlexSPI2_AMBA_BASE_NS
			case FLEXSPI2_BASE:
			case FLEXSPI2_BASE_NS:
				return reinterpret_cast<uint8_t *>(FlexSPI2_AMBA_BASE_NS);
			#endif

			#if defined XSPI0_BASE_NS
			case XSPI0_BASE:
			case XSPI0_BASE_NS:
				return reinterpret_cast<uint8_t *>(XSPI0_AMBA_BASE_NS);
			#endif
			#if defined XSPI1_BASE_NS
			case XSPI1_BASE:
			case XSPI1_BASE_NS:
				return reinterpret_cast<uint8_t *>(XSPI1_AMBA_BASE_NS);
			#endif
			#if defined XSPI2_BASE_NS
			case XSPI2_BASE:
			case XSPI2_BASE_NS:
				return reinterpret_cast<uint8_t *>(XSPI2_AMBA_BASE_NS);
			#endif
			default:
				return nullptr;
		}
	}

	status_t TransferBlocking (Transfer *flashXfer)
	{
		#if defined XSPI0 || defined XSPI1 || defined XSPI2
			// XSPI uses the AMBA Base address, while FlexSPI uses just the offset
			flashXfer-> deviceAddress += reinterpret_cast<uintptr_t>(this->GetAmbaAddress ());
		#endif
		return ::TransferBlocking (this, flashXfer);
	}


	status_t WriteRegister (uint32_t Address, uint32_t value, LUT_CommandOffsets cmd, size_t size  = 1)
	{
		Transfer flashXfer
		{
			Address,					// deviceAddress	- Operation device address.
			CommandType::Write,			// cmdType			- Execution command type.
			static_cast<uint8_t>(cmd),	// seqIndex			- Sequence ID for command.
			1,							// SeqNumber		- Sequence number for command.
			&value,						// data				- Data buffer.
			size						// dataSize			- Data size in bytes.
		};
		return this->TransferBlocking (&flashXfer);
	}

	status_t Write (uint32_t Address, uint32_t *value, size_t size, LUT_CommandOffsets cmd)
	{
		Transfer flashXfer
		{
			Address,					// deviceAddress	- Operation device address.
			CommandType::Write,			// cmdType			- Execution command type.
			static_cast<uint8_t>(cmd),	// seqIndex			- Sequence ID for command.
			1,							// SeqNumber		- Sequence number for command.
			value,						// data				- Data buffer.
			size						// dataSize			- Data size in bytes.
		};
		return this->TransferBlocking (&flashXfer);
	}

	status_t ReadRegister (uint32_t Address, uint32_t &value, LUT_CommandOffsets cmd)
	{
		Transfer flashXfer
		{
			Address,					// deviceAddress	- Operation device address.
			CommandType::Read,			// cmdType			- Execution command type.
			static_cast<uint8_t>(cmd),	// seqIndex			- Sequence ID for command.
			1,							// SeqNumber		- Sequence number for command.
			&value,						// data				- Data buffer.
			4							// dataSize			- Data size in bytes.
		};
		return this->TransferBlocking (&flashXfer);
	}

	status_t Read (uint32_t Address, uint32_t *value, size_t size, LUT_CommandOffsets cmd)
	{
		Transfer flashXfer
		{
			Address,					// deviceAddress	- Operation device address.
			CommandType::Read,			// cmdType			- Execution command type.
			static_cast<uint8_t>(cmd),	// seqIndex			- Sequence ID for command.
			1,							// SeqNumber		- Sequence number for command.
			value,						// data				- Data buffer.
			size						// dataSize			- Data size in bytes.
		};
		return this->TransferBlocking (&flashXfer);
	}

	status_t ReadStatusRegister (uint32_t Address, uint32_t &value)
	{
		Transfer flashXfer
		{
			Address,					// deviceAddress	- Operation device address.
			CommandType::Read,			// cmdType			- Execution command type.
			LUT_ReadStatus,				// seqIndex			- Sequence ID for command.
			1,							// SeqNumber		- Sequence number for command.
			&value,						// data				- Data buffer.
			1							// dataSize			- Data size in bytes.
		};
		return this->TransferBlocking (&flashXfer);
	}

	status_t SendCommand (uint32_t Address, LUT_CommandOffsets cmd, uint8_t sequenceSize = 1)
	{
		Transfer flashXfer
		{
			Address,					// deviceAddress	- Operation device address.
			CommandType::Command,		// cmdType			- Execution command type.
			static_cast<uint8_t>(cmd),	// seqIndex			- Sequence ID for command.
			sequenceSize,				// SeqNumber		- Sequence number for command.
			nullptr,					// data				- Data buffer.
			0							// dataSize			- Data size in bytes.
		};
		return this->TransferBlocking (&flashXfer);
	}

	/*! ReadJEDEC
	\brief Read the JEDEC Device informations
	\param base The Flex-SPI-base to use
	\param info The read Device informations from the flash memory
	\return status_t Status of the Operation - kStatus_Success when successfully */
	status_t ReadJEDEC (DeviceInfo *info)
	{
		uint8_t identification[16] {0U};
		status_t status = this->Read (0, reinterpret_cast<uint32_t *>(identification), sizeof(identification), LUT_ReadJEDEC_ID);
		if (status != kStatus_Success)
			return status;

		// Sanity check of the data, first byte must not be zero or 0xFF
		if (identification[0] == 0 || identification[0] == 0xFF)
			return kStatus_Fail;	// got no ID-Code: No Flash available
	
		// Check if all data are identical
		size_t index = sizeof(identification)/sizeof(identification[0]);
		while (--index>0 && identification[0]==identification[index])
			;
		if (index == 0)
			return kStatus_Fail;	// Data is all identical. Got some transfer error


		int i=0;
		for (; i<8; i++)
		{
			if (identification[i] != ManufactureID_NEXT_MARKER)
				break;
		}

		info->ManufactureID = static_cast<SerialFlash_ManufactureID>(((i+1)<<8U) | identification[i]);
		info->Type          = identification[i+1];
		info->Capacity      = static_cast<Capacity>(identification[i+2]);

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
		bool isBusy {false};
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

	/*! \brief perform a JEDEC reset on the given interface 
	\param base The Base Address of the FlexSPI interface */
	inline void PerformJEDECReset (void)
	{
		switch (this->GetBaseAddr ())
		{
			#ifdef FLEXSPI
			case FLEXSPI_BASE:
				BOARD_PerformJEDECReset_FlexSPI ();
				break;
			#endif
			#ifdef FLEXSPI0
			case FLEXSPI0_BASE:
				BOARD_PerformJEDECReset_FlexSPI0 ();
				break;
			#endif
			#ifdef FLEXSPI1
			case FLEXSPI1_BASE:
				BOARD_PerformJEDECReset_FlexSPI1 ();
				break;
			#endif
			#ifdef FLEXSPI2
			case FLEXSPI2_BASE:
				BOARD_PerformJEDECReset_FlexSPI2 ();
				break;
			#endif
			#ifdef XSPI0
			case XSPI0_BASE:
			case XSPI0_BASE_NS:
				BOARD_PerformJEDECReset_xSPI0 ();
				break;
			#endif
			#ifdef XSPI1
			case XSPI1_BASE:
			case XSPI1_BASE_NS:
				BOARD_PerformJEDECReset_xSPI1 ();
				break;
			#endif
			#ifdef XSPI2
			case XSPI2_BASE:
			case XSPI2_BASE_NS:
				BOARD_PerformJEDECReset_xSPI2 ();
				break;
			#endif
			default:
				return;
		}
	}


	/*! \brief Initialize the pins of the given FlexSPI interface for being used as an octal SPI interface.
	\param base The Base Address of the FlexSPI interface */
	inline void InitOctaSPIPins (void)
	{
		switch (this->GetBaseAddr ())
		{
			#ifdef FLEXSPI
			case FLEXSPI_BASE:
				BOARD_InitOctaSPIPins ();
				break;
			#endif
			#ifdef FLEXSPI0
			case FLEXSPI0_BASE:
				BOARD_InitOctaSPI0Pins ();
				break;
			#endif
			#ifdef FLEXSPI1
			case FLEXSPI1_BASE:
				BOARD_InitOctaSPI1Pins ();
				break;
			#endif
			#ifdef FLEXSPI2
			case FLEXSPI2_BASE:
				BOARD_InitOctaSPI2Pins ();
				break;
			#endif

			#ifdef XSPI0
			case XSPI0_BASE:
			case XSPI0_BASE_NS:
				BOARD_InitOctaSPI0Pins ();
				break;
			#endif
			#ifdef XSPI1
			case XSPI1_BASE:
			case XSPI1_BASE_NS:
				BOARD_InitOctaSPI1Pins ();
				break;
			#endif
			#ifdef XSPI2
			case XSPI2_BASE:
			case XSPI2_BASE_NS:
				BOARD_InitOctaSPI2Pins ();
				break;
			#endif
			default:
				return;
		}
	}

	/*! \brief Initialize the pins of the given FlexSPI interface for being used as an quad SPI interface.
	\param base The Base Address of the FlexSPI interface */
	inline void InitQuadSPIPins (void)
	{
		switch (this->GetBaseAddr ())
		{
			#ifdef FLEXSPI
			case FLEXSPI_BASE:
				BOARD_InitQuadSPIPins ();
				break;
			#endif
			#ifdef FLEXSPI0
			case FLEXSPI0_BASE:
				BOARD_InitQuadSPI0Pins ();
				break;
			#endif
			#ifdef FLEXSPI1
			case FLEXSPI1_BASE:
				BOARD_InitQuadSPI1Pins ();
				break;
			#endif
			#ifdef FLEXSPI2
			case FLEXSPI2_BASE:
				BOARD_InitQuadSPI2Pins ();
				break;
			#endif

			#ifdef XSPI0
			case XSPI0_BASE:
			case XSPI0_BASE_NS:
				BOARD_InitQuadSPI0Pins ();
				break;
			#endif
			#ifdef XSPI1
			case XSPI1_BASE:
			case XSPI1_BASE_NS:
				BOARD_InitQuadSPI1Pins ();
				break;
			#endif
			#ifdef XSPI2
			case XSPI2_BASE:
			case XSPI2_BASE_NS:
				BOARD_InitQuadSPI2Pins ();
				break;
			#endif
			default:
				return;
		}
	}

	/*! \brief Initialize the pins of the given FlexSPI interface for being used like defined in the type parameter.
	\param base The Base Address of the FlexSPI interface
	\param type Type to being used by the FlexSPI interface */
	inline void InitializeSpiPins (MemoryType type)
	{
		switch (type)
		{
			case MemoryType::Hyperflash:
			case MemoryType::Hyperram:
			case MemoryType::OctaSPI_DDR:
			case MemoryType::OctaSPI:
				this->InitOctaSPIPins ();
				break;
			case MemoryType::QuadSPI_DDR:
			case MemoryType::QuadSPI:
				this->InitQuadSPIPins ();
				break;
			case MemoryType::Invalid:
			case MemoryType::SPI:
				break;
		}
	}
};

inline uint8_t *GetAmbaAliasAddress ([[maybe_unused]]const NativeSpiType *base)
{
	return static_cast<const FlexSPI_Helper *>(base)->GetAmbaAliasAddress ();
}

inline int GetPortWidth ([[maybe_unused]]const NativeSpiType *base)
{
	#if (defined(MIMXRT533S_SERIES)  || defined(MIMXRT555S_SERIES) || defined(MIMXRT595S_cm33_SERIES))
		return 8;
	#elif (defined(MIMXRT633S_SERIES) || defined(MIMXRT685S_cm33_SERIES))
		return 8;
	#elif (defined(MIMXRT1011_SERIES) || defined(MIMXRT1015_SERIES) || defined(MIMXRT1021_SERIES) || defined(MIMXRT1024_SERIES) || \
		   defined(MIMXRT1041_SERIES) || defined(MIMXRT1042_SERIES) || defined(MIMXRT1051_SERIES) || defined(MIMXRT1052_SERIES) || \
		   defined(MIMXRT1061_SERIES) || defined(MIMXRT1062_SERIES) || defined(MIMXRT1064_SERIES))
		return 4;
	#elif (defined(MIMXRT735S_cm33_core0_SERIES) || defined(MIMXRT735S_cm33_core1_SERIES) || \
		   defined(MIMXRT758S_cm33_core0_SERIES) || defined(MIMXRT758S_cm33_core1_SERIES) || \
		   defined(MIMXRT798S_cm33_core0_SERIES) || defined(MIMXRT798S_cm33_core1_SERIES))
		return 8; // ToDo: Check if this is correct
	#elif (defined(MIMXRT1165_cm7_SERIES) || defined(MIMXRT1166_cm7_SERIES) || defined(MIMXRT1165_cm4_SERIES) || defined(MIMXRT1166_cm4_SERIES) || \
		   defined(MIMXRT1171_SERIES)     || defined(MIMXRT1172_SERIES)     || defined(MIMXRT1173_cm7_SERIES) || defined(MIMXRT1173_cm4_SERIES) || \
		   defined(MIMXRT1175_cm7_SERIES) || defined(MIMXRT1175_cm4_SERIES) || defined(MIMXRT1176_cm7_SERIES) || defined(MIMXRT1176_cm4_SERIES))
		switch (reinterpret_cast<uint32_t>(base))
		{
			case FLEXSPI1_BASE:
				return 4;
			case FLEXSPI2_BASE:
				return 8;
		}
	#elif (defined(MIMXRT1181_SERIES)     || defined(MIMXRT1182_SERIES)     || defined(MIMXRT1187_cm7_SERIES) || defined(MIMXRT1187_cm33_SERIES) ||\
		   defined(MIMXRT1189_cm7_SERIES) || defined(MIMXRT1189_cm33_SERIES))
		switch (reinterpret_cast<uint32_t>(base))
		{
			case FLEXSPI1_BASE:
				return 8;
			case FLEXSPI2_BASE:
				return 4;
		}
	#else
		#error "unknon controller family"
	#endif
	return 0;
}

inline void PrintMemTypeInfor (MemoryType memoryType)
{
	DebugPrintf ("Init Loader for %s\r\n", MemoryTypeName[static_cast<int>(memoryType)]);
}

#endif // FLEX_SPI_HELPER_H_

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


#ifndef X_SPI_HELPER_H_
#define X_SPI_HELPER_H_

#include <array>
#include "fsl_device_registers.h"
#if defined XSPI0 || defined XSPI1
#include "fsl_xspi.h"
#include "pin_mux.h"

/*! MemoryType:
\brief Supported Memory/Interface types */
enum MemoryType
{
	MemType_Invalid     = 0,
	MemType_Hyperflash  = 1,
	MemType_OctaSPI_DDR = 2,
	MemType_OctaSPI     = 3,
	MemType_QuadSPI_DDR = 4,
	MemType_QuadSPI     = 5,
	MemType_SPI         = 6,
};


class FlexSPI_Helper: public XSPI_Type
{
public:

	uint32_t GetBaseAddr (void) const
	{
		return reinterpret_cast<uint32_t>(this);
	}

	uint8_t *GetAmbaAddress (void)
	{
		switch (this->GetBaseAddr ())
		{
			#ifdef XSPI0
			case XSPI0_BASE:
				return reinterpret_cast<uint8_t *>(XSPI0_AMBA_BASE);
			#endif
			#ifdef XSPI1
			case XSPI1_BASE:
				return reinterpret_cast<uint8_t *>(XSPI1_AMBA_BASE);
			#endif
			#ifdef XSPI2
			case XSPI2_BASE:
				return reinterpret_cast<uint8_t *>(XSPI2_AMBA_BASE);
			#endif
			default:
				return nullptr;
		}
	}


	/*! \brief perform a JEDEC reset on the given interface 
	\param base The Base Address of the FlexSPI interface */
	inline void PerformJEDECReset (void)
	{
		switch (this->GetBaseAddr ())
		{
			#ifdef XSPI0
			case XSPI0_BASE:
				BOARD_PerformJEDECReset_xSPI0();
				break;
			#endif
			#ifdef XSPI1
			case XSPI1_BASE:
				BOARD_PerformJEDECReset_xSPI1();
				break;
			#endif
			#ifdef XSPI2
			case XSPI2_BASE:
				BOARD_PerformJEDECReset_xSPI2();
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
			#ifdef XSPI0
			case XSPI0_BASE:
				BOARD_InitOctaSPI0Pins ();
				break;
			#endif
			#ifdef XSPI1
			case XSPI1_BASE:
				BOARD_InitOctaSPI1Pins ();
				break;
			#endif
			#ifdef XSPI2
			case XSPI2_BASE:
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
			#ifdef XSPI0
			case XSPI0_BASE:
				BOARD_InitQuadSPI0Pins ();
				break;
			#endif
			#ifdef XSPI1
			case XSPI1_BASE:
				BOARD_InitQuadSPI1Pins ();
				break;
			#endif
			#ifdef XSPI2
			case XSPI2_BASE:
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
			case MemType_Hyperflash:
			case MemType_OctaSPI_DDR:
			case MemType_OctaSPI:
				this->InitOctaSPIPins ();
				break;
			case MemType_QuadSPI_DDR:
			case MemType_QuadSPI:
				this->InitQuadSPIPins ();
				break;
			case MemType_Invalid:
			case MemType_SPI:
				break;
		}
	}
};


#endif // defined XSPI0 || defined XSPI1
#endif // X_SPI_HELPER_H_
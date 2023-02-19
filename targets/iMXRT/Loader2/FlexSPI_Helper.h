#ifndef FLEX_SPI_HELPER_H_
#define FLEX_SPI_HELPER_H_

#include "fsl_device_registers.h"
#include "fsl_flexspi.h"
#include "pin_mux.h"

/*! \brief perform a JEDEC reset on the given interface 
\param base The Base Address of the FlexSPI interface */
inline void PerformJEDECReset (const FLEXSPI_Type *base)
{
	switch (reinterpret_cast<uint32_t>(base)) // ToDo: switch (base->GetBaseAddr())
	{
		#ifdef FLEXSPI
		case FLEXSPI_BASE:
			BOARD_PerformJEDECReset_FlexSPI();
			break;
		#endif
		#ifdef FLEXSPI0
		case FLEXSPI0_BASE:
			BOARD_PerformJEDECReset_FlexSPI0();
			break;
		#endif
		#ifdef FLEXSPI1
		case FLEXSPI1_BASE:
			BOARD_PerformJEDECReset_FlexSPI1();
			break;
		#endif
		#ifdef FLEXSPI2
		case FLEXSPI2_BASE:
			BOARD_PerformJEDECReset_FlexSPI2();
			break;
		#endif
		default:
			return;
	}
}

/*! \brief Initialize the pins of the given FlexSPI interface for being used as an octal SPI interface.
\param base The Base Address of the FlexSPI interface */
inline void InitOctaSPIPins (const FLEXSPI_Type *base)
{
	switch (reinterpret_cast<uint32_t>(base)) // ToDo: switch (base->GetBaseAddr())
	{
		#if defined FLEXSPI
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
		default:
			return;
	}
}

/*! \brief Initialize the pins of the given FlexSPI interface for being used as an quad SPI interface.
\param base The Base Address of the FlexSPI interface */
inline void InitQuadSPIPins (const FLEXSPI_Type *base)
{
	switch (reinterpret_cast<uint32_t>(base)) // ToDo: switch (base->GetBaseAddr())
	{
		#if defined FLEXSPI
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
		default:
			return;
	}
}

/*! \brief Initialize the pins of the given FlexSPI interface for being used like defined in the type parameter.
\param base The Base Address of the FlexSPI interface
\param type Type to being used by the FlexSPI interface */
inline void InitializeSpiPins (FLEXSPI_Type *base, MemoryType type)
{
	switch (type)
	{
		case MemType_Hyperflash:
		case MemType_OctaSPI_DDR:
		case MemType_OctaSPI:
			InitOctaSPIPins (base);
			break;
		case MemType_QuadSPI_DDR:
		case MemType_QuadSPI:
			InitQuadSPIPins (base);
			break;
		case MemType_Invalid:
		case MemType_SPI:
			break;
	}
}

inline uint8_t *GetBaseAddress (FLEXSPI_Type *base)
{
	switch (reinterpret_cast<uint32_t>(base))
	{
		#ifdef FLEXSPI
		case FLEXSPI_BASE:
			return reinterpret_cast<uint8_t *>(FlexSPI_AMBA_BASE);
		#endif
		#ifdef FLEXSPI0
		case FLEXSPI0_BASE:
			return reinterpret_cast<uint8_t *>(FlexSPI0_AMBA_BASE);
		#endif
		#ifdef FLEXSPI1
		case FLEXSPI1_BASE:
			return reinterpret_cast<uint8_t *>(FlexSPI1_AMBA_BASE);
		#endif
		#ifdef FLEXSPI2
		case FLEXSPI2_BASE:
			return reinterpret_cast<uint8_t *>(FlexSPI2_AMBA_BASE);
		#endif
		default:
			return nullptr;
	}
}

inline uint8_t *GetAliasBaseAddress (FLEXSPI_Type *base)
{
	#if defined FlexSPI1_ALIAS_BASE && __CORTEX_M == 4
		if (reinterpret_cast<uint32_t>(base) == FLEXSPI1_BASE)
			return reinterpret_cast<uint8_t *>(FlexSPI1_ALIAS_BASE);
	#else
		(void)base;
	#endif

	return nullptr;
}

#endif // FLEX_SPI_HELPER_H_
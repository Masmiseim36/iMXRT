#ifndef FLEX_SPI_HELPER_H_
#define FLEX_SPI_HELPER_H_

#include "fsl_device_registers.h"
#include "fsl_flexspi.h"
#include "pin_mux.h"

/*! \brief perform a JEDEC reset on the given interface 
\param base The Base Address of the FlexSPI interface */
inline void PerformJEDECReset (FLEXSPI_Type *base)
{
	switch (reinterpret_cast<uint32_t>(base)) // ToDo: switch (base->GetBaseAddr ())
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
inline void InitOctaSPIPins (FLEXSPI_Type *base)
{
	switch (reinterpret_cast<uint32_t>(base)) // ToDo: switch (base->GetBaseAddr ())
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
inline void InitQuadSPIPins (FLEXSPI_Type *base)
{
	switch (reinterpret_cast<uint32_t>(base)) // ToDo: switch (base->GetBaseAddr ())
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
	switch (reinterpret_cast<uint32_t>(base)) // ToDo: switch (base->GetBaseAddr ())
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

inline uint8_t *GetAliasBaseAddress (const FLEXSPI_Type *base)
{
	#if defined FlexSPI1_ALIAS_BASE && __CORTEX_M == 4
		if (reinterpret_cast<uint32_t>(base) == FLEXSPI1_BASE)  // ToDo: switch (base->GetBaseAddr ())
			return reinterpret_cast<uint8_t *>(FlexSPI1_ALIAS_BASE);
	#else
		(void)base;
	#endif

	return nullptr;
}

inline int GetPortWidth ([[maybe_unused]]const FLEXSPI_Type *base)
{
	#if (defined(MIMXRT533S_SERIES)  || defined(MIMXRT555S_SERIES) || defined(MIMXRT595S_cm33_SERIES))
		return 8;
	#elif (defined(MIMXRT633S_SERIES) || defined(MIMXRT685S_cm33_SERIES))
		return 8;
	#elif (defined(MIMXRT1011_SERIES) || defined(MIMXRT1015_SERIES) || defined(MIMXRT1021_SERIES) || defined(MIMXRT1024_SERIES) || \
		   defined(MIMXRT1041_SERIES) || defined(MIMXRT1042_SERIES) || defined(MIMXRT1051_SERIES) || defined(MIMXRT1052_SERIES) || \
		   defined(MIMXRT1061_SERIES) || defined(MIMXRT1062_SERIES) || defined(MIMXRT1064_SERIES))
		return 4;
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

#endif // FLEX_SPI_HELPER_H_
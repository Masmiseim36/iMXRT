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


#include "libmem_driver_Hyperflash.h"
#include "libmem_Tools.h"
#include "FlexSPI_Helper.h"
#include "fsl_device_registers.h"
#include "fsl_flexspi.h"
#include "fsl_common.h"
#include "fsl_clock.h"
#if __has_include("fsl_power.h")
	#include "fsl_power.h"
#endif
#include "DebugPrint.h"
#include <array>

static constexpr uint32_t SectorSize = 256 * 1024;


/*! enum FlashCommands:
*/
enum FlashCommands
{
	LUT_ReadData    = 0,
	LUT_WriteData   = 1,
	LUT_ReadStatus  = 2,
	LUT_WriteEnable = 4,
	LUT_EraseSector = 6,
	LUT_PageProgram = 10,
	LUT_EraseChip   = 12
};

static flexspi_device_config_t deviceconfig
{
	.flexspiRootClk       = 42 * 1000 * 1000, // 42MHZ SPI serial clock
	.isSck2Enabled        = false,
	.flashSize            = 0x4000000 / 1024,	// Size in KBytes
	.CSIntervalUnit       = kFLEXSPI_CsIntervalUnit1SckCycle,
	.CSInterval           = 2,
	.CSHoldTime           = 0,
	.CSSetupTime          = 3,
	.dataValidTime        = 1,
	.columnspace          = 3,
	.enableWordAddress    = true,
	.AWRSeqIndex          = LUT_WriteData,
	.AWRSeqNumber         = 1,
	.ARDSeqIndex          = LUT_ReadData,
	.ARDSeqNumber         = 1,
	.AHBWriteWaitUnit     = kFLEXSPI_AhbWriteWaitUnit2AhbCycle,
	.AHBWriteWaitInterval = 20,
	.enableWriteMask      = false,
	#if defined(FSL_FEATURE_FLEXSPI_HAS_ERRATA_051426) && (FSL_FEATURE_FLEXSPI_HAS_ERRATA_051426)
		.isFroClockSource = false //!<  \brief Is FRO clock source or not.
	#endif
};

/*! LUT_HyperFlash:  */
constexpr FlexSPI_LUT LUT_HyperFlash =
{
	// (0) Read Data --> compare @FlashCommands
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0xA0, kFLEXSPI_Command_RADDR_DDR, kFLEXSPI_8PAD, 0x18),
//	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_CADDR_DDR, kFLEXSPI_8PAD, 0x10, kFLEXSPI_Command_READ_DDR,  kFLEXSPI_8PAD, 0x04),
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_CADDR_DDR, kFLEXSPI_8PAD, 0x10, kFLEXSPI_Command_DUMMY_DDR, kFLEXSPI_8PAD, 0x06),
    FLEXSPI_LUT_SEQ (kFLEXSPI_Command_READ_DDR,  kFLEXSPI_8PAD, 0x04, kFLEXSPI_Command_STOP,      kFLEXSPI_1PAD, 0x0),
	0,	// Dummy to fill a block of four

	// (1) Write Data --> compare @FlashCommands
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x20, kFLEXSPI_Command_RADDR_DDR, kFLEXSPI_8PAD, 0x18),
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_CADDR_DDR, kFLEXSPI_8PAD, 0x10, kFLEXSPI_Command_WRITE_DDR, kFLEXSPI_8PAD, 0x02),
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (2) Read Status --> compare @FlashCommands
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x00),
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0xAA), // ADDR 0x555 --> 0x555/8 = 0xAA
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x05), //            --> 0x555%8 = 0x05
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x70), // DATA 0x70
	// (3) continue with Read Status
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0xA0, kFLEXSPI_Command_RADDR_DDR, kFLEXSPI_8PAD, 0x18),
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_CADDR_DDR, kFLEXSPI_8PAD, 0x10, kFLEXSPI_Command_DUMMY_RWDS_DDR, kFLEXSPI_8PAD, 0x0B),
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_READ_DDR,  kFLEXSPI_8PAD, 0x04, kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0x0),
	0,	// Dummy to fill a block of four

	// (4) Write Enable --> compare @FlashCommands
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x00),
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0xAA), // ADDR 0x555 --> 0x555/8 = 0xAA
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x05), //            --> 0x555%8 = 0x05
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0xAA), // DATA 0xAA
	// (5) continue with Write Enable
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x00),
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x55), // ADDR 0x2AA --> 0x2AA/8 = 0x55
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x02), //            --> 0x2AA%8 = 0x02
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x55), // Data 0x55

	// (6) Erase Sector --> compare @FlashCommands
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x00),
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0xAA), // ADDR 0x555 --> 0x555/8 = 0xAA
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x05), //            --> 0x555%8 = 0x05
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x80), // DATA 0x80
	// (7) continue with Erase Sector
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x00),
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0xAA), // ADDR 0x555 --> 0x555/8 = 0xAA
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x05), //            --> 0x555%8 = 0x05
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0xAA), // Data 0x555
	// (8) continue with Erase Sector
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x00),
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x55),
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x02),
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x55),
	// (9) continue with Erase Sector
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_RADDR_DDR, kFLEXSPI_8PAD, 0x18),
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_CADDR_DDR, kFLEXSPI_8PAD, 0x10, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x00),
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x30, kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0x00),
	0,	// Dummy to fill a block of four

	// (10) program page: --> compare @FlashCommands
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x00),
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0xAA), // ADDR 0x555 --> 0x555/8 = 0xAA
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x05), //            --> 0x555%8 = 0x05
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0xA0), // DATA 0xA0
	// (11) continue with program page
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_RADDR_DDR, kFLEXSPI_8PAD, 0x18),
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_CADDR_DDR, kFLEXSPI_8PAD, 0x10, kFLEXSPI_Command_WRITE_DDR, kFLEXSPI_8PAD, 0x80),
	0,	// Dummy to fill a block of four
	0,	// Dummy to fill a block of four

	// (12) Erase chip: --> compare @FlashCommands
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x00),
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0xAA),
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x05),
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x80),
	// (13) continue with Erase chip
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x00),
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0xAA),
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x05),
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0xAA),
	// (14) continue with Erase chip
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x00),
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x55),
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x02),
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x55),
	// (15) continue with Erase chip
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x00),
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0xAA),
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x05),
	FLEXSPI_LUT_SEQ (kFLEXSPI_Command_DDR,       kFLEXSPI_8PAD, 0x00, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x10),
};


// Define the structure of the Flash (Sector Count and Size)
static const libmem_geometry_t geometry[]
{ // count - size
	{100, 0x40000},
	{0, 0} 
};

static int WriteEnable    (FLEXSPI_Type *base, uint32_t baseAddr);
static status_t EraseChip (FLEXSPI_Type *base);

static int EraseSector        (libmem_driver_handle_t *h, libmem_sector_info_t *si);
static int ProgramPage        (libmem_driver_handle_t *h, uint8_t *Dest, const uint8_t *Source);
static int libmem_ProgramPage (libmem_driver_handle_t *h, uint8_t *dest, const uint8_t *src, size_t size);
static int libmem_EraseSector (libmem_driver_handle_t *h, uint8_t *start, size_t size, uint8_t **erase_start, size_t *erase_size);
static int libmem_Flush       (libmem_driver_handle_t *h);
static int libmem_Read        (libmem_driver_handle_t *h, uint8_t *dest, const uint8_t *src, size_t size);
static uint32_t libmem_CRC32  (libmem_driver_handle_t *h, const uint8_t *start, size_t size, uint32_t crc);


/*! DriverFunctions:  */
static const libmem_driver_functions_t DriverFunctions
{
	libmem_ProgramPage,
	nullptr,
	libmem_EraseSector,
	nullptr,
	nullptr,
	libmem_Flush
};

/*! DriverFunctionsExt:  */
static const libmem_ext_driver_functions_t DriverFunctions_Extended
{
	nullptr,
	libmem_Read,
	libmem_CRC32
};



/*! Libmem_InitializeDriver_Hyperflash:
\brief Initialize the FlexSPI interface for using as a SPI-interface
\param FlashHandle The handle which should be initialized
\param base The Flex-SPI-base to use
\return LibmemStatus_t LibmemStaus_Success if the operation was successfully */
LibmemStatus_t Libmem_InitializeDriver_Hyperflash (FLEXSPI_Type *base)
{
	#if (defined(MIMXRT633S_SERIES) || defined(MIMXRT685S_cm33_SERIES))
		// Clock Source
		// 0 --> Main Clock.
		// 1 --> Main PLL Clock (main_pll_clk).
		// 2 --> AUX0 PLL clock (aux0_pll_clk).
		// 3 --> FFRO Clock.
		// 4 --> AUX1 PLL clock (aux1_pll_clk).
		constexpr uint32_t src = 1;	// Use AUX0_PLL as clock source for the FlexSPI
		uint32_t ClockDiv = 9;		// with a divider of four
		if (CLKCTL0->FLEXSPIFCLKSEL != CLKCTL0_FLEXSPIFCLKSEL_SEL(src) || (CLKCTL0->FLEXSPIFCLKDIV & CLKCTL0_FLEXSPIFCLKDIV_DIV_MASK) != (ClockDiv - 1))
		{
			#if !defined(FSL_SDK_DRIVER_QUICK_ACCESS_ENABLE)
				POWER_DisablePD(kPDRUNCFG_APD_FLEXSPI_SRAM);
				POWER_DisablePD(kPDRUNCFG_PPD_FLEXSPI_SRAM);
				POWER_ApplyPD();
			#endif

//			CLKCTL0->PSCCTL0_CLR = CLKCTL0_PSCCTL0_CLR_FLEXSPI_OTFAD_CLK_MASK;	// Disable clock before changing clock source
			CLKCTL0->FLEXSPIFCLKSEL  = CLKCTL0_FLEXSPIFCLKSEL_SEL(src);			// Update flexspi clock.
			CLKCTL0->FLEXSPIFCLKDIV |= CLKCTL0_FLEXSPIFCLKDIV_RESET_MASK;		// Reset the divider counter
			CLKCTL0->FLEXSPIFCLKDIV  = CLKCTL0_FLEXSPIFCLKDIV_DIV(ClockDiv - 1);
			while ((CLKCTL0->FLEXSPIFCLKDIV) & CLKCTL0_FLEXSPIFCLKDIV_REQFLAG_MASK)
				;
//			CLKCTL0->PSCCTL0_SET = CLKCTL0_PSCCTL0_SET_FLEXSPI_OTFAD_CLK_MASK;	// Enable FLEXSPI clock again
		}
		uint32_t ClockHz = CLOCK_GetFlexspiClkFreq ();
		//uint32_t SourceClock_Hz = ClockHz * ClockDiv;
	#elif (defined(MIMXRT533S_SERIES)  || defined(MIMXRT555S_SERIES) || defined(MIMXRT595S_cm33_SERIES))
		constexpr uint32_t src {2};	// Use AUX0_PLL as clock source for the FlexSPI
		uint32_t ClockDiv {4};		// with a divider of four
		uint32_t ClockHz  {};
		if (base == FLEXSPI0 && 
			(CLKCTL0->FLEXSPI0FCLKSEL != CLKCTL0_FLEXSPI0FCLKSEL_SEL(src) || (CLKCTL0->FLEXSPI0FCLKDIV & CLKCTL0_FLEXSPI0FCLKDIV_DIV_MASK) != (ClockDiv - 1)))
		{
			#if !defined(FSL_SDK_DRIVER_QUICK_ACCESS_ENABLE)
				POWER_DisablePD(kPDRUNCFG_APD_FLEXSPI0_SRAM);
				POWER_DisablePD(kPDRUNCFG_PPD_FLEXSPI0_SRAM);
				POWER_ApplyPD();
			#endif

//			CLKCTL0->PSCCTL0_CLR = CLKCTL0_PSCCTL0_CLR_FLEXSPI_OTFAD_CLK_MASK;	// Disable clock before changing clock source
			CLKCTL0->FLEXSPI0FCLKSEL  = CLKCTL0_FLEXSPI0FCLKSEL_SEL(src);		// Update flexspi clock.
			CLKCTL0->FLEXSPI0FCLKDIV |= CLKCTL0_FLEXSPI0FCLKDIV_RESET_MASK;		// Reset the divider counter
			CLKCTL0->FLEXSPI0FCLKDIV  = CLKCTL0_FLEXSPI0FCLKDIV_DIV(ClockDiv - 1);
			while ((CLKCTL0->FLEXSPI0FCLKDIV) & CLKCTL0_FLEXSPI0FCLKDIV_REQFLAG_MASK)
				;
//			CLKCTL0->PSCCTL0_SET = CLKCTL0_PSCCTL0_SET_FLEXSPI_OTFAD_CLK_MASK;	// Enable FLEXSPI clock again
			ClockHz = CLOCK_GetFlexspiClkFreq (0);
		}
		else if (base == FLEXSPI1 &&
			(CLKCTL0->FLEXSPI1FCLKSEL != CLKCTL0_FLEXSPI1FCLKSEL_SEL(src) || (CLKCTL0->FLEXSPI1FCLKDIV & CLKCTL0_FLEXSPI1FCLKDIV_DIV_MASK) != (ClockDiv - 1)))
		{
			#if !defined(FSL_SDK_DRIVER_QUICK_ACCESS_ENABLE)
				POWER_DisablePD(kPDRUNCFG_APD_FLEXSPI1_SRAM);
				POWER_DisablePD(kPDRUNCFG_PPD_FLEXSPI1_SRAM);
				POWER_ApplyPD();
			#endif

//			CLKCTL0->PSCCTL0_CLR = CLKCTL0_PSCCTL0_CLR_FLEXSPI_OTFAD_CLK_MASK;	// Disable clock before changing clock source
			CLKCTL0->FLEXSPI1FCLKSEL  = CLKCTL0_FLEXSPI1FCLKSEL_SEL(src);		// Update flexspi clock.
			CLKCTL0->FLEXSPI1FCLKDIV |= CLKCTL0_FLEXSPI1FCLKDIV_RESET_MASK;		// Reset the divider counter
			CLKCTL0->FLEXSPI1FCLKDIV  = CLKCTL0_FLEXSPI1FCLKDIV_DIV(ClockDiv - 1);
			while ((CLKCTL0->FLEXSPI1FCLKDIV) & CLKCTL0_FLEXSPI1FCLKDIV_REQFLAG_MASK)
				;
//			CLKCTL0->PSCCTL0_SET = CLKCTL0_PSCCTL0_SET_FLEXSPI_OTFAD_CLK_MASK;	// Enable FLEXSPI clock again
			ClockHz = CLOCK_GetFlexspiClkFreq (1);
		}
	#elif (defined(MIMXRT1011_SERIES) || defined(MIMXRT1015_SERIES) || defined(MIMXRT1021_SERIES) || defined(MIMXRT1024_SERIES) || \
		   defined(MIMXRT1041_SERIES) || defined(MIMXRT1042_SERIES) || defined(MIMXRT1051_SERIES) || defined(MIMXRT1052_SERIES) || \
		   defined(MIMXRT1061_SERIES) || defined(MIMXRT1062_SERIES) || defined(MIMXRT1064_SERIES))
		const clock_usb_pll_config_t configUsbPll = {.loopDivider = 0U, .src=0};
		CLOCK_InitUsb1Pll (&configUsbPll);		// PLL3 --> USB1-PLL
		CLOCK_InitUsb1Pfd (kCLOCK_Pfd0, 26);	// Set PLL3 PFD0 clock 520MHZ (480*26/24). PLL3 --> USB1-PLL --> PLL480
		const uint32_t SourceClock_Hz = CLOCK_GetUsb1PfdFreq (kCLOCK_Pfd0);

		const uint32_t ClockDiv = 4; // flexspi clock divide by two --> 130 Mz.
		uint32_t ClockHz = SourceClock_Hz / ClockDiv;
		clock_div_t FlexSPIDiv = kCLOCK_FlexspiDiv;
		switch (reinterpret_cast<uint32_t>(base))
		{
			case FLEXSPI_BASE:
			/*	00 derive clock from semc_clk_root_pre
				01 derive clock from pll3_sw_clk
				10 derive clock from PLL2 PFD2
				11 derive clock from PLL3 PFD0 */
				CLOCK_SetMux (kCLOCK_FlexspiMux, 0x3); // Choose PLL3 PFD0 clock as flexspi source clock.
				FlexSPIDiv = kCLOCK_FlexspiDiv;
				break;
			#ifdef FLEXSPI2
			case FLEXSPI2_BASE:
			/*	00 derive clock from PLL2 PFD2
				01 derive clock from PLL3 PFD0
				10 derive clock from PLL3 PFD1
				11	derive clock from PLL2 (pll2_main_clk) */
				CLOCK_SetMux (kCLOCK_Flexspi2Mux, 0x1); // Choose PLL3 PFD0 clock as flexspi source clock.
				FlexSPIDiv = kCLOCK_Flexspi2Div;
				break;
			#endif
			default:
				break;
		}
		CLOCK_SetDiv (FlexSPIDiv, ClockDiv-1);
	#elif (defined(MIMXRT1165_cm7_SERIES) || defined(MIMXRT1166_cm7_SERIES) || defined(MIMXRT1165_cm4_SERIES) || defined(MIMXRT1166_cm4_SERIES) || \
		   defined(MIMXRT1171_SERIES)     || defined(MIMXRT1172_SERIES)     || defined(MIMXRT1173_cm7_SERIES) || defined(MIMXRT1173_cm4_SERIES) || \
		   defined(MIMXRT1175_cm7_SERIES) || defined(MIMXRT1175_cm4_SERIES) || defined(MIMXRT1176_cm7_SERIES) || defined(MIMXRT1176_cm4_SERIES))
		clock_root_t FlexSPIClock = kCLOCK_Root_Flexspi1;
		clock_lpcg_t FlexSPIClockGate = kCLOCK_Flexspi1;
		switch (reinterpret_cast<uint32_t>(base))
		{
			case FLEXSPI1_BASE:
				FlexSPIClock = kCLOCK_Root_Flexspi1;
				FlexSPIClockGate = kCLOCK_Flexspi1;
				break;
			case FLEXSPI2_BASE:
				FlexSPIClock = kCLOCK_Root_Flexspi2;
				FlexSPIClockGate = kCLOCK_Flexspi2;
				break;
			default:
				return LibmemStaus_InvalidDevice;
		}
		CLOCK_ControlGate (FlexSPIClockGate, kCLOCK_Off);	// The module clock must be disabled during clock switch in order to avoid glitch
		CLOCK_SetRootClockDiv (FlexSPIClock, 4); // --> 396 MHz / 4 = ~100 MHz
		CLOCK_SetRootClockMux (FlexSPIClock, 6); // ClockSource_SysPll2Pfd2 --> 396 MHz  -  SYSPLL2=528 MHz
		CLOCK_ControlGate (FlexSPIClockGate, kCLOCK_On);

		uint32_t ClockHz = CLOCK_GetRootClockFreq (FlexSPIClock);
	#elif (defined(MIMXRT1181_SERIES)     || defined(MIMXRT1182_SERIES)     || defined(MIMXRT1187_cm7_SERIES) || defined(MIMXRT1187_cm33_SERIES) ||\
		   defined(MIMXRT1189_cm7_SERIES) || defined(MIMXRT1189_cm33_SERIES))
		uint32_t ClockHz{};
		switch (reinterpret_cast<uint32_t>(base))
		{
			case FLEXSPI1_BASE:
				CLOCK_SetRootClockDiv (kCLOCK_Root_Flexspi1, 4); // --> 392,7s MHz / 4 = ~98,18 MHz
				CLOCK_SetRootClockMux (kCLOCK_Root_Flexspi1, kCLOCK_FLEXSPI1_ClockRoot_MuxSysPll3Pfd0); // ClockSource_SysPll2 --> 392,7s MHz
				ClockHz = CLOCK_GetRootClockFreq (kCLOCK_Root_Flexspi1);
				break;
			case FLEXSPI2_BASE:
				CLOCK_SetRootClockDiv (kCLOCK_Root_Flexspi2, 4); // --> 392,7s MHz / 4 = ~98,18 MHz
				CLOCK_SetRootClockMux (kCLOCK_Root_Flexspi2, kCLOCK_FLEXSPI2_ClockRoot_MuxSysPll3Pfd2); // ClockSource_SysPll2 --> 392,7s MHz
				ClockHz = CLOCK_GetRootClockFreq (kCLOCK_Root_Flexspi2);
				break;
			default:
				return LibmemStaus_InvalidDevice;
		}
	#else
		#error "unknon controller family"
	#endif
	deviceconfig.flexspiRootClk = ClockHz;

	// Get FLEXSPI default settings and configure the FlexSPI.
	flexspi_config_t config {};
	FLEXSPI_GetDefaultConfig (&config);

	config.ahbConfig.enableAHBPrefetch    = true;	// Set AHB buffer size for reading data through AHB bus.
	config.ahbConfig.enableReadAddressOpt = true;	// Allow AHB read start address do not follow the alignment requirement.
	config.ahbConfig.enableAHBBufferable  = true;
	config.ahbConfig.enableAHBCachable    = true;
	#if !(defined(FSL_FEATURE_FLEXSPI_HAS_NO_MCR2_SCKBDIFFOPT) && FSL_FEATURE_FLEXSPI_HAS_NO_MCR2_SCKBDIFFOPT)
		config.enableSckBDiffOpt          = true;	// enable diff clock and DQS
	#endif
	config.rxSampleClock                  = kFLEXSPI_ReadSampleClkExternalInputFromDqsPad;
	#if !(defined(FSL_FEATURE_FLEXSPI_HAS_NO_MCR0_COMBINATIONEN) && FSL_FEATURE_FLEXSPI_HAS_NO_MCR0_COMBINATIONEN)
		config.enableCombination          = true;
	#endif
	FLEXSPI_Init (base, &config);

	FLEXSPI_SetFlashConfig (base, &deviceconfig, kFLEXSPI_PortA1);  // Configure flash settings according to serial flash feature.
	FLEXSPI_UpdateLUT      (base, 0, &LUT_HyperFlash.front(), LUT_HyperFlash.size()); // Update LUT table.
	FLEXSPI_SoftwareReset  (base);                                  // Do software reset.
//	EraseChip (base);

	static uint8_t write_buffer[HYPERFLASH_PAGE_SIZE];
	LibmemDriver *FlashHandle = LibmemDriver::GetDriver ();
//	libmem_register_driver (FlashHandle, GetBaseAddress(base), BOARD_FLASH_SIZE, geometry, nullptr, &DriverFunctions, &DriverFunctions_Extended);
	libmem_register_driver (FlashHandle, GetBaseAddress(base), BOARD_FLASH_SIZE, geometry, nullptr, &DriverFunctions, nullptr);
	int err = libmem_driver_paged_write_init (&FlashHandle->PageWriteControlBlock, write_buffer, HYPERFLASH_PAGE_SIZE, ProgramPage, 4, 0);
	FlashHandle->user_data = (uint32_t)base;

	uint8_t *AliasAddress = GetAliasBaseAddress (base);
	if (AliasAddress != nullptr && err == LIBMEM_STATUS_SUCCESS)
	{
		FlashHandle = LibmemDriver::GetDriver ();
		libmem_register_driver (FlashHandle, AliasAddress, BOARD_FLASH_SIZE, geometry, nullptr, &DriverFunctions, nullptr);
		err = libmem_driver_paged_write_init (&FlashHandle->PageWriteControlBlock, write_buffer, HYPERFLASH_PAGE_SIZE, ProgramPage, 4, 0);
		FlashHandle->user_data = (uint32_t)base;
		DebugPrint ("### Add Driver for Alias\r\n");
	}
	return static_cast<LibmemStatus_t>(err);
}



/*! WriteEnable:
\brief Send write-enable command
\param base The Flex-SPI-base to use
\param baseAddr The base-address of the command
\return status_t kStatus_Success if the operation was successfully */
static status_t WriteEnable (FLEXSPI_Type *base, uint32_t baseAddr)
{
	flexspi_transfer_t flashXfer;
	flashXfer.deviceAddress = baseAddr;
	flashXfer.port          = kFLEXSPI_PortA1;
	flashXfer.cmdType       = kFLEXSPI_Command;
	flashXfer.SeqNumber     = 2;
	flashXfer.seqIndex      = LUT_WriteEnable;

	return FLEXSPI_TransferBlocking (base, &flashXfer);
}

/*! WaitBusBusy:
\brief Wait until the Write/erase operation is finished and the Flash is not busy anymore
\param base The Flex-SPI-base to use
\return status_t kStatus_Success if the operation was successfully */
status_t WaitBusBusy (FLEXSPI_Type *base)
{
	// Wait status ready.	
	uint32_t readValue{};
	status_t status{};
	flexspi_transfer_t flashXfer{};

	flashXfer.deviceAddress = 0;
	flashXfer.port      = kFLEXSPI_PortA1;
	flashXfer.cmdType   = kFLEXSPI_Read;
	flashXfer.SeqNumber = 2;
	flashXfer.seqIndex  = LUT_ReadStatus;
	flashXfer.data      = &readValue;
	flashXfer.dataSize  = 2;

	bool isBusy = false;
	do
	{
		status = FLEXSPI_TransferBlocking (base, &flashXfer);

		if (status != kStatus_Success)
			return status;

		if (readValue & 0x8000)
			isBusy = false;
		else
			isBusy = true;

		if (readValue & 0x3200)
		{
			status = kStatus_Fail;
			break;
		}
	}
	while (isBusy);

	return status;
}

/*! EraseChip:
\brief Erase the whole-Flash-memory
\param base The FlexSPI-Interface where the Flash is located which should be erased
\return static status_t Status of the Operation - kStatus_Success when successfully */
[[maybe_unused]] static status_t EraseChip (FLEXSPI_Type *base)
{
	DebugPrintf ("EraseChip\r\n");

	status_t status = WriteEnable (base, 0);
	if (status != kStatus_Success)
		return status;

	flexspi_transfer_t flashXfer;
	flashXfer.deviceAddress = 0;
	flashXfer.port          = kFLEXSPI_PortA1;
	flashXfer.cmdType       = kFLEXSPI_Command;
	flashXfer.SeqNumber     = 4;
	flashXfer.seqIndex      = LUT_EraseChip;
	status = FLEXSPI_TransferBlocking (base, &flashXfer);
	if (status != kStatus_Success)
		return LIBMEM_STATUS_ERROR;

	return WaitBusBusy (base);
}

/*! EraseSector:
\brief Erase a sector of the Flash-Memory
\param h Handle to the Flash-Driver
\param si Information about the sector which should be erased
\return static int LIBMEM_STATUS_SUCCESS when the erase operation was successfully, otherwise LIBMEM_STATUS_ERROR */
static status_t EraseSector (libmem_driver_handle_t *h, libmem_sector_info_t *si)
{
	if (IsSectorEmpty (reinterpret_cast<uint32_t *>(si->start), SectorSize))
		return LIBMEM_STATUS_SUCCESS;

	FLEXSPI_Type *base = (FLEXSPI_Type *)h->user_data;
	uint32_t SectorAddr = libmem_CalculateOffset (h, si->start);
	if (SectorAddr == UINT32_MAX)
		return LIBMEM_STATUS_INVALID_RANGE;

	DebugPrintf ("EraseSector at 0x%x, size: %d\r\n", SectorAddr, si->size);

	// Write enable
	status_t status = WriteEnable (base, SectorAddr);
	if (status != kStatus_Success)
		return LIBMEM_STATUS_ERROR;

	flexspi_transfer_t flashXfer;
	flashXfer.deviceAddress = SectorAddr;
	flashXfer.port          = kFLEXSPI_PortA1;
	flashXfer.cmdType       = kFLEXSPI_Command;
	flashXfer.SeqNumber     = 4;
	flashXfer.seqIndex      = LUT_EraseSector;
	status = FLEXSPI_TransferBlocking (base, &flashXfer);
	if (status != kStatus_Success)
		return LIBMEM_STATUS_ERROR;

	status = WaitBusBusy (base);
	if (status != kStatus_Success)
		return LIBMEM_STATUS_ERROR;

	return LIBMEM_STATUS_SUCCESS;
}

/*! ProgramPage:
\brief Write Data to a Flash-Page
\param h Handle to the Flash-Driver
\param Dest Address to write the Data to. This Address is in the Address-Range of the Controller
\param Source Address of the Array with the Data to write
\return static int LIBMEM_STATUS_SUCCESS when the write operation was successfully, otherwise LIBMEM_STATUS_ERROR */
static int ProgramPage (libmem_driver_handle_t *h, uint8_t *Dest, const uint8_t *Source)
{
	FLEXSPI_Type *base = (FLEXSPI_Type *)h->user_data;
	uint32_t DeviceAddress = libmem_CalculateOffset (h, Dest);
	if (DeviceAddress == UINT32_MAX)
		return LIBMEM_STATUS_INVALID_RANGE;

	DebugPrintf ("ProgramPage at 0x%X\r\n", DeviceAddress);

	// Write enable
	status_t status = WriteEnable (base, DeviceAddress);
	if (status != kStatus_Success)
		return LIBMEM_STATUS_ERROR;

	// Prepare page program command
    flexspi_transfer_t flashXfer;
	flashXfer.deviceAddress = DeviceAddress;
	flashXfer.port          = kFLEXSPI_PortA1;
	flashXfer.cmdType       = kFLEXSPI_Write;
	flashXfer.SeqNumber     = 2;
	flashXfer.seqIndex      = LUT_PageProgram;
	flashXfer.data          = (uint32_t *)Source;
	flashXfer.dataSize      = HYPERFLASH_PAGE_SIZE;
	status = FLEXSPI_TransferBlocking (base, &flashXfer);

	if (status != kStatus_Success)
		return LIBMEM_STATUS_ERROR;

	status = WaitBusBusy (base);

	// Program finished, speed the clock to 166M.
//	FLEXSPI_Enable     (base, false);
//	CLOCK_DisableClock (kCLOCK_FlexSpi);
//	CLOCK_SetDiv       (kCLOCK_FlexspiDiv, 0); // flexspi clock 332M, DDR mode, internal clock 166M.
//	CLOCK_EnableClock  (kCLOCK_FlexSpi);
//	FLEXSPI_Enable     (base, true);


	if (status != kStatus_Success)
		return LIBMEM_STATUS_ERROR;

	return LIBMEM_STATUS_SUCCESS;
}

/*! libmem_ProgramPage:
\brief The LIBMEM driver's write function.
\param h    A pointer to the handle of the LIBMEM driver.
\param dest A pointer to the memory address in memory range handled by driver to write data to.
\param src  pointer to the memory address to read data from.
\param size The number of bytes to write.
\return int The LIBMEM status result */
static int libmem_ProgramPage (libmem_driver_handle_t *h, uint8_t *dest, const uint8_t *src, size_t size)
{
	DebugPrintf ("libmem_ProgramPage at 0x%x - size: %d\r\n", dest, size);
	LibmemDriver *driver = static_cast<LibmemDriver *>(h);
	return libmem_driver_paged_write (h, dest, src, size, &driver->PageWriteControlBlock);
}

/*! libmem_EraseSector:
\brief The LIBMEM driver's erase function
\param h           A pointer to the handle of the LIBMEM driver.
\param start       A pointer to the initial memory address in memory range handled by driver to erase.
\param size        The number of bytes to erase.
\param erase_start A pointer to a location in memory to store a pointer to the start of the memory range that has actually been erased or nullptr if not required.
\param erase_size  A pointer to a location in memory to store the size in bytes of the memory range that has actually been erased or nullptr if not required.
\return int        The LIBMEM status result */
static int libmem_EraseSector (libmem_driver_handle_t *h, uint8_t *start, size_t size, uint8_t **erase_start, size_t *erase_size)
{
	return libmem_foreach_sector_in_range (h, start, size, EraseSector, erase_start, erase_size);
}

/*! libmem_Flush:
\brief The LIBMEM driver's flush function.
\param h    A pointer to the handle of the LIBMEM driver. 
\return int The LIBMEM status result */
static int libmem_Flush (libmem_driver_handle_t *h)
{
	DebugPrint ("libmem_Flush\r\n");
	LibmemDriver *driver = static_cast<LibmemDriver *>(h);
	return libmem_driver_paged_write_flush (h, &driver->PageWriteControlBlock);
}

/*! libmem_Read:
\brief The LIBMEM driver's read extended function.
\param h    A pointer to the handle of the LIBMEM driver.
\param dest A pointer to the initial memory address to write data to.
\param src  A pointer to the initial memory address in the memory range handled by the driver to read data from.
\param size The number of bytes to write.
\return int The LIBMEM status result */
static int libmem_Read (libmem_driver_handle_t *h, uint8_t *dest, const uint8_t *src, size_t size)
{
	(void)h;
	if (size)
		memcpy (dest, src, size);
	return LIBMEM_STATUS_SUCCESS;
}

/*! libmem_CRC32:
\brief The LIBMEM driver's crc32 extended function.
\param h     A pointer to the handle of the LIBMEM driver.
\param start A pointer to the start of the address range.
\param size  The size of the address range in bytes.
\param crc   The initial CRC-32 value.
\return uint32_t The computed CRC-32 value. */
static uint32_t libmem_CRC32 (libmem_driver_handle_t *h, const uint8_t *start, size_t size, uint32_t crc)
{
	(void)h;
	crc = libmem_crc32_direct (start, size, crc);
	return crc;
}


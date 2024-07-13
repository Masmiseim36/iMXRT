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


#include "libmem_driver_xSPI.h"
#include "libmem_Tools.h"
#include "fsl_device_registers.h"
#include "fsl_flexspi.h"
#include "fsl_common.h"
#include "fsl_clock.h"
#if __has_include("fsl_power.h")
	#include "fsl_power.h"
#endif
#include "DebugPrint.h"

#include "FlexSPI_Generic.h"
#include "FlexSPI_Adesto.h"
#include "FlexSPI_Macronix.h"
#include "FlexSPI_Winbond.h"
#include "FlexSPI_ISSI.h"


static flexspi_device_config_t deviceconfig
{
	.flexspiRootClk       = 0, // SPI root clock (will be set up later)
	.isSck2Enabled        = false,
	.flashSize            = BOARD_FLASH_SIZE / 1024, // expressed in KByte, will be reset after read the JEDEC information
	.CSIntervalUnit       = kFLEXSPI_CsIntervalUnit1SckCycle,
	.CSInterval           = 2,
	.CSHoldTime           = 3,
	.CSSetupTime          = 3,
	.dataValidTime        = 2,
	.columnspace          = 0, // we don't use columns
	.enableWordAddress    = false,
	.AWRSeqIndex          = 0, // LUT_ProgramPage,
	.AWRSeqNumber         = 0, // 1
	.ARDSeqIndex          = LUT_ReadArray,
	.ARDSeqNumber         = 1,
	.AHBWriteWaitUnit     = kFLEXSPI_AhbWriteWaitUnit2AhbCycle,
	.AHBWriteWaitInterval = 0,
	.enableWriteMask      = false,
	#if defined(FSL_FEATURE_FLEXSPI_HAS_ERRATA_051426) && (FSL_FEATURE_FLEXSPI_HAS_ERRATA_051426)
		.isFroClockSource = false //!<  \brief Is FRO clock source or not.
	#endif
};

// Define the structure of the Flash (Sector Count and Size)
static libmem_geometry_t geometry[]
{ // count - size
	{0x2000, 4096},
	{0, 0} 
};

static status_t EraseChip     (FlexSPI_Helper *base);
static int EraseSector        (libmem_driver_handle_t *h, libmem_sector_info_t *si);
static int ProgramPage        (libmem_driver_handle_t *h, uint8_t *dest_addr, const uint8_t *src_addr);


static int libmem_ProgramPage (libmem_driver_handle_t *h, uint8_t *dest, const uint8_t *src, size_t size);
static int libmem_EraseSector (libmem_driver_handle_t *h, uint8_t *start, size_t size, uint8_t **erase_start, size_t *erase_size);
static int libmem_Flush       (libmem_driver_handle_t *h);
static int libmem_Read        (libmem_driver_handle_t *h, uint8_t *dest, const uint8_t *src, size_t size);
static uint32_t libmem_CRC32  (libmem_driver_handle_t *h, const uint8_t *start, size_t size, uint32_t crc);


static const libmem_driver_functions_t DriverFunctions
{
	libmem_ProgramPage,
	nullptr,
	libmem_EraseSector,
	nullptr,
	nullptr,
	libmem_Flush
};

static const libmem_ext_driver_functions_t DriverFunctions_Extended
{
	nullptr,
	libmem_Read,
	libmem_CRC32
};



/*! Libmem_InitializeDriver_xSPI:
\brief Initialize the FlexSPI interface for using as a SPI-interface
\param FlashHandle The handle which should be initialized
\param base The Flex-SPI-base to use
\param MemType The Type of SPI-interface to use if possible
\return LibmemStatus_t LibmemStaus_Success if the operation was successfully */
LibmemStatus_t Libmem_InitializeDriver_xSPI (FlexSPI_Helper *base, enum MemoryType MemType)
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
		CLOCK_InitUsb1Pfd (kCLOCK_Pfd0, 18);	// Set PLL3 PFD0 clock 480MHZ (480*18/24). PLL3 --> USB1-PLL --> PLL480
		const uint32_t SourceClock_Hz = CLOCK_GetUsb1PfdFreq (kCLOCK_Pfd0);

		uint32_t ClockDiv = 6; // flexspi clock divide by six --> 80Mz.
		uint32_t ClockHz = SourceClock_Hz / ClockDiv;
		clock_div_t FlexSPIDiv = kCLOCK_FlexspiDiv;
		switch (base->GetBaseAddr())
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
		switch (base->GetBaseAddr())
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

	// Need to set the combination-enable option. This options combines 8 data lines from FlexSPI channel A with
	// 4 data lines from FlexSPI channel B to form an 8-line bus for octal. On this SoC this is the only way to enable octal.
	#if !(defined(FSL_FEATURE_FLEXSPI_HAS_NO_MCR0_COMBINATIONEN) && FSL_FEATURE_FLEXSPI_HAS_NO_MCR0_COMBINATIONEN)
		if (GetPortWidth (base) != 8)
			config.enableCombination      = (MemType == MemType_OctaSPI_DDR || MemType == MemType_OctaSPI);	// Only true when using Octa-Mode
	#endif
	config.ahbConfig.enableAHBPrefetch    = true;	// Enable AHB prefetching
	config.ahbConfig.enableAHBBufferable  = true;
	config.ahbConfig.enableReadAddressOpt = true;
	config.ahbConfig.enableAHBCachable    = true;
	config.rxSampleClock                  = kFLEXSPI_ReadSampleClkLoopbackInternally;

	FLEXSPI_Init           (base, &config);
	FLEXSPI_SetFlashConfig (base, &deviceconfig, FlexSPI_Helper::port);   // Configure flash settings according to serial flash feature.
	FLEXSPI_UpdateLUT      (base, 0, &Generic::LUT_SPI.front(), Generic::LUT_SPI.size()); // Update LUT table
	FLEXSPI_SoftwareReset  (base);                                        // Do software reset.

	// Get the JEDEC Informations
	DeviceInfo info {};
	status_t status {};
	static const std::array<status_t (*) (FlexSPI_Helper &, DeviceInfo &), 3> JedecReader
	{
		[](FlexSPI_Helper &b, DeviceInfo &i){return b.ReadJEDEC (&i);},
		Macronix::TryDetect,
		ISSI::TryDetect
	};
	for (auto reader : JedecReader)
	{
		status = reader (*base, info);
		if (status == kStatus_Success && info.ManufactureID != ManufactureID_UNDEF)
			break;
	}

	if (status != kStatus_Success || info.ManufactureID == ManufactureID_UNDEF)
	{
		DebugPrint ("JEDEC read Error\r\n");
		return LibmemStaus_InvalidDevice;
	}

	// Check for the Manufacture-ID and adapt the Configuration
	LibmemStatus_t res = LibmemStaus_Success;
	switch (info.ManufactureID)
	{
		case ManufactureID_AdestoTechnologies:
		case ManufactureID_Atmel:		// Renesas
			res = Adesto::Initialize (*base, MemType, info, config, deviceconfig);
			break;
		case ManufactureID_Nexcom:		// Winbond
			res = Winbond::Initialize (*base, MemType, info, config, deviceconfig);
			break;
		case ManufactureID_Macronix:	// Macronix
			res = Macronix::Initialize (*base, MemType, info, config, deviceconfig);
			break;
		case ManufactureID_Lucent:		// ISSI
			res = ISSI::Initialize (*base, MemType, info, config, deviceconfig);
			break;
		default:
			DebugPrint ("unknown Flash-memory\r\n");
			return LibmemStaus_InvalidDevice;
	}

	if (res != LibmemStaus_Success)
		return res;

	// Reconfigure the interface according to the gathered flash information and configuration
	if (MemType == MemType_OctaSPI_DDR || MemType == MemType_QuadSPI_DDR)
	{
		#if (defined(MIMXRT533S_SERIES)   || defined(MIMXRT555S_SERIES) || defined(MIMXRT595S_cm33_SERIES))
			ClockDiv = 2;
			if (base == FLEXSPI0)
			{
				CLKCTL0->FLEXSPI0FCLKDIV = CLKCTL0_FLEXSPI0FCLKDIV_DIV (ClockDiv - 1);
				while ((CLKCTL0->FLEXSPI0FCLKDIV) & CLKCTL0_FLEXSPI0FCLKDIV_REQFLAG_MASK)
					;
				deviceconfig.flexspiRootClk = CLOCK_GetFlexspiClkFreq (0);
			}
			else if (base == FLEXSPI1)
			{
				CLKCTL0->FLEXSPI1FCLKDIV = CLKCTL0_FLEXSPI1FCLKDIV_DIV (ClockDiv - 1);
				while ((CLKCTL0->FLEXSPI1FCLKDIV) & CLKCTL0_FLEXSPI1FCLKDIV_REQFLAG_MASK)
					;
				deviceconfig.flexspiRootClk = CLOCK_GetFlexspiClkFreq (1);
			}
			else
				return LibmemStaus_Error;
			config.rxSampleClock = kFLEXSPI_ReadSampleClkLoopbackInternally; // To achieve high speeds - always use DQS
		#elif (defined(MIMXRT633S_SERIES) || defined(MIMXRT685S_cm33_SERIES))
			ClockDiv = 9;
			CLKCTL0->FLEXSPIFCLKDIV = CLKCTL0_FLEXSPIFCLKDIV_DIV (ClockDiv - 1);
			while ((CLKCTL0->FLEXSPIFCLKDIV) & CLKCTL0_FLEXSPIFCLKDIV_REQFLAG_MASK)
				;
			deviceconfig.flexspiRootClk = CLOCK_GetFlexspiClkFreq ();
			config.rxSampleClock = kFLEXSPI_ReadSampleClkLoopbackInternally; // To achieve high speeds - always use DQS
		#elif (defined(MIMXRT1011_SERIES) || defined(MIMXRT1015_SERIES) || defined(MIMXRT1021_SERIES) || defined(MIMXRT1024_SERIES) || \
			   defined(MIMXRT1041_SERIES) || defined(MIMXRT1042_SERIES) || defined(MIMXRT1051_SERIES) || defined(MIMXRT1052_SERIES) || \
			   defined(MIMXRT1061_SERIES) || defined(MIMXRT1062_SERIES) || defined(MIMXRT1064_SERIES))
			ClockDiv = 2;
			CLOCK_SetDiv (FlexSPIDiv, ClockDiv-1);	// flexspi clock divide by two --> 240 MHz.
			deviceconfig.flexspiRootClk = SourceClock_Hz / ClockDiv;
			deviceconfig.flexspiRootClk = ClockHz;
		#elif (defined(MIMXRT1165_cm7_SERIES) || defined(MIMXRT1166_cm7_SERIES) || defined(MIMXRT1165_cm4_SERIES) || defined(MIMXRT1166_cm4_SERIES) || \
			   defined(MIMXRT1171_SERIES)     || defined(MIMXRT1172_SERIES)     || \
			   defined(MIMXRT1173_cm7_SERIES) || defined(MIMXRT1175_cm7_SERIES) || defined(MIMXRT1176_cm7_SERIES) || \
			   defined(MIMXRT1173_cm4_SERIES) || defined(MIMXRT1175_cm4_SERIES) || defined(MIMXRT1176_cm4_SERIES))
			CLOCK_ControlGate (FlexSPIClockGate, kCLOCK_Off);	// The module clock must be disabled during clock switch in order to avoid glitch
			CLOCK_SetRootClockMux (FlexSPIClock, 5); // ClockSource_SysPll2 --> 528 MHz
			CLOCK_SetRootClockDiv (FlexSPIClock, 2); // --> 528 MHz / 2 = ~264 MHz
			CLOCK_ControlGate (FlexSPIClockGate, kCLOCK_On);
			deviceconfig.flexspiRootClk = CLOCK_GetRootClockFreq (FlexSPIClock);
		#elif (defined(MIMXRT1181_SERIES)     || defined(MIMXRT1182_SERIES)     || defined(MIMXRT1187_cm7_SERIES) || defined(MIMXRT1187_cm33_SERIES) ||\
			   defined(MIMXRT1189_cm7_SERIES) || defined(MIMXRT1189_cm33_SERIES))
			switch (reinterpret_cast<uint32_t>(base))
			{
				case FLEXSPI1_BASE:
					CLOCK_SetRootClockDiv (kCLOCK_Root_Flexspi1, 2); // --> 392,7s MHz / 2 = ~196,35 MHz
					CLOCK_SetRootClockMux (kCLOCK_Root_Flexspi1, kCLOCK_FLEXSPI1_ClockRoot_MuxSysPll3Pfd0); // ClockSource_SysPll2 --> 392,7s MHz
					ClockHz = CLOCK_GetRootClockFreq (kCLOCK_Root_Flexspi1);
					break;
				case FLEXSPI2_BASE:
					CLOCK_SetRootClockDiv (kCLOCK_Root_Flexspi2, 2); // --> 392,7s MHz / 2 = ~196,35 MHz
					CLOCK_SetRootClockMux (kCLOCK_Root_Flexspi2, kCLOCK_FLEXSPI2_ClockRoot_MuxSysPll3Pfd2); // ClockSource_SysPll2 --> 392,7s MHz
					ClockHz = CLOCK_GetRootClockFreq (kCLOCK_Root_Flexspi2);
					break;
				default:
					return LibmemStaus_InvalidDevice;
			}
		#else
			#error "unknon controller family"
		#endif
		FLEXSPI_Init (base, &config); // changing the clock requires reinitialization
	}

	// Use the size information from the JEDEC-information to configure the interface
	uint32_t FlashSize = CalculateCapacity_KBytes (info.Capacity);
	deviceconfig.flashSize = FlashSize;
	geometry[0].count = FlashSize / (4096 / 1024);
	FlashSize *= 1024;	// Convert kBytes to bytes

	FLEXSPI_SetFlashConfig (base, &deviceconfig, FlexSPI_Helper::port);	// Configure flash settings according to serial flash feature.

	FLEXSPI_SoftwareReset (base);
	status_t stat = base->WaitBusBusy ();
	if (stat != kStatus_Success)
		return LibmemStaus_Error;


	static uint8_t write_buffer[QSPIFLASH_PAGE_SIZE];
	LibmemDriver *FlashHandle = LibmemDriver::GetDriver ();
//	libmem_register_driver (FlashHandle, GetBaseAddress(base), FlashSize, geometry, nullptr, &DriverFunctions, &DriverFunctions_Extended);
	libmem_register_driver (FlashHandle, GetBaseAddress(base), FlashSize, geometry, nullptr, &DriverFunctions, nullptr);
	int err = libmem_driver_paged_write_init (&FlashHandle->PageWriteControlBlock, write_buffer, QSPIFLASH_PAGE_SIZE, ProgramPage, 4, 0);
	FlashHandle->user_data = (uint32_t)base;

	uint8_t *AliasAddress = base->GetAliasBaseAddress ();
	if (AliasAddress != nullptr && err == LIBMEM_STATUS_SUCCESS)
	{
		FlashHandle = LibmemDriver::GetDriver ();
		libmem_register_driver (FlashHandle, AliasAddress, FlashSize, geometry, nullptr, &DriverFunctions, nullptr);
		err = libmem_driver_paged_write_init (&FlashHandle->PageWriteControlBlock, write_buffer, QSPIFLASH_PAGE_SIZE, ProgramPage, 4, 0);
		FlashHandle->user_data = (uint32_t)base;
		DebugPrint ("### Add Driver for Alias\r\n");
	}
	return static_cast<LibmemStatus_t>(err);
}


/*! EraseChip:
\brief Erase the whole-Flash-memory
\param base The FlexSPI-Interface where the Flash is located which should be erased
\return static status_t Status of the Operation - kStatus_Success when successfully */
[[maybe_unused]] static status_t EraseChip (FlexSPI_Helper *base)
{
	DebugPrint ("EraseChip\r\n");

	status_t stat = base->WriteEnable (0);
	if (stat != kStatus_Success)
		return stat;

	stat = base->SendCommand (0, LUT_EraseChip);
	if (stat != kStatus_Success)
		return LIBMEM_STATUS_ERROR;

	return base->WaitBusBusy ();
}

/*! EraseSector:
\brief Erase a sector of the Flash-Memory
\param h Handle to the Flash-Driver
\param si Information about the sector which should be erased
\return static int LibmemStaus_Success when the erase operation was successfully, otherwise LibmemStaus_Error */
static int EraseSector (libmem_driver_handle_t *h, libmem_sector_info_t *si)
{
	if (IsSectorEmpty (reinterpret_cast<uint32_t *>(si->start)))
	{
		DebugPrintf ("EraseSector at 0x%x, is allready erased\r\n", si->start);
		return LIBMEM_STATUS_SUCCESS;
	}

	FlexSPI_Helper *base = reinterpret_cast<FlexSPI_Helper *>(h->user_data);
	uint32_t SectorAddr = libmem_CalculateOffset (h, si->start);
	if (SectorAddr == UINT32_MAX)
		return LibmemStaus_Error;

	DebugPrintf ("EraseSector at 0x%x, size: %d\r\n", SectorAddr, si->size);

	status_t status = base->WriteEnable (SectorAddr);
	if (status != kStatus_Success)
		return LibmemStaus_Error;

	status = base->SendCommand (SectorAddr, LUT_EraseSector);
	if (status != kStatus_Success)
		return LibmemStaus_Error;

	status = base->WaitBusBusy ();
	if (status != kStatus_Success)
		return LibmemStaus_Error;

	return LibmemStaus_Success;
}

/*! ProgramPage:
\brief Write Data to a Flash-Page
\param h Handle to the Flash-Driver
\param Destination Address to write the Data to. This Address is in the Address-Range of the Controller
\param Source Address of the Array with the data to write
\return static int LibmemStaus_Success when the write operation was successfully, otherwise LibmemStaus_Error */
static int ProgramPage (libmem_driver_handle_t *h, uint8_t *dest_addr, const uint8_t *src_addr)
{
	FlexSPI_Helper *base = reinterpret_cast<FlexSPI_Helper *>(h->user_data);
	uint32_t DeviceAddress = libmem_CalculateOffset (h, dest_addr);
	if (DeviceAddress == UINT32_MAX)
		return LibmemStaus_Error;

	DebugPrintf ("ProgramPage at 0x%X\r\n", DeviceAddress);

	// Write enable
	status_t status = base->WriteEnable (DeviceAddress);
	if (status != kStatus_Success)
		return LibmemStaus_Error;

	// Prepare page program command
	flexspi_transfer_t flashXfer
	{
		DeviceAddress,				// deviceAddress	- Operation device address.
		FlexSPI_Helper::port,		// port				- Operation port
		kFLEXSPI_Write,				// cmdType			- Execution command type.
		LUT_ProgramPage,			// seqIndex			- Sequence ID for command.
		1,							// SeqNumber		- Sequence number for command.
		(uint32_t *)src_addr,		// data				- Data buffer.
		QSPIFLASH_PAGE_SIZE			// dataSize			- Data size in bytes.
	};
	status = FLEXSPI_TransferBlocking (base, &flashXfer);
	if (status != kStatus_Success)
		return LibmemStaus_Error;

	status = base->WaitBusBusy ();
	if (status != kStatus_Success)
		return LibmemStaus_Error;

	// Do software reset or clear AHB buffer directly depending on the device capabilities
	#if defined(FSL_FEATURE_SOC_OTFAD_COUNT) && defined(FLEXSPI_AHBCR_CLRAHBRXBUF_MASK) && defined(FLEXSPI_AHBCR_CLRAHBTXBUF_MASK)
		base->AHBCR |= FLEXSPI_AHBCR_CLRAHBRXBUF_MASK | FLEXSPI_AHBCR_CLRAHBTXBUF_MASK;
		base->AHBCR &= ~(FLEXSPI_AHBCR_CLRAHBRXBUF_MASK | FLEXSPI_AHBCR_CLRAHBTXBUF_MASK);
	#else
		FLEXSPI_SoftwareReset (base);
	#endif
	return LibmemStaus_Success;
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
	DebugPrintf ("libmem_EraseSector at 0x%x - size: %d\r\n", start, size);
	int ret = libmem_foreach_sector_in_range (h, start, size, EraseSector, erase_start, erase_size);

	FlexSPI_Helper *base = reinterpret_cast<FlexSPI_Helper *>(h->user_data);
	FLEXSPI_SoftwareReset (base);

	return ret;
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
	DebugPrintf ("Read at 0x%x, size: %d\r\n", src, size);
	if (size == 0)
		return LibmemStaus_InvalidParameter;
//	memcpy (dest, src, size);

	flexspi_transfer_t flashXfer
	{
		(uint32_t)src,		  // Operation device address.
		FlexSPI_Helper::port, // Operation port.
		kFLEXSPI_Read,		  // Execution command type.
		LUT_ReadArray,		  // Sequence ID for command.
		1,					  // Sequence number for command.
		(uint32_t *)dest,	  // Data buffer.
		size				  // Data size in bytes.
	};

	FlexSPI_Helper *base = reinterpret_cast<FlexSPI_Helper *>(h->user_data);
	status_t status = FLEXSPI_TransferBlocking (base, &flashXfer);
	if (status != kStatus_Success)
		return status;

	return LibmemStaus_Success;
}


/*! libmem_CRC32:
\brief The LIBMEM driver's crc32 extended function.
\param h     A pointer to the handle of the LIBMEM driver.
\param start A pointer to the start of the address range.
\param size  The size of the address range in bytes.
\param crc   The initial CRC-32 value.
\return uint32_t The computed CRC-32 value. */
static uint32_t libmem_CRC32 ([[maybe_unused]]libmem_driver_handle_t *h, const uint8_t *start, size_t size, uint32_t crc)
{
	DebugPrintf ("Calculate CRC from 0x%X, size 0x%X, calculated CRC: 0x%X\r\n", start, size, crc);
	static uint8_t page_buffer[4096];
	static constexpr size_t BufferSize = sizeof (page_buffer);

	while (size >= BufferSize)
	{
		libmem_Read(h, page_buffer, (uint8_t*)start, BufferSize);
		crc = libmem_crc32_direct(page_buffer,  BufferSize, crc);
		start += BufferSize;
		size  -= BufferSize;
	}
	if (size)
	{
		libmem_Read(h, page_buffer, (uint8_t*)start, BufferSize);
		crc = libmem_crc32_direct(page_buffer, size, crc);
	}
	return crc;
}


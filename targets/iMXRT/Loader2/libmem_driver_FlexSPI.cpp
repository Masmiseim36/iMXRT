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


#include "libmem_driver_FlexSPI.h"
#if defined FLEXSPI || defined FLEXSPI0 || defined FLEXSPI1
#include "libmem_Tools.h"
#include "libmem_callbacks.h"
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
#include "FlexSPI_Spansion.h"
#include "FlexSPI_Micron.h"
#include "FlexSPI_Helper.h"

namespace Xspi
{
	static const flexspi_device_config_t Deviceconfig
	{
		.flexspiRootClk       = 0, // SPI root clock (will be set up later)
		.isSck2Enabled        = false,
		.flashSize            = 1024 * 8, // expressed in KByte, set a dummy value for now, will be changed after read the JEDEC information. This is necessarry for reading the JEDEC information
		#if defined(FSL_FEATURE_FLEXSPI_SUPPORT_ADDRESS_SHIFT) && (FSL_FEATURE_FLEXSPI_SUPPORT_ADDRESS_SHIFT)
			.addressShift     = 0,
		#endif
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


	static const flexspi_device_config_t Deviceconfig_Hyperram
	{
		.flexspiRootClk       = 0, // SPI root clock (will be set up later)
		.isSck2Enabled        = false,
		.flashSize            = 1024 * 8, // expressed in KByte, set a dummy value for now, will be changed after read the JEDEC information. This is necessarry for reading the JEDEC information
		#if defined(FSL_FEATURE_FLEXSPI_SUPPORT_ADDRESS_SHIFT) && (FSL_FEATURE_FLEXSPI_SUPPORT_ADDRESS_SHIFT)
			.addressShift     = 0,
		#endif
		.CSIntervalUnit       = kFLEXSPI_CsIntervalUnit1SckCycle,
		.CSInterval           = 0,
		.CSHoldTime           = 3,
		.CSSetupTime          = 3,
		.dataValidTime        = 2, // DLLCR_OVRDVAL und sollte 0 sein
		.columnspace          = 3,
		.enableWordAddress    = true,
		.AWRSeqIndex          = 9,
		.AWRSeqNumber         = 1,
		.ARDSeqIndex          = 0,
		.ARDSeqNumber         = 1,
		.AHBWriteWaitUnit     = kFLEXSPI_AhbWriteWaitUnit2AhbCycle,
		.AHBWriteWaitInterval = 0,
		.enableWriteMask      = true,
		#if defined(FSL_FEATURE_FLEXSPI_HAS_ERRATA_051426) && (FSL_FEATURE_FLEXSPI_HAS_ERRATA_051426)
			.isFroClockSource = false //!<  \brief Is FRO clock source or not.
		#endif
	};
}

namespace Hyperflash
{
	static const flexspi_device_config_t Deviceconfig
	{
		.flexspiRootClk       = 0, // SPI root clock (will be set up later)
		.isSck2Enabled        = false,
		.flashSize            = 0x4000000 / 1024,	// Size in KBytes
		#if defined(FSL_FEATURE_FLEXSPI_SUPPORT_ADDRESS_SHIFT) && (FSL_FEATURE_FLEXSPI_SUPPORT_ADDRESS_SHIFT)
			.addressShift     = 0,
		#endif
		.CSIntervalUnit       = kFLEXSPI_CsIntervalUnit1SckCycle,
		.CSInterval           = 2,
		.CSHoldTime           = 0,
		.CSSetupTime          = 3,
		.dataValidTime        = 1,
		.columnspace          = 3,
		.enableWordAddress    = true,
		.AWRSeqIndex          = static_cast<uint8_t>(Spansion::Command::WriteData),
		.AWRSeqNumber         = 1,
		.ARDSeqIndex          = static_cast<uint8_t>(Spansion::Command::ReadData),
		.ARDSeqNumber         = 1,
		.AHBWriteWaitUnit     = kFLEXSPI_AhbWriteWaitUnit2AhbCycle,
		.AHBWriteWaitInterval = 20,
		.enableWriteMask      = false,
		#if defined(FSL_FEATURE_FLEXSPI_HAS_ERRATA_051426) && (FSL_FEATURE_FLEXSPI_HAS_ERRATA_051426)
			.isFroClockSource = false //!<  \brief Is FRO clock source or not.
		#endif
	};
}


namespace
{
	// Define the structure of the Flash (Sector Count and Size)
	libmem_geometry_t geometry []
	{ // count - size
		{0x2000, 4096},
		{0, 0} 
	};

	inline int GetPageSize (MemoryType memType)
	{
		if (memType == MemoryType::Hyperflash)
			return HYPERFLASH_PAGE_SIZE;

		return QSPIFLASH_PAGE_SIZE;

	}
}


/*! Libmem_InitializeDriver_xSPI:
\brief Initialize the FlexSPI interface for using as a SPI-interface
\param base The Flex-SPI-base to use
\param memType The Type of SPI-interface to use if possible
\return LibmemStatus_t LibmemStaus_Success if the operation was successfully */
LibmemStatus_t Libmem_InitializeDriver_xSPI (FlexSPI_Helper *base, MemoryType memType)
{
	#if (defined(MIMXRT533S_SERIES)  || defined(MIMXRT555S_SERIES) || defined(MIMXRT595S_cm33_SERIES))
		constexpr uint32_t src {2};	// Use AUX0_PLL as clock source for the FlexSPI
		uint32_t clockDiv {4};		// with a divider of four
		uint32_t ClockHz  {};
		if (base == FLEXSPI0 && 
			(CLKCTL0->FLEXSPI0FCLKSEL != CLKCTL0_FLEXSPI0FCLKSEL_SEL(src) || (CLKCTL0->FLEXSPI0FCLKDIV & CLKCTL0_FLEXSPI0FCLKDIV_DIV_MASK) != (clockDiv - 1)))
		{
			#if !defined(FSL_SDK_DRIVER_QUICK_ACCESS_ENABLE)
				POWER_DisablePD(kPDRUNCFG_APD_FLEXSPI0_SRAM);
				POWER_DisablePD(kPDRUNCFG_PPD_FLEXSPI0_SRAM);
				POWER_ApplyPD();
			#endif

//			CLKCTL0->PSCCTL0_CLR = CLKCTL0_PSCCTL0_CLR_FLEXSPI_OTFAD_CLK_MASK;	// Disable clock before changing clock source
			CLKCTL0->FLEXSPI0FCLKSEL  = CLKCTL0_FLEXSPI0FCLKSEL_SEL(src);		// Update flexspi clock.
			CLKCTL0->FLEXSPI0FCLKDIV |= CLKCTL0_FLEXSPI0FCLKDIV_RESET_MASK;		// Reset the divider counter
			CLKCTL0->FLEXSPI0FCLKDIV  = CLKCTL0_FLEXSPI0FCLKDIV_DIV(clockDiv - 1);
			while ((CLKCTL0->FLEXSPI0FCLKDIV) & CLKCTL0_FLEXSPI0FCLKDIV_REQFLAG_MASK)
				;
//			CLKCTL0->PSCCTL0_SET = CLKCTL0_PSCCTL0_SET_FLEXSPI_OTFAD_CLK_MASK;	// Enable FLEXSPI clock again
			ClockHz = CLOCK_GetFlexspiClkFreq (0);
		}
		else if (base == FLEXSPI1 &&
			(CLKCTL0->FLEXSPI1FCLKSEL != CLKCTL0_FLEXSPI1FCLKSEL_SEL(src) || (CLKCTL0->FLEXSPI1FCLKDIV & CLKCTL0_FLEXSPI1FCLKDIV_DIV_MASK) != (clockDiv - 1)))
		{
			#if !defined(FSL_SDK_DRIVER_QUICK_ACCESS_ENABLE)
				POWER_DisablePD(kPDRUNCFG_APD_FLEXSPI1_SRAM);
				POWER_DisablePD(kPDRUNCFG_PPD_FLEXSPI1_SRAM);
				POWER_ApplyPD();
			#endif

//			CLKCTL0->PSCCTL0_CLR = CLKCTL0_PSCCTL0_CLR_FLEXSPI_OTFAD_CLK_MASK;	// Disable clock before changing clock source
			CLKCTL0->FLEXSPI1FCLKSEL  = CLKCTL0_FLEXSPI1FCLKSEL_SEL(src);		// Update flexspi clock.
			CLKCTL0->FLEXSPI1FCLKDIV |= CLKCTL0_FLEXSPI1FCLKDIV_RESET_MASK;		// Reset the divider counter
			CLKCTL0->FLEXSPI1FCLKDIV  = CLKCTL0_FLEXSPI1FCLKDIV_DIV(clockDiv - 1);
			while ((CLKCTL0->FLEXSPI1FCLKDIV) & CLKCTL0_FLEXSPI1FCLKDIV_REQFLAG_MASK)
				;
//			CLKCTL0->PSCCTL0_SET = CLKCTL0_PSCCTL0_SET_FLEXSPI_OTFAD_CLK_MASK;	// Enable FLEXSPI clock again
			ClockHz = CLOCK_GetFlexspiClkFreq (1);
		}
	#elif (defined(MIMXRT633S_SERIES) || defined(MIMXRT685S_cm33_SERIES))
		// Clock Source
		// 0 --> Main Clock.
		// 1 --> Main PLL Clock (main_pll_clk).
		// 2 --> AUX0 PLL clock (aux0_pll_clk).
		// 3 --> FFRO Clock.
		// 4 --> AUX1 PLL clock (aux1_pll_clk).
		constexpr uint32_t src = 2;	// Use AUX0_PLL as clock source for the FlexSPI --> 396.0 MHz
		uint32_t clockDiv = 8;		// with a divider of eight                       ->  49.5 MHz
		if (CLKCTL0->FLEXSPIFCLKSEL != CLKCTL0_FLEXSPIFCLKSEL_SEL(src) || (CLKCTL0->FLEXSPIFCLKDIV & CLKCTL0_FLEXSPIFCLKDIV_DIV_MASK) != (clockDiv - 1))
		{
			#if !defined(FSL_SDK_DRIVER_QUICK_ACCESS_ENABLE)
				POWER_DisablePD(kPDRUNCFG_APD_FLEXSPI_SRAM);
				POWER_DisablePD(kPDRUNCFG_PPD_FLEXSPI_SRAM);
				POWER_ApplyPD();
			#endif

			CLKCTL0->PSCCTL0_CLR     = CLKCTL0_PSCCTL0_CLR_FLEXSPI_OTFAD_CLK_MASK;	// Disable clock before changing clock source
			CLKCTL0->FLEXSPIFCLKSEL  = CLKCTL0_FLEXSPIFCLKSEL_SEL(src);				// Update flexspi clock.
			CLKCTL0->FLEXSPIFCLKDIV |= CLKCTL0_FLEXSPIFCLKDIV_RESET_MASK;			// Reset the divider counter
			CLKCTL0->FLEXSPIFCLKDIV  = CLKCTL0_FLEXSPIFCLKDIV_DIV(clockDiv - 1);
			while ((CLKCTL0->FLEXSPIFCLKDIV) & CLKCTL0_FLEXSPIFCLKDIV_REQFLAG_MASK)
				;
			CLKCTL0->PSCCTL0_SET = CLKCTL0_PSCCTL0_SET_FLEXSPI_OTFAD_CLK_MASK;	// Enable FLEXSPI clock again
		}
		uint32_t ClockHz = CLOCK_GetFlexspiClkFreq (); // 49.5 MHz
		//uint32_t sourceClock_Hz = ClockHz * clockDiv;
	#elif (defined(MIMXRT1011_SERIES) || defined(MIMXRT1015_SERIES) || defined(MIMXRT1021_SERIES) || defined(MIMXRT1024_SERIES) || \
		   defined(MIMXRT1041_SERIES) || defined(MIMXRT1042_SERIES) || defined(MIMXRT1051_SERIES) || defined(MIMXRT1052_SERIES) || \
		   defined(MIMXRT1061_SERIES) || defined(MIMXRT1062_SERIES) || defined(MIMXRT1064_SERIES))
		const clock_usb_pll_config_t configUsbPll {.loopDivider = 0U, .src=0};
		CLOCK_InitUsb1Pll (&configUsbPll);		// PLL3 --> USB1-PLL --> PLL480
		CLOCK_InitUsb1Pfd (kCLOCK_Pfd0, 18);	// Set PLL3 PFD0 clock 480MHZ (480*18/24) --> 360 MHz
		const uint32_t sourceClock_Hz = CLOCK_GetUsb1PfdFreq (kCLOCK_Pfd0);

		uint32_t clockDiv = 6; // flexspi clock divide by six --> 60 MHz.
		uint32_t ClockHz = sourceClock_Hz / clockDiv;
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
		CLOCK_SetDiv (FlexSPIDiv, clockDiv-1);
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

		const uint32_t ClockHz = CLOCK_GetRootClockFreq (FlexSPIClock);
	#elif (defined(MIMXRT1181_SERIES)      || defined(MIMXRT1182_SERIES)     || defined(MIMXRT118C_cm33_SERIES) || defined(MIMXRT118C_cm7_SERIES) || \
		   defined(MIMXRT1185_cm33_SERIES) || defined(MIMXRT1185_cm7_SERIES) || defined(MIMXRT1186_cm33_SERIES) || defined(MIMXRT1186_cm7_SERIES) || \
		   defined(MIMXRT1187_cm33_SERIES) || defined(MIMXRT1187_cm7_SERIES) || defined(MIMXRT1189_cm33_SERIES) || defined(MIMXRT1189_cm7_SERIES))
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

	flexspi_device_config_t deviceconfig {Xspi::Deviceconfig};
	const libmem_driver_functions_t *pDriverFunctions {&Xspi::DriverFunctions};
	libmem_driver_page_write_fn_t ProgramPage {Xspi::ProgramPage};
	if (memType == MemoryType::Hyperflash)
	{
		deviceconfig     =  Hyperflash::Deviceconfig;
		pDriverFunctions = &Hyperflash::DriverFunctions;
		ProgramPage      =  Hyperflash::ProgramPage;
	}
	else if (memType == MemoryType::Hyperram)
	{
		deviceconfig     =  Xspi::Deviceconfig_Hyperram;
	}
	deviceconfig.flexspiRootClk = ClockHz;

	// Get FLEXSPI default settings and configure the FlexSPI.
	flexspi_config_t config {};
	FLEXSPI_GetDefaultConfig (&config);

	// Need to set the combination-enable option. This options combines 8 data lines from FlexSPI channel A with
	// 4 data lines from FlexSPI channel B to form an 8-line bus for octal. On this SoC this is the only way to enable octal.
	#if !(defined(FSL_FEATURE_FLEXSPI_HAS_NO_MCR0_COMBINATIONEN) && FSL_FEATURE_FLEXSPI_HAS_NO_MCR0_COMBINATIONEN)
		if (GetPortWidth (base) != 8)
		{
			// Only true when using Octa-Mode
			config.enableCombination = (memType == MemoryType::OctaSPI_DDR || memType == MemoryType::OctaSPI || 
										memType == MemoryType::Hyperflash  || memType == MemoryType::Hyperram);
		}
	#endif
	config.ahbConfig.enableAHBPrefetch    = true;	// Enable AHB prefetching
	config.ahbConfig.enableReadAddressOpt = true;
	config.ahbConfig.enableAHBBufferable  = true;
	config.ahbConfig.enableAHBCachable    = true;
	#if !(defined(FSL_FEATURE_FLEXSPI_HAS_NO_MCR2_SCKBDIFFOPT) && FSL_FEATURE_FLEXSPI_HAS_NO_MCR2_SCKBDIFFOPT)
		config.enableSckBDiffOpt          = (memType == MemoryType::Hyperflash || memType == MemoryType::Hyperram);	// enable diff clock and DQS for hyperflash
	#endif
	#if (defined(MIMXRT633S_SERIES) || defined(MIMXRT685S_cm33_SERIES)) // ToDo - Is there  better way?
		config.rxSampleClock              = kFLEXSPI_ReadSampleClkLoopbackInternally;
	#else
		config.rxSampleClock              = kFLEXSPI_ReadSampleClkLoopbackFromDqsPad;
	#endif

	FLEXSPI_Init           (base, &config);
	FLEXSPI_SetFlashConfig (base, &deviceconfig, Transfer::Port());   // Configure flash settings according to serial flash feature.
	FLEXSPI_UpdateLUT      (base, 0, &Generic::LUT_SPI.front(), Generic::LUT_SPI.size()); // Update LUT table
	FLEXSPI_SoftwareReset  (base);                                        // Do software reset.

	// Get the Flash Informations by CFI or JEDEC depending on the interface type
	DeviceInfo info {};
	status_t status {};
	if (memType == MemoryType::Hyperflash)
	{
		// Read the CFI information
		union
		{
			uint32_t data32[18];
			uint16_t data16[36];
		} data{};
		base->UpdateLUT (Spansion::LUT_HyperFlash);
		FLEXSPI_SoftwareReset  (base);
		status = base->WriteRegister (0x555 * 2, 0x9800,                     static_cast<LUT_CommandOffsets>(Spansion::Command::WriteData), 2);
		status = base->Read          (0x10  * 2, data.data32,  sizeof(data), static_cast<LUT_CommandOffsets>(Spansion::Command::ReadData));
		status = base->WriteRegister (0,         0xF000,                     static_cast<LUT_CommandOffsets>(Spansion::Command::WriteData), 2);

		// Search for the Query Unique ASCII string "QRY" for Infinion Hyperflash
		size_t position {};
		constexpr size_t EndPos {std::size(data.data16)-23};
		for (; position < EndPos; position++)
		{
			if (data.data16[position] == 0x5100 || data.data16[position+1] == 0x5200 || data.data16[position+2] == 0x5900)
				break;
		}
		if (position >= EndPos)
			return LibmemStaus_InvalidDevice;

		info.Capacity      = static_cast<Capacity>(data.data16[position+23] >> 8U);
		info.ManufactureID = ManufactureID_Spansion; // Only Spansion/Infinion is supported
		info.Type          = 0;
		
		geometry[0].size = 0x40000; // 256 KByte / 2MBit

		config.rxSampleClock = kFLEXSPI_ReadSampleClkExternalInputFromDqsPad; // To achieve high speeds - always use DQS
	}
	else if (memType == MemoryType::Hyperram)
	{
		base->UpdateLUT (Generic::LUT_Hyperram);
		info.Capacity      = static_cast<Capacity>(Capacity_256MBit);
		config.rxWatermark = 64;
		config.txWatermark = 0;
		config.enableDoze  = false;
		#if !(defined(FSL_FEATURE_FLEXSPI_HAS_NO_MCR0_COMBINATIONEN) && FSL_FEATURE_FLEXSPI_HAS_NO_MCR0_COMBINATIONEN)
		config.enableCombination = true;
		#endif
		config.rxSampleClock = kFLEXSPI_ReadSampleClkExternalInputFromDqsPad; // To achieve high speeds - always use DQS
	}
	else
	{
		// Get the JEDEC Informations
		static const std::array<status_t (*) (FlexSPI_Helper &, DeviceInfo &), 4> JedecReader
		{
			[](FlexSPI_Helper &b, DeviceInfo &i){return b.ReadJEDEC (&i);},
			Macronix::TryDetect,
			ISSI::TryDetect,
			Micron::TryDetect
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
				res = Adesto::Initialize (*base, memType, info);
				break;
			case ManufactureID_Nexcom:		// Winbond
				res = Winbond::Initialize (*base, memType, info);
				break;
			case ManufactureID_Macronix:	// Macronix
				res = Macronix::Initialize (*base, memType, info);
				break;
			case ManufactureID_Lucent:		// ISSI
				res = ISSI::Initialize (*base, memType, info);
				break;
			case ManufactureID_MicronTechnology:
				res = Micron::Initialize (*base, memType, info);
				break;
			default:
				DebugPrint ("unknown Flash-memory\r\n");
				return LibmemStaus_InvalidDevice;
		}

		if (res != LibmemStaus_Success)
			return res;
	}

	// Reconfigure the interface according to the gathered flash information and configuration
	if (memType == MemoryType::OctaSPI_DDR || memType == MemoryType::QuadSPI_DDR || memType == MemoryType::Hyperflash || memType == MemoryType::Hyperram)
	{
		config.rxSampleClock = kFLEXSPI_ReadSampleClkExternalInputFromDqsPad;// To achieve high speeds - always use DQS
		#if (defined(MIMXRT533S_SERIES)   || defined(MIMXRT555S_SERIES) || defined(MIMXRT595S_cm33_SERIES))
			clockDiv = 2;
			if (base == FLEXSPI0)
			{
				CLKCTL0->FLEXSPI0FCLKDIV = CLKCTL0_FLEXSPI0FCLKDIV_DIV (clockDiv - 1);
				while ((CLKCTL0->FLEXSPI0FCLKDIV) & CLKCTL0_FLEXSPI0FCLKDIV_REQFLAG_MASK)
					;
				deviceconfig.flexspiRootClk = CLOCK_GetFlexspiClkFreq (0);
			}
			else if (base == FLEXSPI1)
			{
				CLKCTL0->FLEXSPI1FCLKDIV = CLKCTL0_FLEXSPI1FCLKDIV_DIV (clockDiv - 1);
				while ((CLKCTL0->FLEXSPI1FCLKDIV) & CLKCTL0_FLEXSPI1FCLKDIV_REQFLAG_MASK)
					;
				deviceconfig.flexspiRootClk = CLOCK_GetFlexspiClkFreq (1);
			}
			else
				return LibmemStaus_Error;
			config.rxSampleClock = kFLEXSPI_ReadSampleClkLoopbackInternally; // No DQS
		#elif (defined(MIMXRT633S_SERIES) || defined(MIMXRT685S_cm33_SERIES))
			// No need to change the clock here.
			config.rxSampleClock = kFLEXSPI_ReadSampleClkLoopbackInternally; // No DQS
		#elif (defined(MIMXRT1011_SERIES) || defined(MIMXRT1015_SERIES) || defined(MIMXRT1021_SERIES) || defined(MIMXRT1024_SERIES) || \
			   defined(MIMXRT1041_SERIES) || defined(MIMXRT1042_SERIES) || defined(MIMXRT1051_SERIES) || defined(MIMXRT1052_SERIES) || \
			   defined(MIMXRT1061_SERIES) || defined(MIMXRT1062_SERIES) || defined(MIMXRT1064_SERIES))
			if (memType == MemoryType::Hyperflash)
				clockDiv = 7;	// With Hyperflash writing must be done with reduces speed (50 HMz max)
			else
				clockDiv = 2;
			CLOCK_SetDiv (FlexSPIDiv, clockDiv-1);	// flexspi clock divide by two --> 240 MHz.
			ClockHz = sourceClock_Hz / clockDiv;
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
	#elif (defined(MIMXRT1181_SERIES)      || defined(MIMXRT1182_SERIES)     || defined(MIMXRT118C_cm33_SERIES) || defined(MIMXRT118C_cm7_SERIES) || \
		   defined(MIMXRT1185_cm33_SERIES) || defined(MIMXRT1185_cm7_SERIES) || defined(MIMXRT1186_cm33_SERIES) || defined(MIMXRT1186_cm7_SERIES) || \
		   defined(MIMXRT1187_cm33_SERIES) || defined(MIMXRT1187_cm7_SERIES) || defined(MIMXRT1189_cm33_SERIES) || defined(MIMXRT1189_cm7_SERIES))
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
	uint32_t flashSize = CalculateCapacity_KBytes (info.Capacity);
	deviceconfig.flashSize = flashSize;
	geometry[0].count = flashSize / (geometry[0].size / 1024);
	flashSize *= 1024;	// Convert kBytes to bytes

	FLEXSPI_SetFlashConfig (base, &deviceconfig, Transfer::Port());	// Configure flash settings according to serial flash feature.
	FLEXSPI_SoftwareReset (base);

	static uint8_t writeBuffer[HYPERFLASH_PAGE_SIZE];
	LibmemDriver *FlashHandle = LibmemDriver::GetDriver ();
//	libmem_register_driver (FlashHandle, GetBaseAddress(base), flashSize, &geometry[0], nullptr, &DriverFunctions, &DriverFunctions_Extended);
	libmem_register_driver (FlashHandle, base->GetAmbaAddress (), flashSize, &geometry[0], nullptr, pDriverFunctions, nullptr);
	int err = libmem_driver_paged_write_init (&FlashHandle->PageWriteControlBlock, &writeBuffer[0], GetPageSize (memType), ProgramPage, 4, 0);
	FlashHandle->user_data = reinterpret_cast<uint32_t>(base);

	uint8_t *AliasAddress = base->GetAmbaAliasAddress ();
	if (AliasAddress != nullptr && err == LIBMEM_STATUS_SUCCESS)
	{
		FlashHandle = LibmemDriver::GetDriver ();
		libmem_register_driver (FlashHandle, AliasAddress, flashSize, &geometry[0], nullptr, pDriverFunctions, nullptr);
		err = libmem_driver_paged_write_init (&FlashHandle->PageWriteControlBlock, &writeBuffer[0], GetPageSize (memType), ProgramPage, 4, 0);
		FlashHandle->user_data = reinterpret_cast<uint32_t>(base);
		DebugPrint ("### Add Driver for Alias\r\n");
	}
	return static_cast<LibmemStatus_t>(err);
}

#endif	// defined FLEXSPI || defined FLEXSPI0 || defined FLEXSPI1

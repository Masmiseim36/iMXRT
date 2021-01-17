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


#include "libmem_driver_Hyperflash.h"
#include "libmem_Tools.h"
#include "fsl_device_registers.h"
#include "fsl_flexspi.h"
#include "fsl_common.h"
#include "fsl_clock.h"
#include "DebugPrint.h"

static libmem_driver_paged_write_ctrlblk_t PagedWrite_CtrlBlk;


/** enum FlashCommands:
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

/** deviceconfig:  */
static flexspi_device_config_t deviceconfig =
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
	.enableWriteMask      = false
};

/** customLUT:  */
static const uint32_t customLUT[CUSTOM_LUT_LENGTH] =
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
static const libmem_geometry_t geometry[] =
{ // count - size
	{100, 0x40000},
	{0, 0} 
};

static int WriteEnable    (FLEXSPI_Type *base, uint32_t baseAddr);
static status_t EraseChip (FLEXSPI_Type *base);

static int EraseSector        (libmem_driver_handle_t *h, libmem_sector_info_t *si);
static int ProgramPage        (libmem_driver_handle_t *h, uint8_t *dest_addr, const uint8_t *src_addr);
static int libmem_ProgramPage (libmem_driver_handle_t *h, uint8_t *dest, const uint8_t *src, size_t size);
static int libmem_EraseSector (libmem_driver_handle_t *h, uint8_t *start, size_t size, uint8_t **erase_start, size_t *erase_size);
static int libmem_Flush       (libmem_driver_handle_t *h);
static int libmem_Read        (libmem_driver_handle_t *h, uint8_t *dest, const uint8_t *src, size_t size);
static uint32_t libmem_CRC32  (libmem_driver_handle_t *h, const uint8_t *start, size_t size, uint32_t crc);


/** DriverFunctions:  */
static const libmem_driver_functions_t DriverFunctions =
{
	libmem_ProgramPage,
	0,
	libmem_EraseSector,
	0,
	0,
	libmem_Flush
};

/** DriverFunctionsExt:  */
static const libmem_ext_driver_functions_t DriverFunctions_Extended =
{
	0,
	libmem_Read,
	libmem_CRC32
};



/** Libmem_InitializeDriver_Hyperflash:
Initialize the FlexSPI Interface for using as a SPI-Interface
@param FlashHandle The handle which should be initialized
@param base The Flex-SPI-base to use
@return LibmemStatus_t LibmemStaus_Success if the operation was successfully */
LibmemStatus_t Libmem_InitializeDriver_Hyperflash (libmem_driver_handle_t *FlashHandle, FLEXSPI_Type *base)
{
	#if (defined MIMXRT633S_SERIES) || defined (MIMXRT685S_cm33_SERIES) || defined(MIMXRT595S_cm33_SERIES)
		uint32_t src = 2;		// Use AUX0_PLL as clock source for the FlexSPI
		uint32_t divider = 4;	// with a divider of four
		if (CLKCTL0->FLEXSPIFCLKSEL != CLKCTL0_FLEXSPIFCLKSEL_SEL(src) || (CLKCTL0->FLEXSPIFCLKDIV & CLKCTL0_FLEXSPIFCLKDIV_DIV_MASK) != (divider - 1))
		{
			CLKCTL0->PSCCTL0_CLR = CLKCTL0_PSCCTL0_CLR_FLEXSPI_OTFAD_CLK_MASK;	// Disable clock before changing clock source
			CLKCTL0->FLEXSPIFCLKSEL = CLKCTL0_FLEXSPIFCLKSEL_SEL(src);			// Update flexspi clock.
			CLKCTL0->FLEXSPIFCLKDIV |= CLKCTL0_FLEXSPIFCLKDIV_RESET_MASK;		// Reset the divider counter
			CLKCTL0->FLEXSPIFCLKDIV = CLKCTL0_FLEXSPIFCLKDIV_DIV(divider - 1);
			while ((CLKCTL0->FLEXSPIFCLKDIV) & CLKCTL0_FLEXSPIFCLKDIV_REQFLAG_MASK)
				;
			CLKCTL0->PSCCTL0_SET = CLKCTL0_PSCCTL0_SET_FLEXSPI_OTFAD_CLK_MASK;	// Enable FLEXSPI clock again
		}
	#elif (defined MIMXRT1011_SERIES) || (defined MIMXRT1015_SERIES) || (defined MIMXRT1021_SERIES) || (defined MIMXRT1051_SERIES) || \
		  (defined MIMXRT1052_SERIES) || (defined MIMXRT1061_SERIES) || (defined MIMXRT1062_SERIES) || (defined MIMXRT1064_SERIES)
		// Set flexspi root clock to 166MHZ.
		CLOCK_InitUsb1Pfd (kCLOCK_Pfd0, 26);        // Set PLL3 PFD0 clock 332MHZ.
		CLOCK_SetMux      (kCLOCK_FlexspiMux, 0x3); // Choose PLL3 PFD0 clock as flexspi source clock.
		CLOCK_SetDiv      (kCLOCK_FlexspiDiv, 3);   // flexspi clock 83M, DDR mode, internal clock 42M.
	#else
		#error "unknon controller family"
	#endif

	// Get FLEXSPI default settings and configure the flexspi.
	flexspi_config_t config;
	FLEXSPI_GetDefaultConfig (&config);

	config.ahbConfig.enableAHBPrefetch    = true;	// Set AHB buffer size for reading data through AHB bus.
	config.ahbConfig.enableReadAddressOpt = true;	// Allow AHB read start address do not follow the alignment requirement.
	config.ahbConfig.enableAHBBufferable  = true;
	config.ahbConfig.enableAHBCachable    = true;
	config.enableSckBDiffOpt              = true;	// enable diff clock and DQS
	config.rxSampleClock                  = kFLEXSPI_ReadSampleClkExternalInputFromDqsPad;
	config.enableCombination              = true;
	FLEXSPI_Init (base, &config);

	FLEXSPI_SetFlashConfig (base, &deviceconfig, kFLEXSPI_PortA1);  // Configure flash settings according to serial flash feature.
	FLEXSPI_UpdateLUT      (base, 0, customLUT, CUSTOM_LUT_LENGTH); // Update LUT table.
	FLEXSPI_SoftwareReset  (base);                                  // Do software reset.
//	EraseChip (base);

	static uint8_t write_buffer[HYPERFLASH_PAGE_SIZE];
//	libmem_register_driver (FlashHandle, (uint8_t *)libmem_GetBaseAddress(base), BOARD_FLASH_SIZE, geometry, 0, &DriverFunctions, &DriverFunctions_Extended);
	libmem_register_driver (FlashHandle, (uint8_t *)libmem_GetBaseAddress(base), BOARD_FLASH_SIZE, geometry, 0, &DriverFunctions, NULL);
	FlashHandle->user_data = (uint32_t)base;
	return static_cast<LibmemStatus_t>(libmem_driver_paged_write_init (&PagedWrite_CtrlBlk, write_buffer, HYPERFLASH_PAGE_SIZE, ProgramPage, 4, 0));
}



/** WriteEnable:
Send write-enable command
@param base The Flex-SPI-base to use
@param baseAddr The base-address of the command
@return status_t kStatus_Success if the operation was successfully */
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

/** WaitBusBusy:
Wait until the Write/erase operation is finished and the Flash is not busy anymore
@param base The Flex-SPI-base to use
@return status_t kStatus_Success if the operation was successfully */
status_t WaitBusBusy (FLEXSPI_Type *base)
{
	// Wait status ready.	
	uint32_t readValue;
	status_t status;
	flexspi_transfer_t flashXfer;

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

/** EraseChip:
Erase the whole-Flash-memory
@param base The FlexSPI-Interface where the Flash is located which should be erased
@return static status_t Status of the Operation - kStatus_Success when successfully */
[[maybe_unused]] static status_t EraseChip (FLEXSPI_Type *base)
{
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
		return status;

	return WaitBusBusy (base);
}

/** EraseSector:
Erase a sector of the Flash-Memory
@param h Handle to the Flash-Driver
@param si Information about the sector which should be erased
@return static int LIBMEM_STATUS_SUCCESS when the erase operation was successfully, otherwise LIBMEM_STATUS_ERROR */
static status_t EraseSector (libmem_driver_handle_t *h, libmem_sector_info_t *si)
{
	FLEXSPI_Type *base = (FLEXSPI_Type *)h->user_data;
	uint32_t SectorAddr = libmem_CalculateOffset (base, (uint32_t)si->start);
	if (SectorAddr == UINT32_MAX)
		return LIBMEM_STATUS_ERROR;

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

/** ProgramPage:
Write Data to a Flash-Page
@param h Handle to the Flash-Driver
@param Dest Adress to write the Data to. This Address is in the Address-Range of the Controller
@param Source Address of the Array with the Data to write
@return static int LIBMEM_STATUS_SUCCESS when the write operation was successfully, otherwise LIBMEM_STATUS_ERROR */
static int ProgramPage (libmem_driver_handle_t *h, uint8_t *Dest, const uint8_t *Source)
{
	FLEXSPI_Type *base = (FLEXSPI_Type *)h->user_data;
	uint32_t DeviceAddress = libmem_CalculateOffset (base, (uint32_t)Dest);
	if (DeviceAddress == UINT32_MAX)
		return LIBMEM_STATUS_ERROR;

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

/** libmem_ProgramPage:
The LIBMEM driver's write function.
@param h    A pointer to the handle of the LIBMEM driver.
@param dest A pointer to the memory address in memory range handled by driver to write data to.
@param src  pointer to the memory address to read data from.
@param size The number of bytes to write.
@return int The LIBMEM status result */
static int libmem_ProgramPage (libmem_driver_handle_t *h, uint8_t *dest, const uint8_t *src, size_t size)
{
	return libmem_driver_paged_write (h, dest, src, size, &PagedWrite_CtrlBlk);
}

/** libmem_EraseSector:
The LIBMEM driver's erase function
@param h           A pointer to the handle of the LIBMEM driver.
@param start       A pointer to the initial memory address in memory range handled by driver to erase.
@param size        The number of bytes to erase.
@param erase_start A pointer to a location in memory to store a pointer to the start of the memory range that has actually been erased or NULL if not required.
@param erase_size  A pointer to a location in memory to store the size in bytes of the memory range that has actually been erased or NULL if not required.
@return int        The LIBMEM status result */
static int libmem_EraseSector (libmem_driver_handle_t *h, uint8_t *start, size_t size, uint8_t **erase_start, size_t *erase_size)
{
	return libmem_foreach_sector_in_range (h, start, size, EraseSector, erase_start, erase_size);
}

/** libmem_Flush:
The LIBMEM driver's flush function.
@param h    A pointer to the handle of the LIBMEM driver. 
@return int The LIBMEM status result */
static int libmem_Flush (libmem_driver_handle_t *h)
{
	int res = libmem_driver_paged_write_flush (h, &PagedWrite_CtrlBlk);
	return res;
}

/** libmem_Read:
The LIBMEM driver's read extended function.
@param h    A pointer to the handle of the LIBMEM driver.
@param dest A pointer to the initial memory address to write data to.
@param src  A pointer to the initial memory address in the memory range handled by the driver to read data from.
@param size The number of bytes to write.
@return int The LIBMEM status result */
static int libmem_Read (libmem_driver_handle_t *h, uint8_t *dest, const uint8_t *src, size_t size)
{
	(void)h;
	if (size)
		memcpy (dest, src, size);
	return LIBMEM_STATUS_SUCCESS;
}

/** libmem_CRC32:
The LIBMEM driver's crc32 extended function.
@param h     A pointer to the handle of the LIBMEM driver.
@param start A pointer to the start of the address range.
@param size  The size of the address range in bytes.
@param crc   The initial CRC-32 value.
@return uint32_t The computed CRC-32 value. */
static uint32_t libmem_CRC32 (libmem_driver_handle_t *h, const uint8_t *start, size_t size, uint32_t crc)
{
	(void)h;
	crc = libmem_crc32_direct (start, size, crc);
	return crc;
}


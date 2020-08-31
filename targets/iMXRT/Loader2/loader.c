/** Loader for iMXRT-Family
Copyright (C) 2019-2020  Markus Klein
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

#include <libmem.h>
#include <libmem_loader.h>
#include <libmem_flm_driver.h>
#include <stdio.h>

#include "board.h"
#include "fsl_common.h"
#include "fsl_clock.h"
#include "libmem_Tools.h"
#include "libmem_driver_Hyperflash.h"
#include "libmem_driver_xSPI.h"
#include "pin_mux.h"
#include "DebugPrint.h"

extern void BOARD_BootClockGate (void);

enum LibmemStatus Init_Libmem (libmem_driver_handle_t *handle, enum eMemoryType, FLEXSPI_Type *base);
void InitOctaSPIPins (FLEXSPI_Type *base);
void InitQuadSPIPins (FLEXSPI_Type *base);

void ExecuteTest (uint32_t *MemPointer)
{
	static uint32_t buffer[512];
	memset (buffer, 0, sizeof(buffer));
	for (size_t i=0; i<sizeof(buffer)/sizeof(buffer[0]); i++)
		buffer[i] = (uint32_t)(((uint32_t *)buffer) + i);

	uint8_t *erase_start = 0;
	size_t erase_size = 0;
	LibmemStatus_t res = libmem_erase ((uint8_t *)MemPointer, sizeof(buffer), &erase_start, &erase_size);
	if (res != LIBMEM_STATUS_SUCCESS)
		DebugPrintf ("Error '%s' occurred\r\n", Libmem_GetErrorString (res));

	// Check if everything is erased
	uint32_t ErrorCounter = 0;
	for (size_t i=0; i<erase_size/sizeof(uint32_t); i++)
	{
		if (MemPointer[i] != 0xFFFFFFFF)
			ErrorCounter++;
	}
	if (ErrorCounter > 0)
		DebugPrintf ("Invalid memory-chunks on erase: %d", ErrorCounter);

	res = libmem_write ((uint8_t *)MemPointer, (uint8_t *)buffer, sizeof(buffer));
	res = libmem_flush ();

	if (memcmp (MemPointer, buffer, sizeof (buffer)) != 0)
		DebugPrint ("Writing failed\r\n");
}

int main (uint32_t flags, uint32_t param)
{
	BOARD_ConfigMPU     ();
	BOARD_BootClockGate ();
	BOARD_BootClockRUN  ();
	#if defined FSL_FEATURE_SOC_CCM_ANALOG_COUNT && FSL_FEATURE_SOC_CCM_ANALOG_COUNT > 0
		const clock_usb_pll_config_t ConfigUsbPll = {.loopDivider = 0U};
		CLOCK_InitUsb1Pll   (&ConfigUsbPll);
	#endif
	#if defined FSL_FEATURE_SOC_IOMUXC_COUNT && FSL_FEATURE_SOC_IOMUXC_COUNT > 0
		CLOCK_EnableClock (kCLOCK_Iomuxc);
	#endif
	#if defined (__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
		SCB_DisableDCache   ();
	#endif
	ConfigUart          ();
	DebugPrint ("Hello iMXRT Loader\r\n");

	static libmem_driver_handle_t FlexSPI1_Handle;
	#if defined FLEXSPI2
		static libmem_driver_handle_t FlexSPI2_Handle;
	#endif



	#ifdef DEBUG
		// some test Code, because the Loader can not be debuged while using it in real scenarios
//		#if defined CPU_MIMXRT1015CAF4A || defined CPU_MIMXRT1021DAF5A || defined CPU_MIMXRT1062DVJ6A || defined CPU_MIMXRT1064DVL6A
//			BOARD_InitQuadSPIPins (); 
//			enum LibmemStatus res = Libmem_InitializeDriver_xSPI (&FlexSPI1_Handle, FLEXSPI, MemType_QuadSPI);	// Register iMX-RT internal FLASH driver
//		#else
//			BOARD_InitOctaSPIPins ();
//			enum LibmemStatus res = Libmem_InitializeDriver_Hyperflash (&FlexSPI1_Handle, FLEXSPI);	// Register iMX-RT internal FLASH driver
//		#endif

//		BOARD_InitQuadSPIPins ();
		BOARD_InitOctaSPIPins ();
//		LibmemStatus_t res = Libmem_InitializeDriver_Hyperflash (&FlexSPI1_Handle, FLEXSPI);
		LibmemStatus_t res = Libmem_InitializeDriver_xSPI (&FlexSPI1_Handle, FLEXSPI, MemType_OctaSPI_DDR);
//		LibmemStatus_t res = Libmem_InitializeDriver_xSPI (&FlexSPI1_Handle, FLEXSPI, MemType_QuadSPI);
		if (res == LibmemStaus_Success)
			ExecuteTest ((uint32_t *)(FLASH_START_ADDRESS + 0x40000));		// Testcode

		#if defined FLEXSPI2
			BOARD_InitQuadSPI2Pins ();
			LibmemStatus_t res2 = Libmem_InitializeDriver_xSPI (&FlexSPI2_Handle, FLEXSPI2, MemType_QuadSPI);
			if (res2 == LibmemStaus_Success)
				ExecuteTest ((uint32_t *)(FLASH2_START_ADDRESS + 0x40000));	// Testcode
		#endif

	#else
		enum LibmemStatus res = LibmemStaus_Success;
		if (flags & LIBMEM_RPC_LOADER_FLAG_PARAM)
		{
			DebugPrintf ("libmem Parameter: 0x%X\r\n", param);
			// Register iMX-RT internal FLASH driver
			LibmemStatus_t res1 = Init_Libmem (&FlexSPI1_Handle, (enum eMemoryType)(param & 0x0F), FLEXSPI);
			#if defined FLEXSPI2
				LibmemStatus_t res2 = Init_Libmem (&FlexSPI2_Handle, (enum eMemoryType)((param & 0xF0) >> 4), FLEXSPI2);
			#endif
			if (res1 == LibmemStatus_InvalidMemoryType 
			#if defined FLEXSPI2
				&& res2 == LibmemStatus_InvalidMemoryType
			#endif
			)
			{
				// No valid option for an Flash-memory-Interface selected
				char ErrorString[64];
				sprintf (ErrorString, "No valid Interface selected. Parameter: 0x%X\r\n", param);
				DebugPrint (ErrorString);
				libmem_rpc_loader_exit (LIBMEM_STATUS_NO_DRIVER, ErrorString);
				return LIBMEM_STATUS_ERROR;
			}

		}
		else
		{
			DebugPrint ("No libmem Parameter. Load default settings\r\n");
		}
	#endif

	// Start loader
	if (res == LIBMEM_STATUS_SUCCESS)
	{
		#if defined INITIALIZE_TCM_SECTIONS
		DebugPrint ("Start RPC loader\r\n");
			extern uint8_t __DTCM_segment_start__;
			extern uint8_t __DTCM_segment_used_end__;
			extern uint8_t __DTCM_segment_end__;
		res = libmem_rpc_loader_start (&__DTCM_segment_used_end__, &__DTCM_segment_end__ - 1);
		#else
			extern uint8_t __SRAM_segment_start__;
			extern uint8_t __SRAM_segment_used_end__;
			extern uint8_t __SRAM_segment_end__;
			res = libmem_rpc_loader_start (&__SRAM_segment_used_end__, &__SRAM_segment_end__ - 1);
		#endif
	}
	else
		DebugPrint ("Libmem Driver coudn't be loaded\r\n");

	// Terminate loader and return error String if an Error occurred
    if (res == LIBMEM_STATUS_SUCCESS)
	{
		DebugPrint ("RPC loader finished successfully\r\n");
		for (int i=0; i<1024*1024;i++)
			;
		libmem_rpc_loader_exit (res, NULL);
	}
	else
	{
		char ErrorString[64];
		sprintf (ErrorString, "Error '%s' occurred\r\n", Libmem_GetErrorString (res));
		DebugPrint (ErrorString);
		libmem_rpc_loader_exit (res, ErrorString);
	}

	return 0;
}



enum LibmemStatus Init_Libmem (libmem_driver_handle_t *handle, enum eMemoryType MemType, FLEXSPI_Type *base)
{
	enum LibmemStatus status;
	uint32_t Trials = 0;

	switch (MemType)
	{
		case MemType_Hyperflash:
			do
			{
				// Init for Hyperflash
				DebugPrint ("Init Loader for Hyperflash\r\n");
				InitOctaSPIPins (base);
				status = Libmem_InitializeDriver_Hyperflash (handle, base);
				if (status != LibmemStaus_Success)
				{
					Trials ++;
					BOARD_PerformJEDECReset ();
				}
			}
			while (status != LibmemStaus_Success && Trials < 3);
			break;
		case MemType_OctaSPI_DDR:
		case MemType_OctaSPI:
			do
			{
				// Init for Octal-SPI with DDR
				DebugPrint ("Init Loader for Octal-SPI (DDR)\r\n");

				/* A small delay is need here */
				for(int i=0; i<1000000; i++)
				{
					__asm__ volatile("nop");
				}

				InitOctaSPIPins (base);
				status =  Libmem_InitializeDriver_xSPI (handle, base, MemType);
				if (status != LibmemStaus_Success)
				{
					Trials ++;
					BOARD_PerformJEDECReset ();
				}
			}
			while (status != LibmemStaus_Success && Trials < 3);
			break;
		case MemType_QuadSPI_DDR:
//		case MemType_QuadSPI:
			do
			{
				// Init for Octal-SPI with DDR
				DebugPrint ("Init Loader for Quad-SPI-DDR\r\n");
				InitQuadSPIPins (base);
				status =  Libmem_InitializeDriver_xSPI (handle, base, MemType);
				if (status != LibmemStaus_Success)
				{
					Trials ++;
					BOARD_PerformJEDECReset ();
				}
			}
			while (status != LibmemStaus_Success && Trials < 3);
			break;
		case MemType_QuadSPI:
			do
			{
				// Init for Quad-SPI
				DebugPrint ("Init Loader for Quad-SPI\r\n");
				InitQuadSPIPins (base);
				status =  Libmem_InitializeDriver_xSPI (handle, base, MemType);
				if (status != LibmemStaus_Success)
				{
					Trials ++;
					BOARD_PerformJEDECReset ();
				}
			}
			while (status != LibmemStaus_Success && Trials < 3);
			break;
		default:
			return LibmemStatus_InvalidMemoryType;
	}
	return status;
}

void InitOctaSPIPins (FLEXSPI_Type *base)
{
	switch ((uint32_t)base)
	{
		case FLEXSPI_BASE:
			BOARD_InitOctaSPIPins ();
			break;
		#ifdef FLEXSPI2
		case FLEXSPI2_BASE:
			BOARD_InitOctaSPI2Pins ();
			break;
		#endif
		default:
			return;
	}
}

void InitQuadSPIPins (FLEXSPI_Type *base)
{
	switch ((uint32_t)base)
	{
		case FLEXSPI_BASE:
			BOARD_InitQuadSPIPins ();
			break;
		#ifdef FLEXSPI2
		case FLEXSPI2_BASE:
			BOARD_InitQuadSPI2Pins ();
			break;
		#endif
		default:
			return;
	}
}
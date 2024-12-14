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

extern "C"
{
	#include <libmem.h>
	#include <libmem_loader.h>
}
#include <cstdio>

#include "libmem_Tools.h"
#include "libmem_driver_FlexSPI.h"
#include "DebugPrint.h"
#include "FlexSPI_Helper.h"


enum LibmemStatus Init_Libmem (FlexSPI_Helper *base, MemoryType memoryType);

uint32_t Compare (const uint32_t *memPointer, uint32_t Comp, size_t size)
{
	uint32_t errorCounter = 0;
	for (size_t i=0; i<size/sizeof(uint32_t); i++)
	{
		if (memPointer[i] != Comp)
			errorCounter++;
	}
	return errorCounter;
}

uint32_t Compare (const uint32_t *memPointer, const uint32_t *pComp, size_t size)
{
	uint32_t errorCounter = 0;
	for (size_t i=0; i<size/sizeof(uint32_t); i++)
	{
		if (memPointer[i] != pComp[i])
			errorCounter++;
	}
	return errorCounter;
}

void ExecuteTest (uint32_t *memPointer)
{
	static std::array<uint32_t, 4096> buffer;

	uint8_t *erase_start = 0;
	size_t erase_size = 0;
	LibmemStatus_t res = static_cast<LibmemStatus_t>(libmem_erase (reinterpret_cast<uint8_t *>(memPointer), sizeof(buffer), &erase_start, &erase_size));
	if (res != LIBMEM_STATUS_SUCCESS)
		DebugPrintf ("Error '%s' occurred\r\n", Libmem_GetErrorString (res));
	res = static_cast<LibmemStatus_t>(libmem_flush ());

	// Check if everything is erased
	libmem_read (reinterpret_cast<uint8_t *>(buffer.data()), reinterpret_cast<const uint8_t *>(memPointer), buffer.size());
	uint32_t errorCounter = Compare (memPointer, 0xFFFFFFFF, erase_size);
	if (errorCounter > 0)
		DebugPrintf ("Invalid memory-chunks on erase: %d\r\n", errorCounter);

	// Initialize the array with test data
	for (size_t i=0; i<sizeof(buffer)/sizeof(buffer[0]); i++)
		buffer[i] = reinterpret_cast<uint32_t>((buffer.data ()) + i);
	res = static_cast<LibmemStatus_t>(libmem_write ((uint8_t *)memPointer, (uint8_t *)&buffer[0], sizeof(buffer)));
	res = static_cast<LibmemStatus_t>(libmem_flush ());

	errorCounter = Compare (memPointer, &buffer[0], sizeof (buffer));
	if (errorCounter > 0)
		DebugPrintf ("Invalid memory-chunks on write %d\r\n", errorCounter);
}

//static constexpr uint32_t FourMegabyteOffset = 4 * 1024 * 1024;
static constexpr uint32_t TwoMegabyteOffset = 2 * 1024 * 1024;

void InitializeAndTest (FlexSPI_Helper *base, MemoryType memoryType)
{
	LibmemStatus res = Init_Libmem (base, memoryType);
	if (res == LibmemStaus_Success)
		ExecuteTest ((uint32_t *)(base->GetAmbaAddress () + TwoMegabyteOffset));
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmain"
int main ([[maybe_unused]]uint32_t flags, [[maybe_unused]]uint32_t param)
{
#pragma GCC diagnostic pop
	BOARD_ConfigMPU     ();
	BOARD_BootClockGate ();
	BOARD_BootClockRUN  ();
	ConfigUart ();
	DebugPrint ("Hello iMXRT Loader\r\n");

	enum LibmemStatus res {LibmemStaus_Success};
	#ifdef DEBUG
		// some test code, because the loader can not be debugged while using it in real scenarios
		#if defined FLEXSPI
			InitializeAndTest (static_cast<FlexSPI_Helper *>(FLEXSPI), MemType_QuadSPI); // MemType_Hyperflash - MemType_OctaSPI_DDR - MemType_QuadSPI
		#endif
		#if defined FLEXSPI0
			InitializeAndTest (static_cast<FlexSPI_Helper *>(FLEXSPI0), MemType_OctaSPI_DDR);
		#endif
		#if defined FLEXSPI1
			InitializeAndTest (static_cast<FlexSPI_Helper *>(FLEXSPI1), MemType_OctaSPI_DDR);
		#endif
		#if defined FLEXSPI2
			InitializeAndTest (static_cast<FlexSPI_Helper *>(FLEXSPI2), MemType_QuadSPI);
		#endif
	#else
		if (param != 0)
		{
			DebugPrintf ("libmem parameter: 0x%X\r\n", param);
			// Register iMX-RT internal FLASH driver
			#if defined FLEXSPI
				// devices with a single FlexSPI interface or two interfaces (imxRT106x)
				res = Init_Libmem (static_cast<FlexSPI_Helper *>(FLEXSPI), static_cast<MemoryType>(param & 0x0F));
				#if defined FLEXSPI2
					LibmemStatus res2 = Init_Libmem (static_cast<FlexSPI_Helper *>(FLEXSPI2), static_cast<MemoryType>((param & 0xF0) >> 4));
				#endif
				if (res == LibmemStatus_InvalidMemoryType
				#if defined FLEXSPI2
					&& res2 == LibmemStatus_InvalidMemoryType
				#endif
				)
			#elif defined FLEXSPI0
				// devices with one or two FlexSPI Interfaces counting from zero
				LibmemStatus res0 = Init_Libmem (static_cast<FlexSPI_Helper *>(FLEXSPI0), static_cast<MemoryType>(param & 0x0F));
				#if defined FLEXSPI1
					LibmemStatus_t res1 = Init_Libmem (static_cast<FlexSPI_Helper *>(FLEXSPI1), static_cast<MemoryType>((param & 0xF0) >> 4));
				#endif
				if (res0 == LibmemStatus_InvalidMemoryType 
				#if defined FLEXSPI1
					&& res1 == LibmemStatus_InvalidMemoryType
				#endif
				)
			#elif defined FLEXSPI1
				// devices with two FlexSPI Interfaces counting from one
				LibmemStatus res1 = Init_Libmem (static_cast<FlexSPI_Helper *>(FLEXSPI1), static_cast<MemoryType>(param & 0x0F));
				LibmemStatus res2 = Init_Libmem (static_cast<FlexSPI_Helper *>(FLEXSPI2), static_cast<MemoryType>((param & 0xF0) >> 4));
				if (res1 == LibmemStatus_InvalidMemoryType && res2 == LibmemStatus_InvalidMemoryType)
			#endif
			{
				// No valid option for an Flash-memory-interface selected
				char ErrorString[64];
				sprintf (ErrorString, "No valid interface selected. Parameter: 0x%X\r\n", param);
				DebugPrint (ErrorString);
				libmem_rpc_loader_exit (LIBMEM_STATUS_NO_DRIVER, ErrorString);
				return LIBMEM_STATUS_INVALID_DEVICE;
			}
		}
		else
		{
			DebugPrintf ("No or invalid libmem parameter: 0x%X\r\n", param);
			return LIBMEM_STATUS_INVALID_PARAMETER;
		}
	#endif

	// Start loader
	DebugPrint ("Start RPC loader\r\n");
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Warray-bounds"
	#if defined INITIALIZE_TCM_SECTIONS
		//extern uint8_t __DTCM_segment_start__;
		extern uint8_t __DTCM_segment_used_end__[];
		extern uint8_t __DTCM_segment_end__[];
		res = static_cast<LibmemStatus>(libmem_rpc_loader_start (__DTCM_segment_used_end__, __DTCM_segment_end__ - 1));
	#else
		extern uint8_t __SRAM_data_segment_used_end__[];
		extern uint8_t __SRAM_data_segment_end__[];
		res = static_cast<LibmemStatus>(libmem_rpc_loader_start (__SRAM_data_segment_used_end__, __SRAM_data_segment_end__ - 1));
	#endif
	#pragma GCC diagnostic pop

	// Terminate loader and return error String if an Error occurred
	if (res == LIBMEM_STATUS_SUCCESS)
	{
		DebugPrint ("RPC loader finished successfully\r\n");
		for (int i=0; i<1024*1024;i++)
			;
		libmem_rpc_loader_exit (res, nullptr);
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



enum LibmemStatus Init_Libmem (FlexSPI_Helper *base, MemoryType memoryType)
{
	enum LibmemStatus status {LibmemStaus_Success};
	uint32_t trials = 0;

	if (memoryType == MemType_Invalid)
		return status;

	PrintMemTypeInfor (memoryType);
	base->PerformJEDECReset ();

	do
	{
		// A small delay is need here
		for (int i=0; i<1000000; i++)
			__asm__ volatile("nop");

		base->InitializeSpiPins (memoryType);
		status =  Libmem_InitializeDriver_xSPI (base, memoryType);
		if (status != LibmemStaus_Success)
		{
			trials ++;
			base->PerformJEDECReset ();
		}
	}
	while (status != LibmemStaus_Success && trials < 3);
	return status;
}


#if defined MIMXRT685S_cm33_SERIES
	void SystemInitHook (void)
	{
		CACHE64->CCR &= ~CACHE64_CTRL_CCR_ENCACHE_MASK; // disable the cache
	}
#endif
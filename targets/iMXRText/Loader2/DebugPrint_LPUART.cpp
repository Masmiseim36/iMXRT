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


#include <cstdint>
#include <cstdarg>
#include <cstdio>
#include "DebugPrint.h"
#include "pin_mux.h"
#include "fsl_clock.h"


#if defined LPUART_BASE_PTRS
#if defined ENABLE_DEBUG_PRINT && ENABLE_DEBUG_PRINT != 0
	static LPUART_Type * const uart [] = LPUART_BASE_PTRS;

	#include "fsl_lpuart.h"

	/*! ConfigUart:
	\brief Configure a UART to use it to print debug-messages
	\param  void
	\return bool @true if successfully */
	bool ConfigUart (void)
	{
		BOARD_InitUARTPins ();

		#if (defined(MIMXRT1011_SERIES) || defined(MIMXRT1015_SERIES) || defined(MIMXRT1021_SERIES) || defined(MIMXRT1024_SERIES) || \
			 defined(MIMXRT1041_SERIES) || defined(MIMXRT1042_SERIES) || defined(MIMXRT1051_SERIES) || defined(MIMXRT1052_SERIES) || \
			 defined(MIMXRT1061_SERIES) || defined(MIMXRT1062_SERIES) || defined(MIMXRT1064_SERIES))
			uint32_t ClockFrequency = 0;
			if (CLOCK_GetMux (kCLOCK_UartMux) == 0) // --> PLL3 div6 80M
				ClockFrequency = (CLOCK_GetPllFreq (kCLOCK_PllUsb1) / 6U) / (CLOCK_GetDiv(kCLOCK_UartDiv) + 1U);
			else
				ClockFrequency = CLOCK_GetOscFreq() / (CLOCK_GetDiv(kCLOCK_UartDiv) + 1U);
		#elif (defined(MIMXRT1165_cm7_SERIES) || defined(MIMXRT1166_cm7_SERIES) || defined(MIMXRT1165_cm4_SERIES) || defined(MIMXRT1166_cm4_SERIES) || \
			   defined(MIMXRT1171_SERIES)     || defined(MIMXRT1172_SERIES)     || \
			   defined(MIMXRT1173_cm7_SERIES) || defined(MIMXRT1175_cm7_SERIES) || defined(MIMXRT1176_cm7_SERIES) || \
			   defined(MIMXRT1173_cm4_SERIES) || defined(MIMXRT1175_cm4_SERIES) || defined(MIMXRT1176_cm4_SERIES))
			// Configure Lpuartx using SysPll2
			static const clock_root_t RootClocks [] = {static_cast<clock_root_t>(0x7F), kCLOCK_Root_Lpuart1, kCLOCK_Root_Lpuart2, kCLOCK_Root_Lpuart3, kCLOCK_Root_Lpuart4, kCLOCK_Root_Lpuart5, kCLOCK_Root_Lpuart6, kCLOCK_Root_Lpuart7, kCLOCK_Root_Lpuart8, kCLOCK_Root_Lpuart9, kCLOCK_Root_Lpuart10, kCLOCK_Root_Lpuart11, kCLOCK_Root_Lpuart12};
			static const clock_ip_name_t clocks [] = LPUART_CLOCKS;

			clock_root_config_t rootCfg {};
			rootCfg.mux = 6;
			rootCfg.div = 21;
			CLOCK_SetRootClock (RootClocks[BOARD_DEBUG_UART_INSTANCE], &rootCfg);
			CLOCK_ControlGate  (clocks[BOARD_DEBUG_UART_INSTANCE], kCLOCK_On);

			uint32_t ClockFrequency = CLOCK_GetRootClockFreq (RootClocks[BOARD_DEBUG_UART_INSTANCE]);
		#elif (defined(MIMXRT1181_SERIES)     || defined(MIMXRT1182_SERIES)     || defined(MIMXRT1187_cm7_SERIES) || defined(MIMXRT1187_cm33_SERIES) ||\
				 defined(MIMXRT1189_cm7_SERIES) || defined(MIMXRT1189_cm33_SERIES))
			// Configure Lpuartx using SysPll2
			static const clock_root_t RootClocks [] 
			{
 				static_cast<clock_root_t>(0x7F), kCLOCK_Root_Lpuart0102, kCLOCK_Root_Lpuart0102, kCLOCK_Root_Lpuart0304, kCLOCK_Root_Lpuart0304,
				kCLOCK_Root_Lpuart0506, kCLOCK_Root_Lpuart0506, kCLOCK_Root_Lpuart0708, kCLOCK_Root_Lpuart0708, kCLOCK_Root_Lpuart0910,
				kCLOCK_Root_Lpuart0910, kCLOCK_Root_Lpuart1112, kCLOCK_Root_Lpuart1112
			};
			static const clock_ip_name_t clocks [] = LPUART_CLOCKS;

			clock_root_config_t rootCfg {};
			rootCfg.mux = 6;
			rootCfg.div = 21;
			CLOCK_SetRootClock (RootClocks[BOARD_DEBUG_UART_INSTANCE], &rootCfg);
			CLOCK_ControlGate  (clocks[BOARD_DEBUG_UART_INSTANCE], kCLOCK_On);

			uint32_t ClockFrequency = CLOCK_GetRootClockFreq (RootClocks[BOARD_DEBUG_UART_INSTANCE]);
		#else
			#error "unknon controller family"
		#endif

		lpuart_config_t config;
		LPUART_GetDefaultConfig (&config);
		config.baudRate_Bps = 115200U;
		config.enableTx     = true;
		config.enableRx     = true;
		status_t status = LPUART_Init (uart[BOARD_DEBUG_UART_INSTANCE], &config, ClockFrequency);
		return status == kStatus_Success;
	}

	/*! DebugPrint:
	\brief Send a message to the UART
	\param Message The zero terminated string to send
	\return void */
	void DebugPrint (const char *Message)
	{
		LPUART_WriteBlocking (uart[BOARD_DEBUG_UART_INSTANCE], (uint8_t *)Message, strlen(Message));
	}
	/*! DebugPrintf:
	\brief Send a formated Message to the UART
	\param Message The zero terminated string to send including formating information
	\param  ... Sequence of additional arguments used to replace a format specifier
	\return void */
	void DebugPrintf (const char *Message, ...)
	{
		static char Buffer [128];
		va_list ArgPtr;
		va_start(ArgPtr, Message);
		size_t Length = vsnprintf (Buffer, sizeof(Buffer), Message, ArgPtr);
		va_end(ArgPtr);

		if (Length > 0 && Length <= sizeof(Buffer))
			LPUART_WriteBlocking (uart[BOARD_DEBUG_UART_INSTANCE], (uint8_t *)Buffer, Length);
	}
#else
	bool ConfigUart (void)
	{
		return true;
	}

	void DebugPrint (const char *Message)
	{
		(void)Message;
	}

	void DebugPrintf (const char *Message, ...)
	{
		(void)Message;
	}
#endif // defined ENABLE_DEBUG_PRINT && ENABLE_DEBUG_PRINT != 0
#endif // defined LPUART_BASE_PTRS
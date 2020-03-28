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


#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include "DebugPrint.h"
#include "pin_mux.h"
#include "fsl_clock.h"
#include "fsl_lpuart.h"

#define ENABLE_DEBUG_PRINT 0

#if defined ENABLE_DEBUG_PRINT && ENABLE_DEBUG_PRINT != 0
	static LPUART_Type * const uart [] = LPUART_BASE_PTRS;

	/** ConfigUart:
	Configure a UART to use it to print debug-messages
	@param  void
	@return void */
	void ConfigUart (void)
	{
		BOARD_InitUARTPins ();

		uint32_t ClockFrequency = 0;
		if (CLOCK_GetMux (kCLOCK_UartMux) == 0) // --> PLL3 div6 80M
			ClockFrequency = (CLOCK_GetPllFreq (kCLOCK_PllUsb1) / 6U) / (CLOCK_GetDiv(kCLOCK_UartDiv) + 1U);
		else
			ClockFrequency = CLOCK_GetOscFreq() / (CLOCK_GetDiv(kCLOCK_UartDiv) + 1U);

		lpuart_config_t config;
		LPUART_GetDefaultConfig (&config);
		config.baudRate_Bps = 115200U;
		config.enableTx     = true;
		config.enableRx     = true;
		LPUART_Init (uart[BOARD_DEBUG_UART_INSTANCE], &config, ClockFrequency);
	}

	/** DebugPrint:
	Send a message to the UART
	@param Message The zero terminated string to send
	@return void */
	void DebugPrint (const char *Message)
	{
		LPUART_WriteBlocking (uart[BOARD_DEBUG_UART_INSTANCE], Message, strlen(Message));
	}
	/** DebugPrintf:
	Send a formated Message to the UART
	@param Message The zero terminated string to send including formating information
	@param  ... Sequence of additional arguments used to replace a format specifier
	@return void */
	void DebugPrintf (const char *Message, ...)
	{
		static char Buffer [128];
		va_list ArgPtr;
		va_start(ArgPtr, Message);
		int Length = vsnprintf (Buffer, sizeof(Buffer), Message, ArgPtr);
		va_end(ArgPtr);

		if (Length > 0 && Length <= sizeof(Buffer))
			LPUART_WriteBlocking (uart[BOARD_DEBUG_UART_INSTANCE], Buffer, Length);
	}
#else
	inline void ConfigUart (void)
	{
	}

	inline void DebugPrint (const char *Message)
	{
		(void)Message;
	}

	inline void DebugPrintf (const char *Message, ...)
	{
		(void)Message;
	}
#endif
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


#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include "DebugPrint.h"
#include "pin_mux.h"
#include "fsl_clock.h"


#if defined USART_BASE_PTRS
#if defined ENABLE_DEBUG_PRINT && ENABLE_DEBUG_PRINT != 0
	static USART_Type * const uart [] = USART_BASE_PTRS;

	#include "fsl_usart.h"

	/** ConfigUart:
	Configure a UART to use it to print debug-messages
	@param  void
	@return bool @true if successfully */
	bool ConfigUart (void)
	{
		BOARD_InitUARTPins ();
		const clock_frg_clk_config_t ClockConfig = 
		{
			0, kCLOCK_FrgPllDiv, 255, 0
		};

		CLOCK_SetFRGClock (&ClockConfig);
		const clock_attach_id_t Clock[] = {kFRG_to_FLEXCOMM0, kFRG_to_FLEXCOMM1, kFRG_to_FLEXCOMM2, kFRG_to_FLEXCOMM3, kFRG_to_FLEXCOMM4, kFRG_to_FLEXCOMM5, kFRG_to_FLEXCOMM6, kFRG_to_FLEXCOMM7};
		CLOCK_AttachClk   (Clock[DEBUG_CONSOLE_UART_INDEX]);


		usart_config_t config;
		USART_GetDefaultConfig (&config);
		config.baudRate_Bps     = 115200U;
		config.enableTx         = true;
		config.enableRx         = true;
		status_t status         = USART_Init (uart[DEBUG_CONSOLE_UART_INDEX], &config, CLOCK_GetFlexCommClkFreq(DEBUG_CONSOLE_UART_INDEX));
		return status == kStatus_Success;
	}

	/** DebugPrint:
	Send a message to the UART
	@param Message The zero terminated string to send
	@return void */
	void DebugPrint (const char *Message)
	{
		USART_WriteBlocking (uart[DEBUG_CONSOLE_UART_INDEX], Message, strlen(Message));
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
			USART_WriteBlocking (uart[DEBUG_CONSOLE_UART_INDEX], Buffer, Length);
	}
#else // defined ENABLE_DEBUG_PRINT && ENABLE_DEBUG_PRINT != 0
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
#endif // defined ENABLE_DEBUG_PRINT && ENABLE_DEBUG_PRINT != 0 ... else
#endif // defined USART_BASE_PTRS
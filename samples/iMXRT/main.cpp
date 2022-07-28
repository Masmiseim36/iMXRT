/** Sample for iMXRT-Family
Copyright (C) 2019-2022 Markus Klein
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
#include <fsl_device_registers.h>
#include <__cross_studio_io.h>


volatile uint32_t TickCounter = 0;
uint32_t TickLength = 0;


extern "C" void SysTick_Handler (void)
{
	TickCounter++;
}


bool InitializeSystemTick (uint32_t SysTickInMS)
{
	if (SysTickInMS == 0)
		return false;

	TickLength = SysTickInMS;


	SysTick->LOAD	= (SystemCoreClock / (1000/SysTickInMS)) -1;
	SysTick->VAL	= 0;
	SysTick->CTRL	= SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;

	__enable_irq ();
	return true;
}



void Sleep (uint32_t Milliseconds)
{
	uint32_t StartTime = TickCounter + (Milliseconds/TickLength);

	while (StartTime > TickCounter)
		;
}


int main (void)
{	
	SystemCoreClockUpdate ();
    if (debug_enabled ())
		debug_printf ("System-Clock is %d Hz\r\n", SystemCoreClock);

	InitializeSystemTick (1);

	while (true)
	{
		Sleep (1000);
		if (debug_enabled ())
			debug_printf ("Hello iMXRT\r\n");
	}
	return 0;
}
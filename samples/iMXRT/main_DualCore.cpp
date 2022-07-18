/** Sample for iMXRT-Family
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

#if __CORTEX_M == 7
	void InitializeM4Core (void)
	{
		// Start the core
		extern uint32_t __FlexSPI_segment_end__;
		uint32_t bootAddress = (uint32_t)&__FlexSPI_segment_end__;
		IOMUXC_LPSR_GPR->GPR0 = IOMUXC_LPSR_GPR_GPR0_CM4_INIT_VTOR_LOW ((uint32_t)bootAddress >> 3);
		IOMUXC_LPSR_GPR->GPR1 = IOMUXC_LPSR_GPR_GPR1_CM4_INIT_VTOR_HIGH((uint32_t)bootAddress >> 16);
		SRC->SCR |= SRC_SCR_BT_RELEASE_M4_MASK;
	}
#else
	static GPIO_Type *const GPIObase[] = GPIO_BASE_PTRS;
#endif



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

	#if __CORTEX_M == 7
		InitializeM4Core ();
		while (true)
		{
			Sleep (1000);
			if (debug_enabled ())
				debug_printf ("Hello iMXRT117x_M7\r\n");
		}
	#else
		IOMUXC->SW_MUX_CTL_PAD [67] = IOMUXC_SW_MUX_CTL_PAD_MUX_MODE (5) | IOMUXC_SW_MUX_CTL_PAD_SION (0);
		GPIObase[3]->GDIR |= (1 << 3);

		while (true)
		{
			Sleep (1000);
			GPIObase[3]->DR_TOGGLE = (1 << 3);
		}
	#endif
	return 0;
}

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
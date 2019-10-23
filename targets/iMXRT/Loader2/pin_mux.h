/** Loader for iMXRT-Family
Copyright (C) 2019  Markus Klein

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


#ifndef _PIN_MUX_H_
#define _PIN_MUX_H_

#include "fsl_device_registers.h"

enum BOARD_PinConfiguratiomn
{
	//                       Slow Slew Rate               | DSE_6_R0_6 (43 Ohm)          | medium(100MHz)                 | Pull/Keeper Enabled           | Hysteresis Disabled | Open Drain Disabled | Keeper | 100K Ohm Pull Down
	BOARD_UartPinConfig    = IOMUXC_SW_PAD_CTL_PAD_SRE(0) | IOMUXC_SW_PAD_CTL_PAD_DSE(6) | IOMUXC_SW_PAD_CTL_PAD_SPEED(2) | IOMUXC_SW_PAD_CTL_PAD_PKE(1),	// 0x10B0

	//                       Fast Slew Rate               | DSE_6_R0_6 (43 Ohm)          | max(200MHz)                    | Pull/Keeper Enabled           | Hysteresis Disabled | Open Drain Disabled | Keeper | 100K Ohm Pull Down
	BOARD_FlexSPIPinConfig = IOMUXC_SW_PAD_CTL_PAD_SRE(1) | IOMUXC_SW_PAD_CTL_PAD_DSE(6) | IOMUXC_SW_PAD_CTL_PAD_SPEED(3) | IOMUXC_SW_PAD_CTL_PAD_PKE(1)	// 0x0130F1
};


void BOARD_InitUARTPins    (void);
void BOARD_InitQuadSPIPins (void);
void BOARD_InitOctaSPIPins (void);

#if defined FLEXSPI2
	void BOARD_InitQuadSPI2Pins (void);
	void BOARD_InitOctaSPI2Pins (void);
#endif

#endif // _PIN_MUX_H_

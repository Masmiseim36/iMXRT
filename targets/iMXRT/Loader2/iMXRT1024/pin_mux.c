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


#include "fsl_common.h"
#include "fsl_iomuxc.h"
#include "fsl_gpio.h"
#include "pin_mux.h"


/** BOARD_InitUARTPins
Set pin muxing and configure electrical properties for the UART */
void BOARD_InitUARTPins (void)
{
	IOMUXC_SetPinMux    (IOMUXC_GPIO_AD_B0_06_LPUART1_TX, 0U);
	IOMUXC_SetPinConfig (IOMUXC_GPIO_AD_B0_06_LPUART1_TX, BOARD_UartPinConfig);

	IOMUXC_SetPinMux    (IOMUXC_GPIO_AD_B0_07_LPUART1_RX, 0U);
	IOMUXC_SetPinConfig (IOMUXC_GPIO_AD_B0_07_LPUART1_RX, BOARD_UartPinConfig);
}


/** BOARD_InitQuadSPIPins
Set pin muxing and configure electrical properties for QSPI */
void BOARD_InitQuadSPIPins (void)
{
	IOMUXC_SetPinMux    (IOMUXC_GPIO_SD_B1_05_FLEXSPI_A_DQS, 1U);
	IOMUXC_SetPinConfig (IOMUXC_GPIO_SD_B1_05_FLEXSPI_A_DQS, BOARD_FlexSPIPinConfig);

	IOMUXC_SetPinMux (IOMUXC_GPIO_AD_B1_00_FLEXSPI_A_DATA3, 1U);
	IOMUXC_SetPinMux (IOMUXC_GPIO_AD_B1_01_FLEXSPI_A_SCLK,   1U);
	IOMUXC_SetPinMux (IOMUXC_GPIO_AD_B1_02_FLEXSPI_A_DATA0, 1U);
	IOMUXC_SetPinMux (IOMUXC_GPIO_AD_B1_03_FLEXSPI_A_DATA2, 1U);
	IOMUXC_SetPinMux (IOMUXC_GPIO_AD_B1_04_FLEXSPI_A_DATA1, 1U);
	IOMUXC_SetPinMux (IOMUXC_GPIO_AD_B1_05_FLEXSPI_A_SS0_B,  1U);
}


/** BOARD_InitOctaSPIPins
Set pin muxing and configure electrical properties for Hyperflash/OctaSPI */ 
void BOARD_InitOctaSPIPins (void)
{
	BOARD_InitQuadSPIPins ();
}


void BOARD_PerformJEDECReset_FlexSPI (void)
{
	gpio_pin_config_t jreset_pin_config = 
	{
		kGPIO_DigitalOutput, 1, kGPIO_NoIntmode
	};

	// Configure the 3 pins used in JEDEC reset as GPIOs
	IOMUXC_SetPinMux (IOMUXC_GPIO_AD_B1_05_GPIO1_IO21, 1);	// IOMUXC_GPIO_AD_B1_05_FLEXSPI_A_SS0_B
	IOMUXC_SetPinMux (IOMUXC_GPIO_AD_B1_02_GPIO1_IO18, 1);	// IOMUXC_GPIO_AD_B1_02_FLEXSPI_A_DATA00
	IOMUXC_SetPinMux (IOMUXC_GPIO_AD_B1_01_GPIO1_IO17, 1);	// IOMUXC_GPIO_AD_B1_01_FLEXSPI_A_SCLK

	// Set the direction of 3 pins used in JEDEC reset to output
	GPIO_PinInit (GPIO1, 21, &jreset_pin_config); // CS		31 --> 21
	GPIO_PinInit (GPIO1, 18, &jreset_pin_config); // SI/IO0	28 --> 18
	GPIO_PinInit (GPIO1, 17, &jreset_pin_config); // SCK	27 --> 17


	// Perform a reset sequence:
	// CS goes low 4 times with alternating values of SOUT
	// SCK is drive low or high and must stay in one state
	GPIO_WritePinOutput(GPIO1, 17, 0); // set SCK low
	for (uint32_t i = 0; i < 4; i++)
	{
		// drive CS low
		GPIO_WritePinOutput (GPIO1, 21, 0);
		SDK_DelayAtLeastUs (1, SystemCoreClock);
		// drive SI low or high: alternate its state every iteration
		GPIO_WritePinOutput (GPIO1, 18, (i&1));
		// drive CS high; SI state will be captured on the CS rising edge
		GPIO_WritePinOutput (GPIO1, 21, 1);
		SDK_DelayAtLeastUs (1, SystemCoreClock);
	}

	SDK_DelayAtLeastUs (110, SystemCoreClock);
}

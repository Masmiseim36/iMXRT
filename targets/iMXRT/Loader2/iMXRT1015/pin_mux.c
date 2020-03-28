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

	IOMUXC_SetPinMux    (IOMUXC_GPIO_SD_B1_06_FLEXSPI_A_DATA03, 1U);
	IOMUXC_SetPinConfig (IOMUXC_GPIO_SD_B1_06_FLEXSPI_A_DATA03, BOARD_FlexSPIPinConfig);

	IOMUXC_SetPinMux    (IOMUXC_GPIO_SD_B1_07_FLEXSPI_A_SCLK, 1U);
	IOMUXC_SetPinConfig (IOMUXC_GPIO_SD_B1_07_FLEXSPI_A_SCLK, BOARD_FlexSPIPinConfig);

	IOMUXC_SetPinMux    (IOMUXC_GPIO_SD_B1_08_FLEXSPI_A_DATA00, 1U);
	IOMUXC_SetPinConfig (IOMUXC_GPIO_SD_B1_08_FLEXSPI_A_DATA00, BOARD_FlexSPIPinConfig);

	IOMUXC_SetPinMux    (IOMUXC_GPIO_SD_B1_09_FLEXSPI_A_DATA02, 1U);
	IOMUXC_SetPinConfig (IOMUXC_GPIO_SD_B1_09_FLEXSPI_A_DATA02, BOARD_FlexSPIPinConfig);

	IOMUXC_SetPinMux    (IOMUXC_GPIO_SD_B1_10_FLEXSPI_A_DATA01, 1U);
	IOMUXC_SetPinConfig (IOMUXC_GPIO_SD_B1_10_FLEXSPI_A_DATA01, BOARD_FlexSPIPinConfig);

	IOMUXC_SetPinMux    (IOMUXC_GPIO_SD_B1_11_FLEXSPI_A_SS0_B, 1U);
	IOMUXC_SetPinConfig (IOMUXC_GPIO_SD_B1_11_FLEXSPI_A_SS0_B, BOARD_FlexSPIPinConfig);
}


/** BOARD_InitOctaSPIPins
Set pin muxing and configure electrical properties for Hyperflash/OctaSPI */ 
void BOARD_InitOctaSPIPins (void)
{
	IOMUXC_SetPinMux    (IOMUXC_GPIO_SD_B1_00_FLEXSPI_B_DATA03, 1U);
	IOMUXC_SetPinConfig (IOMUXC_GPIO_SD_B1_00_FLEXSPI_B_DATA03, BOARD_FlexSPIPinConfig);

	IOMUXC_SetPinMux    (IOMUXC_GPIO_SD_B1_01_FLEXSPI_B_SCLK, 1U);
	IOMUXC_SetPinConfig (IOMUXC_GPIO_SD_B1_01_FLEXSPI_B_SCLK, BOARD_FlexSPIPinConfig);

	IOMUXC_SetPinMux    (IOMUXC_GPIO_SD_B1_02_FLEXSPI_B_DATA00, 1U);
	IOMUXC_SetPinConfig (IOMUXC_GPIO_SD_B1_02_FLEXSPI_B_DATA00, BOARD_FlexSPIPinConfig);

	IOMUXC_SetPinMux    (IOMUXC_GPIO_SD_B1_03_FLEXSPI_B_DATA02, 1U);
	IOMUXC_SetPinConfig (IOMUXC_GPIO_SD_B1_03_FLEXSPI_B_DATA02, BOARD_FlexSPIPinConfig);

	IOMUXC_SetPinMux    (IOMUXC_GPIO_SD_B1_04_FLEXSPI_B_DATA01, 1U);
	IOMUXC_SetPinConfig (IOMUXC_GPIO_SD_B1_04_FLEXSPI_B_DATA01, BOARD_FlexSPIPinConfig);

	BOARD_InitQuadSPIPins ();
}


void BOARD_PerformJEDECReset (void)
{
	gpio_pin_config_t jreset_pin_config = 
	{
		kGPIO_DigitalOutput, 1, kGPIO_NoIntmode
	};

	// Configure the 3 pins used in JEDEC reset as GPIOs
	IOMUXC_SetPinMux (IOMUXC_GPIO_SD_B1_11_GPIO3_IO31, 1);	// IOMUXC_GPIO_SD_B1_11_FLEXSPI_A_SS0_B
	IOMUXC_SetPinMux (IOMUXC_GPIO_SD_B1_02_GPIO3_IO22, 1);	// IOMUXC_GPIO_SD_B1_02_FLEXSPI_B_DATA00
	IOMUXC_SetPinMux (IOMUXC_GPIO_SD_B1_07_GPIO3_IO27, 1);	// IOMUXC_GPIO_SD_B1_07_FLEXSPI_A_SCLK

	// Set the direction of 3 pins used in JEDEC reset to output
	GPIO_PinInit (GPIO3, 31, &jreset_pin_config); // CS
	GPIO_PinInit (GPIO3, 22, &jreset_pin_config); // SI/IO0
	GPIO_PinInit (GPIO3, 27, &jreset_pin_config); // SCK


	// Perform a reset sequence:
	// CS goes low 4 times with alternating values of SOUT
	// SCK is drive low or high and must stay in one state
	GPIO_WritePinOutput(GPIO3, 27, 0); // set SCK low
	for (uint32_t i = 0; i < 4; i++)
	{
		// drive CS low
		GPIO_WritePinOutput (GPIO3, 31, 0);
		//for(j=0; j<100; j++);
		// drive SI low or high: alternate its state every iteration
		GPIO_WritePinOutput (GPIO3, 22, (i&1));
		// drive CS high; SI state will be captured on the CS rising edge
		GPIO_WritePinOutput (GPIO3, 31, 1);
		//for(j=0; j<100; j++);
	}
}
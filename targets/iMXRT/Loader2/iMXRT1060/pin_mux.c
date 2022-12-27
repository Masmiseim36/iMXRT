/** Loader for iMXRT-Family
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

#include "fsl_common.h"
#include "fsl_iomuxc.h"
#include "fsl_gpio.h"
#include "pin_mux.h"

/** BOARD_InitUARTPins
Set pin muxing and configure electrical properties for the UART */
void BOARD_InitUARTPins (void)
{
	IOMUXC_SetPinMux    (IOMUXC_GPIO_AD_B0_12_LPUART1_TX, 0U);
	IOMUXC_SetPinConfig (IOMUXC_GPIO_AD_B0_12_LPUART1_TX, BOARD_UartPinConfig);

	IOMUXC_SetPinMux    (IOMUXC_GPIO_AD_B0_13_LPUART1_RX, 0U);
	IOMUXC_SetPinConfig (IOMUXC_GPIO_AD_B0_13_LPUART1_RX, BOARD_UartPinConfig);
}


/** BOARD_InitQuadSPIPins
Set pin muxing and configure electrical properties for QSPI */
void BOARD_InitQuadSPIPins (void)
{
	// alternative pin output: IOMUXC_GPIO_AD_B1_09_FLEXSPIA_DQS
	IOMUXC_SetPinMux    (IOMUXC_GPIO_SD_B1_05_FLEXSPIA_DQS, 1U);
	IOMUXC_SetPinConfig (IOMUXC_GPIO_SD_B1_05_FLEXSPIA_DQS, BOARD_FlexSPIPinConfig);

	// alternative pin output: IOMUXC_GPIO_AD_B1_15_FLEXSPIA_SS0_B
	IOMUXC_SetPinMux    (IOMUXC_GPIO_SD_B1_06_FLEXSPIA_SS0_B, 1U);
	IOMUXC_SetPinConfig (IOMUXC_GPIO_SD_B1_06_FLEXSPIA_SS0_B, BOARD_FlexSPIPinConfig);

	// alternative pin output: IOMUXC_GPIO_AD_B1_14_FLEXSPIA_SCLK
	IOMUXC_SetPinMux    (IOMUXC_GPIO_SD_B1_07_FLEXSPIA_SCLK, 1U);
	IOMUXC_SetPinConfig (IOMUXC_GPIO_SD_B1_07_FLEXSPIA_SCLK, BOARD_FlexSPIPinConfig);

	// alternative pin output: IOMUXC_GPIO_AD_B1_13_FLEXSPIA_DATA00
	IOMUXC_SetPinMux    (IOMUXC_GPIO_SD_B1_08_FLEXSPIA_DATA00, 1U);
	IOMUXC_SetPinConfig (IOMUXC_GPIO_SD_B1_08_FLEXSPIA_DATA00, BOARD_FlexSPIPinConfig);

	// alternative pin output: IOMUXC_GPIO_AD_B1_12_FLEXSPIA_DATA01
	IOMUXC_SetPinMux    (IOMUXC_GPIO_SD_B1_09_FLEXSPIA_DATA01, 1U);
	IOMUXC_SetPinConfig (IOMUXC_GPIO_SD_B1_09_FLEXSPIA_DATA01, BOARD_FlexSPIPinConfig);

	// alternative pin output: IOMUXC_GPIO_AD_B1_11_FLEXSPIA_DATA02
	IOMUXC_SetPinMux    (IOMUXC_GPIO_SD_B1_10_FLEXSPIA_DATA02, 1U);
	IOMUXC_SetPinConfig (IOMUXC_GPIO_SD_B1_10_FLEXSPIA_DATA02, BOARD_FlexSPIPinConfig);

	// alternative pin output: IOMUXC_GPIO_AD_B1_10_FLEXSPIA_DATA03
	IOMUXC_SetPinMux    (IOMUXC_GPIO_SD_B1_11_FLEXSPIA_DATA03, 1U);
	IOMUXC_SetPinConfig (IOMUXC_GPIO_SD_B1_11_FLEXSPIA_DATA03, BOARD_FlexSPIPinConfig);
}

/** BOARD_InitOctaSPIPins
Set pin muxing and configure electrical properties for Hyperflash/OctaSPI */ 
void BOARD_InitOctaSPIPins (void)
{
	// alternative pin output: IOMUXC_GPIO_AD_B1_04_FLEXSPIB_DATA03
	IOMUXC_SetPinMux    (IOMUXC_GPIO_SD_B1_00_FLEXSPIB_DATA03, 1U);
	IOMUXC_SetPinConfig (IOMUXC_GPIO_SD_B1_00_FLEXSPIB_DATA03, BOARD_FlexSPIPinConfig);

	// alternative pin output: IOMUXC_GPIO_AD_B1_05_FLEXSPIB_DATA02
	IOMUXC_SetPinMux    (IOMUXC_GPIO_SD_B1_01_FLEXSPIB_DATA02, 1U);
	IOMUXC_SetPinConfig (IOMUXC_GPIO_SD_B1_01_FLEXSPIB_DATA02, BOARD_FlexSPIPinConfig);

	// alternative pin output: IOMUXC_GPIO_AD_B1_06_FLEXSPIB_DATA01
	IOMUXC_SetPinMux    (IOMUXC_GPIO_SD_B1_02_FLEXSPIB_DATA01, 1U);
	IOMUXC_SetPinConfig (IOMUXC_GPIO_SD_B1_02_FLEXSPIB_DATA01, BOARD_FlexSPIPinConfig);

	// alternative pin output: IOMUXC_GPIO_AD_B1_07_FLEXSPIB_DATA00
	IOMUXC_SetPinMux    (IOMUXC_GPIO_SD_B1_03_FLEXSPIB_DATA00, 1U);
	IOMUXC_SetPinConfig (IOMUXC_GPIO_SD_B1_03_FLEXSPIB_DATA00, BOARD_FlexSPIPinConfig);

	// alternative pin output: no alternative, maybe IOMUXC_GPIO_AD_B1_08_FLEXSPIA_SS1_B??
	IOMUXC_SetPinMux    (IOMUXC_GPIO_SD_B1_04_FLEXSPIB_SCLK, 1U);
	IOMUXC_SetPinConfig (IOMUXC_GPIO_SD_B1_04_FLEXSPIB_SCLK, BOARD_FlexSPIPinConfig);

	BOARD_InitQuadSPIPins ();
}

void BOARD_PerformJEDECReset_FlexSPI1 (void)
{
	gpio_pin_config_t jreset_pin_config = 
	{
		kGPIO_DigitalOutput, 1, kGPIO_NoIntmode
	};

	// Configure the 3 pins used in JEDEC reset as GPIOs
	IOMUXC_SetPinMux (IOMUXC_GPIO_SD_B1_06_GPIO3_IO06, 1);	// IOMUXC_GPIO_SD_B1_06_FLEXSPIA_SS0_B
	IOMUXC_SetPinMux (IOMUXC_GPIO_SD_B1_08_GPIO3_IO08, 1);	// IOMUXC_GPIO_SD_B1_08_FLEXSPIA_DATA00
	IOMUXC_SetPinMux (IOMUXC_GPIO_SD_B1_07_GPIO3_IO07, 1);	// IOMUXC_GPIO_SD_B1_07_FLEXSPIA_SCLK

	// Set the direction of 3 pins used in JEDEC reset to output
	GPIO_PinInit (GPIO3, 6, &jreset_pin_config); // CS
	GPIO_PinInit (GPIO3, 8, &jreset_pin_config); // SI/IO0
	GPIO_PinInit (GPIO3, 7, &jreset_pin_config); // SCK


	// Perform a reset sequence:
	// CS goes low 4 times with alternating values of SOUT
	// SCK is drive low or high and must stay in one state
	GPIO_WritePinOutput (GPIO3, 7, 0); // set SCK low
	for (uint32_t i = 0; i < 4; i++)
	{
		// drive CS low
		GPIO_WritePinOutput (GPIO3, 6, 0);
		SDK_DelayAtLeastUs (1, SystemCoreClock);
		// drive SI low or high: alternate its state every iteration
		GPIO_WritePinOutput (GPIO3, 8, (i&1));
		// drive CS high; SI state will be captured on the CS rising edge
		GPIO_WritePinOutput (GPIO3, 6, 1);
		SDK_DelayAtLeastUs (1, SystemCoreClock);
	}

	SDK_DelayAtLeastUs (110, SystemCoreClock);
}



/** BOARD_InitQuadSPI2Pins
Set pin muxing and configure electrical properties for QSPI */
void BOARD_InitQuadSPI2Pins (void)
{
	IOMUXC_SetPinMux    (IOMUXC_GPIO_EMC_23_FLEXSPI2_A_DQS, 1U);
	IOMUXC_SetPinConfig (IOMUXC_GPIO_EMC_23_FLEXSPI2_A_DQS, BOARD_FlexSPIPinConfig);

	IOMUXC_SetPinMux    (IOMUXC_GPIO_EMC_24_FLEXSPI2_A_SS0_B, 1U);
	IOMUXC_SetPinConfig (IOMUXC_GPIO_EMC_24_FLEXSPI2_A_SS0_B, BOARD_FlexSPIPinConfig);

	IOMUXC_SetPinMux    (IOMUXC_GPIO_EMC_25_FLEXSPI2_A_SCLK, 1U);
	IOMUXC_SetPinConfig (IOMUXC_GPIO_EMC_25_FLEXSPI2_A_SCLK, BOARD_FlexSPIPinConfig);

	IOMUXC_SetPinMux    (IOMUXC_GPIO_EMC_26_FLEXSPI2_A_DATA00, 1U);
	IOMUXC_SetPinConfig (IOMUXC_GPIO_EMC_26_FLEXSPI2_A_DATA00, BOARD_FlexSPIPinConfig);

	IOMUXC_SetPinMux    (IOMUXC_GPIO_EMC_27_FLEXSPI2_A_DATA01, 1U);
	IOMUXC_SetPinConfig (IOMUXC_GPIO_EMC_27_FLEXSPI2_A_DATA01, BOARD_FlexSPIPinConfig);

	IOMUXC_SetPinMux    (IOMUXC_GPIO_EMC_28_FLEXSPI2_A_DATA02, 1U);
	IOMUXC_SetPinConfig (IOMUXC_GPIO_EMC_28_FLEXSPI2_A_DATA02, BOARD_FlexSPIPinConfig);

	IOMUXC_SetPinMux    (IOMUXC_GPIO_EMC_29_FLEXSPI2_A_DATA03, 1U);
	IOMUXC_SetPinConfig (IOMUXC_GPIO_EMC_29_FLEXSPI2_A_DATA03, BOARD_FlexSPIPinConfig);
}

/** BOARD_InitOctaSPI2Pins
Set pin muxing and configure electrical properties for Hyperflash/OctaSPI */ 
void BOARD_InitOctaSPI2Pins (void)
{
	// alternative pin output: IOMUXC_GPIO_AD_B1_04_FLEXSPIB_DATA03
	IOMUXC_SetPinMux    (IOMUXC_GPIO_EMC_16_FLEXSPI2_B_DATA03, 1U);
	IOMUXC_SetPinConfig (IOMUXC_GPIO_EMC_16_FLEXSPI2_B_DATA03, BOARD_FlexSPIPinConfig);

	// alternative pin output: IOMUXC_GPIO_AD_B1_05_FLEXSPIB_DATA02
	IOMUXC_SetPinMux    (IOMUXC_GPIO_EMC_15_FLEXSPI2_B_DATA02, 1U);
	IOMUXC_SetPinConfig (IOMUXC_GPIO_EMC_15_FLEXSPI2_B_DATA02, BOARD_FlexSPIPinConfig);

	// alternative pin output: IOMUXC_GPIO_AD_B1_06_FLEXSPIB_DATA01
	IOMUXC_SetPinMux    (IOMUXC_GPIO_EMC_14_FLEXSPI2_B_DATA01, 1U);
	IOMUXC_SetPinConfig (IOMUXC_GPIO_EMC_14_FLEXSPI2_B_DATA01, BOARD_FlexSPIPinConfig);

	// alternative pin output: IOMUXC_GPIO_AD_B1_07_FLEXSPIB_DATA00
	IOMUXC_SetPinMux    (IOMUXC_GPIO_EMC_13_FLEXSPI2_B_DATA00, 1U);
	IOMUXC_SetPinConfig (IOMUXC_GPIO_EMC_13_FLEXSPI2_B_DATA00, BOARD_FlexSPIPinConfig);

	// alternative pin output: no alternative, maybe IOMUXC_GPIO_AD_B1_08_FLEXSPIA_SS1_B??
	IOMUXC_SetPinMux    (IOMUXC_GPIO_EMC_12_FLEXSPI2_B_SCLK, 1U);
	IOMUXC_SetPinConfig (IOMUXC_GPIO_EMC_12_FLEXSPI2_B_SCLK, BOARD_FlexSPIPinConfig);

	BOARD_InitQuadSPI2Pins ();
}

void BOARD_PerformJEDECReset_FlexSPI2 (void)
{
	gpio_pin_config_t jreset_pin_config = 
	{
		kGPIO_DigitalOutput, 1, kGPIO_NoIntmode
	};

	// Configure the 3 pins used in JEDEC reset as GPIOs
	IOMUXC_SetPinMux (IOMUXC_GPIO_EMC_24_GPIO4_IO24, 1);	// IOMUXC_GPIO_EMC_24_FLEXSPI2_A_SS0_B
	IOMUXC_SetPinMux (IOMUXC_GPIO_EMC_26_GPIO4_IO26, 1);	// IOMUXC_GPIO_EMC_26_FLEXSPI2_A_DATA00
	IOMUXC_SetPinMux (IOMUXC_GPIO_EMC_25_GPIO4_IO25, 1);	// IOMUXC_GPIO_EMC_25_FLEXSPI2_A_SCLK

	// Set the direction of 3 pins used in JEDEC reset to output
	GPIO_PinInit (GPIO4, 24, &jreset_pin_config); // CS
	GPIO_PinInit (GPIO4, 26, &jreset_pin_config); // SI/IO0
	GPIO_PinInit (GPIO4, 25, &jreset_pin_config); // SCK


	// Perform a reset sequence:
	// CS goes low 4 times with alternating values of SOUT
	// SCK is drive low or high and must stay in one state
	GPIO_WritePinOutput (GPIO4, 25, 0); // set SCK low
	for (uint32_t i = 0; i < 4; i++)
	{
		// drive CS low
		GPIO_WritePinOutput (GPIO4, 24, 0);
		SDK_DelayAtLeastUs (1, SystemCoreClock);
		// drive SI low or high: alternate its state every iteration
		GPIO_WritePinOutput (GPIO4, 26, (i&1));
		// drive CS high; SI state will be captured on the CS rising edge
		GPIO_WritePinOutput (GPIO4, 24, 1);
		SDK_DelayAtLeastUs (1, SystemCoreClock);
	}

	SDK_DelayAtLeastUs (110, SystemCoreClock);
}
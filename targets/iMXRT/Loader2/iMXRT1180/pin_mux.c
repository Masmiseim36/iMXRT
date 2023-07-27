/** Loader for iMXRT-Family
Copyright (C) 2019-2023 Markus Klein
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
#include "pin_mux.h"
#include "board.h"

/*! BOARD_InitUARTPins
Set pin muxing and configure electrical properties for the UART */

void BOARD_InitUARTPins (void)
{
	CLOCK_EnableClock (kCLOCK_Iomuxc1);          /* Turn on LPCG: LPCG is ON. */
	CLOCK_EnableClock (kCLOCK_Iomuxc2);          /* Turn on LPCG: LPCG is ON. */

	#if DEBUG_CONSOLE_UART_INDEX == 1
		IOMUXC_SetPinMux (IOMUXC_GPIO_AON_08_LPUART1_TX, 0U); /* Software Input On Field: Input Path is determined by functionality */
		IOMUXC_SetPinMux (IOMUXC_GPIO_AON_09_LPUART1_RX, 0U); /* Software Input On Field: Input Path is determined by functionality */
	#else
		#error "Unknown serial port"
	#endif
}


/*! \brief Set pin muxing and configure electrical properties for QSPI */
void BOARD_InitQuadSPI1_A_Pins (void)
{
	CLOCK_EnableClock (kCLOCK_Iomuxc1);          // Turn on LPCG
	CLOCK_EnableClock (kCLOCK_Iomuxc2);          // Turn on LPCG

	IOMUXC_SetPinMux    (IOMUXC_GPIO_B2_08_FLEXSPI1_BUS2BIT_A_SCLK,   1U); /* Software Input On Field: Force input path of pad GPIO_B2_08 */
	IOMUXC_SetPinConfig (IOMUXC_GPIO_B2_08_FLEXSPI1_BUS2BIT_A_SCLK,   IOMUXC_SW_PAD_CTL_PAD_PUS_MASK | IOMUXC_SW_PAD_CTL_PAD_DSE_MASK);
	IOMUXC_SetPinMux    (IOMUXC_GPIO_B2_09_FLEXSPI1_BUS2BIT_A_SS0_B,  1U); /* Software Input On Field: Force input path of pad GPIO_B2_09 */
	IOMUXC_SetPinConfig (IOMUXC_GPIO_B2_09_FLEXSPI1_BUS2BIT_A_SS0_B,  IOMUXC_SW_PAD_CTL_PAD_PUS_MASK | IOMUXC_SW_PAD_CTL_PAD_DSE_MASK);
	IOMUXC_SetPinMux    (IOMUXC_GPIO_B2_10_FLEXSPI1_BUS2BIT_A_DATA00, 1U); /* Software Input On Field: Force input path of pad GPIO_B2_10 */
	IOMUXC_SetPinConfig (IOMUXC_GPIO_B2_10_FLEXSPI1_BUS2BIT_A_DATA00, IOMUXC_SW_PAD_CTL_PAD_PUS_MASK | IOMUXC_SW_PAD_CTL_PAD_DSE_MASK);
	IOMUXC_SetPinMux    (IOMUXC_GPIO_B2_11_FLEXSPI1_BUS2BIT_A_DATA01, 1U); /* Software Input On Field: Force input path of pad GPIO_B2_11 */
	IOMUXC_SetPinConfig (IOMUXC_GPIO_B2_11_FLEXSPI1_BUS2BIT_A_DATA01, IOMUXC_SW_PAD_CTL_PAD_PUS_MASK | IOMUXC_SW_PAD_CTL_PAD_DSE_MASK);
	IOMUXC_SetPinMux    (IOMUXC_GPIO_B2_12_FLEXSPI1_BUS2BIT_A_DATA02, 1U); /* Software Input On Field: Force input path of pad GPIO_B2_12 */
	IOMUXC_SetPinConfig (IOMUXC_GPIO_B2_12_FLEXSPI1_BUS2BIT_A_DATA02, IOMUXC_SW_PAD_CTL_PAD_PUS_MASK | IOMUXC_SW_PAD_CTL_PAD_DSE_MASK);
	IOMUXC_SetPinMux    (IOMUXC_GPIO_B2_13_FLEXSPI1_BUS2BIT_A_DATA03, 1U); /* Software Input On Field: Force input path of pad GPIO_B2_13 */
	IOMUXC_SetPinConfig (IOMUXC_GPIO_B2_13_FLEXSPI1_BUS2BIT_A_DATA03, IOMUXC_SW_PAD_CTL_PAD_PUS_MASK | IOMUXC_SW_PAD_CTL_PAD_DSE_MASK);
}

/*! \brief Set pin muxing and configure electrical properties for Hyperflash/OctaSPI */ 
void BOARD_InitOctaSPI1_A_Pins (void)
{
	BOARD_InitQuadSPI1_A_Pins ();

	IOMUXC_SetPinMux    (IOMUXC_GPIO_B2_07_FLEXSPI1_BUS2BIT_A_DQS,    1U); /* Software Input On Field: Force input path of pad GPIO_B2_07 */
	IOMUXC_SetPinConfig (IOMUXC_GPIO_B2_07_FLEXSPI1_BUS2BIT_A_DQS,    IOMUXC_SW_PAD_CTL_PAD_PUS_MASK | IOMUXC_SW_PAD_CTL_PAD_DSE_MASK);
	IOMUXC_SetPinMux    (IOMUXC_GPIO_B2_03_FLEXSPI1_BUS2BIT_A_DATA04, 1U);
	IOMUXC_SetPinConfig (IOMUXC_GPIO_B2_03_FLEXSPI1_BUS2BIT_A_DATA04, IOMUXC_SW_PAD_CTL_PAD_PUS_MASK | IOMUXC_SW_PAD_CTL_PAD_DSE_MASK);
	IOMUXC_SetPinMux    (IOMUXC_GPIO_B2_04_FLEXSPI1_BUS2BIT_A_DATA05, 1U);
	IOMUXC_SetPinConfig (IOMUXC_GPIO_B2_04_FLEXSPI1_BUS2BIT_A_DATA05, IOMUXC_SW_PAD_CTL_PAD_PUS_MASK | IOMUXC_SW_PAD_CTL_PAD_DSE_MASK);
	IOMUXC_SetPinMux    (IOMUXC_GPIO_B2_05_FLEXSPI1_BUS2BIT_A_DATA06, 1U);
	IOMUXC_SetPinConfig (IOMUXC_GPIO_B2_05_FLEXSPI1_BUS2BIT_A_DATA06, IOMUXC_SW_PAD_CTL_PAD_PUS_MASK | IOMUXC_SW_PAD_CTL_PAD_DSE_MASK);
	IOMUXC_SetPinMux    (IOMUXC_GPIO_B2_06_FLEXSPI1_BUS2BIT_A_DATA07, 1U);
	IOMUXC_SetPinConfig (IOMUXC_GPIO_B2_06_FLEXSPI1_BUS2BIT_A_DATA07, IOMUXC_SW_PAD_CTL_PAD_PUS_MASK | IOMUXC_SW_PAD_CTL_PAD_DSE_MASK);
}

/*! \brief Perform a JEDEC compatible reset sequence FlexSPI1 Port A */ 
void BOARD_PerformJEDECReset_FlexSPI1_A (void)
{
	rgpio_pin_config_t jreset_pin_config = 
	{
		kRGPIO_DigitalOutput, 1
	};

	// Configure the 3 pins used in JEDEC reset as GPIOs
	IOMUXC_SetPinMux (IOMUXC_GPIO_B2_09_GPIO6_IO23, 1);	// IOMUXC_GPIO_B2_09_FLEXSPI1_BUS2BIT_A_SS0_B	-	IOMUXC_GPIO_B2_09_GPIO6_IO23
	IOMUXC_SetPinMux (IOMUXC_GPIO_B2_10_GPIO6_IO24, 1);	// IOMUXC_GPIO_B2_10_FLEXSPI1_BUS2BIT_A_DATA00	-	IOMUXC_GPIO_B2_10_GPIO6_IO24
	IOMUXC_SetPinMux (IOMUXC_GPIO_B2_08_GPIO6_IO22, 1);	// IOMUXC_GPIO_B2_08_FLEXSPI1_BUS2BIT_A_SCLK	-	IOMUXC_GPIO_B2_08_GPIO6_IO22

	// Set the direction of 3 pins used in JEDEC reset to output
	RGPIO_PinInit (RGPIO6, 23, &jreset_pin_config); // CS
	RGPIO_PinInit (RGPIO6, 24, &jreset_pin_config); // SI/IO0
	RGPIO_PinInit (RGPIO6, 22, &jreset_pin_config); // SCK


	// Perform a reset sequence:
	// CS goes low 4 times with alternating values of SOUT
	// SCK is drive low or high and must stay in one state
	RGPIO_WritePinOutput (RGPIO6, 22, 0); // set SCK low
	for (uint32_t i = 0; i < 4; i++)
	{
		// drive CS low
		RGPIO_WritePinOutput (RGPIO6, 23, 0);
		SDK_DelayAtLeastUs (1, SystemCoreClock);
		// drive SI low or high: alternate its state every iteration
		RGPIO_WritePinOutput (RGPIO6, 24, (i&1));
		// drive CS high; SI state will be captured on the CS rising edge
		RGPIO_WritePinOutput (RGPIO6, 23, 1);
		SDK_DelayAtLeastUs (1, SystemCoreClock);
	}

	SDK_DelayAtLeastUs (110, SystemCoreClock);
}


/*! \brief Set pin muxing and configure electrical properties for QSPI */
void BOARD_InitQuadSPI1_B_Pins (void)
{
	CLOCK_EnableClock (kCLOCK_Iomuxc1);          // Turn on LPCG
	CLOCK_EnableClock (kCLOCK_Iomuxc2);          // Turn on LPCG

//	#define IOMUXC_GPIO_SD_B1_03_FLEXSPI1_BUS2BIT_B_SS1_B     0x42A101A8U, 0x9U,        0x0U, 0x0U, 0x42A103F0U
//	#define IOMUXC_GPIO_SD_B1_04_FLEXSPI1_BUS2BIT_B_SS0_B     0x42A101ACU, 0x8U,        0x0U, 0x0U, 0x42A103F4U
//	#define IOMUXC_GPIO_SD_B1_05_FLEXSPI1_BUS2BIT_B_SS0_B     0x42A101B0U, 0x9U,        0x0U, 0x0U, 0x42A103F8U

	IOMUXC_SetPinMux    (IOMUXC_GPIO_SD_B2_07_FLEXSPI1_BUS2BIT_B_SCLK,   1U); /* Software Input On Field: Force input path of pad GPIO_SD_B2_07 */
	IOMUXC_SetPinConfig (IOMUXC_GPIO_SD_B2_07_FLEXSPI1_BUS2BIT_B_SCLK,   IOMUXC_SW_PAD_CTL_PAD_PUS_MASK | IOMUXC_SW_PAD_CTL_PAD_DSE_MASK);
	IOMUXC_SetPinMux    (IOMUXC_GPIO_SD_B2_06_FLEXSPI1_BUS2BIT_B_SS0_B,  1U); /* Software Input On Field: Force input path of pad GPIO_SD_B2_06 */
	IOMUXC_SetPinConfig (IOMUXC_GPIO_SD_B2_06_FLEXSPI1_BUS2BIT_B_SS0_B,  IOMUXC_SW_PAD_CTL_PAD_PUS_MASK | IOMUXC_SW_PAD_CTL_PAD_DSE_MASK);
	IOMUXC_SetPinMux    (IOMUXC_GPIO_SD_B2_08_FLEXSPI1_BUS2BIT_B_DATA00, 1U); /* Software Input On Field: Force input path of pad GPIO_SD_B2_08 */
	IOMUXC_SetPinConfig (IOMUXC_GPIO_SD_B2_08_FLEXSPI1_BUS2BIT_B_DATA00, IOMUXC_SW_PAD_CTL_PAD_PUS_MASK | IOMUXC_SW_PAD_CTL_PAD_DSE_MASK);
	IOMUXC_SetPinMux    (IOMUXC_GPIO_SD_B2_09_FLEXSPI1_BUS2BIT_B_DATA01, 1U); /* Software Input On Field: Force input path of pad GPIO_SD_B2_09 */
	IOMUXC_SetPinConfig (IOMUXC_GPIO_SD_B2_09_FLEXSPI1_BUS2BIT_B_DATA01, IOMUXC_SW_PAD_CTL_PAD_PUS_MASK | IOMUXC_SW_PAD_CTL_PAD_DSE_MASK);
	IOMUXC_SetPinMux    (IOMUXC_GPIO_SD_B2_10_FLEXSPI1_BUS2BIT_B_DATA02, 1U); /* Software Input On Field: Force input path of pad GPIO_SD_B2_10 */
	IOMUXC_SetPinConfig (IOMUXC_GPIO_SD_B2_10_FLEXSPI1_BUS2BIT_B_DATA02, IOMUXC_SW_PAD_CTL_PAD_PUS_MASK | IOMUXC_SW_PAD_CTL_PAD_DSE_MASK);
	IOMUXC_SetPinMux    (IOMUXC_GPIO_SD_B2_11_FLEXSPI1_BUS2BIT_B_DATA03, 1U); /* Software Input On Field: Force input path of pad GPIO_SD_B2_11 */
	IOMUXC_SetPinConfig (IOMUXC_GPIO_SD_B2_11_FLEXSPI1_BUS2BIT_B_DATA03, IOMUXC_SW_PAD_CTL_PAD_PUS_MASK | IOMUXC_SW_PAD_CTL_PAD_DSE_MASK);

//	#define IOMUXC_GPIO_B1_10_FLEXSPI1_BUS2BIT_B_DATA03       0x42A10210U, 0x7U, 0x42A10558U, 0x1U, 0x42A10458U
//	#define IOMUXC_GPIO_B1_11_FLEXSPI1_BUS2BIT_B_DATA02       0x42A10214U, 0x7U, 0x42A10554U, 0x1U, 0x42A1045CU
//	#define IOMUXC_GPIO_B1_12_FLEXSPI1_BUS2BIT_B_DATA01       0x42A10218U, 0x7U, 0x42A10550U, 0x1U, 0x42A10460U
//	#define IOMUXC_GPIO_B1_13_FLEXSPI1_BUS2BIT_B_DATA00       0x42A1021CU, 0x7U, 0x42A1054CU, 0x1U, 0x42A10464U
//	#define IOMUXC_GPIO_B2_02_FLEXSPI1_BUS2BIT_B_SCLK         0x42A10228U, 0x6U, 0x42A1056CU, 0x2U, 0x42A10470U
//	#define IOMUXC_GPIO_B1_04_FLEXSPI1_BUS2BIT_B_SS0_B        0x42A101F8U, 0x7U,        0x0U, 0x0U, 0x42A10440U
//	#define IOMUXC_GPIO_B1_05_FLEXSPI1_BUS2BIT_B_SCLK         0x42A101FCU, 0x7U, 0x42A1056CU, 0x1U, 0x42A10444U
//	#define IOMUXC_GPIO_B1_02_FLEXSPI1_BUS2BIT_B_SS1_B        0x42A101F0U, 0x7U,        0x0U, 0x0U, 0x42A10438U
}

/*! \brief Set pin muxing and configure electrical properties for Hyperflash/OctaSPI */ 
void BOARD_InitOctaSPI1_B_Pins (void)
{
	BOARD_InitQuadSPI1_B_Pins ();

//	#define IOMUXC_GPIO_SD_B2_04_FLEXSPI1_BUS2BIT_B_SS1_B     0x42A101C4U, 0x1U,        0x0U, 0x0U, 0x42A1040CU
//	#define IOMUXC_GPIO_SD_B2_12_DUMMY_FLEXSPI1_BUS2BIT_B_DQS 0x42A101E4U, 0x1U, 0x42A10548U, 0x1U, 0x42A1042CU

	IOMUXC_SetPinMux    (IOMUXC_GPIO_SD_B2_05_FLEXSPI1_BUS2BIT_B_DQS,    1U); /* Software Input On Field: Force input path of pad GPIO_SD_B2_05 */
	IOMUXC_SetPinConfig (IOMUXC_GPIO_SD_B2_05_FLEXSPI1_BUS2BIT_B_DQS,    IOMUXC_SW_PAD_CTL_PAD_PUS_MASK | IOMUXC_SW_PAD_CTL_PAD_DSE_MASK);
	IOMUXC_SetPinMux    (IOMUXC_GPIO_SD_B2_00_FLEXSPI1_BUS2BIT_B_DATA04, 1U);
	IOMUXC_SetPinConfig (IOMUXC_GPIO_SD_B2_00_FLEXSPI1_BUS2BIT_B_DATA04, IOMUXC_SW_PAD_CTL_PAD_PUS_MASK | IOMUXC_SW_PAD_CTL_PAD_DSE_MASK);
	IOMUXC_SetPinMux    (IOMUXC_GPIO_SD_B2_01_FLEXSPI1_BUS2BIT_B_DATA05, 1U);
	IOMUXC_SetPinConfig (IOMUXC_GPIO_SD_B2_01_FLEXSPI1_BUS2BIT_B_DATA05, IOMUXC_SW_PAD_CTL_PAD_PUS_MASK | IOMUXC_SW_PAD_CTL_PAD_DSE_MASK);
	IOMUXC_SetPinMux    (IOMUXC_GPIO_SD_B2_02_FLEXSPI1_BUS2BIT_B_DATA06, 1U);
	IOMUXC_SetPinConfig (IOMUXC_GPIO_SD_B2_02_FLEXSPI1_BUS2BIT_B_DATA06, IOMUXC_SW_PAD_CTL_PAD_PUS_MASK | IOMUXC_SW_PAD_CTL_PAD_DSE_MASK);
	IOMUXC_SetPinMux    (IOMUXC_GPIO_SD_B2_03_FLEXSPI1_BUS2BIT_B_DATA07, 1U);
	IOMUXC_SetPinConfig (IOMUXC_GPIO_SD_B2_03_FLEXSPI1_BUS2BIT_B_DATA07, IOMUXC_SW_PAD_CTL_PAD_PUS_MASK | IOMUXC_SW_PAD_CTL_PAD_DSE_MASK);

//	#define IOMUXC_GPIO_B1_03_FLEXSPI1_BUS2BIT_B_DQS          0x42A101F4U, 0x7U, 0x42A10548U, 0x2U, 0x42A1043CU
//	#define IOMUXC_GPIO_B1_06_FLEXSPI1_BUS2BIT_B_DATA07       0x42A10200U, 0x7U, 0x42A10568U, 0x1U, 0x42A10448U
//	#define IOMUXC_GPIO_B1_07_FLEXSPI1_BUS2BIT_B_DATA06       0x42A10204U, 0x7U, 0x42A10564U, 0x1U, 0x42A1044CU
//	#define IOMUXC_GPIO_B1_08_FLEXSPI1_BUS2BIT_B_DATA05       0x42A10208U, 0x7U, 0x42A10560U, 0x1U, 0x42A10450U
//	#define IOMUXC_GPIO_B1_09_FLEXSPI1_BUS2BIT_B_DATA04       0x42A1020CU, 0x7U, 0x42A1055CU, 0x1U, 0x42A10454U
}

/*! \brief Perform a JEDEC compatible reset sequence FlexSPI1 Port A */ 
void BOARD_PerformJEDECReset_FlexSPI1_B (void)
{
	rgpio_pin_config_t jreset_pin_config = 
	{
		kRGPIO_DigitalOutput, 1
	};

	// Configure the 3 pins used in JEDEC reset as GPIOs
	IOMUXC_SetPinMux (IOMUXC_GPIO_SD_B2_06_GPIO5_IO16, 1);	// IOMUXC_GPIO_SD_B2_06_FLEXSPI1_BUS2BIT_B_SS0_B	-	IOMUXC_GPIO_SD_B2_06_GPIO5_IO16
	IOMUXC_SetPinMux (IOMUXC_GPIO_SD_B2_08_GPIO5_IO18, 1);	// IOMUXC_GPIO_SD_B2_08_FLEXSPI1_BUS2BIT_B_DATA00	-	IOMUXC_GPIO_SD_B2_08_GPIO5_IO18
	IOMUXC_SetPinMux (IOMUXC_GPIO_SD_B2_07_GPIO5_IO17, 1);	// IOMUXC_GPIO_SD_B2_07_FLEXSPI1_BUS2BIT_B_SCLK		-	IOMUXC_GPIO_SD_B2_07_GPIO5_IO17

	// Set the direction of 3 pins used in JEDEC reset to output
	RGPIO_PinInit (RGPIO5, 16, &jreset_pin_config); // CS
	RGPIO_PinInit (RGPIO5, 18, &jreset_pin_config); // SI/IO0
	RGPIO_PinInit (RGPIO5, 17, &jreset_pin_config); // SCK


	// Perform a reset sequence:
	// CS goes low 4 times with alternating values of SOUT
	// SCK is drive low or high and must stay in one state
	RGPIO_WritePinOutput (RGPIO5, 17, 0); // set SCK low
	for (uint32_t i = 0; i < 4; i++)
	{
		// drive CS low
		RGPIO_WritePinOutput (RGPIO5, 16, 0);
		SDK_DelayAtLeastUs (1, SystemCoreClock);
		// drive SI low or high: alternate its state every iteration
		RGPIO_WritePinOutput (RGPIO5, 18, (i&1));
		// drive CS high; SI state will be captured on the CS rising edge
		RGPIO_WritePinOutput (RGPIO5, 16, 1);
		SDK_DelayAtLeastUs (1, SystemCoreClock);
	}

	SDK_DelayAtLeastUs (110, SystemCoreClock);
}


void BOARD_InitQuadSPI1Pins (void)
{
	BOARD_InitQuadSPI1_A_Pins ();

	// ToDo Remove this, this is testcode for the 1180 evaluation board
//	rgpio_pin_config_t pinConfig = 
//	{
//		kRGPIO_DigitalOutput, 1
//	};
//	IOMUXC_SetPinMux     (IOMUXC_GPIO_AD_04_GPIO4_IO04, 1U);
//	RGPIO_PinInit        (RGPIO4, 4, &pinConfig);
//	RGPIO_WritePinOutput (RGPIO4, 4, 0);
}

void BOARD_InitOctaSPI1Pins (void)
{
	BOARD_InitOctaSPI1_A_Pins ();
}

void BOARD_PerformJEDECReset_FlexSPI1 (void)
{
	BOARD_PerformJEDECReset_FlexSPI1_B ();
}


////////////////////////// FlexSPI 2 //////////////////////////////////

/*! \brief Set pin muxing and configure electrical properties for QSPI */
void BOARD_InitQuadSPI2_A_Pins (void)
{
	CLOCK_EnableClock (kCLOCK_Iomuxc1);          /* Turn on LPCG: LPCG is ON. */
	CLOCK_EnableClock (kCLOCK_Iomuxc2);          /* Turn on LPCG: LPCG is ON. */

	IOMUXC_SetPinMux    (IOMUXC_GPIO_EMC_B1_41_FLEXSPI2_BUS2BIT_A_SCLK,   1U); /* Software Input On Field: Force input path of pad GPIO_EMC_B1_41 */
	IOMUXC_SetPinConfig (IOMUXC_GPIO_EMC_B1_41_FLEXSPI2_BUS2BIT_A_SCLK,   IOMUXC_SW_PAD_CTL_PAD_PUS_MASK | IOMUXC_SW_PAD_CTL_PAD_DSE_MASK);
	IOMUXC_SetPinMux    (IOMUXC_GPIO_EMC_B1_39_FLEXSPI2_BUS2BIT_A_SS0_B,  1U); /* Software Input On Field: Force input path of pad GPIO_EMC_B1_39 */
	IOMUXC_SetPinConfig (IOMUXC_GPIO_EMC_B1_39_FLEXSPI2_BUS2BIT_A_SS0_B,  IOMUXC_SW_PAD_CTL_PAD_PUS_MASK | IOMUXC_SW_PAD_CTL_PAD_DSE_MASK);
	IOMUXC_SetPinMux    (IOMUXC_GPIO_EMC_B1_35_FLEXSPI2_BUS2BIT_A_DATA00, 1U); /* Software Input On Field: Force input path of pad GPIO_EMC_B1_35 */
	IOMUXC_SetPinConfig (IOMUXC_GPIO_EMC_B1_35_FLEXSPI2_BUS2BIT_A_DATA00, IOMUXC_SW_PAD_CTL_PAD_PUS_MASK | IOMUXC_SW_PAD_CTL_PAD_DSE_MASK);
	IOMUXC_SetPinMux    (IOMUXC_GPIO_EMC_B1_36_FLEXSPI2_BUS2BIT_A_DATA01, 1U); /* Software Input On Field: Force input path of pad GPIO_EMC_B1_36 */
	IOMUXC_SetPinConfig (IOMUXC_GPIO_EMC_B1_36_FLEXSPI2_BUS2BIT_A_DATA01, IOMUXC_SW_PAD_CTL_PAD_PUS_MASK | IOMUXC_SW_PAD_CTL_PAD_DSE_MASK);
	IOMUXC_SetPinMux    (IOMUXC_GPIO_EMC_B1_37_FLEXSPI2_BUS2BIT_A_DATA02, 1U); /* Software Input On Field: Force input path of pad GPIO_EMC_B1_37 */
	IOMUXC_SetPinConfig (IOMUXC_GPIO_EMC_B1_37_FLEXSPI2_BUS2BIT_A_DATA02, IOMUXC_SW_PAD_CTL_PAD_PUS_MASK | IOMUXC_SW_PAD_CTL_PAD_DSE_MASK);
	IOMUXC_SetPinMux    (IOMUXC_GPIO_EMC_B1_38_FLEXSPI2_BUS2BIT_A_DATA03, 1U); /* Software Input On Field: Force input path of pad GPIO_EMC_B1_38 */
	IOMUXC_SetPinConfig (IOMUXC_GPIO_EMC_B1_38_FLEXSPI2_BUS2BIT_A_DATA03, IOMUXC_SW_PAD_CTL_PAD_PUS_MASK | IOMUXC_SW_PAD_CTL_PAD_DSE_MASK);
}

/*! \brief Set pin muxing and configure electrical properties for QSPI */
void BOARD_InitQuadSPI2_B_Pins (void)
{
	CLOCK_EnableClock (kCLOCK_Iomuxc1);          /* Turn on LPCG: LPCG is ON. */
	CLOCK_EnableClock (kCLOCK_Iomuxc2);          /* Turn on LPCG: LPCG is ON. */

	IOMUXC_SetPinMux    (IOMUXC_GPIO_EMC_B1_34_FLEXSPI2_BUS2BIT_B_SCLK,   1U); /* Software Input On Field: Force input path of pad GPIO_EMC_B1_34 */
	IOMUXC_SetPinConfig (IOMUXC_GPIO_EMC_B1_34_FLEXSPI2_BUS2BIT_B_SCLK,   IOMUXC_SW_PAD_CTL_PAD_PUS_MASK | IOMUXC_SW_PAD_CTL_PAD_DSE_MASK);
	IOMUXC_SetPinMux    (IOMUXC_GPIO_EMC_B1_28_FLEXSPI2_BUS2BIT_B_SS0_B,  1U); /* Software Input On Field: Force input path of pad GPIO_EMC_B1_28 */
	IOMUXC_SetPinConfig (IOMUXC_GPIO_EMC_B1_28_FLEXSPI2_BUS2BIT_B_SS0_B,  IOMUXC_SW_PAD_CTL_PAD_PUS_MASK | IOMUXC_SW_PAD_CTL_PAD_DSE_MASK);
	IOMUXC_SetPinMux    (IOMUXC_GPIO_EMC_B1_33_FLEXSPI2_BUS2BIT_B_DATA00, 1U); /* Software Input On Field: Force input path of pad GPIO_EMC_B1_33 */
	IOMUXC_SetPinConfig (IOMUXC_GPIO_EMC_B1_33_FLEXSPI2_BUS2BIT_B_DATA00, IOMUXC_SW_PAD_CTL_PAD_PUS_MASK | IOMUXC_SW_PAD_CTL_PAD_DSE_MASK);
	IOMUXC_SetPinMux    (IOMUXC_GPIO_EMC_B1_32_FLEXSPI2_BUS2BIT_B_DATA01, 1U); /* Software Input On Field: Force input path of pad GPIO_EMC_B1_32 */
	IOMUXC_SetPinConfig (IOMUXC_GPIO_EMC_B1_32_FLEXSPI2_BUS2BIT_B_DATA01, IOMUXC_SW_PAD_CTL_PAD_PUS_MASK | IOMUXC_SW_PAD_CTL_PAD_DSE_MASK);
	IOMUXC_SetPinMux    (IOMUXC_GPIO_EMC_B1_31_FLEXSPI2_BUS2BIT_B_DATA02, 1U); /* Software Input On Field: Force input path of pad GPIO_EMC_B1_31 */
	IOMUXC_SetPinConfig (IOMUXC_GPIO_EMC_B1_31_FLEXSPI2_BUS2BIT_B_DATA02, IOMUXC_SW_PAD_CTL_PAD_PUS_MASK | IOMUXC_SW_PAD_CTL_PAD_DSE_MASK);
	IOMUXC_SetPinMux    (IOMUXC_GPIO_EMC_B1_30_FLEXSPI2_BUS2BIT_B_DATA03, 1U); /* Software Input On Field: Force input path of pad GPIO_EMC_B1_30 */
	IOMUXC_SetPinConfig (IOMUXC_GPIO_EMC_B1_30_FLEXSPI2_BUS2BIT_B_DATA03, IOMUXC_SW_PAD_CTL_PAD_PUS_MASK | IOMUXC_SW_PAD_CTL_PAD_DSE_MASK);
}

void BOARD_InitQuadSPI2Pins (void)
{
	BOARD_InitQuadSPI2_B_Pins ();
}

/*! \brief Set pin muxing and configure electrical properties for Hyperflash/OctaSPI */ 
void BOARD_InitOctaSPI2Pins (void)
{
	BOARD_InitQuadSPI2_A_Pins ();

	IOMUXC_SetPinMux    (IOMUXC_GPIO_EMC_B1_40_FLEXSPI2_BUS2BIT_A_DQS,    1U); /* Software Input On Field: Force input path of pad GPIO_EMC_B1_40 */
	IOMUXC_SetPinConfig (IOMUXC_GPIO_EMC_B1_40_FLEXSPI2_BUS2BIT_A_DQS,    IOMUXC_SW_PAD_CTL_PAD_PUS_MASK | IOMUXC_SW_PAD_CTL_PAD_DSE_MASK);
	IOMUXC_SetPinMux    (IOMUXC_GPIO_EMC_B1_33_FLEXSPI2_BUS2BIT_B_DATA00, 1U); /* Software Input On Field: Force input path of pad GPIO_EMC_B1_33 */
	IOMUXC_SetPinConfig (IOMUXC_GPIO_EMC_B1_33_FLEXSPI2_BUS2BIT_B_DATA00, IOMUXC_SW_PAD_CTL_PAD_PUS_MASK | IOMUXC_SW_PAD_CTL_PAD_DSE_MASK);
	IOMUXC_SetPinMux    (IOMUXC_GPIO_EMC_B1_32_FLEXSPI2_BUS2BIT_B_DATA01, 1U); /* Software Input On Field: Force input path of pad GPIO_EMC_B1_32 */
	IOMUXC_SetPinConfig (IOMUXC_GPIO_EMC_B1_32_FLEXSPI2_BUS2BIT_B_DATA01, IOMUXC_SW_PAD_CTL_PAD_PUS_MASK | IOMUXC_SW_PAD_CTL_PAD_DSE_MASK);
	IOMUXC_SetPinMux    (IOMUXC_GPIO_EMC_B1_31_FLEXSPI2_BUS2BIT_B_DATA02, 1U); /* Software Input On Field: Force input path of pad GPIO_EMC_B1_31 */
	IOMUXC_SetPinConfig (IOMUXC_GPIO_EMC_B1_31_FLEXSPI2_BUS2BIT_B_DATA02, IOMUXC_SW_PAD_CTL_PAD_PUS_MASK | IOMUXC_SW_PAD_CTL_PAD_DSE_MASK);
	IOMUXC_SetPinMux    (IOMUXC_GPIO_EMC_B1_30_FLEXSPI2_BUS2BIT_B_DATA03, 1U); /* Software Input On Field: Force input path of pad GPIO_EMC_B1_30 */
	IOMUXC_SetPinConfig (IOMUXC_GPIO_EMC_B1_30_FLEXSPI2_BUS2BIT_B_DATA03, IOMUXC_SW_PAD_CTL_PAD_PUS_MASK | IOMUXC_SW_PAD_CTL_PAD_DSE_MASK);
}

void BOARD_PerformJEDECReset_FlexSPI2_A (void)
{
	rgpio_pin_config_t jreset_pin_config = 
	{
		kRGPIO_DigitalOutput, 1
	};

	// Configure the 3 pins used in JEDEC reset as GPIOs
	IOMUXC_SetPinMux (IOMUXC_GPIO_B2_09_GPIO6_IO23, 1);	// IOMUXC_GPIO_EMC_B1_39_FLEXSPI2_BUS2BIT_A_SS0_B	-	IOMUXC_GPIO_EMC_B1_39_GPIO3_IO07
	IOMUXC_SetPinMux (IOMUXC_GPIO_B2_10_GPIO6_IO24, 1);	// IOMUXC_GPIO_EMC_B1_35_FLEXSPI2_BUS2BIT_A_DATA00	-	IOMUXC_GPIO_EMC_B1_35_GPIO3_IO03
	IOMUXC_SetPinMux (IOMUXC_GPIO_B2_08_GPIO6_IO22, 1);	// IOMUXC_GPIO_EMC_B1_41_FLEXSPI2_BUS2BIT_A_SCLK	-	IOMUXC_GPIO_EMC_B1_41_GPIO3_IO09

	// Set the direction of 3 pins used in JEDEC reset to output
	RGPIO_PinInit (RGPIO3, 7, &jreset_pin_config); // CS
	RGPIO_PinInit (RGPIO3, 3, &jreset_pin_config); // SI/IO0
	RGPIO_PinInit (RGPIO3, 9, &jreset_pin_config); // SCK


	// Perform a reset sequence:
	// CS goes low 4 times with alternating values of SOUT
	// SCK is drive low or high and must stay in one state
	RGPIO_WritePinOutput (RGPIO3, 9, 0); // set SCK low
	for (uint32_t i = 0; i < 4; i++)
	{
		// drive CS low
		RGPIO_WritePinOutput (RGPIO3, 7, 0);
		SDK_DelayAtLeastUs (1, SystemCoreClock);
		// drive SI low or high: alternate its state every iteration
		RGPIO_WritePinOutput (RGPIO3, 3, (i&1));
		// drive CS high; SI state will be captured on the CS rising edge
		RGPIO_WritePinOutput (RGPIO3, 7, 1);
		SDK_DelayAtLeastUs (1, SystemCoreClock);
	}

	SDK_DelayAtLeastUs (110, SystemCoreClock);
}

void BOARD_PerformJEDECReset_FlexSPI2_B (void)
{
	rgpio_pin_config_t jreset_pin_config = 
	{
		kRGPIO_DigitalOutput, 1
	};

	// Configure the 3 pins used in JEDEC reset as GPIOs
	IOMUXC_SetPinMux (IOMUXC_GPIO_B2_09_GPIO6_IO23, 1);	// IOMUXC_GPIO_EMC_B1_28_FLEXSPI2_BUS2BIT_B_SS0_B	-	IOMUXC_GPIO_EMC_B1_28_GPIO2_IO28
	IOMUXC_SetPinMux (IOMUXC_GPIO_B2_10_GPIO6_IO24, 1);	// IOMUXC_GPIO_EMC_B1_33_FLEXSPI2_BUS2BIT_B_DATA00	-	IOMUXC_GPIO_EMC_B1_33_GPIO3_IO01
	IOMUXC_SetPinMux (IOMUXC_GPIO_B2_08_GPIO6_IO22, 1);	// IOMUXC_GPIO_EMC_B1_34_FLEXSPI2_BUS2BIT_B_SCLK	-	IOMUXC_GPIO_EMC_B1_34_GPIO3_IO02

	// Set the direction of 1 pins used in JEDEC reset to output
	RGPIO_PinInit (RGPIO2, 28, &jreset_pin_config); // CS
	RGPIO_PinInit (RGPIO3,  1, &jreset_pin_config); // SI/IO0
	RGPIO_PinInit (RGPIO3,  2, &jreset_pin_config); // SCK


	// Perform a reset sequence:
	// CS goes low 4 times with alternating values of SOUT
	// SCK is drive low or high and must stay in one state
	RGPIO_WritePinOutput (RGPIO3, 2, 0); // set SCK low
	for (uint32_t i = 0; i < 4; i++)
	{
		// drive CS low
		RGPIO_WritePinOutput (RGPIO3, 28, 0);
		SDK_DelayAtLeastUs (1, SystemCoreClock);
		// drive SI low or high: alternate its state every iteration
		RGPIO_WritePinOutput (RGPIO3, 1, (i&1));
		// drive CS high; SI state will be captured on the CS rising edge
		RGPIO_WritePinOutput (RGPIO3, 28, 1);
		SDK_DelayAtLeastUs (1, SystemCoreClock);
	}

	SDK_DelayAtLeastUs (110, SystemCoreClock);
}

void BOARD_PerformJEDECReset_FlexSPI2 (void)
{
	BOARD_PerformJEDECReset_FlexSPI2_B ();
}
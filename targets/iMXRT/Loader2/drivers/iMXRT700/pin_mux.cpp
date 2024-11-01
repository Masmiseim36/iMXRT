/** Loader for iMXRT-Family
Copyright (C) 2019-2025 Markus Klein
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
#include "fsl_iopctl.h"
#include "fsl_gpio.h"
#include "pin_mux.h"


#define IOPCTL_PIO_ANAMUX_DI 0x00u        /*!< \brief Analog mux is disabled */
#define IOPCTL_PIO_DRIVE_100OHM 0x00u     /*!< \brief Selects transmitter current drive 100ohm */
#define IOPCTL_PIO_FULLDRIVE_DI 0x00u     /*!< \brief Normal drive */
#define IOPCTL_PIO_FUNC1 0x01u            /*!< \brief Selects pin function 1 */
#define IOPCTL_PIO_FULLDRIVE_EN 0x0100u   /*!< \brief Full drive enable */
#define IOPCTL_PIO_FUNC0 0x00u            /*!< \brief Selects pin function 0 */
#define IOPCTL_PIO_FUNC5 0x05u            /*!< \brief Selects pin function 5 */
#define IOPCTL_PIO_FUNC6 0x06u            /*!< \brief Selects pin function 6 */
#define IOPCTL_PIO_INBUF_DI 0x00u         /*!< \brief Disable input buffer function */
#define IOPCTL_PIO_INBUF_EN 0x40u         /*!< \brief Enables input buffer function */
#define IOPCTL_PIO_INV_DI 0x00u           /*!< \brief Input function is not inverted */
#define IOPCTL_PIO_PSEDRAIN_DI 0x00u      /*!< \brief Pseudo Output Drain is disabled */
#define IOPCTL_PIO_PULLDOWN_EN 0x00u      /*!< \brief Enable pull-down function */
#define IOPCTL_PIO_PUPD_DI 0x00u          /*!< \brief Disable pull-up / pull-down function */
#define IOPCTL_PIO_SLEW_RATE_NORMAL 0x00u /*!< \brief Normal mode */

void BOARD_InitFlexSPI0BPins (void); /* Function assigned for the Cortex-M33 */


static constexpr uint32_t UartTx_PinConfig
{
	IOPCTL_PIO_FUNC1       | // Pin is configured as LP_FLEXCOMM0_P1
	IOPCTL_PIO_PUPD_DI     | // Disable pull-up / pull-down function
	IOPCTL_PIO_PULLDOWN_EN | // Enable pull-down function
	IOPCTL_PIO_INBUF_DI    | // Disable input buffer function
	IOPCTL_PIO_PSEDRAIN_DI | // Pseudo Output Drain is disabled
	IOPCTL_PIO_INV_DI      | // Input function is not inverted
	IOPCTL_PIO_DRIVE_100OHM  // Selects transmitter current drive 100ohm
};
static constexpr uint32_t UartRx_PinConfig
{
	IOPCTL_PIO_FUNC1       | // Pin is configured as LP_FLEXCOMM0_P0
	IOPCTL_PIO_PUPD_DI     | // Disable pull-up / pull-down function
	IOPCTL_PIO_PULLDOWN_EN | // Enable pull-down function
	IOPCTL_PIO_INBUF_EN    | // Enables input buffer function
	IOPCTL_PIO_PSEDRAIN_DI | // Pseudo Output Drain is disabled
	IOPCTL_PIO_INV_DI      | // Input function is not inverted
	IOPCTL_PIO_DRIVE_100OHM  // Selects transmitter current drive 100ohm
};

void BOARD_InitUARTPins (void)
{
	RESET_ClearPeripheralReset (kIOPCTL0_RST_SHIFT_RSTn);
	IOPCTL_PinMuxSet (1U, 0U, UartTx_PinConfig);
	IOPCTL_PinMuxSet (0U, 31U, UartRx_PinConfig);
}


static constexpr uint32_t FlexSPI_PinConfig
{
	IOPCTL_PIO_FUNC1       | // Pin is configured as XSPI0_SCK_A_N
	IOPCTL_PIO_PUPD_DI     | // Disable pull-up / pull-down function
	IOPCTL_PIO_PULLDOWN_EN | // Enable pull-down function
	IOPCTL_PIO_INBUF_EN    | // Enables input buffer function
	IOPCTL_PIO_PSEDRAIN_DI | // Pseudo Output Drain is disabled
	IOPCTL_PIO_INV_DI      | // Input function is not inverted
	IOPCTL_PIO_DRIVE_100OHM  // Selects transmitter current drive 100ohm
};

void BOARD_InitQuadSPI0Pins (void)
{
	RESET_ClearPeripheralReset (kIOPCTL0_RST_SHIFT_RSTn);

	IOPCTL_PinMuxSet (6U,  0U, FlexSPI_PinConfig); // PORT6 PIN0 (coords: D16) is configured as XSPI0_SCK_A_N
	IOPCTL_PinMuxSet (6U,  1U, FlexSPI_PinConfig); // PORT6 PIN1 (coords: D17) is configured as XSPI0_SCK_A
	IOPCTL_PinMuxSet (6U, 12U, FlexSPI_PinConfig); // PORT6 PIN12 (coords: G10) is configured as XSPI0_PCS_A_1
	IOPCTL_PinMuxSet (6U,  2U, FlexSPI_PinConfig); // PORT6 PIN2 (coords: G11) is configured as XSPI0_PCS_A_0
	IOPCTL_PinMuxSet (6U,  3U, FlexSPI_PinConfig); // PORT6 PIN3 (coords: F13) is configured as XSPI0_DATA_A0
	IOPCTL_PinMuxSet (6U,  4U, FlexSPI_PinConfig); // PORT6 PIN4 (coords: D14) is configured as XSPI0_DATA_A1
	IOPCTL_PinMuxSet (6U,  5U, FlexSPI_PinConfig); // PORT6 PIN5 (coords: C15) is configured as XSPI0_DATA_A2
	IOPCTL_PinMuxSet (6U,  6U, FlexSPI_PinConfig); // PORT6 PIN6 (coords: E13) is configured as XSPI0_DATA_A3
	IOPCTL_PinMuxSet (6U,  7U, FlexSPI_PinConfig); // PORT6 PIN7 (coords: D15) is configured as XSPI0_DQS_A_0
}

void BOARD_InitOctaSPI0Pins (void)
{
	IOPCTL_PinMuxSet (6U,  8U, FlexSPI_PinConfig); // PORT6 PIN8 (coords: G13) is configured as XSPI0_DATA_A4
	IOPCTL_PinMuxSet (6U,  9U, FlexSPI_PinConfig); // PORT6 PIN9 (coords: F15) is configured as XSPI0_DATA_A5
	IOPCTL_PinMuxSet (6U, 10U, FlexSPI_PinConfig); // PORT6 PIN10 (coords: E14) is configured as XSPI0_DATA_A6
	IOPCTL_PinMuxSet (6U, 11U, FlexSPI_PinConfig); // PORT6 PIN11 (coords: F14) is configured as XSPI0_DATA_A7

	BOARD_InitQuadSPI0Pins ();
}


static constexpr uint32_t  Gpio_PinConfig
{
	IOPCTL_PIO_FUNC0       | /* Pin is configured as PIO0_17 */
	IOPCTL_PIO_PUPD_DI     | /* Disable pull-up / pull-down function */
	IOPCTL_PIO_PULLDOWN_EN | /* Enable pull-down function */
	IOPCTL_PIO_INBUF_DI    | /* Disable input buffer function */
	IOPCTL_PIO_PSEDRAIN_DI | /* Pseudo Output Drain is disabled */
	IOPCTL_PIO_INV_DI      | /* Input function is not inverted */
	IOPCTL_PIO_DRIVE_100OHM  /* Selects transmitter current drive 100ohm */
};


void BOARD_PerformJEDECReset_xSPI0 (void)
{
	gpio_pin_config_t jreset_pin_config {kGPIO_DigitalOutput, 1};

	IOPCTL_PinMuxSet (6U, 12U, Gpio_PinConfig);	// CS
	IOPCTL_PinMuxSet (6U,  3U, Gpio_PinConfig);	// Data0
	IOPCTL_PinMuxSet (6U,  1U, Gpio_PinConfig);	// CLK

	// Set the direction of 3 pins used in JEDEC reset to output
	GPIO_PinInit (GPIO6, 12U, &jreset_pin_config); // CS
	GPIO_PinInit (GPIO6,  3U, &jreset_pin_config); // SI/IO0
	GPIO_PinInit (GPIO6,  1U, &jreset_pin_config); // CLK

	// Perform a reset sequence:
	// CS goes low 4 times with alternating values of SOUT
	// CLK is drive low or high and must stay in one state
	GPIO_PinWrite (GPIO6, 1U, 0); // set CLK low
	for (uint32_t i = 0; i < 4; i++)
	{
		// drive CS low
		GPIO_PinWrite (GPIO6, 12U, 0);
		SDK_DelayAtLeastUs (1, SystemCoreClock);
		// drive SI low or high: alternate its state every iteration
		GPIO_PinWrite (GPIO6, 3U, (i&1));
		// drive CS high; SI state will be captured on the CS rising edge
		GPIO_PinWrite (GPIO6, 12U, 1);
		SDK_DelayAtLeastUs (1, SystemCoreClock);
	}
	SDK_DelayAtLeastUs (110, SystemCoreClock); 
}




void BOARD_InitQuadSPI1Pins (void)
{
}

void BOARD_InitOctaSPI1Pins (void)
{
}

void BOARD_PerformJEDECReset_xSPI1 (void)
{
}

void BOARD_InitQuadSPI2Pins (void)
{
}

void BOARD_InitOctaSPI2Pins (void)
{
}

void BOARD_PerformJEDECReset_xSPI2 (void)
{
}
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
#include "fsl_iopctl.h"
#include "fsl_gpio.h"
#include "pin_mux.h"


#define IOPCTL_PIO_ANAMUX_DI 0x00u        /*!< \brief Analog mux is disabled */
#define IOPCTL_PIO_FULLDRIVE_DI 0x00u     /*!< \brief Normal drive */
#define IOPCTL_PIO_FULLDRIVE_EN 0x0100u   /*!< \brief Full drive enable */
#define IOPCTL_PIO_FUNC0 0x00u            /*!< \brief Selects pin function 0 */
#define IOPCTL_PIO_FUNC1 0x01u            /*!< \brief Selects pin function 1 */
#define IOPCTL_PIO_INBUF_DI 0x00u         /*!< \brief Disable input buffer function */
#define IOPCTL_PIO_INBUF_EN 0x40u         /*!< \brief Enables input buffer function */
#define IOPCTL_PIO_INV_DI 0x00u           /*!< \brief Input function is not inverted */
#define IOPCTL_PIO_PSEDRAIN_DI 0x00u      /*!< \brief Pseudo Output Drain is disabled */
#define IOPCTL_PIO_INV_DI 0x00u           /*!< \brief Input function is not inverted */
#define IOPCTL_PIO_PULLDOWN_EN 0x00u      /*!< \brief Enable pull-down function */
#define IOPCTL_PIO_PUPD_DI 0x00u          /*!< \brief Disable pull-up / pull-down function */
#define IOPCTL_PIO_PUPD_EN 0x10u          /*!< \brief Enable pull-up / pull-down function */
#define IOPCTL_PIO_SLEW_RATE_NORMAL 0x00u /*!< \brief Normal mode */


void BOARD_InitUARTPins (void)
{

    const uint32_t port0_pin1_config = (IOPCTL_PIO_FUNC1 |            /* Pin is configured as FC0_TXD_SCL_MISO_WS */
                                        IOPCTL_PIO_PUPD_DI |          /* Disable pull-up / pull-down function */
                                        IOPCTL_PIO_PULLDOWN_EN |      /* Enable pull-down function */
                                        IOPCTL_PIO_INBUF_DI |         /* Disable input buffer function */
                                        IOPCTL_PIO_SLEW_RATE_NORMAL | /* Normal mode */
                                        IOPCTL_PIO_FULLDRIVE_DI |     /* Normal drive */
                                        IOPCTL_PIO_ANAMUX_DI |        /* Analog mux is disabled */
                                        IOPCTL_PIO_PSEDRAIN_DI |      /* Pseudo Output Drain is disabled */
                                        IOPCTL_PIO_INV_DI);           /* Input function is not inverted */
    /* PORT0 PIN1 (coords: G16) is configured as FC0_TXD_SCL_MISO_WS */
    IOPCTL_PinMuxSet(IOPCTL, 0U, 1U, port0_pin1_config);

    const uint32_t port0_pin2_config = (IOPCTL_PIO_FUNC1 |            /* Pin is configured as FC0_RXD_SDA_MOSI_DATA */
                                        IOPCTL_PIO_PUPD_DI |          /* Disable pull-up / pull-down function */
                                        IOPCTL_PIO_PULLDOWN_EN |      /* Enable pull-down function */
                                        IOPCTL_PIO_INBUF_EN |         /* Enables input buffer function */
                                        IOPCTL_PIO_SLEW_RATE_NORMAL | /* Normal mode */
                                        IOPCTL_PIO_FULLDRIVE_DI |     /* Normal drive */
                                        IOPCTL_PIO_ANAMUX_DI |        /* Analog mux is disabled */
                                        IOPCTL_PIO_PSEDRAIN_DI |      /* Pseudo Output Drain is disabled */
                                        IOPCTL_PIO_INV_DI);           /* Input function is not inverted */
    /* PORT0 PIN2 (coords: H16) is configured as FC0_RXD_SDA_MOSI_DATA */
    IOPCTL_PinMuxSet(IOPCTL, 0U, 2U, port0_pin2_config);
}

static constexpr uint32_t FlexSPI_PinConfig
{
	IOPCTL_PIO_FUNC1 |            // Pin is configured for FLEXSPI
	IOPCTL_PIO_PUPD_DI |          // Disable pull-up / pull-down function
	IOPCTL_PIO_PULLDOWN_EN |      // Enable pull-down function
	IOPCTL_PIO_INBUF_EN |         // Enables input buffer function
	IOPCTL_PIO_SLEW_RATE_NORMAL | // Normal mode
	IOPCTL_PIO_FULLDRIVE_EN |     // Full drive enable
	IOPCTL_PIO_ANAMUX_DI |        // Analog mux is disabled
	IOPCTL_PIO_PSEDRAIN_DI |      // Pseudo Output Drain is disabled
	IOPCTL_PIO_INV_DI             // Input function is not inverted
};
static constexpr uint32_t FlexSPI_DqsConfig
{
	IOPCTL_PIO_FUNC1 |            // Pin is configured for FLEXSPI
	IOPCTL_PIO_PUPD_EN |          // Enable pull-up / pull-down function
	IOPCTL_PIO_PULLDOWN_EN |      // Enable pull-down function
	IOPCTL_PIO_INBUF_EN |         // Enables input buffer function
	IOPCTL_PIO_SLEW_RATE_NORMAL | // Normal mode
	IOPCTL_PIO_FULLDRIVE_EN |     // Full drive enable
	IOPCTL_PIO_ANAMUX_DI |        // Analog mux is disabled
	IOPCTL_PIO_PSEDRAIN_DI |      // Pseudo Output Drain is disabled
	IOPCTL_PIO_INV_DI             // Input function is not inverted
};


void BOARD_InitQuadSPI0Pins (void)
{
    IOPCTL_PinMuxSet (IOPCTL, 1U, 20U, FlexSPI_PinConfig);  // PORT1 PIN20 (coords: T15) is configured as FLEXSPI0_DATA0
    IOPCTL_PinMuxSet (IOPCTL, 1U, 21U, FlexSPI_PinConfig);  // PORT1 PIN21 (coords: T14) is configured as FLEXSPI0_DATA1
    IOPCTL_PinMuxSet (IOPCTL, 1U, 22U, FlexSPI_PinConfig);  // PORT1 PIN22 (coords: R13) is configured as FLEXSPI0_DATA2
    IOPCTL_PinMuxSet (IOPCTL, 1U, 23U, FlexSPI_PinConfig);  // PORT1 PIN23 (coords: R12) is configured as FLEXSPI0_DATA3
    IOPCTL_PinMuxSet (IOPCTL, 1U, 18U, FlexSPI_PinConfig);  // PORT1 PIN18 (coords: T17) is configured as FLEXSPI0_SCLK
    IOPCTL_PinMuxSet (IOPCTL, 1U, 19U, FlexSPI_PinConfig);  // PORT1 PIN19 (coords: U16) is configured as FLEXSPI0_SS0_N
}

void BOARD_InitOctaSPI0Pins (void)
{
    IOPCTL_PinMuxSet (IOPCTL, 1U, 24U, FlexSPI_PinConfig);  // PORT1 PIN24 (coords: N12) is configured as FLEXSPI0_DATA4
    IOPCTL_PinMuxSet (IOPCTL, 1U, 25U, FlexSPI_PinConfig);  // PORT1 PIN25 (coords: R14) is configured as FLEXSPI0_DATA5
    IOPCTL_PinMuxSet (IOPCTL, 1U, 26U, FlexSPI_PinConfig);  // PORT1 PIN26 (coords: P14) is configured as FLEXSPI0_DATA6
    IOPCTL_PinMuxSet (IOPCTL, 1U, 27U, FlexSPI_PinConfig);  // PORT1 PIN27 (coords: P13) is configured as FLEXSPI0_DATA7
    IOPCTL_PinMuxSet (IOPCTL, 1U, 28U, FlexSPI_DqsConfig);  // PORT1 PIN28 (coords: U14) is configured as FLEXSPI0_DQS

	BOARD_InitQuadSPI0Pins ();
}


void BOARD_PerformJEDECReset_FlexSPI0 (void)
{
	constexpr uint32_t GpioConfig
	{
		IOPCTL_PIO_FUNC0 |				// Pin is configured as PIO2_12
		IOPCTL_PIO_PUPD_DI |			// Disable pull-up / pull-down function
		IOPCTL_PIO_PULLDOWN_EN |		// Enable pull-down function
		IOPCTL_PIO_INBUF_DI |			// Disable input buffer function
		IOPCTL_PIO_SLEW_RATE_NORMAL |	// Normal mode
		IOPCTL_PIO_FULLDRIVE_DI |		// Normal drive
		IOPCTL_PIO_ANAMUX_DI |			// Analog mux is disabled
		IOPCTL_PIO_PSEDRAIN_DI |		// Pseudo Output Drain is disabled
		IOPCTL_PIO_INV_DI				// Input function is not inverted
	};


	GPIO_PortInit (GPIO, 1);
	GPIO_PortInit (GPIO, 2);

	IOPCTL_PinMuxSet (IOPCTL, 1U, 19U, GpioConfig);	// SS0
	IOPCTL_PinMuxSet (IOPCTL, 1U, 20U, GpioConfig);	// Data0
	IOPCTL_PinMuxSet (IOPCTL, 1U, 18U, GpioConfig);	// CLK

	// Set the direction of 3 pins used in JEDEC reset to output
	gpio_pin_config_t jreset_pin_config = {kGPIO_DigitalOutput, 1};
	GPIO_PinInit (GPIO, 1U, 19U, &jreset_pin_config); // SS0
	GPIO_PinInit (GPIO, 1U, 20U, &jreset_pin_config); // Data0
	GPIO_PinInit (GPIO, 1U, 18U, &jreset_pin_config); // CLK

	// Perform a reset sequence:
	// CS goes low 4 times with alternating values of SOUT
	// SCK is drive low or high and must stay in one state
	GPIO_PinWrite (GPIO, 1U, 18U, 0); // set CLK low
	for (uint32_t i = 0; i < 4; i++)
	{
		// drive CS low
		GPIO_PinWrite (GPIO, 1U, 19U, 0); 	  // SS0
		SDK_DelayAtLeastUs (1, SystemCoreClock);
		// drive SI low or high: alternate its state every iteration
		GPIO_PinWrite (GPIO, 1U, 20U, (i&1)); // DATA0
		// drive CS high; SI state will be captured on the CS rising edge
		GPIO_PinWrite (GPIO, 1U, 19U, 1); 	  // SS0
		SDK_DelayAtLeastUs (1, SystemCoreClock);
	}
	SDK_DelayAtLeastUs (110, SystemCoreClock);
}


void BOARD_InitQuadSPI1Pins (void)
{
    IOPCTL_PinMuxSet (IOPCTL, 4U, 12U, FlexSPI_PinConfig);  // PORT4 PIN12 (coords: H1 ) is configured as FLEXSPI1_DATA0
    IOPCTL_PinMuxSet (IOPCTL, 4U, 13U, FlexSPI_PinConfig);  // PORT4 PIN13 (coords: G2 ) is configured as FLEXSPI1_DATA1
    IOPCTL_PinMuxSet (IOPCTL, 4U, 14U, FlexSPI_PinConfig);  // PORT4 PIN14 (coords: F1 ) is configured as FLEXSPI1_DATA2
    IOPCTL_PinMuxSet (IOPCTL, 4U, 15U, FlexSPI_PinConfig);  // PORT4 PIN15 (coords: K3 ) is configured as FLEXSPI1_DATA3
    IOPCTL_PinMuxSet (IOPCTL, 4U, 11U, FlexSPI_PinConfig);  // PORT4 PIN11 (coords: H2 ) is configured as FLEXSPI1_SCLK
    IOPCTL_PinMuxSet (IOPCTL, 4U, 18U, FlexSPI_PinConfig);  // PORT4 PIN18 (coords: E13) is configured as FLEXSPI1_SS0_N
}

void BOARD_InitOctaSPI1Pins (void)
{
    IOPCTL_PinMuxSet (IOPCTL, 5U, 15U, FlexSPI_PinConfig);  // PORT5 PIN15 (coords: H5 ) is configured as FLEXSPI1_DATA4
    IOPCTL_PinMuxSet (IOPCTL, 5U, 16U, FlexSPI_PinConfig);  // PORT5 PIN16 (coords: H4 ) is configured as FLEXSPI1_DATA5
    IOPCTL_PinMuxSet (IOPCTL, 5U, 17U, FlexSPI_PinConfig);  // PORT5 PIN17 (coords: J3 ) is configured as FLEXSPI1_DATA6
    IOPCTL_PinMuxSet (IOPCTL, 5U, 18U, FlexSPI_PinConfig);  // PORT5 PIN18 (coords: J4 ) is configured as FLEXSPI1_DATA7
    IOPCTL_PinMuxSet (IOPCTL, 4U, 16U, FlexSPI_DqsConfig);  // PORT4 PIN16 (coords: H3 ) is configured as FLEXSPI1_DQS

	BOARD_InitQuadSPI1Pins ();
}


void BOARD_PerformJEDECReset_FlexSPI1 (void)
{
	constexpr uint32_t GpioConfig
	{
		IOPCTL_PIO_FUNC0 |				// Pin is configured as PIO2_12
		IOPCTL_PIO_PUPD_DI |			// Disable pull-up / pull-down function
		IOPCTL_PIO_PULLDOWN_EN |		// Enable pull-down function
		IOPCTL_PIO_INBUF_DI |			// Disable input buffer function
		IOPCTL_PIO_SLEW_RATE_NORMAL |	// Normal mode
		IOPCTL_PIO_FULLDRIVE_DI |		// Normal drive
		IOPCTL_PIO_ANAMUX_DI |			// Analog mux is disabled
		IOPCTL_PIO_PSEDRAIN_DI |		// Pseudo Output Drain is disabled
		IOPCTL_PIO_INV_DI				// Input function is not inverted
	};


	GPIO_PortInit (GPIO, 1);
	GPIO_PortInit (GPIO, 2);

	IOPCTL_PinMuxSet (IOPCTL, 4U, 18U, GpioConfig);	// SS0
	IOPCTL_PinMuxSet (IOPCTL, 4U, 12U, GpioConfig);	// DATA0
	IOPCTL_PinMuxSet (IOPCTL, 4U, 11U, GpioConfig);	// CLK

	// Set the direction of 3 pins used in JEDEC reset to output
	gpio_pin_config_t jreset_pin_config = {kGPIO_DigitalOutput, 1};
	GPIO_PinInit (GPIO, 4U, 18U, &jreset_pin_config); // SS0
	GPIO_PinInit (GPIO, 4U, 12U, &jreset_pin_config); // DATA0
	GPIO_PinInit (GPIO, 4U, 11U, &jreset_pin_config); // CLK

	// Perform a reset sequence:
	// CS goes low 4 times with alternating values of SOUT
	// SCK is drive low or high and must stay in one state
	GPIO_PinWrite (GPIO, 4U, 11U, 0); // set CLK low
	for (uint32_t i = 0; i < 4; i++)
	{
		// drive CS low
		GPIO_PinWrite (GPIO, 4U, 18U, 0); 	   // SS0
		SDK_DelayAtLeastUs (1, SystemCoreClock);
		// drive SI low or high: alternate its state every iteration
		GPIO_PinWrite (GPIO, 4U, 12U, (i&1)); // DATA0
		// drive CS high; SI state will be captured on the CS rising edge
		GPIO_PinWrite (GPIO, 4U, 18U, 1); 	  // SS0
		SDK_DelayAtLeastUs (1, SystemCoreClock);
	}
	SDK_DelayAtLeastUs (110, SystemCoreClock);
}
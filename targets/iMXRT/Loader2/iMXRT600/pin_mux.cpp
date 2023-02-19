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
	IOPCTL_PIO_FUNC1 |			  // Pin is configured as FC0_TXD_SCL_MISO_WS
	IOPCTL_PIO_PUPD_DI |		  // Disable pull-up / pull-down function
	IOPCTL_PIO_PULLDOWN_EN |	  // Enable pull-down function
	IOPCTL_PIO_INBUF_DI |		  // Disable input buffer function
	IOPCTL_PIO_SLEW_RATE_NORMAL | // Normal mode
	IOPCTL_PIO_FULLDRIVE_DI |	  // Normal drive
	IOPCTL_PIO_ANAMUX_DI |		  // Analog mux is disabled
	IOPCTL_PIO_PSEDRAIN_DI |	  // Pseudo Output Drain is disabled
	IOPCTL_PIO_INV_DI			  // Input function is not inverted
};
static constexpr uint32_t UartRx_PinConfig
{
	IOPCTL_PIO_FUNC1 |			  // Pin is configured as FC0_RXD_SDA_MOSI_DATA
	IOPCTL_PIO_PUPD_DI |		  // Disable pull-up / pull-down function
	IOPCTL_PIO_PULLDOWN_EN |	  // Enable pull-down function
	IOPCTL_PIO_INBUF_EN |		  // Enables input buffer function
	IOPCTL_PIO_SLEW_RATE_NORMAL | // Normal mode
	IOPCTL_PIO_FULLDRIVE_DI |	  // Normal drive
	IOPCTL_PIO_ANAMUX_DI |		  // Analog mux is disabled
	IOPCTL_PIO_PSEDRAIN_DI |	  // Pseudo Output Drain is disabled
	IOPCTL_PIO_INV_DI			  // Input function is not inverted
};

void BOARD_InitUARTPins (void)
{
    IOPCTL_PinMuxSet (IOPCTL, 0U, 1U, UartTx_PinConfig);  // PORT0 PIN1 (coords: G2) is configured as FC0_TXD_SCL_MISO_WS
    IOPCTL_PinMuxSet (IOPCTL, 0U, 2U, UartRx_PinConfig);  // PORT0 PIN2 (coords: G4) is configured as FC0_RXD_SDA_MOSI_DATA
}


static constexpr uint32_t FlexSPI_PinConfig
{
	IOPCTL_PIO_FUNC6 |            // Pin is configured for FLEXSPI
	IOPCTL_PIO_PUPD_DI |          // Disable pull-up / pull-down function
	IOPCTL_PIO_PULLDOWN_EN |      // Enable pull-down function
	IOPCTL_PIO_INBUF_EN |         // Enables input buffer function
	IOPCTL_PIO_SLEW_RATE_NORMAL | // Normal mode
	IOPCTL_PIO_FULLDRIVE_EN |     // Full drive enable
	IOPCTL_PIO_ANAMUX_DI |        // Analog mux is disabled
	IOPCTL_PIO_PSEDRAIN_DI |      // Pseudo Output Drain is disabled
	IOPCTL_PIO_INV_DI             // Input function is not inverted
};

static constexpr uint32_t FlexSPI_ClockConfig
{
	IOPCTL_PIO_FUNC5 |            // Pin is configured for FLEXSPI
	IOPCTL_PIO_PUPD_DI |          // Disable pull-up / pull-down function
	IOPCTL_PIO_PULLDOWN_EN |      // Enable pull-down function
	IOPCTL_PIO_INBUF_EN |         // Enables input buffer function
	IOPCTL_PIO_SLEW_RATE_NORMAL | // Normal mode
	IOPCTL_PIO_FULLDRIVE_EN |     // Full drive enable
	IOPCTL_PIO_ANAMUX_DI |        // Analog mux is disabled
	IOPCTL_PIO_PSEDRAIN_DI |      // Pseudo Output Drain is disabled
	IOPCTL_PIO_INV_DI             // Input function is not inverted
};

void BOARD_InitQuadSPIPins (void)
{
    GPIO_PortInit (GPIO, 1);
    GPIO_PortInit (GPIO, 2);
    /* Define the init structure for the reset pin*/
    gpio_pin_config_t reset_config = {
        kGPIO_DigitalOutput,
        1,
    };
    GPIO_PinInit  (GPIO, 2, 12, &reset_config);

    IOPCTL_PinMuxSet(IOPCTL, 1U, 11U, FlexSPI_PinConfig);   // PORT1 PIN11 (coords: L2) is configured as FLEXSPI0B_DATA0
    IOPCTL_PinMuxSet(IOPCTL, 1U, 12U, FlexSPI_PinConfig);   // PORT1 PIN12 (coords: M2) is configured as FLEXSPI0B_DATA1
    IOPCTL_PinMuxSet(IOPCTL, 1U, 13U, FlexSPI_PinConfig);   // PORT1 PIN13 (coords: N1) is configured as FLEXSPI0B_DATA2
    IOPCTL_PinMuxSet(IOPCTL, 1U, 14U, FlexSPI_PinConfig);   // PORT1 PIN14 (coords: N2) is configured as FLEXSPI0B_DATA3
    IOPCTL_PinMuxSet(IOPCTL, 1U, 29U, FlexSPI_ClockConfig); // PORT1 PIN29 (coords: U3) is configured as FLEXSPI0B_SCLK
    IOPCTL_PinMuxSet(IOPCTL, 2U, 19U, FlexSPI_PinConfig);   // PORT2 PIN19 (coords: T2) is configured as FLEXSPI0B_SS0_N

    const uint32_t port2_pin12_config = (IOPCTL_PIO_FUNC0 |			/* Pin is configured as PIO2_12 */
                                         IOPCTL_PIO_PUPD_DI |		/* Disable pull-up / pull-down function */
                                         IOPCTL_PIO_PULLDOWN_EN |	/* Enable pull-down function */
                                         IOPCTL_PIO_INBUF_DI |		/* Disable input buffer function */
                                         IOPCTL_PIO_SLEW_RATE_NORMAL |/* Normal mode */
                                         IOPCTL_PIO_FULLDRIVE_DI |	/* Normal drive */
                                         IOPCTL_PIO_ANAMUX_DI |		/* Analog mux is disabled */
                                         IOPCTL_PIO_PSEDRAIN_DI |	/* Pseudo Output Drain is disabled */
                                         IOPCTL_PIO_INV_DI);		/* Input function is not inverted */
    /* PORT2 PIN12 (coords: T3) is configured as PIO2_12 */	// MK: Reset Signal
    IOPCTL_PinMuxSet(IOPCTL, 2U, 12U, port2_pin12_config);
}

void BOARD_InitOctaSPIPins (void)
{
    IOPCTL_PinMuxSet(IOPCTL, 2U, 17U, FlexSPI_PinConfig);  // PORT2 PIN17 (coords: U1) is configured as FLEXSPI0B_DATA4
    IOPCTL_PinMuxSet(IOPCTL, 2U, 18U, FlexSPI_PinConfig);  // PORT2 PIN18 (coords: R2) is configured as FLEXSPI0B_DATA5
    IOPCTL_PinMuxSet(IOPCTL, 2U, 22U, FlexSPI_PinConfig);  // PORT2 PIN22 (coords: P3) is configured as FLEXSPI0B_DATA6
    IOPCTL_PinMuxSet(IOPCTL, 2U, 23U, FlexSPI_PinConfig);  // PORT2 PIN23 (coords: P5) is configured as FLEXSPI0B_DATA7

    BOARD_InitQuadSPIPins ();
}

void BOARD_PerformJEDECReset_FlexSPI (void)
{
	const uint32_t GpioConfig = IOPCTL_PIO_FUNC0 |				/* Pin is configured as PIO2_12 */
                                IOPCTL_PIO_PUPD_DI |			/* Disable pull-up / pull-down function */
                                IOPCTL_PIO_PULLDOWN_EN |		/* Enable pull-down function */
                                IOPCTL_PIO_INBUF_DI |			/* Disable input buffer function */
                                IOPCTL_PIO_SLEW_RATE_NORMAL |	/* Normal mode */
                                IOPCTL_PIO_FULLDRIVE_DI |		/* Normal drive */
                                IOPCTL_PIO_ANAMUX_DI |			/* Analog mux is disabled */
                                IOPCTL_PIO_PSEDRAIN_DI |		/* Pseudo Output Drain is disabled */
                                IOPCTL_PIO_INV_DI;				/* Input function is not inverted */


	GPIO_PortInit (GPIO, 1);
	GPIO_PortInit (GPIO, 2);

	IOPCTL_PinMuxSet (IOPCTL, 2U, 19U, GpioConfig);	// SS0
	IOPCTL_PinMuxSet (IOPCTL, 1U, 11U, GpioConfig);	// Data0
	IOPCTL_PinMuxSet (IOPCTL, 1U, 29U, GpioConfig);	// CLK

	// Set the direction of 3 pins used in JEDEC reset to output
	gpio_pin_config_t jreset_pin_config = {kGPIO_DigitalOutput, 1};
	GPIO_PinInit (GPIO, 2U, 19U, &jreset_pin_config); // CS
	GPIO_PinInit (GPIO, 1U, 11U, &jreset_pin_config); // SI/IO0
	GPIO_PinInit (GPIO, 1U, 29U, &jreset_pin_config); // SCK

	// Perform a reset sequence:
	// CS goes low 4 times with alternating values of SOUT
	// SCK is drive low or high and must stay in one state
	GPIO_PinWrite (GPIO, 1U, 29U, 0); // set SCK low
	for (uint32_t i = 0; i < 4; i++)
	{
		// drive CS low
		GPIO_PinWrite (GPIO, 2U, 19U, 0);
		SDK_DelayAtLeastUs (1, SystemCoreClock);
		// drive SI low or high: alternate its state every iteration
		GPIO_PinWrite (GPIO, 1U, 11U, (i&1));
		// drive CS high; SI state will be captured on the CS rising edge
		GPIO_PinWrite (GPIO, 2U, 19U, 1);
		SDK_DelayAtLeastUs (1, SystemCoreClock);
	}
	SDK_DelayAtLeastUs (110, SystemCoreClock);
}



/* BOARD_SetFlexspiClock run in RAM used to configure FlexSPI clock source and divider when XIP. */
void BOARD_SetFlexspiClock (uint32_t src, uint32_t divider)
{
    if ((CLKCTL0->FLEXSPIFCLKSEL != CLKCTL0_FLEXSPIFCLKSEL_SEL(src)) ||
        ((CLKCTL0->FLEXSPIFCLKDIV & CLKCTL0_FLEXSPIFCLKDIV_DIV_MASK) != (divider - 1)))
    {
        /* Disable clock before changing clock source */
        CLKCTL0->PSCCTL0_CLR = CLKCTL0_PSCCTL0_CLR_FLEXSPI_OTFAD_CLK_MASK;
        /* Update flexspi clock. */
        CLKCTL0->FLEXSPIFCLKSEL = CLKCTL0_FLEXSPIFCLKSEL_SEL(src);
        CLKCTL0->FLEXSPIFCLKDIV |= CLKCTL0_FLEXSPIFCLKDIV_RESET_MASK; /* Reset the divider counter */
        CLKCTL0->FLEXSPIFCLKDIV = CLKCTL0_FLEXSPIFCLKDIV_DIV(divider - 1);
        while ((CLKCTL0->FLEXSPIFCLKDIV) & CLKCTL0_FLEXSPIFCLKDIV_REQFLAG_MASK)
        {
        }
        /* Enable FLEXSPI clock again */
        CLKCTL0->PSCCTL0_SET = CLKCTL0_PSCCTL0_SET_FLEXSPI_OTFAD_CLK_MASK;
    }
}

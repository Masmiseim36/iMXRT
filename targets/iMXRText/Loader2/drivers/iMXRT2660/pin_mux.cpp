/** Loader for iMXRT-Family
Copyright (C) 2019-2026 Markus Klein
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
#include <array>


/*! BOARD_InitUARTPins
Set pin muxing and configure electrical properties for the UART */
void BOARD_InitUARTPins (void)
{
  IOMUXC_SetPin_Mux_Config (IOMUXC_PIO3_30_HSP_LPUART0_TXD, 0x80U); // Input Buffer Enable: Enables
  IOMUXC_SetPin_Mux_Config (IOMUXC_PIO3_31_HSP_LPUART0_RXD, 0x80U); // Input Buffer Enable: Enables
}


namespace
{
	struct PinConfig
	{
		uint32_t muxRegister;
		uint32_t muxMode;
		uint32_t inputRegister;
		uint32_t inputDaisy;
	};


	inline void SetPinMux (struct PinConfig config, uint32_t inputOnfield)
	{
		IOMUXC_SetPin_Mux_Config (config.muxRegister, config.muxMode, config.inputRegister, config.inputDaisy, inputOnfield);
	}

	struct FlexSPiPinConfig
	{
		PinConfig clock;
		PinConfig chipSelect;
		PinConfig dqs;
		std::array<PinConfig, 8> data;

		// For JEDEC Reset we need the GPIO configuration of chipselect, clock and data 0
		PinConfig clock_gpio;
		PinConfig chipSelect_gpio;
		PinConfig data_gpio;
		GPIO_Type *port;

		uint8_t clock_PinNumber;
		uint8_t chipSelect_PinNumber;
		uint8_t data_PinNumber;
	};

	const std::array<FlexSPiPinConfig, 1> FlexSpi0_PinConfig
	{
		// Other Pins:
		// IOMUXC_PIO2_22_MAIN_XSPI0_ECS
		// IOMUXC_PIO3_3_MAIN_XSPI0_ECS
		// IOMUXC_PIO3_30_MAIN_XSPI0_TST_TRG
		// IOMUXC_PIO4_26_MAIN_XSPI0_ECS
		// IOMUXC_PIO6_11_MAIN_XSPI0_SS1_N
		// IOMUXC_PIO6_12_MAIN_XSPI0_SCLK0_N
		FlexSPiPinConfig
		{
			IOMUXC_PIO6_0_MAIN_XSPI0_SCLK0, IOMUXC_PIO6_1_MAIN_XSPI0_SS0_N, IOMUXC_PIO6_6_MAIN_XSPI0_DQS0,
			{
				IOMUXC_PIO6_2_MAIN_XSPI0_DATA0,
				IOMUXC_PIO6_3_MAIN_XSPI0_DATA1,
				IOMUXC_PIO6_4_MAIN_XSPI0_DATA2,
				IOMUXC_PIO6_5_MAIN_XSPI0_DATA3,
				IOMUXC_PIO6_7_MAIN_XSPI0_DATA4,
				IOMUXC_PIO6_8_MAIN_XSPI0_DATA5,
				IOMUXC_PIO6_9_MAIN_XSPI0_DATA6,
				IOMUXC_PIO6_10_MAIN_XSPI0_DATA7
			},
			// clock                       chip select                    data0
			IOMUXC_PIO6_0_HSP_GPIO4_GPIO0, IOMUXC_PIO6_1_HSP_GPIO4_GPIO1, IOMUXC_PIO6_2_HSP_GPIO4_GPIO2,
			HSP__GPIO_4, 0, 1, 2
		}
	};


	const std::array<FlexSPiPinConfig, 2> FlexSpi1_PinConfig
	{
		// Other Pins
		// IOMUXC_PIO3_31_MAIN_XSPI1_TST_TRG
		// IOMUXC_PIO5_11_MAIN_XSPI1_SCLK0_N
		FlexSPiPinConfig
		{
			IOMUXC_PIO5_5_MAIN_XSPI1_SCLK0, IOMUXC_PIO5_0_MAIN_XSPI1_SS0_N, IOMUXC_PIO5_10_MAIN_XSPI1_DQS0,
			{
				IOMUXC_PIO5_1_MAIN_XSPI1_DATA0,
				IOMUXC_PIO5_2_MAIN_XSPI1_DATA1,
				IOMUXC_PIO5_3_MAIN_XSPI1_DATA2,
				IOMUXC_PIO5_4_MAIN_XSPI1_DATA3,
				IOMUXC_PIO5_6_MAIN_XSPI1_DATA4,
				IOMUXC_PIO5_7_MAIN_XSPI1_DATA5,
				IOMUXC_PIO5_8_MAIN_XSPI1_DATA6,
				IOMUXC_PIO5_9_MAIN_XSPI1_DATA7
			},
			// clock                       chip select                    data0
			IOMUXC_PIO5_5_HSP_GPIO3_GPIO5, IOMUXC_PIO5_0_HSP_GPIO3_GPIO0, IOMUXC_PIO5_10_HSP_GPIO3_GPIO10,
			HSP__GPIO_3, 5, 0, 10
		},
		FlexSPiPinConfig
		{
			IOMUXC_PIO5_5_MAIN_XSPI1_SCLK0, IOMUXC_PIO5_11_MAIN_XSPI1_SS1_N, IOMUXC_PIO5_16_MAIN_XSPI1_DQS1,
			{
				IOMUXC_PIO5_12_MAIN_XSPI1_DATA8,
				IOMUXC_PIO5_13_MAIN_XSPI1_DATA9,
				IOMUXC_PIO5_14_MAIN_XSPI1_DATA10,
				IOMUXC_PIO5_15_MAIN_XSPI1_DATA11,
				IOMUXC_PIO5_17_MAIN_XSPI1_DATA12,
				IOMUXC_PIO5_18_MAIN_XSPI1_DATA13,
				IOMUXC_PIO5_19_MAIN_XSPI1_DATA14,
				IOMUXC_PIO5_20_MAIN_XSPI1_DATA15
			},
			// clock                       chip select                      data0
			IOMUXC_PIO5_5_HSP_GPIO3_GPIO5, IOMUXC_PIO5_11_HSP_GPIO3_GPIO11, IOMUXC_PIO5_12_HSP_GPIO3_GPIO12,
			HSP__GPIO_3, 5, 11, 12
		}
	};
//	QuadSpi	
//	constexpr uint32_t PinConfig_Clock = IOMUXC_PIO_DRIVE(3); // Drive Strength: 33 ohm
//	constexpr uint32_t PinConfig_Data  = IOMUXC_PIO_DRIVE(3) | IOMUXC_PIO_IBENA_MASK; // Input Buffer Enable: Enables - Drive Strength: 33 ohm

	// High Speed
	//                                   Drive Strength 50 ohm |    High-speed mode    | Input Buffer Enable   | Enables pulldown
	constexpr uint32_t PinConfig_Clock = IOMUXC_PIO_DRIVE(2) | IOMUXC_PIO_SLEWRATE (3) | IOMUXC_PIO_IBENA_MASK; // 0x2380U
	constexpr uint32_t PinConfig_Data  = IOMUXC_PIO_DRIVE(2) | IOMUXC_PIO_SLEWRATE (3) | IOMUXC_PIO_IBENA_MASK; // 0x2380U
	constexpr uint32_t PinConfig_Dqs   = IOMUXC_PIO_DRIVE(2) | IOMUXC_PIO_SLEWRATE (3) | IOMUXC_PIO_IBENA_MASK | IOMUXC_PIO_PULLENA(1); // 0x2390U

	void InitQuadSpi (const FlexSPiPinConfig &config)
	{
		SetPinMux (config.clock,      PinConfig_Clock); // Drive Strength: 33 ohm
		SetPinMux (config.chipSelect, PinConfig_Clock); // Drive Strength: 33 ohm
		SetPinMux (config.data[0],    PinConfig_Data); // Input Buffer Enable: Enables - Drive Strength: 33 ohm
		SetPinMux (config.data[1],    PinConfig_Data);
		SetPinMux (config.data[2],    PinConfig_Data);
		SetPinMux (config.data[3],    PinConfig_Data);
	}


	void InitOctaSpi (const FlexSPiPinConfig &config)
	{
		InitQuadSpi (config);

		SetPinMux (config.dqs,     PinConfig_Dqs);
		SetPinMux (config.data[4], PinConfig_Data);
		SetPinMux (config.data[5], PinConfig_Data);
		SetPinMux (config.data[6], PinConfig_Data);
		SetPinMux (config.data[7], PinConfig_Data);
	}


	/*! \brief Perform a JEDEC compatible reset sequence FlexSPI1 Port A */ 
	void PerformJEDECReset (const FlexSPiPinConfig &config)
	{
		const gpio_pin_config_t jreset_pin_config {kGPIO_DigitalOutput, 1};

		// Configure the 3 pins used in JEDEC reset as GPIOs
		SetPinMux (config.clock_gpio,      1);
		SetPinMux (config.chipSelect_gpio, 1);
		SetPinMux (config.data_gpio,       1);

		// Set the direction of 3 pins used in JEDEC reset to output
		GPIO_PinInit (config.port, config.chipSelect_PinNumber, &jreset_pin_config); // CS
		GPIO_PinInit (config.port, config.data_PinNumber,       &jreset_pin_config); // SI/IO0
		GPIO_PinInit (config.port, config.clock_PinNumber,      &jreset_pin_config); // SCK


		// Perform a reset sequence:
		// CS goes low 4 times with alternating values of SOUT
		// SCK is drive low or high and must stay in one state
		GPIO_PinWrite (config.port, config.clock_PinNumber, 0); // set SCK low
		for (uint32_t i = 0; i < 4; i++)
		{
			// drive CS low
			GPIO_PinWrite (config.port, config.chipSelect_PinNumber, 0);
			SDK_DelayAtLeastUs (1, SystemCoreClock);
			// drive SI low or high: alternate its state every iteration
			GPIO_PinWrite (config.port, config.data_PinNumber, (i & 1U));
			// drive CS high; SI state will be captured on the CS rising edge
			GPIO_PinWrite (config.port, config.chipSelect_PinNumber, 1);
			SDK_DelayAtLeastUs (1, SystemCoreClock);
		}

		SDK_DelayAtLeastUs (110, SystemCoreClock);
	}
}


////////////////////////// FlexSPI 0 //////////////////////////////////
/*! \brief Set pin muxing and configure electrical properties for QSPI */
void BOARD_InitQuadSPI0Pins (void)
{
	InitQuadSpi (FlexSpi0_PinConfig[0]);
}

/*! \brief Set pin muxing and configure electrical properties for Hyperflash/OctaSPI */ 
void BOARD_InitOctaSPI0Pins (void)
{
	InitOctaSpi (FlexSpi0_PinConfig[0]);
}

void BOARD_PerformJEDECReset_xSPI0 (void)
{
	PerformJEDECReset (FlexSpi0_PinConfig[0]);
}


////////////////////////// FlexSPI 1 //////////////////////////////////
void BOARD_InitQuadSPI1Pins (void)
{
	InitQuadSpi (FlexSpi1_PinConfig[0]);
}

/*! \brief Set pin muxing and configure electrical properties for Hyperflash/OctaSPI */ 
void BOARD_InitOctaSPI1Pins (void)
{
	InitOctaSpi (FlexSpi1_PinConfig[0]);
}

void BOARD_PerformJEDECReset_xSPI1 (void)
{
	PerformJEDECReset (FlexSpi1_PinConfig[0]);
}

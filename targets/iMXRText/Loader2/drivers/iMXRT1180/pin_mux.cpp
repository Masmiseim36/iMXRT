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

#include "fsl_device_registers.h"
#include "fsl_iomuxc.h"
#include "fsl_clock.h"
#include "pin_mux.h"
#include "board.h"
#include <array>

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


struct PinConfig
{
	uint32_t muxRegister;
	uint32_t muxMode;
	uint32_t inputRegister;
	uint32_t inputDaisy;
	uint32_t configRegister;
};

namespace 
{
	inline void SetPinMux (struct PinConfig config, uint32_t inputOnfield)
	{
		IOMUXC_SetPinMux (config.muxRegister, config.muxMode, config.inputRegister, config.inputDaisy, config.configRegister, inputOnfield);
	}

	inline void SetPinConfig (struct PinConfig config, uint32_t configValue)
	{
		IOMUXC_SetPinConfig (config.muxRegister, config.muxMode, config.inputRegister, config.inputDaisy, config.configRegister, configValue);
	}


	inline void ConfigPin (struct PinConfig config, uint32_t inputOnfield, uint32_t configValue = IOMUXC_SW_PAD_CTL_PAD_PUS_MASK | IOMUXC_SW_PAD_CTL_PAD_DSE_MASK)
	{
		IOMUXC_SetPinMux    (config.muxRegister, config.muxMode, config.inputRegister, config.inputDaisy, config.configRegister, inputOnfield);
		IOMUXC_SetPinConfig (config.muxRegister, config.muxMode, config.inputRegister, config.inputDaisy, config.configRegister, configValue);
	}

	struct FlexSPiPinConfig
	{
		PinConfig clock;
		PinConfig chipSelect;
		PinConfig dqs;
		std::array<PinConfig, 8> data;

		// For JTAG Reset we need the GPIO config of chipselect, clock and data 0
		PinConfig clock_gpio;
		PinConfig chipSelect_gpio;
		PinConfig data_gpio;
		RGPIO_Type *port;

		uint8_t clock_PinNumber;
		uint8_t chipSelect_PinNumber;
		uint8_t data_PinNumber;
	};


	const std::array<FlexSPiPinConfig, 3> FlexSpi1_PinConfig
	{
		FlexSPiPinConfig
		{
			// Used in iMXRT1180-EVK for MT35XU512 OcaSPI or W25Q128 QuadSPI (default) selectable via Jumper
			// Used on FRDM-1186 for S26HL512TFPBHM000 Hyperflash (Page 8) - optional need to change resistors
			//	#define IOMUXC_GPIO_B2_02_FLEXSPI1_BUS2BIT_B_SCLK
			//	#define IOMUXC_GPIO_SD_B2_04_FLEXSPI1_BUS2BIT_B_SS1_B
			//	#define IOMUXC_GPIO_SD_B2_12_DUMMY_FLEXSPI1_BUS2BIT_B_DQS
			IOMUXC_GPIO_B2_08_FLEXSPI1_BUS2BIT_A_SCLK, IOMUXC_GPIO_B2_09_FLEXSPI1_BUS2BIT_A_SS0_B, IOMUXC_GPIO_B2_07_FLEXSPI1_BUS2BIT_A_DQS,
			{
				IOMUXC_GPIO_B2_10_FLEXSPI1_BUS2BIT_A_DATA00,
				IOMUXC_GPIO_B2_11_FLEXSPI1_BUS2BIT_A_DATA01,
				IOMUXC_GPIO_B2_12_FLEXSPI1_BUS2BIT_A_DATA02,
				IOMUXC_GPIO_B2_13_FLEXSPI1_BUS2BIT_A_DATA03,
				IOMUXC_GPIO_B2_03_FLEXSPI1_BUS2BIT_A_DATA04,
				IOMUXC_GPIO_B2_04_FLEXSPI1_BUS2BIT_A_DATA05,
				IOMUXC_GPIO_B2_05_FLEXSPI1_BUS2BIT_A_DATA06,
				IOMUXC_GPIO_B2_06_FLEXSPI1_BUS2BIT_A_DATA07
			},
			// clock                      chip select                   data0
			IOMUXC_GPIO_B2_08_GPIO6_IO22, IOMUXC_GPIO_B2_09_GPIO6_IO23, IOMUXC_GPIO_B2_10_GPIO6_IO24,
			RGPIO6, 22, 23, 24
		},
		FlexSPiPinConfig
		{
			// Used in iMXRT1180-EVK for W25Q128JWSIQ QuadSPI and Flash Doughter Card
			// Used on FRDM-1186 for W956A8MBYA5K OctaSPI optional used for FlexSPI-Follower (need to change resistors)
			//	#define IOMUXC_GPIO_SD_B1_03_FLEXSPI1_BUS2BIT_B_SS1_B
			//	#define IOMUXC_GPIO_SD_B1_04_FLEXSPI1_BUS2BIT_B_SS0_B
			//	#define IOMUXC_GPIO_SD_B1_05_FLEXSPI1_BUS2BIT_B_SS0_B
			IOMUXC_GPIO_SD_B2_07_FLEXSPI1_BUS2BIT_B_SCLK, IOMUXC_GPIO_SD_B2_06_FLEXSPI1_BUS2BIT_B_SS0_B, IOMUXC_GPIO_SD_B2_05_FLEXSPI1_BUS2BIT_B_DQS,
			{
				IOMUXC_GPIO_SD_B2_08_FLEXSPI1_BUS2BIT_B_DATA00,
				IOMUXC_GPIO_SD_B2_09_FLEXSPI1_BUS2BIT_B_DATA01,
				IOMUXC_GPIO_SD_B2_10_FLEXSPI1_BUS2BIT_B_DATA02,
				IOMUXC_GPIO_SD_B2_11_FLEXSPI1_BUS2BIT_B_DATA03,
				IOMUXC_GPIO_SD_B2_00_FLEXSPI1_BUS2BIT_B_DATA04,
				IOMUXC_GPIO_SD_B2_01_FLEXSPI1_BUS2BIT_B_DATA05,
				IOMUXC_GPIO_SD_B2_02_FLEXSPI1_BUS2BIT_B_DATA06,
				IOMUXC_GPIO_SD_B2_03_FLEXSPI1_BUS2BIT_B_DATA07
			},
			IOMUXC_GPIO_SD_B2_07_GPIO5_IO17, IOMUXC_GPIO_SD_B2_06_GPIO5_IO16, IOMUXC_GPIO_SD_B2_08_GPIO5_IO18,
			RGPIO5, 17, 16, 18
		},
		FlexSPiPinConfig
		{
			//	#define IOMUXC_GPIO_B1_02_FLEXSPI1_BUS2BIT_B_SS1_B
			IOMUXC_GPIO_B1_05_FLEXSPI1_BUS2BIT_B_SCLK, IOMUXC_GPIO_B1_04_FLEXSPI1_BUS2BIT_B_SS0_B, IOMUXC_GPIO_B1_03_FLEXSPI1_BUS2BIT_B_DQS,
			{
				IOMUXC_GPIO_B1_13_FLEXSPI1_BUS2BIT_B_DATA00,
				IOMUXC_GPIO_B1_12_FLEXSPI1_BUS2BIT_B_DATA01,
				IOMUXC_GPIO_B1_11_FLEXSPI1_BUS2BIT_B_DATA02,
				IOMUXC_GPIO_B1_10_FLEXSPI1_BUS2BIT_B_DATA03,
				IOMUXC_GPIO_B1_09_FLEXSPI1_BUS2BIT_B_DATA04,
				IOMUXC_GPIO_B1_08_FLEXSPI1_BUS2BIT_B_DATA05,
				IOMUXC_GPIO_B1_07_FLEXSPI1_BUS2BIT_B_DATA06,
				IOMUXC_GPIO_B1_06_FLEXSPI1_BUS2BIT_B_DATA07
			},
			IOMUXC_GPIO_B1_05_GPIO6_IO05, IOMUXC_GPIO_B1_04_GPIO6_IO04, IOMUXC_GPIO_B1_13_GPIO6_IO13,
			RGPIO6, 5, 4, 13
		}
	};

	const std::array<FlexSPiPinConfig, 4> FlexSpi2_PinConfig
	{
		FlexSPiPinConfig
		{
			IOMUXC_GPIO_EMC_B1_41_FLEXSPI2_BUS2BIT_A_SCLK, IOMUXC_GPIO_EMC_B1_39_FLEXSPI2_BUS2BIT_A_SS0_B, IOMUXC_GPIO_EMC_B1_21_FLEXSPI2_BUS2BIT_B_DQS,
			{
				IOMUXC_GPIO_EMC_B1_35_FLEXSPI2_BUS2BIT_A_DATA00,
				IOMUXC_GPIO_EMC_B1_36_FLEXSPI2_BUS2BIT_A_DATA01,
				IOMUXC_GPIO_EMC_B1_37_FLEXSPI2_BUS2BIT_A_DATA02,
				IOMUXC_GPIO_EMC_B1_38_FLEXSPI2_BUS2BIT_A_DATA03,
				IOMUXC_GPIO_EMC_B1_33_FLEXSPI2_BUS2BIT_B_DATA00,
				IOMUXC_GPIO_EMC_B1_32_FLEXSPI2_BUS2BIT_B_DATA01,
				IOMUXC_GPIO_EMC_B1_31_FLEXSPI2_BUS2BIT_B_DATA02,
				IOMUXC_GPIO_EMC_B1_30_FLEXSPI2_BUS2BIT_B_DATA03
			},
			// clock                          chip select                   data0
			IOMUXC_GPIO_EMC_B1_41_GPIO3_IO09, IOMUXC_GPIO_EMC_B1_39_GPIO3_IO07, IOMUXC_GPIO_EMC_B1_35_GPIO3_IO03,
			RGPIO3, 9, 7, 3
		},
		FlexSPiPinConfig
		{
			// Only QuadSPI- B-Channel from configuration above
			IOMUXC_GPIO_EMC_B1_34_FLEXSPI2_BUS2BIT_B_SCLK, IOMUXC_GPIO_EMC_B1_28_FLEXSPI2_BUS2BIT_B_SS0_B, IOMUXC_GPIO_EMC_B1_29_FLEXSPI2_BUS2BIT_B_DQS,
			{
				//	#define IOMUXC_GPIO_EMC_B1_22_FLEXSPI2_BUS2BIT_B_DATA03
				//	#define IOMUXC_GPIO_EMC_B1_23_FLEXSPI2_BUS2BIT_B_DATA02
				//	#define IOMUXC_GPIO_EMC_B1_24_FLEXSPI2_BUS2BIT_B_DATA01
				//	#define IOMUXC_GPIO_EMC_B1_25_FLEXSPI2_BUS2BIT_B_DATA00
				IOMUXC_GPIO_EMC_B1_33_FLEXSPI2_BUS2BIT_B_DATA00,
				IOMUXC_GPIO_EMC_B1_32_FLEXSPI2_BUS2BIT_B_DATA01,
				IOMUXC_GPIO_EMC_B1_31_FLEXSPI2_BUS2BIT_B_DATA02,
				IOMUXC_GPIO_EMC_B1_30_FLEXSPI2_BUS2BIT_B_DATA03,
				{0, 0, 0, 0, 0},
				{0, 0, 0, 0, 0},
				{0, 0, 0, 0, 0},
				{0, 0, 0, 0, 0},
			},
			// clock                          chip select                   data0
			IOMUXC_GPIO_EMC_B1_34_GPIO3_IO02, IOMUXC_GPIO_EMC_B1_28_GPIO2_IO28, IOMUXC_GPIO_EMC_B1_29_GPIO2_IO29,
			RGPIO2, 2, 28, 29
		},
		FlexSPiPinConfig
		{
			//	#define IOMUXC_GPIO_AON_20_FLEXSPI2_BUS2BIT_B_DQS
			//	#define IOMUXC_GPIO_AON_19_FLEXSPI2_BUS2BIT_A_SS1_B
			//	#define IOMUXC_GPIO_AON_20_FLEXSPI2_BUS2BIT_A_SS1_B
			//	#define IOMUXC_GPIO_AON_28_DUMMY_FLEXSPI2_BUS2BIT_B_DQS
			IOMUXC_GPIO_AON_19_FLEXSPI2_BUS2BIT_B_SCLK, IOMUXC_GPIO_AON_21_FLEXSPI2_BUS2BIT_B_SS0_B, IOMUXC_GPIO_AON_20_FLEXSPI2_BUS2BIT_B_DQS,
			{
				IOMUXC_GPIO_AON_18_FLEXSPI2_BUS2BIT_B_DATA00,
				IOMUXC_GPIO_AON_17_FLEXSPI2_BUS2BIT_B_DATA01,
				IOMUXC_GPIO_AON_16_FLEXSPI2_BUS2BIT_B_DATA02,
				IOMUXC_GPIO_AON_15_FLEXSPI2_BUS2BIT_B_DATA03,
				IOMUXC_GPIO_AON_18_FLEXSPI2_BUS2BIT_B_DATA00,
				IOMUXC_GPIO_AON_17_FLEXSPI2_BUS2BIT_B_DATA01,
				IOMUXC_GPIO_AON_16_FLEXSPI2_BUS2BIT_B_DATA02,
				IOMUXC_GPIO_AON_15_FLEXSPI2_BUS2BIT_B_DATA03 // Stimmt das mit zwei mal Kanal-B????
			},
			// clock                          chip select                   data0
			IOMUXC_GPIO_AON_19_GPIO1_IO19, IOMUXC_GPIO_AON_21_GPIO1_IO21, IOMUXC_GPIO_AON_18_GPIO1_IO18,
			RGPIO1, 19, 21, 18
		},
		FlexSPiPinConfig
		{
			// Used on FRDM-1186 for W25Q128JVSIQ QuadSPI (Page 8 of the schematics)
			// #define IOMUXC_GPIO_AON_28_DUMMY_FLEXSPI2_BUS2BIT_A_DQS
			IOMUXC_GPIO_AON_23_FLEXSPI2_BUS2BIT_A_SCLK, IOMUXC_GPIO_AON_22_FLEXSPI2_BUS2BIT_A_SS0_B, IOMUXC_GPIO_AON_21_FLEXSPI2_BUS2BIT_A_DQS,
			{
				IOMUXC_GPIO_AON_24_FLEXSPI2_BUS2BIT_A_DATA00,
				IOMUXC_GPIO_AON_25_FLEXSPI2_BUS2BIT_A_DATA01,
				IOMUXC_GPIO_AON_26_FLEXSPI2_BUS2BIT_A_DATA02,
				IOMUXC_GPIO_AON_27_FLEXSPI2_BUS2BIT_A_DATA03,
				IOMUXC_GPIO_AON_18_FLEXSPI2_BUS2BIT_B_DATA00,
				IOMUXC_GPIO_AON_17_FLEXSPI2_BUS2BIT_B_DATA01,
				IOMUXC_GPIO_AON_16_FLEXSPI2_BUS2BIT_B_DATA02,
				IOMUXC_GPIO_AON_15_FLEXSPI2_BUS2BIT_B_DATA03
			},
			// clock                          chip select                   data0
			IOMUXC_GPIO_AON_23_GPIO1_IO23, IOMUXC_GPIO_AON_22_GPIO1_IO22, IOMUXC_GPIO_AON_24_GPIO1_IO24,
			RGPIO2, 23, 22, 24
		}
	};

	void InitQuadSpi (const FlexSPiPinConfig &config)
	{
		CLOCK_EnableClock (kCLOCK_Iomuxc1);          // Turn on LPCG
		CLOCK_EnableClock (kCLOCK_Iomuxc2);          // Turn on LPCG

		ConfigPin (config.clock,      1U);
		ConfigPin (config.chipSelect, 1U);
		ConfigPin (config.data[0],    1U);
		ConfigPin (config.data[1],    1U);
		ConfigPin (config.data[2],    1U);
		ConfigPin (config.data[3],    1U);
	}


	void InitOctaSpi (const FlexSPiPinConfig &config)
	{
		InitQuadSpi (config);

		ConfigPin (config.dqs,     1U);
		ConfigPin (config.data[4], 1U);
		ConfigPin (config.data[5], 1U);
		ConfigPin (config.data[6], 1U);
		ConfigPin (config.data[7], 1U);
	}


	/*! \brief Perform a JEDEC compatible reset sequence FlexSPI1 Port A */ 
	void PerformJEDECReset (const FlexSPiPinConfig &config)
	{
		const rgpio_pin_config_t jreset_pin_config {kRGPIO_DigitalOutput, 1};

		// Configure the 3 pins used in JEDEC reset as GPIOs
		SetPinMux (config.clock_gpio,      1);
		SetPinMux (config.chipSelect_gpio, 1);
		SetPinMux (config.data_gpio,       1);

		// Set the direction of 3 pins used in JEDEC reset to output
		RGPIO_PinInit (config.port, config.chipSelect_PinNumber, &jreset_pin_config); // CS
		RGPIO_PinInit (config.port, config.data_PinNumber,       &jreset_pin_config); // SI/IO0
		RGPIO_PinInit (config.port, config.clock_PinNumber,      &jreset_pin_config); // SCK


		// Perform a reset sequence:
		// CS goes low 4 times with alternating values of SOUT
		// SCK is drive low or high and must stay in one state
		RGPIO_WritePinOutput (config.port, config.clock_PinNumber, 0); // set SCK low
		for (uint32_t i = 0; i < 4; i++)
		{
			// drive CS low
			RGPIO_WritePinOutput (config.port, config.chipSelect_PinNumber, 0);
			SDK_DelayAtLeastUs (1, SystemCoreClock);
			// drive SI low or high: alternate its state every iteration
			RGPIO_WritePinOutput (config.port, config.data_PinNumber, (i & 1U));
			// drive CS high; SI state will be captured on the CS rising edge
			RGPIO_WritePinOutput (config.port, config.chipSelect_PinNumber, 1);
			SDK_DelayAtLeastUs (1, SystemCoreClock);
		}

		SDK_DelayAtLeastUs (110, SystemCoreClock);
	}
}


////////////////////////// FlexSPI 1 //////////////////////////////////
/*! \brief Set pin muxing and configure electrical properties for QSPI */
void BOARD_InitQuadSPI1Pins (void)
{
	InitQuadSpi (FlexSpi1_PinConfig[0]);
}

/*! \brief Set pin muxing and configure electrical properties for Hyperflash/OctaSPI */ 
void BOARD_InitOctaSPI1Pins (void)
{
	InitOctaSpi (FlexSpi1_PinConfig[0]);
}

void BOARD_PerformJEDECReset_FlexSPI1 (void)
{
	PerformJEDECReset (FlexSpi1_PinConfig[0]);
}


////////////////////////// FlexSPI 2 //////////////////////////////////

void BOARD_InitQuadSPI2Pins (void)
{
	InitQuadSpi (FlexSpi2_PinConfig[3]);
}

/*! \brief Set pin muxing and configure electrical properties for Hyperflash/OctaSPI */ 
void BOARD_InitOctaSPI2Pins (void)
{
	InitOctaSpi (FlexSpi2_PinConfig[3]);
}

void BOARD_PerformJEDECReset_FlexSPI2 (void)
{
	PerformJEDECReset (FlexSpi2_PinConfig[3]);
}
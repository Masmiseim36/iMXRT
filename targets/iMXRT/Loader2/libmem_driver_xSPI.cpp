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


#include "libmem_driver_xSPI.h"
#if defined XSPI0 || defined XSPI1 || defined XSPI2
	#include "libmem_Tools.h"
	#include "libmem_callbacks.h"

	#include "FlexSPI_Generic.h"
	#include "FlexSPI_Adesto.h"
	#include "FlexSPI_Macronix.h"
	#include "FlexSPI_Winbond.h"
	#include "FlexSPI_ISSI.h"
	#include "FlexSPI_Spansion.h"
	#include "FlexSPI_Micron.h"
	#include "FlexSPI_Helper.h"

	#include "fsl_power.h"
	#include "fsl_xspi.h"


	namespace Xspi
	{
		namespace
		{
			constexpr size_t FlashSize {0x10000}; /* 64MB/KByte */
			constexpr size_t PageSize  {    256};
			xspi_device_ddr_config_t FlashDDrConfig
			{
				.enableDdr                 = true,
				.ddrDataAlignedClk         = kXSPI_DDRDataAlignedWith2xInternalRefClk,
				.enableByteSwapInOctalMode = false,
			};
			/*!
			* @brief Configuration for MX25UW51345GXD100.
			* 
			* General Features:
			*      - Density: 512 Mb
			*      - Page Size: 256 byte
			*      - Support Data strobe signal
			*      - Maximum frequency(DDR mode):
			*          - Single I/O mode: 133MHz
			*          - Octal I/O mode:  200MHz
			*/
			xspi_device_config_t DeviceConfig
			{
				.xspiRootClk           = 400000000,  /*!< 400MHz */
				.enableCknPad          = false,  /*!< Do not support differential clock */      
				.deviceInterface       = kXSPI_StrandardExtendedSPI,  /*!< Support Single IO and Octal IO */
				.interfaceSettings
				{
					.strandardExtendedSPISettings
					{
						.pageSize      = PageSize,   /*!< 256 byte page buffer. */
					},
/*					.hyperBusSettings // union definition, would override strandardExtendedSPISettings.pageSize
					{
						.x16Mode       = kXSPI_x16ModeDisable,
						.enableVariableLatency = false,
						.forceBit10To1 = false,
						.pageSize      = PageSize,
					}*/
				},
				.CSHoldTime            = 3,
				.CSSetupTime           = 3,
				.sampleClkConfig
				{
					.sampleClkSource   = kXSPI_SampleClkFromDqsPadLoopback, // ToDo Check this // kXSPI_SampleClkFromExternalDQS,  /*!< Device support Data strobe signal.  */
					.enableDQSLatency  = false,
					.dllConfig
					{
						.dllMode       = kXSPI_AutoUpdateMode,
						.useRefValue   = true,
						.dllCustomPara
						{
							.bypassModePara
							{
								.delayElementCoarseValue = 0,
								.delayElementFineValue   = 0,
								.offsetDelayElementCount = 0,
								.enableHighFreq          = 0,
								.bypassModeReserved      = 0,
							},
/*							.autoUpdateModoPara // union definition, would override bypassModePara
							{
								.referenceCounter              = 0,
								.resolution                    = 0,
								.offsetDelayElementCount       = 0,
								.tDiv16OffsetDelayElementCount = 0,
								.enableHighFreq                = 0,
							} */
						},
						.dllCustomDelayTapNum = 0,
						.enableCdl8           = true,
					},
				},
				.ptrDeviceDdrConfig    = &FlashDDrConfig,
				.addrMode              = kXSPI_DeviceByteAddressable,
				.columnAddrWidth       = 0U,
				.enableCASInterleaving = false,
				.deviceSize
				{
					FlashSize,
					FlashSize,   // Single DIE flash, so deviceSize1 should equal to deviceSize0
				},
				.ptrDeviceRegInfo      = nullptr,      /*!< Not used in this example. */
			};
		} // namespace 
	} // namespace Xspi

	namespace Hyperflash
	{
	}

	namespace
	{
		// Define the structure of the Flash (Sector Count and Size)
		libmem_geometry_t geometry []
		{ // count - size
			{0x2000, 4096},
			{0, 0} 
		};

		inline int GetPageSize (MemoryType memType)
		{
			if (memType == MemoryType::Hyperflash)
				return HYPERFLASH_PAGE_SIZE;

			return QSPIFLASH_PAGE_SIZE;

		}
	}

	/*! Libmem_InitializeDriver_xSPI:
	\brief Initialize the FlexSPI interface for using as a SPI-interface
	\param base The Flex-SPI-base to use
	\param memType The Type of SPI-interface to use if possible
	\return LibmemStatus_t LibmemStaus_Success if the operation was successfully */
	LibmemStatus_t Libmem_InitializeDriver_xSPI (FlexSPI_Helper *base, enum MemoryType memType)
	{
		CLOCK_AttachClk (kMAIN_PLL_PFD1_to_XSPI0);
		CLOCK_SetClkDiv (kCLOCK_DivXspi0Clk, 1U);     // 400MHz

		POWER_DisablePD (kPDRUNCFG_APD_XSPI0);
		POWER_DisablePD (kPDRUNCFG_PPD_XSPI0);
		POWER_ApplyPD   ();

		xspi_ahb_access_config_t xspiAhbAccessConfig
		{
			.buffer
			{
				{
					.masterId               = 0,
					.enaPri
					{
						.enablePriority     = false,
					},
					.bufferSize             = 0x80U,
					.ptrSubBuffer0Config    = nullptr,
					.ptrSubBuffer1Config    = nullptr,
					.ptrSubBuffer2Config    = nullptr,
					.ptrSubBuffer3Config    = nullptr,
				},
				{
					.masterId               = 1,
					.enaPri
					{
						.enablePriority     = false,
					},
					.bufferSize             = 0x80U,
					.ptrSubBuffer0Config    = nullptr,
					.ptrSubBuffer1Config    = nullptr,
					.ptrSubBuffer2Config    = nullptr,
					.ptrSubBuffer3Config    = nullptr,
				},
				{
					.masterId               = 2,
					.enaPri
					{
						.enableAllMaster    = false,
					},
					.bufferSize             = 0x80U,
					.ptrSubBuffer0Config    = nullptr,
					.ptrSubBuffer1Config    = nullptr,
					.ptrSubBuffer2Config    = nullptr,
					.ptrSubBuffer3Config    = nullptr,
				},
				{
					.masterId               = 3,
					.enaPri
					{
						.enablePriority     = true,
					},
					.bufferSize             = 0x80U,
					.ptrSubBuffer0Config    = nullptr,
					.ptrSubBuffer1Config    = nullptr,
					.ptrSubBuffer2Config    = nullptr,
					.ptrSubBuffer3Config    = nullptr,
				},
			},
			.ARDSeqIndex               = LUT_ReadArray,
			.enableAHBPrefetch         = true,
			.enableAHBBufferWriteFlush = true,
			.ahbSplitSize              = kXSPI_AhbSplitSizeDisabled,
			.ahbAlignment              = kXSPI_AhbAlignmentNoLimit,
			.ptrAhbWriteConfig         = nullptr,
			.ahbErrorPayload
			{
				.highPayload           = 0x5A5A5A5AUL,
				.lowPayload            = 0x5A5A5A5AUL
			}
		};
		xspi_ip_access_config_t xspiIpAccessConfig
		{
			.ptrSfpMdadConfig               = nullptr,
			.ptrSfpFradConfig               = nullptr, // Disable SFD feature for now
			.sfpArbitrationLockTimeoutValue = 0xFFFFFFUL,
			.ipAccessTimeoutValue           = 0xFFFFFFFFUL
		};
		const xspi_config_t config
		{
			#if (defined(FSL_FEATURE_XSPI_HAS_END_CFG) && FSL_FEATURE_XSPI_HAS_END_CFG)
				.byteOrder = kXSPI_64BitLE,
			#else
				.byteOrder = kXSPI_32BitLE,
			#endif
			.enableDoze         = false,
			.ptrAhbAccessConfig = &xspiAhbAccessConfig,
			.ptrIpAccessConfig  = &xspiIpAccessConfig
		};

        // Disable the caches. Otherwise written data may not be visible
        XSPI_Cache64_DisableCache (CACHE64_CTRL0_NS);
        XSPI_Cache64_DisableCache (CACHE64_CTRL1_NS);

		XSPI_Init            (base, &config);
		XSPI_SetDeviceConfig (base, &Xspi::DeviceConfig);
		XSPI_UpdateLUT       (base, 0, &Generic::LUT_SPI.front(), Generic::LUT_SPI.size()); // Update LUT table


		uint32_t flashSize {};
		// Reconfigure the interface according to the gathered flash information and configuration
		if (memType == MemoryType::Hyperflash)
		{
			// ToDo: We need to implement this
		}
		else if (memType == MemoryType::Hyperram)
		{
			// ToDo: We need to implement this
		}
		else if (memType == MemoryType::OctaSPI_DDR || memType == MemoryType::QuadSPI_DDR || memType == MemoryType::Hyperflash || memType == MemoryType::Hyperram)
		{
			DeviceInfo info {};
			status_t status {};
			// ##########################
			// Copied - maybe place in a function
			// Get the JEDEC Informations
			static const std::array<status_t (*) (FlexSPI_Helper &, DeviceInfo &), 4> JedecReader
			{
				[](FlexSPI_Helper &b, DeviceInfo &i){return b.ReadJEDEC (&i);},
				Macronix::TryDetect,
				ISSI::TryDetect,
				Micron::TryDetect
			};
			for (auto reader : JedecReader)
			{
				status = reader (*base, info);
				if (status == kStatus_Success && info.ManufactureID != ManufactureID_UNDEF)
					break;
			}

			if (status != kStatus_Success || info.ManufactureID == ManufactureID_UNDEF)
			{
				DebugPrint ("JEDEC read Error\r\n");
				return LibmemStaus_InvalidDevice;
			}

			// Check for the Manufacture-ID and adapt the Configuration
			LibmemStatus_t res = LibmemStaus_Success;
			switch (info.ManufactureID)
			{
				case ManufactureID_AdestoTechnologies:
				case ManufactureID_Atmel:		// Renesas
					res = Adesto::Initialize (*base, memType, info);
					break;
				case ManufactureID_Nexcom:		// Winbond
					res = Winbond::Initialize (*base, memType, info);
					break;
				case ManufactureID_Macronix:	// Macronix
					res = Macronix::Initialize (*base, memType, info);
					break;
				case ManufactureID_Lucent:		// ISSI
					res = ISSI::Initialize (*base, memType, info);
					break;
				case ManufactureID_MicronTechnology:
					res = Micron::Initialize (*base, memType, info);
					break;
				default:
					DebugPrint ("unknown Flash-memory\r\n");
					return LibmemStaus_InvalidDevice;
			}

			if (res != LibmemStaus_Success)
				return res;
			// copy end
			// ###############################################################


			// Use the size information from the JEDEC-information to configure the interface
			flashSize = CalculateCapacity_KBytes (info.Capacity);
			Xspi::DeviceConfig.deviceSize[0] = flashSize;
			Xspi::DeviceConfig.deviceSize[1] = flashSize;
			geometry[0].count = flashSize / (geometry[0].size / 1024);
			flashSize *= 1024;	// Convert kBytes to bytes

			Xspi::DeviceConfig.sampleClkConfig.sampleClkSource = kXSPI_SampleClkFromExternalDQS; // To achieve high speeds - always use DQS
			XSPI_SetDeviceConfig (base, &Xspi::DeviceConfig); // changing the clock requires reinitialization
		}

		static uint8_t writeBuffer[HYPERFLASH_PAGE_SIZE];
		LibmemDriver *FlashHandle = LibmemDriver::GetDriver ();
		// No use of the extended driver callbacks as the external Flash is memorry mapped. We use direct access for reading
//		libmem_register_driver (FlashHandle, GetBaseAddress(base), flashSize, &geometry[0], nullptr, &DriverFunctions, &DriverFunctions_Extended);
		libmem_register_driver (FlashHandle, base->GetAmbaAddress (), flashSize, &geometry[0], nullptr, &Xspi::DriverFunctions, nullptr);
		int err = libmem_driver_paged_write_init (&FlashHandle->PageWriteControlBlock, &writeBuffer[0], GetPageSize (memType), Xspi::ProgramPage, 4, 0);
		FlashHandle->user_data = reinterpret_cast<uint32_t>(base);

		uint8_t *AliasAddress = base->GetAmbaAliasAddress ();
		if (AliasAddress != nullptr && err == LIBMEM_STATUS_SUCCESS)
		{
			FlashHandle = LibmemDriver::GetDriver ();
			libmem_register_driver (FlashHandle, AliasAddress, flashSize, &geometry[0], nullptr, &Xspi::DriverFunctions, nullptr);
			err = libmem_driver_paged_write_init (&FlashHandle->PageWriteControlBlock, &writeBuffer[0], GetPageSize (memType), Xspi::ProgramPage, 4, 0);
			FlashHandle->user_data = reinterpret_cast<uint32_t>(base);
			DebugPrint ("### Add Driver for Alias\r\n");
		}

		return static_cast<LibmemStatus_t>(err);
	}

#endif  // defined XSPI0 || defined XSPI1 || defined XSPI2
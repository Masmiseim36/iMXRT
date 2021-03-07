/** Loader for iMXRT-Family
Copyright (C) 2019-2021  Markus Klein
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

#ifndef _LIBMEM_TOOLS_H_
#define _LIBMEM_TOOLS_H_

#include "fsl_device_registers.h"
#include "libmem.h"

#ifdef __cplusplus
extern "C"
{
#endif

	#ifndef FlexSPI_AMBA_BASE
		#define FlexSPI_AMBA_BASE FlexSPI1_AMBA_BASE 
		#define FLEXSPI FLEXSPI1
		#define FLEXSPI_BASE FLEXSPI1_BASE
	#endif

	enum FlashParmeter
	{
		HYPERFLASH_PAGE_SIZE = 512U,
		QSPIFLASH_PAGE_SIZE  = 256U,
//		FLASH_SECTORE_SIZE   = 0x40000,	// 4096U,
		FLASH_START_ADDRESS  = FlexSPI_AMBA_BASE,
		#if defined FLEXSPI2
		FLASH2_START_ADDRESS = FlexSPI2_AMBA_BASE,
		#endif
		CUSTOM_LUT_LENGTH    = 64
	};

	/** enum Capacity:
	Device Size = 2^n in number of bytes.
	@author Markus Klein*/
	enum Capacity
	{
		Capacity_UNDEF    = 0x00,
		Capacity_4MBit    = 0x13,
		Capacity_8MBit    = 0x14,
		Capacity_16MBit   = 0x15,
		Capacity_32MBit   = 0x16,
		Capacity_64MBit   = 0x17,
		Capacity_128MBit  = 0x18,
		Capacity_256MBit  = 0x19,
		Capacity_512MBit  = 0x1A,
		Capacity_1024MBit = 0x1B,
	};

	uint32_t libmem_CalculateOffset         (libmem_driver_handle_t *handle, uint8_t *Addr);
	uint8_t *libmem_GetBaseAddress          (FLEXSPI_Type *base);
	uint8_t *libmem_GetAliasBaseAddress     (FLEXSPI_Type *base);
	const char * Libmem_GetErrorString      (int Error);
	uint_least32_t CalculateCapacity_KBytes (enum Capacity c);

	inline bool IsSectorEmpty (const uint32_t *const start)
	{
		for (uint32_t i=0; i<4096/sizeof(uint32_t); i++)
			if (start[i] != 0xFFFFFFFFU)
				return false;

		return true;
	}

	class LibmemDriver: public libmem_driver_handle_t
	{
	private:
		static LibmemDriver Drivers[4];	// We should not need more than four
		static uint32_t NextFree;
	public:
		static LibmemDriver *GetDriver (void)
		{
			if (NextFree >= sizeof (LibmemDriver::Drivers)/sizeof(LibmemDriver::Drivers[0]))
				return nullptr;
			else
			{
				LibmemDriver *drv = &LibmemDriver::Drivers[LibmemDriver::NextFree];
				LibmemDriver::NextFree++;
				return drv;
			}
		}

		uint32_t CalculateOffset (uint8_t *Addr)
		{
			return Addr - this->start;
		}
	};


	enum MemoryType
	{
		MemType_Invalid     = 0,
		MemType_Hyperflash  = 1,
		MemType_OctaSPI_DDR = 2,
		MemType_OctaSPI     = 3,
		MemType_QuadSPI_DDR = 4,
		MemType_QuadSPI     = 5,
		MemType_SPI         = 6,
	};


	typedef enum LibmemStatus
	{
		LibmemStaus_Success          = (int)LIBMEM_STATUS_SUCCESS,
		LibmemStaus_Error            = (int)LIBMEM_STATUS_ERROR,
		LibmemStaus_Timeout          = (int)LIBMEM_STATUS_TIMEOUT,
		LibmemStaus_Locked           = (int)LIBMEM_STATUS_LOCKED,
		LibmemStaus_NotImplemented   = (int)LIBMEM_STATUS_NOT_IMPLEMENTED,
		LibmemStaus_Overflow         = (int)LIBMEM_STATUS_GEOMETRY_REGION_OVERFLOW,
		LibmemStaus_NoDriver         = (int)LIBMEM_STATUS_NO_DRIVER,
		LibmemStaus_CFIerror         = (int)LIBMEM_STATUS_CFI_ERROR,
		LibmemStaus_InvalidRange     = (int)LIBMEM_STATUS_INVALID_RANGE,
		LibmemStaus_InvalidParameter = (int)LIBMEM_STATUS_INVALID_PARAMETER,
		LibmemStaus_InvalidWidth     = (int)LIBMEM_STATUS_INVALID_WIDTH,
		LibmemStaus_InvalidDevice    = (int)LIBMEM_STATUS_INVALID_DEVICE,

		// extension
		LibmemStatus_InvalidMemoryType = -127
	}LibmemStatus_t;



	/** enum ManufactureID:
	The manufacturer’s identification code is defined by one or more eight (8) bit fields, each
	consisting of seven (7) data bits plus one (1) odd parity bit. The manufacturer’s identification
	code is assigned, maintained and updated by the JEDEC office. It is a
	single field, limiting the possible number of vendors to 128. To expand the maximum number of
	identification codes, a continuation scheme has been defined. The code 7F
	indicates that the manufacturer’s code is beyond the limit of this field and the next sequential
	manufacturer’s identification field is used.  Multiple continuation fields are permitted and when
	used, shall comprise of the identification code.
	@author Markus Klein*/
	enum SerialFlash_ManufactureID
	{
		ManufactureID_UNDEF                        = 0x00,
		ManufactureID_NEXT_MARKER                  = 0x7F,

		// The following numbers are all in bank one:
		ManufactureID_Spansion                     = 0x100 | 0x01, // originally was AMD
		ManufactureID_AMI                          = 0x100 | 0x02,
		ManufactureID_Fairchild                    = 0x100 | 0x83,
		ManufactureID_Fujitsu                      = 0x100 | 0x04, 
		ManufactureID_GTE                          = 0x100 | 0x85,
		ManufactureID_Harris                       = 0x100 | 0x86,
		ManufactureID_Hitachi                      = 0x100 | 0x07,
		ManufactureID_Inmos                        = 0x100 | 0x08,
		ManufactureID_Intel                        = 0x100 | 0x89,
		ManufactureID_ITT                          = 0x100 | 0x8A,	// 10
		ManufactureID_Intersil                     = 0x100 | 0x0B,
		ManufactureID_MonolithicMemories           = 0x100 | 0x8C,
		ManufactureID_Mostek                       = 0x100 | 0x0D,
		ManufactureID_Motorola                     = 0x100 | 0x0E,
		ManufactureID_National                     = 0x100 | 0x8F,
		ManufactureID_NEC                          = 0x100 | 0x10,
		ManufactureID_RCA                          = 0x100 | 0x91,
		ManufactureID_Raytheon                     = 0x100 | 0x92,
		ManufactureID_Conexant                     = 0x100 | 0x13,	// originally was Rockwell
		ManufactureID_Seeq                         = 0x100 | 0x94,	// 20
		ManufactureID_PhilipsSemiconductor         = 0x100 | 0x15,
		ManufactureID_Synetek                      = 0x100 | 0x16,
		ManufactureID_TexasInstruments             = 0x100 | 0x97,
		ManufactureID_Toshiba                      = 0x100 | 0x98,
		ManufactureID_Xicor                        = 0x100 | 0x19,
		ManufactureID_Zilog                        = 0x100 | 0x1A,
		ManufactureID_Eurotechnique                = 0x100 | 0x9B,
		ManufactureID_Mitsubishi                   = 0x100 | 0x1C,
		ManufactureID_Lucent                       = 0x100 | 0x9D,	// AT&T - ISSI?
		ManufactureID_Exel                         = 0x100 | 0x9E,	// 30
		ManufactureID_Atmel                        = 0x100 | 0x1F,	// Bought by Adesto; October 01, 2012
		ManufactureID_SGSThomson                   = 0x100 | 0x20,
		ManufactureID_LatticeSemiconductor         = 0x100 | 0xA1,
		ManufactureID_NCR                          = 0x100 | 0xA2,
		ManufactureID_WafeScaleIntegration         = 0x100 | 0x23,
		ManufactureID_IBM                          = 0x100 | 0xA4,
		ManufactureID_Tristar                      = 0x100 | 0x25,
		ManufactureID_Visic                        = 0x100 | 0x26,
		ManufactureID_InternationalCMOStech        = 0x100 | 0xA7,
		ManufactureID_SSSI                         = 0x100 | 0xA8,	// 40
		ManufactureID_MicrochipTechnology          = 0x100 | 0x29,
		ManufactureID_Ricoh                        = 0x100 | 0x2A,
		ManufactureID_VLSI                         = 0x100 | 0xAB,
		ManufactureID_MicronTechnology             = 0x100 | 0x2C,
		ManufactureID_SK_Hynix                     = 0x100 | 0xAD,
		ManufactureID_OKISemiconductor             = 0x100 | 0xAE,
		ManufactureID_ACTEL                        = 0x100 | 0x2F,
		ManufactureID_Sharp                        = 0x100 | 0xB0,
		ManufactureID_Catalyst                     = 0x100 | 0x31,
		ManufactureID_Panasonic                    = 0x100 | 0x32,	// 50
		ManufactureID_IDT                          = 0x100 | 0xB3,
		ManufactureID_Cypress                      = 0x100 | 0x34,
		ManufactureID_DEC                          = 0x100 | 0xB5,
		ManufactureID_LSILogic                     = 0x100 | 0xB6,
		ManufactureID_Zarlink_Mitel                = 0x100 | 0x37,	// originally Plessey
		ManufactureID_UTMC                         = 0x100 | 0x38,
		ManufactureID_ThinkingMachine              = 0x100 | 0xB9,
		ManufactureID_Thomson_CSF                  = 0x100 | 0xBA,
		ManufactureID_IntegratedCMOS               = 0x100 | 0x3B,	// Vertex
		ManufactureID_Honeywell                    = 0x100 | 0xBC,	// 60
		ManufactureID_Tektronix                    = 0x100 | 0x3D,
		ManufactureID_SunMicrosystem               = 0x100 | 0x3E,
		ManufactureID_SST                          = 0x100 | 0xBF,
		ManufactureID_MOSEL                        = 0x100 | 0x40,
		ManufactureID_Infineon                     = 0x100 | 0xC1,
		ManufactureID_Macronix                     = 0x100 | 0xC2,
		ManufactureID_Xerox                        = 0x100 | 0x43,
		ManufactureID_PlusLogic                    = 0x100 | 0xC4,
		ManufactureID_SunDisk                      = 0x100 | 0x45,
		ManufactureID_ElanCircuitTech              = 0x100 | 0x46,	// 70
		ManufactureID_EuropeanSilicon              = 0x100 | 0xC7,
		ManufactureID_AppleComputer                = 0x100 | 0xC8,
		ManufactureID_Xilinx                       = 0x100 | 0xC9,
		ManufactureID_Compaq                       = 0x100 | 0x4A,
		ManufactureID_ProtocolEngines              = 0x100 | 0xCB,
		ManufactureID_SCI                          = 0x100 | 0x4C,
		ManufactureID_SeikoInstruments             = 0x100 | 0xCD,
		ManufactureID_Samsung                      = 0x100 | 0xCE,
		ManufactureID_I3DesignSystem               = 0x100 | 0x4F,
		ManufactureID_Klic                         = 0x100 | 0xD0,	// 80
		ManufactureID_CrosspointSolutions          = 0x100 | 0x51,
		ManufactureID_AllianceSemiconductor        = 0x100 | 0x52,
		ManufactureID_Tandem                       = 0x100 | 0xD3,
		ManufactureID_HewlettPackard               = 0x100 | 0xD4,
		ManufactureID_IntgSilicon                  = 0x100 | 0xD5,
		ManufactureID_Brooktree                    = 0x100 | 0xD6,
		ManufactureID_NewMedia                     = 0x100 | 0x57,
		ManufactureID_MHSElectronic                = 0x100 | 0x58,
		ManufactureID_PerformanceSemi              = 0x100 | 0xD9,
		ManufactureID_Winbond                      = 0x100 | 0xDA,	// 90
		ManufactureID_KawasakiSteel                = 0x100 | 0x5B,
		ManufactureID_BrightMicro                  = 0x100 | 0xDC,
		ManufactureID_TECMAR                       = 0x100 | 0x5D,
		ManufactureID_Exar                         = 0x100 | 0x5E,
		ManufactureID_PCMCIA                       = 0x100 | 0xDF,
		ManufactureID_LG_Semiconductor             = 0x100 | 0xE0,
		ManufactureID_NorthernTelecom              = 0x100 | 0x61,
		ManufactureID_Sanyo                        = 0x100 | 0x62,
		ManufactureID_ArrayMicrosystems            = 0x100 | 0xE3,
		ManufactureID_CrystalSemiconductor         = 0x100 | 0x64,	// 100
		ManufactureID_AnalogDevices                = 0x100 | 0xE5,
		ManufactureID_PMC_Sierra                   = 0x100 | 0xE6,
		ManufactureID_Asparix                      = 0x100 | 0x67,
		ManufactureID_ConvexComputer               = 0x100 | 0x68,
		ManufactureID_QualitySemiconductor         = 0x100 | 0xE9,
		ManufactureID_NimbusTechnology             = 0x100 | 0xEA,
		ManufactureID_Transwitch                   = 0x100 | 0x6B,
		ManufactureID_Micronas                     = 0x100 | 0xEC,	// (ITT Intermetall)
		ManufactureID_Cannon                       = 0x100 | 0x6D,
		ManufactureID_Altera                       = 0x100 | 0x6E,	// 110
		ManufactureID_Nexcom                       = 0x100 | 0xEF,	// Nexcom was acquired by Winbond
		ManufactureID_QUALCOMM                     = 0x100 | 0x70,
		ManufactureID_Sony                         = 0x100 | 0xF1,
		ManufactureID_CrayResearch                 = 0x100 | 0xF2,
		ManufactureID_AMS                          = 0x100 | 0x73,	// (Austria Micro)
		ManufactureID_Vitesse                      = 0x100 | 0xF4,
		ManufactureID_AsterElectronics             = 0x100 | 0x75,
		ManufactureID_BayNetworks                  = 0x100 | 0x76,	// (Synoptic)
		ManufactureID_ZentrumOrZMD                 = 0x100 | 0xF7,
		ManufactureID_TRW                          = 0x100 | 0xF8,	// 120
		ManufactureID_Thesys                       = 0x100 | 0x79,
		ManufactureID_SolbourneComputer            = 0x100 | 0x7A,
		ManufactureID_AlliedSignal                 = 0x100 | 0xFB,
		ManufactureID_Dialog                       = 0x100 | 0x7C,
		ManufactureID_MediaVision                  = 0x100 | 0xFD,
		ManufactureID_NumonyxCorporation           = 0x100 | 0xFE,

		// The following numbers are all in bank two:
		ManufactureID_CirrusLogic                  = 0x200 | 0x01,
		ManufactureID_NationalInstruments          = 0x200 | 0x02,
		ManufactureID_ILC_DataDevice               = 0x200 | 0x83,
		ManufactureID_AlcatelMietec                = 0x200 | 0x04, 
		ManufactureID_MicroLinear                  = 0x200 | 0x85,
		ManufactureID_UniversityOfNC               = 0x200 | 0x86,
		ManufactureID_JTAG_Technologies            = 0x200 | 0x07,
		ManufactureID_BAE_Systems                  = 0x200 | 0x08,
		ManufactureID_Nchip                        = 0x200 | 0x89,
		ManufactureID_GalileoTech                  = 0x200 | 0x8A,	// 10
		ManufactureID_BestlinkSystems              = 0x200 | 0x0B,
		ManufactureID_Graychip                     = 0x200 | 0x8C,
		ManufactureID_GENNUM                       = 0x200 | 0x0D,
		ManufactureID_VideoLogic                   = 0x200 | 0x0E,
		ManufactureID_RobertBosch                  = 0x200 | 0x8F,
		ManufactureID_ChipExpress                  = 0x200 | 0x10,
		ManufactureID_DATARAM                      = 0x200 | 0x91,
		ManufactureID_UnitedMicroelectronics       = 0x200 | 0x92,
		ManufactureID_TCSI                         = 0x200 | 0x13,
		ManufactureID_SmartModular                 = 0x200 | 0x94,	// 20
		ManufactureID_HughesAircraft               = 0x200 | 0x15,
		ManufactureID_LanstarSemiconductor         = 0x200 | 0x16,
		ManufactureID_Qlogic                       = 0x200 | 0x97,
		ManufactureID_Kingston                     = 0x200 | 0x98,
		ManufactureID_MusicSemi                    = 0x200 | 0x19,
		ManufactureID_EricssonComponents           = 0x200 | 0x1A,
		ManufactureID_SpaSE                        = 0x200 | 0x9B,
		ManufactureID_Eon_SiliconDevices           = 0x200 | 0x1C,
		ManufactureID_ProgrammableMicro            = 0x200 | 0x9D,
		ManufactureID_DoD                          = 0x200 | 0x9E,	// 30
		ManufactureID_IntegratedMemories           = 0x200 | 0x1F,
		ManufactureID_CorollaryInc                 = 0x200 | 0x20,
		ManufactureID_DallasSemiconductor          = 0x200 | 0xA1,
		ManufactureID_Omnivision                   = 0x200 | 0xA2,
		ManufactureID_EIV                          = 0x200 | 0x23,
		ManufactureID_NovatelWireless              = 0x200 | 0xA4,
		ManufactureID_Zarlink                      = 0x200 | 0x25,
		ManufactureID_Clearpoint                   = 0x200 | 0x26,
		ManufactureID_Cabletron                    = 0x200 | 0xA7,
		ManufactureID_STEC                         = 0x200 | 0xA8,	// 40
		ManufactureID_Vanguard                     = 0x200 | 0x29,
		ManufactureID_HagiwaraSysCom               = 0x200 | 0x2A,
		ManufactureID_Vantis                       = 0x200 | 0xAB,
		ManufactureID_Celestica                    = 0x200 | 0x2C,
		ManufactureID_Century                      = 0x200 | 0xAD,
		ManufactureID_HalComputers                 = 0x200 | 0xAE,
		ManufactureID_RohmCompany                  = 0x200 | 0x2F,
		ManufactureID_JuniperNetworks              = 0x200 | 0xB0,
		ManufactureID_LibitSignalProcessing        = 0x200 | 0x31,
		ManufactureID_MushkinEnhancedMemory        = 0x200 | 0x32,	// 50
		ManufactureID_TundraSemiconductor          = 0x200 | 0xB3,
		ManufactureID_Adaptec                      = 0x200 | 0x34,
		ManufactureID_LightSpeedSemi               = 0x200 | 0xB5,
		ManufactureID_ZSP_Corp                     = 0x200 | 0xB6,
		ManufactureID_AMIC_Technology              = 0x200 | 0x37,
		ManufactureID_AdobeSystems                 = 0x200 | 0x38,
		ManufactureID_Dynachip                     = 0x200 | 0xB9,
		ManufactureID_PNY_Electronics              = 0x200 | 0xBA,
		ManufactureID_NewportDigital               = 0x200 | 0x3B,
		ManufactureID_MMC_Networks                 = 0x200 | 0xBC,	// 60
		ManufactureID_T_Square                     = 0x200 | 0x3D,
		ManufactureID_SeikoEpson                   = 0x200 | 0x3E,
		ManufactureID_Broadcom                     = 0x200 | 0xBF,
		ManufactureID_VikingComponents             = 0x200 | 0x40,
		ManufactureID_V3_Semiconductor             = 0x200 | 0xC1,
		ManufactureID_Flextronics                  = 0x200 | 0xC2,
		ManufactureID_SuwaElectronics              = 0x200 | 0x43,
		ManufactureID_Transmeta                    = 0x200 | 0xC4,
		ManufactureID_Micron_CMS                   = 0x200 | 0x45,
		ManufactureID_AmericanComputerDigital      = 0x200 | 0x46,	// 70
		ManufactureID_Enhance3000Inc               = 0x200 | 0xC7,
		ManufactureID_TowerSemiconductor           = 0x200 | 0xC8,
		ManufactureID_CPU_Design                   = 0x200 | 0xC9,
		ManufactureID_PricePoint                   = 0x200 | 0x4A,
		ManufactureID_MaximIntegratedProduct       = 0x200 | 0xCB,
		ManufactureID_Tellabs                      = 0x200 | 0x4C,
		ManufactureID_CentaurTechnology            = 0x200 | 0xCD,
		ManufactureID_UnigenCorporation            = 0x200 | 0xCE,
		ManufactureID_TranscendInformation         = 0x200 | 0x4F,
		ManufactureID_MemoryCardTechnology         = 0x200 | 0xD0,
		ManufactureID_CKDCorporationLtd            = 0x200 | 0x51,
		ManufactureID_CapitalInstruments           = 0x200 | 0x52,
		ManufactureID_AicaKogyo                    = 0x200 | 0xD3,
		ManufactureID_LinvexTechnology             = 0x200 | 0xD4,
		ManufactureID_MSCVertriebs                 = 0x200 | 0xD5,
		ManufactureID_AKM_Company                  = 0x200 | 0xD6,
		ManufactureID_Dynamem                      = 0x200 | 0x57,
		ManufactureID_NERA_ASA                     = 0x200 | 0x58,
		ManufactureID_GSI_Technology               = 0x200 | 0xD9,
		ManufactureID_Dane_Elec                    = 0x200 | 0xDA,
		ManufactureID_AcornComputers               = 0x200 | 0x5B,
		ManufactureID_LaraTechnology               = 0x200 | 0xDC,
		ManufactureID_OakTechnology                = 0x200 | 0x5D,
		ManufactureID_ItecMemory                   = 0x200 | 0x5E,
		ManufactureID_TanisysTechnology            = 0x200 | 0xDF,
		ManufactureID_Truevision                   = 0x200 | 0xE0,
		ManufactureID_WintecIndustries             = 0x200 | 0x61,
		ManufactureID_Super_PC_Memory              = 0x200 | 0x62,
		ManufactureID_MGV_Memory                   = 0x200 | 0xE3,
		ManufactureID_Galvantech                   = 0x200 | 0x64,	// 100
		ManufactureID_GadzooxNetworks              = 0x200 | 0xE5,
		ManufactureID_MultiDimensional             = 0x200 | 0xE6,
		ManufactureID_GateField                    = 0x200 | 0x67,
		ManufactureID_IntegratedMemorySystem       = 0x200 | 0x68,
		ManufactureID_Triscend                     = 0x200 | 0xE9,
		ManufactureID_XaQti                        = 0x200 | 0xEA,
		ManufactureID_Goldenram                    = 0x200 | 0x6B,
		ManufactureID_ClearLogic                   = 0x200 | 0xEC,
		ManufactureID_CimaronCommunications        = 0x200 | 0x6D,
		ManufactureID_NipponSteelSemi              = 0x200 | 0x6E,	// 110
		ManufactureID_AdvantageMemory              = 0x200 | 0xEF,
		ManufactureID_AMCC                         = 0x200 | 0x70,
		ManufactureID_LeCroy                       = 0x200 | 0xF1,
		ManufactureID_YamahaCorporation            = 0x200 | 0xF2,
		ManufactureID_DigitalMicrowave             = 0x200 | 0x73,
		ManufactureID_NetLogicMicrosystems         = 0x200 | 0xF4,
		ManufactureID_MIMOSSemiconductor           = 0x200 | 0x75,
		ManufactureID_AdvancedFibre                = 0x200 | 0x76,
		ManufactureID_BF_GoodrichData              = 0x200 | 0xF7,
		ManufactureID_Epigram                      = 0x200 | 0xF8,	// 120
		ManufactureID_AcbelPolytech                = 0x200 | 0x79,
		ManufactureID_ApacerTechnology             = 0x200 | 0x7A,
		ManufactureID_AdmorMemory                  = 0x200 | 0xFB,
		ManufactureID_FOXCONN                      = 0x200 | 0x7C,
		ManufactureID_QuadraticsSuperconductor     = 0x200 | 0xFD,
		ManufactureID_3COM                         = 0x200 | 0xFE,

		// The following numbers are all in bank three:
		ManufactureID_CamintonnCorporation         = 0x300 | 0x01,
		ManufactureID_ISOA_Incorporated            = 0x300 | 0x02,
		ManufactureID_AgateSemiconductor           = 0x300 | 0x83,
		ManufactureID_ADMtekIncorporated           = 0x300 | 0x04, 
		ManufactureID_HYPERTEC                     = 0x300 | 0x85,
		ManufactureID_AdhocTechnologies            = 0x300 | 0x86,
		ManufactureID_MOSAIDTechnologies           = 0x300 | 0x07,
		ManufactureID_ArdentTechnologies           = 0x300 | 0x08,
		ManufactureID_Switchcore                   = 0x300 | 0x89,
		ManufactureID_CiscoSystems                 = 0x300 | 0x8A,	// 10
		ManufactureID_AllayerTechnologies          = 0x300 | 0x0B,
		ManufactureID_WorkX                        = 0x300 | 0x8C,
		ManufactureID_OasisSemiconductor           = 0x300 | 0x0D,
		ManufactureID_NovanetSemiconductor         = 0x300 | 0x0E,
		ManufactureID_EM_Solutions                 = 0x300 | 0x8F,
		ManufactureID_PowerGeneral                 = 0x300 | 0x10,
		ManufactureID_AdvancedHardware             = 0x300 | 0x91,
		ManufactureID_InovaSemiconductors          = 0x300 | 0x92,
		ManufactureID_Telocity                     = 0x300 | 0x13,
		ManufactureID_DelkinDevices                = 0x300 | 0x94,	// 20
		ManufactureID_SymageryMicrosystems         = 0x300 | 0x15,
		ManufactureID_C_PortCorporation            = 0x300 | 0x16,
		ManufactureID_SiberCoreTechnologies        = 0x300 | 0x97,
		ManufactureID_SouthlandMicrosystems        = 0x300 | 0x98,
		ManufactureID_MalleableTechnologies        = 0x300 | 0x19,
		ManufactureID_KendinCommunications         = 0x300 | 0x1A,
		ManufactureID_GreatTechnologyMicrocomputer = 0x300 | 0x9B,
		ManufactureID_SanminaCorporation           = 0x300 | 0x1C,
		ManufactureID_HADCOCorporation             = 0x300 | 0x9D,
		ManufactureID_Corsair                      = 0x300 | 0x9E,	// 30
		ManufactureID_ActransSystem                = 0x300 | 0x1F,
		ManufactureID_ALPHA_Technologies           = 0x300 | 0x20,
		ManufactureID_SiliconLaboratories          = 0x300 | 0xA1,
		ManufactureID_ArtesynTechnologies          = 0x300 | 0xA2,
		ManufactureID_AlignManufacturing           = 0x300 | 0x23,
		ManufactureID_PeregrineSemiconductor       = 0x300 | 0xA4,
		ManufactureID_ChameleonSystems             = 0x300 | 0x25,
		ManufactureID_AplusFlashTechnology         = 0x300 | 0x26,
		ManufactureID_MIPS_Technologies            = 0x300 | 0xA7,
		ManufactureID_ChrysalisITS                 = 0x300 | 0xA8,	// 40
		ManufactureID_ADTEC_Corporation            = 0x300 | 0x29,
		ManufactureID_KentronTechnologies          = 0x300 | 0x2A,
		ManufactureID_WinTechnologies              = 0x300 | 0xAB,
		ManufactureID_TezzaronSemiconductor        = 0x300 | 0x2C,
		ManufactureID_ExtremePacketDevices         = 0x300 | 0xAD,
		ManufactureID_RF_MicroDevices              = 0x300 | 0xAE,
		ManufactureID_Siemens                      = 0x300 | 0x2F,
		ManufactureID_SarnoffCorporation           = 0x300 | 0xB0,
		ManufactureID_Itautec_SA                   = 0x300 | 0x31,
		ManufactureID_Radiata                      = 0x300 | 0x32,	// 50
		ManufactureID_BenchmarkElectric            = 0x300 | 0xB3,
		ManufactureID_Legend                       = 0x300 | 0x34,
		ManufactureID_SpecTekIncorporated          = 0x300 | 0xB5,
		ManufactureID_Hi_fn                        = 0x300 | 0xB6,
		ManufactureID_EnikiaIncorporated           = 0x300 | 0x37,
		ManufactureID_SwitchOnNetworks             = 0x300 | 0x38,
		ManufactureID_AANetcomIncorporated         = 0x300 | 0xB9,
		ManufactureID_MicroMemoryBank              = 0x300 | 0xBA,
		ManufactureID_ESS_Technology               = 0x300 | 0x3B,
		ManufactureID_VirataCorporation            = 0x300 | 0xBC,	// 60
		ManufactureID_ExcessBandwidth              = 0x300 | 0x3D,
		ManufactureID_WestBaySemiconductor         = 0x300 | 0x3E,
		ManufactureID_DSPGroup                     = 0x300 | 0xBF,
		ManufactureID_NewportCommunications        = 0x300 | 0x40,
		ManufactureID_Chip2ChipIncorporated        = 0x300 | 0xC1,
		ManufactureID_PhobosCorporation            = 0x300 | 0xC2,
		ManufactureID_IntellitechCorporation       = 0x300 | 0x43,
		ManufactureID_Nordic_VLSI_ASA              = 0x300 | 0xC4,
		ManufactureID_IshoniNetworks               = 0x300 | 0x45,
		ManufactureID_SiliconSpice                 = 0x300 | 0x46,	// 70
		ManufactureID_AlchemySemiconductor         = 0x300 | 0xC7,
		ManufactureID_AgilentTechnologies          = 0x300 | 0xC8,
		ManufactureID_CentilliumCommunications     = 0x300 | 0xC9,
		ManufactureID_W_L_Gore                     = 0x300 | 0x4A,
		ManufactureID_HanBitElectronics            = 0x300 | 0xCB,
		ManufactureID_GlobeSpan                    = 0x300 | 0x4C,
		ManufactureID_Element_14                   = 0x300 | 0xCD,
		ManufactureID_Pycon                        = 0x300 | 0xCE,
		ManufactureID_SaifunSemiconductors         = 0x300 | 0x4F,
		ManufactureID_SibyteIncorporated           = 0x300 | 0xD0,	// 80
		ManufactureID_MetaLinkTechnologies         = 0x300 | 0x51,
		ManufactureID_FeiyaTechnology              = 0x300 | 0x52,
		ManufactureID_I_C_Technology               = 0x300 | 0xD3,
		ManufactureID_Shikatronics                 = 0x300 | 0xD4,
		ManufactureID_Elektrobit                   = 0x300 | 0xD5,
		ManufactureID_Megic                        = 0x300 | 0xD6,
		ManufactureID_Com_Tier                     = 0x300 | 0x57,
		ManufactureID_MalaysiaMicroSolutions       = 0x300 | 0x58,
		ManufactureID_Hyperchip                    = 0x300 | 0xD9,
		ManufactureID_GemstoneCommunications       = 0x300 | 0xDA,	// 90
		ManufactureID_Anadigm                      = 0x300 | 0x5B,
		ManufactureID_3ParData                     = 0x300 | 0xDC,
		ManufactureID_MellanoxTechnologies         = 0x300 | 0x5D,
		ManufactureID_TenxTechnologies             = 0x300 | 0x5E,
		ManufactureID_Helix                        = 0x300 | 0xDF,
		ManufactureID_Domosys                      = 0x300 | 0xE0,
		ManufactureID_SkyupTechnology              = 0x300 | 0x61,
		ManufactureID_HiNT_Corporation             = 0x300 | 0x62,
		ManufactureID_Chiaro                       = 0x300 | 0xE3,
		ManufactureID_MDT_Technologies             = 0x300 | 0x64,	// 100
		ManufactureID_ExbitTechnology              = 0x300 | 0xE5,
		ManufactureID_IntegratedTechnologyExpress  = 0x300 | 0xE6,
		ManufactureID_AVED_Memory                  = 0x300 | 0x67,
		ManufactureID_Legerity                     = 0x300 | 0x68,
		ManufactureID_JasmineNetworks              = 0x300 | 0xE9,
		ManufactureID_CaspianNetworks              = 0x300 | 0xEA,
		ManufactureID_nCUBE                        = 0x300 | 0x6B,
		ManufactureID_SiliconAccessNetworks        = 0x300 | 0xEC,
		ManufactureID_FDK_Corporation              = 0x300 | 0x6D,
		ManufactureID_HighBandwidthAccess          = 0x300 | 0x6E,	// 110
		ManufactureID_MultiLinkTechnology          = 0x300 | 0xEF,
		ManufactureID_BRECIS                       = 0x300 | 0x70,
		ManufactureID_WorldWidePackets             = 0x300 | 0xF1,
		ManufactureID_APW                          = 0x300 | 0xF2,
		ManufactureID_ChicorySystems               = 0x300 | 0x73,
		ManufactureID_XstreamLogic                 = 0x300 | 0xF4,
		ManufactureID_FastChip                     = 0x300 | 0x75,
		ManufactureID_ZucottoWireless              = 0x300 | 0x76,
		ManufactureID_Realchip                     = 0x300 | 0xF7,
		ManufactureID_GalaxyPower                  = 0x300 | 0xF8,	// 120
		ManufactureID_eSilicon                     = 0x300 | 0x79,
		ManufactureID_MorphicsTechnology           = 0x300 | 0x7A,
		ManufactureID_AccelerantNetworks           = 0x300 | 0xFB,
		ManufactureID_SiliconWave                  = 0x300 | 0x7C,
		ManufactureID_SandCraft                    = 0x300 | 0xFD,
		ManufactureID_Elpida                       = 0x300 | 0xFE,

		// The following numbers are all in bank four:
		ManufactureID_Solectron                    = 0x400 | 0x01,
		ManufactureID_OptosysTechnologies          = 0x400 | 0x02,
		ManufactureID_Buffalo                      = 0x400 | 0x83,	// (Formerly Melco)
		ManufactureID_TriMediaTechnologies         = 0x400 | 0x04, 
		ManufactureID_CyanTechnologies             = 0x400 | 0x85,
		ManufactureID_GlobalLocate                 = 0x400 | 0x86,
		ManufactureID_Optillion                    = 0x400 | 0x07,
		ManufactureID_TeragoCommunications         = 0x400 | 0x08,
		ManufactureID_IkanosCommunications         = 0x400 | 0x89,
		ManufactureID_PrincetonTechnology          = 0x400 | 0x8A,	// 10
		ManufactureID_NanyaTechnology              = 0x400 | 0x0B,
		ManufactureID_EliteFlashStorage            = 0x400 | 0x8C,
		ManufactureID_Mysticom                     = 0x400 | 0x0D,
		ManufactureID_LightSandCommunications      = 0x400 | 0x0E,
		ManufactureID_ATI_Technologies             = 0x400 | 0x8F,
		ManufactureID_AgereSystems                 = 0x400 | 0x10,
		ManufactureID_NeoMagic                     = 0x400 | 0x91,
		ManufactureID_AuroraNetics                 = 0x400 | 0x92,
		ManufactureID_GoldenEmpire                 = 0x400 | 0x13,
		ManufactureID_Mushkin                      = 0x400 | 0x94,	// 20
		ManufactureID_TiogaTechnologies            = 0x400 | 0x15,
		ManufactureID_Netlist                      = 0x400 | 0x16,
		ManufactureID_TeraLogic                    = 0x400 | 0x97,
		ManufactureID_CicadaSemiconductor          = 0x400 | 0x98,
		ManufactureID_CentonElectronics            = 0x400 | 0x19,
		ManufactureID_TycoElectronics              = 0x400 | 0x1A,
		ManufactureID_MagisWorks                   = 0x400 | 0x9B,
		ManufactureID_Zettacom                     = 0x400 | 0x1C,
		ManufactureID_CogencySemiconductor         = 0x400 | 0x9D,
		ManufactureID_Chipcon_AS                   = 0x400 | 0x9E,	// 30
		ManufactureID_AspexTechnology              = 0x400 | 0x1F,
		ManufactureID_F5_Networks                  = 0x400 | 0x20,
		ManufactureID_ProgrammableSiliconSolutions = 0x400 | 0xA1,
		ManufactureID_ChipWrights                  = 0x400 | 0xA2,
		ManufactureID_AcornNetworks                = 0x400 | 0x23,
		ManufactureID_Quicklogic                   = 0x400 | 0xA4,
		ManufactureID_KingmaxSemiconductor         = 0x400 | 0x25,
		ManufactureID_BOPS                         = 0x400 | 0x26,
		ManufactureID_Flasys                       = 0x400 | 0xA7,
		ManufactureID_BitBlitzCommunications       = 0x400 | 0xA8,	// 40
		ManufactureID_eMemoryTechnology            = 0x400 | 0x29,
		ManufactureID_ProcketNetworks              = 0x400 | 0x2A,
		ManufactureID_PurpleRay                    = 0x400 | 0xAB,
		ManufactureID_TrebiaNetworks               = 0x400 | 0x2C,
		ManufactureID_DeltaElectronics             = 0x400 | 0xAD,
		ManufactureID_OnexCommunications           = 0x400 | 0xAE,
		ManufactureID_AmpleCommunications          = 0x400 | 0x2F,
		ManufactureID_MemoryExpertsIntl            = 0x400 | 0xB0,
		ManufactureID_AstuteNetworks               = 0x400 | 0x31,
		ManufactureID_AzandaNetworkDevices         = 0x400 | 0x32,	// 50
		ManufactureID_Dibcom                       = 0x400 | 0xB3,
		ManufactureID_Tekmos                       = 0x400 | 0x34,
		ManufactureID_API_NetWorks                 = 0x400 | 0xB5,
		ManufactureID_BayMicrosystems              = 0x400 | 0xB6,
		ManufactureID_Firecron                     = 0x400 | 0x37,
		ManufactureID_ResonextCommunications       = 0x400 | 0x38,
		ManufactureID_TachysTechnologies           = 0x400 | 0xB9,
		ManufactureID_EquatorTechnology            = 0x400 | 0xBA,
		ManufactureID_ConceptComputer              = 0x400 | 0x3B,
		ManufactureID_SILCOM                       = 0x400 | 0xBC,	// 60
		ManufactureID_3Dlabs                       = 0x400 | 0x3D,
		ManufactureID_c_t_Magazine                 = 0x400 | 0x3E,
		ManufactureID_SaneraSystems                = 0x400 | 0xBF,
		ManufactureID_SiliconPackets               = 0x400 | 0x40,
		ManufactureID_ViasystemsGroup              = 0x400 | 0xC1,
		ManufactureID_Simtek                       = 0x400 | 0xC2,
		ManufactureID_SemiconDevicesSingapore      = 0x400 | 0x43,
		ManufactureID_SatronHandelsges             = 0x400 | 0xC4,
		ManufactureID_ImprovSystems                = 0x400 | 0x45,
		ManufactureID_INDUSYS                      = 0x400 | 0x46,	// 70
		ManufactureID_Corrent                      = 0x400 | 0xC7,
		ManufactureID_InfrantTechnologies          = 0x400 | 0xC8,
		ManufactureID_RitekCorp                    = 0x400 | 0xC9,
		ManufactureID_empowerTelNetworks           = 0x400 | 0x4A,
		ManufactureID_Hypertec                     = 0x400 | 0xCB,
		ManufactureID_CaviumNetworks               = 0x400 | 0x4C,
		ManufactureID_PLX_Technology               = 0x400 | 0xCD,
		ManufactureID_MassanaDesign                = 0x400 | 0xCE,
		ManufactureID_Intrinsity                   = 0x400 | 0x4F,
		ManufactureID_ValenceSemiconductor         = 0x400 | 0xD0,	// 80
		ManufactureID_TerawaveCommunications       = 0x400 | 0x51,
		ManufactureID_IceFyreSemiconductor         = 0x400 | 0x52,
		ManufactureID_Primarion                    = 0x400 | 0xD3,
		ManufactureID_PicochipDesigns              = 0x400 | 0xD4,
		ManufactureID_SilverbackSystems            = 0x400 | 0xD5,
		ManufactureID_JadeStarTechnologies         = 0x400 | 0xD6,
		ManufactureID_PijnenburgSecurealink        = 0x400 | 0x57,
		ManufactureID_takeMS_Ultron                = 0x400 | 0x58,
		ManufactureID_CambridgeSiliconRadio        = 0x400 | 0xD9,
		ManufactureID_Swissbit                     = 0x400 | 0xDA,	// 90
		ManufactureID_NazomiCommunications         = 0x400 | 0x5B,
		ManufactureID_eWaveSystem                  = 0x400 | 0xDC,
		ManufactureID_RockwellCollins              = 0x400 | 0x5D,
		ManufactureID_Picocel                      = 0x400 | 0x5E,
		ManufactureID_Alphamosaic                  = 0x400 | 0xDF,
		ManufactureID_Sandburst                    = 0x400 | 0xE0,
		ManufactureID_SiConVideo                   = 0x400 | 0x61,
		ManufactureID_NanoAmpSolutions             = 0x400 | 0x62,
		ManufactureID_EricssonTechnology           = 0x400 | 0xE3,
		ManufactureID_PrairieComm                  = 0x400 | 0x64,	// 100
		ManufactureID_MitacInternational           = 0x400 | 0xE5,
		ManufactureID_Layer_N_Networks             = 0x400 | 0xE6,
		ManufactureID_MtekVision                   = 0x400 | 0x67,
		ManufactureID_AllegroNetworks              = 0x400 | 0x68,
		ManufactureID_MarvellSemiconductors        = 0x400 | 0xE9,
		ManufactureID_NetergyMicroelectronic       = 0x400 | 0xEA,
		ManufactureID_NVIDIA                       = 0x400 | 0x6B,
		ManufactureID_InternetMachines             = 0x400 | 0xEC,
		ManufactureID_PeakElectronics              = 0x400 | 0x6D,
		ManufactureID_LitchfieldCommunication      = 0x400 | 0x6E,	// 110
		ManufactureID_AcctonTechnology             = 0x400 | 0xEF,
		ManufactureID_TeradiantNetworks            = 0x400 | 0x70,
		ManufactureID_ScaleoChip                   = 0x400 | 0xF1,
		ManufactureID_CortinaSystems               = 0x400 | 0xF2,
		ManufactureID_RAM_Components               = 0x400 | 0x73,
		ManufactureID_RaqiaNetworks                = 0x400 | 0xF4,
		ManufactureID_ClearSpeed                   = 0x400 | 0x75,
		ManufactureID_MatsushitaBattery            = 0x400 | 0x76,
		ManufactureID_Xelerated                    = 0x400 | 0xF7,
		ManufactureID_SimpleTech                   = 0x400 | 0xF8,	// 120
		ManufactureID_UtronTechnology              = 0x400 | 0x79,
		ManufactureID_AstecInternational           = 0x400 | 0x7A,
		ManufactureID_AVM                          = 0x400 | 0xFB,
		ManufactureID_ReduxCommunications          = 0x400 | 0x7C,
		ManufactureID_DotHillSystems               = 0x400 | 0xFD,
		ManufactureID_TeraChip                     = 0x400 | 0xFE,

		// The following numbers are all in bank five:
		ManufactureID_T_RAM_Incorporated           = 0x500 | 0x01,
		ManufactureID_InnovicsWireless             = 0x500 | 0x02,
		ManufactureID_Teknovus                     = 0x500 | 0x83,
		ManufactureID_KeyEyeCommunications         = 0x500 | 0x04, 
		ManufactureID_RuncomTechnologie            = 0x500 | 0x85,
		ManufactureID_RedSwitch                    = 0x500 | 0x86,
		ManufactureID_Dotcast                      = 0x500 | 0x07,
		ManufactureID_SiliconMountainMemory        = 0x500 | 0x08,
		ManufactureID_SigniaTechnologies           = 0x500 | 0x89,
		ManufactureID_Pixim                        = 0x500 | 0x8A,	// 10
		ManufactureID_GalazarNetworks              = 0x500 | 0x0B,
		ManufactureID_WhiteElectronicDesigns       = 0x500 | 0x8C,
		ManufactureID_PatriotScientific            = 0x500 | 0x0D,
		ManufactureID_NeoaxiomCorporation          = 0x500 | 0x0E,
		ManufactureID_3Y_PowerTechnology           = 0x500 | 0x8F,
		ManufactureID_ScaleoChip2                  = 0x500 | 0x10,
		ManufactureID_PotentiaPowerSystems         = 0x500 | 0x91,
		ManufactureID_C_guysIncorporated           = 0x500 | 0x92,
		ManufactureID_DigitalCommunications_TI     = 0x500 | 0x13,
		ManufactureID_SiliconBasedTechnology       = 0x500 | 0x94,	// 20
		ManufactureID_FulcrumMicrosystems          = 0x500 | 0x15,
		ManufactureID_PositivoInformatica          = 0x500 | 0x16,
		ManufactureID_XIOtechCorporation           = 0x500 | 0x97,
		ManufactureID_PortalPlayer                 = 0x500 | 0x98,
		ManufactureID_ZhiyingSoftware              = 0x500 | 0x19,
		ManufactureID_ParkerVision                 = 0x500 | 0x1A,
		ManufactureID_PhonexBroadband              = 0x500 | 0x9B,
		ManufactureID_SkyworksSolutions            = 0x500 | 0x1C,
		ManufactureID_EntropicCommunications       = 0x500 | 0x9D,
		ManufactureID_IM_IntelligentMemory         = 0x500 | 0x9E,	// 30
		ManufactureID_Zensys_AS                    = 0x500 | 0x1F,
		ManufactureID_LegendSiliconCorporation     = 0x500 | 0x20,
		ManufactureID_Sci_worx                     = 0x500 | 0xA1,
		ManufactureID_SMSC                         = 0x500 | 0xA2,	// (Standard Microsystems)
		ManufactureID_RenesasElectronics           = 0x500 | 0x23,
		ManufactureID_RazaMicroelectronics         = 0x500 | 0xA4,
		ManufactureID_Phyworks                     = 0x500 | 0x25,
		ManufactureID_MediaTek                     = 0x500 | 0x26,
		ManufactureID_Non_cents_Productions        = 0x500 | 0xA7,
		ManufactureID_US_Modular                   = 0x500 | 0xA8,	// 40
		ManufactureID_Wintegra                     = 0x500 | 0x29,
		ManufactureID_Mathstar                     = 0x500 | 0x2A,
		ManufactureID_StarCore                     = 0x500 | 0xAB,
		ManufactureID_OplusTechnologies            = 0x500 | 0x2C,
		ManufactureID_Mindspeed                    = 0x500 | 0xAD,
		ManufactureID_JustYoungComputer            = 0x500 | 0xAE,
		ManufactureID_RadiaCommunications          = 0x500 | 0x2F,
		ManufactureID_OCZ                          = 0x500 | 0xB0,
		ManufactureID_Emuzed                       = 0x500 | 0x31,
		ManufactureID_LOGIC_Devices                = 0x500 | 0x32,	// 50
		ManufactureID_InphiCorporation             = 0x500 | 0xB3,
		ManufactureID_QuakeTechnologies            = 0x500 | 0x34,
		ManufactureID_Vixel                        = 0x500 | 0xB5,
		ManufactureID_SolusTek                     = 0x500 | 0xB6,
		ManufactureID_KongsbergMaritime            = 0x500 | 0x37,
		ManufactureID_FaradayTechnology            = 0x500 | 0x38,
		ManufactureID_Altium                       = 0x500 | 0xB9,
		ManufactureID_Insyte                       = 0x500 | 0xBA,
		ManufactureID_ARM                          = 0x500 | 0x3B,
		ManufactureID_DigiVision                   = 0x500 | 0xBC,	// 60
		ManufactureID_VativTechnologies            = 0x500 | 0x3D,
		ManufactureID_EndicottInterconnect         = 0x500 | 0x3E,
		ManufactureID_Pericom                      = 0x500 | 0xBF,
		ManufactureID_Bandspeed                    = 0x500 | 0x40,
		ManufactureID_LeWizCommunications          = 0x500 | 0xC1,
		ManufactureID_CPU_Technology               = 0x500 | 0xC2,
		ManufactureID_RamaxelTechnology            = 0x500 | 0x43,
		ManufactureID_DSP_Group                    = 0x500 | 0xC4,
		ManufactureID_AxisCommunications           = 0x500 | 0x45,
		ManufactureID_LegacyElectronics            = 0x500 | 0x46,	// 70
		ManufactureID_Chrontel                     = 0x500 | 0xC7,
		ManufactureID_PowerchipSemiconductor       = 0x500 | 0xC8,
		ManufactureID_MobilEyeTechnologies         = 0x500 | 0xC9,
		ManufactureID_ExcelSemiconductor           = 0x500 | 0x4A,
		ManufactureID_A_DATA_Technology            = 0x500 | 0xCB,
		ManufactureID_VirtualDigm                  = 0x500 | 0x4C,
		ManufactureID_G_Skill_Intl                 = 0x500 | 0xCD,
		ManufactureID_QuantaComputer               = 0x500 | 0xCE,
		ManufactureID_YieldMicroelectronics        = 0x500 | 0x4F,
		ManufactureID_AfaTechnologies              = 0x500 | 0xD0,	// 80
		ManufactureID_KINGBOX_Technology           = 0x500 | 0x51,
		ManufactureID_Ceva                         = 0x500 | 0x52,
		ManufactureID_iStorNetworks                = 0x500 | 0xD3,
		ManufactureID_AdvanceModules               = 0x500 | 0xD4,
		ManufactureID_Microsoft                    = 0x500 | 0xD5,
		ManufactureID_Open_Silicon                 = 0x500 | 0xD6,
		ManufactureID_GoalSemiconductor            = 0x500 | 0x57,
		ManufactureID_ARC_International            = 0x500 | 0x58,
		ManufactureID_Simmtec                      = 0x500 | 0xD9,
		ManufactureID_Metanoia                     = 0x500 | 0xDA,	// 90
		ManufactureID_KeyStream                    = 0x500 | 0x5B,
		ManufactureID_LowranceElectronics          = 0x500 | 0xDC,
		ManufactureID_Adimos                       = 0x500 | 0x5D,
		ManufactureID_SiGeSemiconductor            = 0x500 | 0x5E,
		ManufactureID_FodusCommunications          = 0x500 | 0xDF,
		ManufactureID_CredenceSystemsCorporation   = 0x500 | 0xE0,
		ManufactureID_GenesisMicrochip             = 0x500 | 0x61,
		ManufactureID_Vihana                       = 0x500 | 0x62,
		ManufactureID_WIS_Technologies             = 0x500 | 0xE3,
		ManufactureID_GateChangeTechnologies       = 0x500 | 0x64,	// 100
		ManufactureID_HighDensityDevices           = 0x500 | 0xE5,
		ManufactureID_Synopsys                     = 0x500 | 0xE6,
		ManufactureID_Gigaram                      = 0x500 | 0x67,
		ManufactureID_EnigmaSemiconductor          = 0x500 | 0x68,
		ManufactureID_CenturyMicro                 = 0x500 | 0xE9,
		ManufactureID_IceraSemiconductor           = 0x500 | 0xEA,
		ManufactureID_MediaworksIntegratedSystems  = 0x500 | 0x6B,
		ManufactureID_ONeilProductDevelopment      = 0x500 | 0xEC,
		ManufactureID_SupremeTopTechnology         = 0x500 | 0x6D,
		ManufactureID_MicroDisplayCorporation      = 0x500 | 0x6E,	// 110
		ManufactureID_TeamGroup                    = 0x500 | 0xEF,
		ManufactureID_SinettCorporation            = 0x500 | 0x70,
		ManufactureID_ToshibaCorporation           = 0x500 | 0xF1,
		ManufactureID_Tensilica                    = 0x500 | 0xF2,
		ManufactureID_SiRFTechnology               = 0x500 | 0x73,
		ManufactureID_Bacoc                        = 0x500 | 0xF4,
		ManufactureID_SMaL_CameraTechnologies      = 0x500 | 0x75,
		ManufactureID_ThomsonSC                    = 0x500 | 0x76,
		ManufactureID_AirgoNetworks                = 0x500 | 0xF7,
		ManufactureID_Wisair                       = 0x500 | 0xF8,	// 120
		ManufactureID_SigmaTel                     = 0x500 | 0x79,
		ManufactureID_Arkados                      = 0x500 | 0x7A,
		ManufactureID_Compete                      = 0x500 | 0xFB,
		ManufactureID_EudarTechnology              = 0x500 | 0x7C,
		ManufactureID_FocusEnhancements            = 0x500 | 0xFD,
		ManufactureID_Xyratex                      = 0x500 | 0xFE,

		// The following numbers are all in bank six:
		ManufactureID_SpecularNetworks             = 0x600 | 0x01,
		ManufactureID_PatriotMemory                = 0x600 | 0x02,
		ManufactureID_U_ChipTechnologyCorporation  = 0x600 | 0x83,
		ManufactureID_SiliconOptix                 = 0x600 | 0x04, 
		ManufactureID_GreenfieldNetworks           = 0x600 | 0x85,
		ManufactureID_CompuRAM                     = 0x600 | 0x86,
		ManufactureID_Stargen                      = 0x600 | 0x07,
		ManufactureID_NetCellCorporation           = 0x600 | 0x08,
		ManufactureID_ExcalibrusTechnologies       = 0x600 | 0x89,
		ManufactureID_SCM_Microsystems             = 0x600 | 0x8A,	// 10
		ManufactureID_XsigoSystems                 = 0x600 | 0x0B,
		ManufactureID_CHIPSandSystems              = 0x600 | 0x8C,
		ManufactureID_Tier1_MultichipSolutions     = 0x600 | 0x0D,
		ManufactureID_CWRL_Labs                    = 0x600 | 0x0E,
		ManufactureID_Teradici                     = 0x600 | 0x8F,
		ManufactureID_Gigaram2                     = 0x600 | 0x10,
		ManufactureID_g2_Microsystems              = 0x600 | 0x91,
		ManufactureID_PowerFlashSemiconductor      = 0x600 | 0x92,
		ManufactureID_PA_Semi                      = 0x600 | 0x13,
		ManufactureID_NovaTechSolutions            = 0x600 | 0x94,	// 20
		ManufactureID_c2Microsystems               = 0x600 | 0x15,
		ManufactureID_Level5Networks               = 0x600 | 0x16,
		ManufactureID_COS_Memory                   = 0x600 | 0x97,
		ManufactureID_InnovasicSemiconductor       = 0x600 | 0x98,
		ManufactureID_02IC_Co                      = 0x600 | 0x19,
		ManufactureID_Tabula                       = 0x600 | 0x1A,
		ManufactureID_CrucialTechnology            = 0x600 | 0x9B,
		ManufactureID_ChelsioCommunications        = 0x600 | 0x1C,
		ManufactureID_SolarflareCommunications     = 0x600 | 0x9D,
		ManufactureID_Xambala                      = 0x600 | 0x9E,	// 30
		ManufactureID_EADS_Astrium                 = 0x600 | 0x1F,
		ManufactureID_TerraSemiconductor           = 0x600 | 0x20,
		ManufactureID_ImagingWorks                 = 0x600 | 0xA1,
		ManufactureID_AstuteNetworks2              = 0x600 | 0xA2,
		ManufactureID_Tzero                        = 0x600 | 0x23,
		ManufactureID_Emulex                       = 0x600 | 0xA4,
		ManufactureID_PowerOne                     = 0x600 | 0x25,
		ManufactureID_Pulse_LINK                   = 0x600 | 0x26,
		ManufactureID_HonHaiPrecisionIndustry      = 0x600 | 0xA7,
		ManufactureID_WhiteRockNetworks            = 0x600 | 0xA8,	// 40
		ManufactureID_TelegentSystems              = 0x600 | 0x29,
		ManufactureID_AtruaTechnologies            = 0x600 | 0x2A,
		ManufactureID_AcbelPolytech2               = 0x600 | 0xAB,
		ManufactureID_eRide                        = 0x600 | 0x2C,
		ManufactureID_ULiElectronics               = 0x600 | 0xAD,
		ManufactureID_MagnumSemiconductor          = 0x600 | 0xAE,
		ManufactureID_neoOneTechnology             = 0x600 | 0x2F,
		ManufactureID_ConnexTechnology             = 0x600 | 0xB0,
		ManufactureID_StreamProcessors             = 0x600 | 0x31,
		ManufactureID_FocusEnhancements2           = 0x600 | 0x32,	// 50
		ManufactureID_TelecisWireless              = 0x600 | 0xB3,
		ManufactureID_uNavMicroelectronics         = 0x600 | 0x34,
		ManufactureID_Tarari                       = 0x600 | 0xB5,
		ManufactureID_Ambric                       = 0x600 | 0xB6,
		ManufactureID_NewportMedia                 = 0x600 | 0x37,
		ManufactureID_VMTS                         = 0x600 | 0x38,
		ManufactureID_EnucliaSemiconductor         = 0x600 | 0xB9,
		ManufactureID_VirtiumTechnology            = 0x600 | 0xBA,
		ManufactureID_SolidStateSystem             = 0x600 | 0x3B,
		ManufactureID_KianTechLLC                  = 0x600 | 0xBC,	// 60
		ManufactureID_Artimi                       = 0x600 | 0x3D,
		ManufactureID_PowerQuotientInternational   = 0x600 | 0x3E,
		ManufactureID_AvagoTechnologies            = 0x600 | 0xBF,
		ManufactureID_ADTechnology                 = 0x600 | 0x40,
		ManufactureID_SigmaDesigns                 = 0x600 | 0xC1,
		ManufactureID_SiCortex                     = 0x600 | 0xC2,
		ManufactureID_VenturaTechnologyGroup       = 0x600 | 0x43,
		ManufactureID_eASIC                        = 0x600 | 0xC4,
		ManufactureID_MHS_SAS                      = 0x600 | 0x45,
		ManufactureID_MicroStarInternational       = 0x600 | 0x46,	// 70
		ManufactureID_Rapport                      = 0x600 | 0xC7,
		ManufactureID_MakwayInternational          = 0x600 | 0xC8,
		ManufactureID_BroadReachEngineering        = 0x600 | 0xC9,
		ManufactureID_SemiconductorMfgIntlCorp     = 0x600 | 0x4A,
		ManufactureID_SiConnect                    = 0x600 | 0xCB,
		ManufactureID_FCI_USA                      = 0x600 | 0x4C,
		ManufactureID_ValiditySensors              = 0x600 | 0xCD,
		ManufactureID_ConeyTechnology              = 0x600 | 0xCE,
		ManufactureID_SpansLogic                   = 0x600 | 0x4F,
		ManufactureID_Neterion                     = 0x600 | 0xD0,	// 80
		ManufactureID_Qimonda                      = 0x600 | 0x51,
		ManufactureID_NewJapanRadio                = 0x600 | 0x52,
		ManufactureID_Velogix                      = 0x600 | 0xD3,
		ManufactureID_MontalvoSystems              = 0x600 | 0xD4,
		ManufactureID_iVivity                      = 0x600 | 0xD5,
		ManufactureID_WaltonChaintech              = 0x600 | 0xD6,
		ManufactureID_AENEON                       = 0x600 | 0x57,
		ManufactureID_LoromIndustrial              = 0x600 | 0x58,
		ManufactureID_RadiospireNetworks           = 0x600 | 0xD9,
		ManufactureID_SensioTechnologies           = 0x600 | 0xDA,	// 90
		ManufactureID_NethraImaging                = 0x600 | 0x5B,
		ManufactureID_HexonTechnologyPte           = 0x600 | 0xDC,
		ManufactureID_CompuStocx                   = 0x600 | 0x5D,
		ManufactureID_MethodeElectronics           = 0x600 | 0x5E,
		ManufactureID_ConnectOne                   = 0x600 | 0xDF,
		ManufactureID_OpulanTechnologies           = 0x600 | 0xE0,
		ManufactureID_Septentrio_NV                = 0x600 | 0x61,
		ManufactureID_GoldenmarsTechnology         = 0x600 | 0x62,
		ManufactureID_KretonCorporation            = 0x600 | 0xE3,
		ManufactureID_Cochlear                     = 0x600 | 0x64,	// 100
		ManufactureID_AltairSemiconductor          = 0x600 | 0xE5,
		ManufactureID_NetEffect                    = 0x600 | 0xE6,
		ManufactureID_Spansion2                    = 0x600 | 0x67,
		ManufactureID_TaiwanSemiconductor          = 0x600 | 0x68,
		ManufactureID_EmphanySystems               = 0x600 | 0xE9,
		ManufactureID_ApaceWaveTechnologies        = 0x600 | 0xEA,
		ManufactureID_MobilygenCorporation         = 0x600 | 0x6B,
		ManufactureID_Tego                         = 0x600 | 0xEC,
		ManufactureID_CswitchCorporation           = 0x600 | 0x6D,
		ManufactureID_Haier                        = 0x600 | 0x6E,	// 110 - (Beijing) IC Design Co.
		ManufactureID_MetaRAM                      = 0x600 | 0xEF,
		ManufactureID_AxelElectronics              = 0x600 | 0x70,
		ManufactureID_TileraCorporation            = 0x600 | 0xF1,
		ManufactureID_Aquantia                     = 0x600 | 0xF2,
		ManufactureID_VivaceSemiconductor          = 0x600 | 0x73,
		ManufactureID_RedpineSignals               = 0x600 | 0xF4,
		ManufactureID_Octalica                     = 0x600 | 0x75,
		ManufactureID_InterDigitalCommunications   = 0x600 | 0x76,
		ManufactureID_AvantTechnology              = 0x600 | 0xF7,
		ManufactureID_Asrock                       = 0x600 | 0xF8,	// 120
		ManufactureID_Availink                     = 0x600 | 0x79,
		ManufactureID_Quartics                     = 0x600 | 0x7A,
		ManufactureID_Element_CXI                  = 0x600 | 0xFB,
		ManufactureID_InnovacionesMicroelectronicas= 0x600 | 0x7C,
		ManufactureID_VeriSiliconMicroelectronics  = 0x600 | 0xFD,
		ManufactureID_W5_Networks                  = 0x600 | 0xFE,

		// The following numbers are all in bank seven:
		ManufactureID_MOVEKING                     = 0x700 | 0x01,
		ManufactureID_MavrixTechnology             = 0x700 | 0x02,
		ManufactureID_CellGuide                    = 0x700 | 0x83,
		ManufactureID_FaradayTechnology2           = 0x700 | 0x04, 
		ManufactureID_DiabloTechnologies           = 0x700 | 0x85,
		ManufactureID_Jennic                       = 0x700 | 0x86,
		ManufactureID_Octasic                      = 0x700 | 0x07,
		ManufactureID_MolexIncorporated            = 0x700 | 0x08,
		ManufactureID_3LeafNetworks                = 0x700 | 0x89,
		ManufactureID_BrightMicronTechnology       = 0x700 | 0x8A,	// 10
		ManufactureID_Netxen                       = 0x700 | 0x0B,
		ManufactureID_NextWaveBroadband            = 0x700 | 0x8C,
		ManufactureID_DisplayLink                  = 0x700 | 0x0D,
		ManufactureID_ZMOS_Technology              = 0x700 | 0x0E,
		ManufactureID_Tec_Hill                     = 0x700 | 0x8F,
		ManufactureID_Multigig                     = 0x700 | 0x10,
		ManufactureID_Amimon                       = 0x700 | 0x91,
		ManufactureID_EuphonicTechnologies         = 0x700 | 0x92,
		ManufactureID_BRN_Phoenix                  = 0x700 | 0x13,
		ManufactureID_InSilica                     = 0x700 | 0x94,	// 20
		ManufactureID_EmberCorporation             = 0x700 | 0x15,
		ManufactureID_AvexirTechnologiesCorporation= 0x700 | 0x16,
		ManufactureID_EchelonCorporation           = 0x700 | 0x97,
		ManufactureID_EdgewaterComputerSystems     = 0x700 | 0x98,
		ManufactureID_XMOS_Semiconductor           = 0x700 | 0x19,
		ManufactureID_GENUSION                     = 0x700 | 0x1A,
		ManufactureID_MemoryCorp_NV                = 0x700 | 0x9B,
		ManufactureID_SiliconBlueTechnologies      = 0x700 | 0x1C,
		ManufactureID_Rambus                       = 0x700 | 0x9D,
		ManufactureID_AndesTechnologyCorporation   = 0x700 | 0x9E,	// 30
		ManufactureID_CoronisSystems               = 0x700 | 0x1F,
		ManufactureID_AchronixSemiconductor        = 0x700 | 0x20,
		ManufactureID_SianoMobileSilicon           = 0x700 | 0xA1,
		ManufactureID_SemtechCorporation           = 0x700 | 0xA2,
		ManufactureID_Pixelworks                   = 0x700 | 0x23,
		ManufactureID_GaislerResearch_AB           = 0x700 | 0xA4,
		ManufactureID_Teranetics                   = 0x700 | 0x25,
		ManufactureID_ToppanPrintingCo             = 0x700 | 0x26,
		ManufactureID_Kingxcon                     = 0x700 | 0xA7,
		ManufactureID_SiliconIntegratedSystems     = 0x700 | 0xA8,	// 40
		ManufactureID_IO_DataDevice                = 0x700 | 0x29,
		ManufactureID_NDS_Americas                 = 0x700 | 0x2A,
		ManufactureID_SolomonSystechLimited        = 0x700 | 0xAB,
		ManufactureID_OnDemandMicroelectronics     = 0x700 | 0x2C,
		ManufactureID_AmicusWireless               = 0x700 | 0xAD,
		ManufactureID_SMARDTV_SNC                  = 0x700 | 0xAE,
		ManufactureID_ComsysCommunication          = 0x700 | 0x2F,
		ManufactureID_Movidia                      = 0x700 | 0xB0,
		ManufactureID_Javad_GNSS                   = 0x700 | 0x31,
		ManufactureID_MontageTechnologyGroup       = 0x700 | 0x32,	// 50
		ManufactureID_TridentMicrosystems          = 0x700 | 0xB3,
		ManufactureID_SuperTalent                  = 0x700 | 0x34,
		ManufactureID_Optichron                    = 0x700 | 0xB5,
		ManufactureID_FutureWaves_UK               = 0x700 | 0xB6,
		ManufactureID_SiBEAM                       = 0x700 | 0x37,
		ManufactureID_Inicore                      = 0x700 | 0x38,
		ManufactureID_ViridenSystems               = 0x700 | 0xB9,
		ManufactureID_M2000                        = 0x700 | 0xBA,
		ManufactureID_ZeroG_Wireless               = 0x700 | 0x3B,
		ManufactureID_GingleTechnology_Co          = 0x700 | 0xBC,	// 60
		ManufactureID_SpaceMicro                   = 0x700 | 0x3D,
		ManufactureID_Wilocity                     = 0x700 | 0x3E,
		ManufactureID_Novafora                     = 0x700 | 0xBF,
		ManufactureID_iKoaCorporation              = 0x700 | 0x40,
		ManufactureID_ASintTechnology              = 0x700 | 0xC1,
		ManufactureID_Ramtron                      = 0x700 | 0xC2,
		ManufactureID_PlatoNetworks                = 0x700 | 0x43,
		ManufactureID_IPtronics_AS                 = 0x700 | 0xC4,
		ManufactureID_InfiniteMemories             = 0x700 | 0x45,
		ManufactureID_ParadeTechnologies           = 0x700 | 0x46,	// 70
		ManufactureID_DuneNetworks                 = 0x700 | 0xC7,
		ManufactureID_GigaDeviceSemiconductor      = 0x700 | 0xC8,
		ManufactureID_Modu                         = 0x700 | 0xC9,
		ManufactureID_CEITEC                       = 0x700 | 0x4A,
		ManufactureID_NorthropGrumman              = 0x700 | 0xCB,
		ManufactureID_XRONETCorporation            = 0x700 | 0x4C,
		ManufactureID_SiconSemiconductor_AB        = 0x700 | 0xCD,
		ManufactureID_AtlaElectronics_Co           = 0x700 | 0xCE,
		ManufactureID_TOPRAMTechnology             = 0x700 | 0x4F,
		ManufactureID_SilegoTechnology             = 0x700 | 0xD0,	// 80
		ManufactureID_Kinglife                     = 0x700 | 0x51,
		ManufactureID_AbilityIndustries            = 0x700 | 0x52,
		ManufactureID_SiliconPowerComputerCom      = 0x700 | 0xD3,
		ManufactureID_AugustaTechnology            = 0x700 | 0xD4,
		ManufactureID_NantronicsSemiconductors     = 0x700 | 0xD5,
		ManufactureID_HilscherGesellschaft         = 0x700 | 0xD6,
		ManufactureID_Quixant                      = 0x700 | 0x57,
		ManufactureID_Percello                     = 0x700 | 0x58,
		ManufactureID_NextIO                       = 0x700 | 0xD9,
		ManufactureID_Scanimetrics                 = 0x700 | 0xDA,	// 90
		ManufactureID_FS_SemiCompany               = 0x700 | 0x5B,
		ManufactureID_InfineraCorporation          = 0x700 | 0xDC,
		ManufactureID_SandForce                    = 0x700 | 0x5D,
		ManufactureID_LexarMedia                   = 0x700 | 0x5E,
		ManufactureID_Teradyne                     = 0x700 | 0xDF,
		ManufactureID_MemoryExchangeCorporation    = 0x700 | 0xE0,
		ManufactureID_SuzhouSmartekElectronics     = 0x700 | 0x61,
		ManufactureID_AvantiumCorporation          = 0x700 | 0x62,
		ManufactureID_ATP_Electronics              = 0x700 | 0xE3,
		ManufactureID_ValensSemiconductor          = 0x700 | 0x64,	// 100
		ManufactureID_AgateLogic                   = 0x700 | 0xE5,
		ManufactureID_Netronome                    = 0x700 | 0xE6,
		ManufactureID_Zenverge                     = 0x700 | 0x67,
		ManufactureID_N_trig                       = 0x700 | 0x68,
		ManufactureID_SanMaxTechnologies           = 0x700 | 0xE9,
		ManufactureID_ContourSemiconductor         = 0x700 | 0xEA,
		ManufactureID_TwinMOS                      = 0x700 | 0x6B,
		ManufactureID_SiliconSystems               = 0x700 | 0xEC,
		ManufactureID_V_ColorTechnology            = 0x700 | 0x6D,
		ManufactureID_CerticomCorporation          = 0x700 | 0x6E,	// 110
		ManufactureID_JSC_ICC_Milandr              = 0x700 | 0xEF,
		ManufactureID_PhotoFastGlobal              = 0x700 | 0x70,
		ManufactureID_InnoDiskCorporation          = 0x700 | 0xF1,
		ManufactureID_MusclePower                  = 0x700 | 0xF2,
		ManufactureID_EnergyMicro                  = 0x700 | 0x73,
		ManufactureID_Innofidei                    = 0x700 | 0xF4,
		ManufactureID_CopperGateCommunications     = 0x700 | 0x75,
		ManufactureID_HoltekSemiconductor          = 0x700 | 0x76,
		ManufactureID_MysonCentury                 = 0x700 | 0xF7,
		ManufactureID_FIDELIX                      = 0x700 | 0xF8,	// 120
		ManufactureID_RedDigitalCinema             = 0x700 | 0x79,
		ManufactureID_DensbitsTechnology           = 0x700 | 0x7A,
		ManufactureID_Zempro                       = 0x700 | 0xFB,
		ManufactureID_MoSys                        = 0x700 | 0x7C,
		ManufactureID_Provigent                    = 0x700 | 0xFD,
		ManufactureID_TriadSemiconductor           = 0x700 | 0xFE,

		// The following numbers are all in bank eight:
		ManufactureID_SikluCommunication           = 0x800 | 0x01,
		ManufactureID_A_ForceManufacturing         = 0x800 | 0x02,
		ManufactureID_Strontium                    = 0x800 | 0x83,
		ManufactureID_AbilisSystems                = 0x800 | 0x04, 
		ManufactureID_Siglead                      = 0x800 | 0x85,
		ManufactureID_Ubicom                       = 0x800 | 0x86,
		ManufactureID_UnifosaCorporation           = 0x800 | 0x07,
		ManufactureID_Stretch                      = 0x800 | 0x08,
		ManufactureID_LantiqDeutschland            = 0x800 | 0x89,
		ManufactureID_Visipro                      = 0x800 | 0x8A,	// 10
		ManufactureID_EKMemory                     = 0x800 | 0x0B,
		ManufactureID_MicroelectronicsInstitute    = 0x800 | 0x8C,
		ManufactureID_Cognovo                      = 0x800 | 0x0D,
		ManufactureID_CarryTechnology              = 0x800 | 0x0E,
		ManufactureID_Nokia                        = 0x800 | 0x8F,
		ManufactureID_KingTigerTechnology          = 0x800 | 0x10,
		ManufactureID_SierraWireless               = 0x800 | 0x91,
		ManufactureID_HT_Micron                    = 0x800 | 0x92,
		ManufactureID_AlbatronTechnology           = 0x800 | 0x13,
		ManufactureID_LeicaGeosystems              = 0x800 | 0x94,	// 20
		ManufactureID_BroadLight                   = 0x800 | 0x15,
		ManufactureID_AEXEA                        = 0x800 | 0x16,
		ManufactureID_ClariPhyCommunications       = 0x800 | 0x97,
		ManufactureID_GreenPlug                    = 0x800 | 0x98,
		ManufactureID_DesignArtNetworks            = 0x800 | 0x19,
		ManufactureID_MachXtremeTechnology         = 0x800 | 0x1A,
		ManufactureID_ATO_Solutions                = 0x800 | 0x9B,
		ManufactureID_Ramsta                       = 0x800 | 0x1C,
		ManufactureID_GreenliantSystems            = 0x800 | 0x9D,
		ManufactureID_Teikon                       = 0x800 | 0x9E,	// 30
		ManufactureID_AnteHadron                   = 0x800 | 0x1F,
		ManufactureID_NavComTechnology             = 0x800 | 0x20,
		ManufactureID_ShanghaiFudanMicroelectronics= 0x800 | 0xA1,
		ManufactureID_Calxeda                      = 0x800 | 0xA2,
		ManufactureID_JSC_EDC_Electronics          = 0x800 | 0x23,
		ManufactureID_KanditTechnology             = 0x800 | 0xA4,
		ManufactureID_RamosTechnology              = 0x800 | 0x25,
		ManufactureID_GoldenmarsTechnology2        = 0x800 | 0x26,
		ManufactureID_XeL_Technology               = 0x800 | 0xA7,
		ManufactureID_NewzoneCorporation           = 0x800 | 0xA8,	// 40
		ManufactureID_ShenZhenMercyPowerTech       = 0x800 | 0x29,
		ManufactureID_NanjingYihuoTechnology       = 0x800 | 0x2A,
		ManufactureID_NethraImaging2               = 0x800 | 0xAB,
		ManufactureID_SiTelSemiconductor_BV        = 0x800 | 0x2C,
		ManufactureID_SolidGearCorporation         = 0x800 | 0xAD,
		ManufactureID_TopowerComputerInd           = 0x800 | 0xAE,
		ManufactureID_Wilocity2                    = 0x800 | 0x2F,
		ManufactureID_Profichip                    = 0x800 | 0xB0,
		ManufactureID_GeradTechnologies            = 0x800 | 0x31,
		ManufactureID_RitekCorporation             = 0x800 | 0x32,	// 50
		ManufactureID_GomosTechnologyLimited       = 0x800 | 0xB3,
		ManufactureID_MemorightCorporation         = 0x800 | 0x34,
		ManufactureID_D_Broad                      = 0x800 | 0xB5,
		ManufactureID_HiSiliconTechnologies        = 0x800 | 0xB6,
		ManufactureID_Syndiant                     = 0x800 | 0x37,
		ManufactureID_Enverv                       = 0x800 | 0x38,
		ManufactureID_Cognex                       = 0x800 | 0xB9,
		ManufactureID_XinnovaTechnology            = 0x800 | 0xBA,
		ManufactureID_Ultron                       = 0x800 | 0x3B,
		ManufactureID_ConcordIdeaCorporation       = 0x800 | 0xBC,	// 60
		ManufactureID_AIM_Corporation              = 0x800 | 0x3D,
		ManufactureID_LifetimeMemoryProducts       = 0x800 | 0x3E,
		ManufactureID_Ramsway                      = 0x800 | 0xBF,
		ManufactureID_RecoreSystems                = 0x800 | 0x40,
		ManufactureID_HaotianJinshiboScienceTech   = 0x800 | 0xC1,
		ManufactureID_BeingAdvancedMemory          = 0x800 | 0xC2,
		ManufactureID_AdestoTechnologies           = 0x800 | 0x43,
		ManufactureID_GiantecSemiconductor         = 0x800 | 0xC4,
		ManufactureID_HMD_Electronics              = 0x800 | 0x45,
		ManufactureID_GlowayInternational          = 0x800 | 0x46,	// 70
		ManufactureID_Kingcore                     = 0x800 | 0xC7,
		ManufactureID_AnucellTechnologyHolding     = 0x800 | 0xC8,
		ManufactureID_AccordSoftware_Systems       = 0x800 | 0xC9,
		ManufactureID_Active_Semi                  = 0x800 | 0x4A,
		ManufactureID_DensoCorporation             = 0x800 | 0xCB,
		ManufactureID_TLSI                         = 0x800 | 0x4C,
		ManufactureID_Qidan                        = 0x800 | 0xCD,
		ManufactureID_Mustang                      = 0x800 | 0xCE,
		ManufactureID_OrcaSystems                  = 0x800 | 0x4F,
		ManufactureID_PassifSemiconductor          = 0x800 | 0xD0,	// 80
		ManufactureID_GigaDeviceSemiconductor2     = 0x800 | 0x51,
		ManufactureID_MemphisElectronic            = 0x800 | 0x52,
		ManufactureID_BeckhoffAutomation           = 0x800 | 0xD3,
		ManufactureID_HarmonySemiconductor         = 0x800 | 0xD4,
		ManufactureID_AirComputers                 = 0x800 | 0xD5,
		ManufactureID_TMTMemory                    = 0x800 | 0xD6,
		ManufactureID_EorexCorporation             = 0x800 | 0x57,
		ManufactureID_Xingtera                     = 0x800 | 0x58,
		ManufactureID_Netsol                       = 0x800 | 0xD9,
		ManufactureID_BestdonTechnology            = 0x800 | 0xDA,	// 90
		ManufactureID_Baysand                      = 0x800 | 0x5B,
		ManufactureID_UroadTechnology              = 0x800 | 0xDC,
		ManufactureID_WilkElektronik               = 0x800 | 0x5D,
		ManufactureID_AAI                          = 0x800 | 0x5E,
		ManufactureID_Harman                       = 0x800 | 0xDF,
		ManufactureID_BergMicroelectronics         = 0x800 | 0xE0,
		ManufactureID_ASSIA                        = 0x800 | 0x61,
		ManufactureID_VisiontekProducts            = 0x800 | 0x62,
		ManufactureID_OCMEMORY                     = 0x800 | 0xE3,
		ManufactureID_WelinkSolution               = 0x800 | 0x64,	// 100
		ManufactureID_SharkGaming                  = 0x800 | 0xE5,
		ManufactureID_AvalancheTechnology          = 0x800 | 0xE6,
		ManufactureID_RnD_Center_ELVEES_OJSC       = 0x800 | 0x67,
		ManufactureID_KingboMarsTechnology         = 0x800 | 0x68,
		ManufactureID_HighBridgeSolutionsIndustria = 0x800 | 0xE9,
		ManufactureID_TranscendTechnology          = 0x800 | 0xEA,
		ManufactureID_EverspinTechnologies         = 0x800 | 0x6B,
		ManufactureID_HonHaiPrecision              = 0x800 | 0xEC,
		ManufactureID_SmartStorageSystems          = 0x800 | 0x6D,
		ManufactureID_ToumazGroup                  = 0x800 | 0x6E,	// 110
		ManufactureID_ZentelElectronicsCorporation = 0x800 | 0xEF,
		ManufactureID_PanramInternationalCorporatio= 0x800 | 0x70,
		ManufactureID_SiliconSpaceTechnology       = 0x800 | 0xF1,
		ManufactureID_LITE_ON_IT_Corporation       = 0x800 | 0xF2,
		ManufactureID_Inuitive                     = 0x800 | 0x73,
		ManufactureID_HMicro                       = 0x800 | 0xF4,
		ManufactureID_BittWare                     = 0x800 | 0x75,
		ManufactureID_GLOBALFOUNDRIES              = 0x800 | 0x76,
		ManufactureID_ACPI_Digital                 = 0x800 | 0xF7,
		ManufactureID_AnnapurnaLabs                = 0x800 | 0xF8,	// 120
		ManufactureID_AcSiP_TechnologyCorporation  = 0x800 | 0x79,
		ManufactureID_IdeaElectronicSystems        = 0x800 | 0x7A,
		ManufactureID_GoweTechnology               = 0x800 | 0xFB,
		ManufactureID_HermesTestingSolutions       = 0x800 | 0x7C,
		ManufactureID_Positivo_BGH                 = 0x800 | 0xFD,
		ManufactureID_IntelligenceSiliconTechnology= 0x800 | 0xFE,
	};

	struct DeviceInfo
	{
		enum SerialFlash_ManufactureID ManufactureID; // Unique manufacturer ID
		uint8_t                        Type;          // Device specific type, defined by manufacturer
		enum Capacity                  Capacity;      // Flash capacity in Bits
	};


#ifdef __cplusplus
}
#endif

#endif	// _LIBMEM_TOOLS_H_

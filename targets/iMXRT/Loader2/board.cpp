/*
 * Copyright 2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "board_generic.h"
#include "fsl_device_registers.h"
#include "libmem_Tools.h"
#include "pin_mux.h"

#if defined ARM_MPU_ARMV7_H
	enum MPU_Regions
	{
		MPU_RegionSize_32B   = ARM_MPU_REGION_SIZE_32B,
		MPU_RegionSize_64B   = ARM_MPU_REGION_SIZE_64B,
		MPU_RegionSize_128B  = ARM_MPU_REGION_SIZE_128B,
		MPU_RegionSize_256B  = ARM_MPU_REGION_SIZE_256B,
		MPU_RegionSize_512B  = ARM_MPU_REGION_SIZE_512B,
		MPU_RegionSize_1KB   = ARM_MPU_REGION_SIZE_1KB,
		MPU_RegionSize_2KB   = ARM_MPU_REGION_SIZE_2KB,
		MPU_RegionSize_4KB   = ARM_MPU_REGION_SIZE_4KB,
		MPU_RegionSize_8KB   = ARM_MPU_REGION_SIZE_8KB,
		MPU_RegionSize_16KB  = ARM_MPU_REGION_SIZE_16KB,
		MPU_RegionSize_32KB  = ARM_MPU_REGION_SIZE_32KB,
		MPU_RegionSize_64KB  = ARM_MPU_REGION_SIZE_64KB,
		MPU_RegionSize_128KB = ARM_MPU_REGION_SIZE_128KB,
		MPU_RegionSize_256KB = ARM_MPU_REGION_SIZE_256KB,
		MPU_RegionSize_512KB = ARM_MPU_REGION_SIZE_512KB,
		MPU_RegionSize_1MB   = ARM_MPU_REGION_SIZE_1MB,
		MPU_RegionSize_2MB   = ARM_MPU_REGION_SIZE_2MB, 
		MPU_RegionSize_4MB   = ARM_MPU_REGION_SIZE_4MB, 
		MPU_RegionSize_8MB   = ARM_MPU_REGION_SIZE_8MB, 
		MPU_RegionSize_16MB  = ARM_MPU_REGION_SIZE_16MB, 
		MPU_RegionSize_32MB  = ARM_MPU_REGION_SIZE_32MB,
		MPU_RegionSize_64MB  = ARM_MPU_REGION_SIZE_64MB,
		MPU_RegionSize_128MB = ARM_MPU_REGION_SIZE_128MB,
		MPU_RegionSize_256MB = ARM_MPU_REGION_SIZE_256MB,
		MPU_RegionSize_512MB = ARM_MPU_REGION_SIZE_512MB,
		MPU_RegionSize_1GB   = ARM_MPU_REGION_SIZE_1GB,
		MPU_RegionSize_2GB   = ARM_MPU_REGION_SIZE_2GB, 
		MPU_RegionSize_4GB   = ARM_MPU_REGION_SIZE_4GB, 
	};

	struct SizeMap
	{
		MPU_Regions Region;
		uint32_t Size;
	}

	constexpr const SizeMap [] =
	{
		{MPU_RegionSize_32B   ,  32U},
		{MPU_RegionSize_64B   ,  64U},
		{MPU_RegionSize_128B  , 128U},
		{MPU_RegionSize_256B  , 256U},
		{MPU_RegionSize_512B  , 512U},
		{MPU_RegionSize_1KB   ,   1U * 1024},
		{MPU_RegionSize_2KB   ,   2U * 1024},
		{MPU_RegionSize_4KB   ,   4U * 1024},
		{MPU_RegionSize_8KB   ,   8U * 1024},
		{MPU_RegionSize_16KB  ,  16U * 1024},
		{MPU_RegionSize_32KB  ,  32U * 1024},
		{MPU_RegionSize_64KB  ,  64U * 1024},
		{MPU_RegionSize_128KB , 128U * 1024},
		{MPU_RegionSize_256KB , 256U * 1024},
		{MPU_RegionSize_512KB , 512U * 1024},
		{MPU_RegionSize_1MB   ,   1U * 1024 * 1024},
		{MPU_RegionSize_2MB   ,   2U * 1024 * 1024},
		{MPU_RegionSize_4MB   ,   4U * 1024 * 1024},
		{MPU_RegionSize_8MB   ,   8U * 1024 * 1024},
		{MPU_RegionSize_16MB  ,  16U * 1024 * 1024},
		{MPU_RegionSize_32MB  ,  32U * 1024 * 1024},
		{MPU_RegionSize_64MB  ,  64U * 1024 * 1024},
		{MPU_RegionSize_128MB , 128U * 1024 * 1024},
		{MPU_RegionSize_256MB , 256U * 1024 * 1024},
		{MPU_RegionSize_512MB , 512U * 1024 * 1024},
		{MPU_RegionSize_1GB   ,   1U * 1024 * 1024 * 1024},
		{MPU_RegionSize_2GB   ,   2U * 1024 * 1024 * 1024},
		{MPU_RegionSize_4GB   ,   4U * 1024 * 1024 * 1024}
	};

	constexpr MPU_Regions CalculateMpuRegion (uint32_t Size, uint32_t StartIndex=0)
	{
		return (Size <= SizeMap[StartIndex].Size) ? SizeMap[StartIndex].Region : CalculateMpuRegion (Size, ++StartIndex);
	}
#endif

#if defined ARM_MPU_ARMV7_H
	#if defined LMEM
		#include "cm4/fsl_cache.h"
		// The M4 core has a NXP properity cache
		void BOARD_ConfigMPU (void)
		{
			// Cleans and invalidates the processor system bus cache.
			//L1CACHE_CleanInvalidateSystemCache ();
			L1CACHE_DisableCodeCache ();
			L1CACHE_DisableSystemCache ();

			// Disable MPU
			ARM_MPU_Disable ();

			/*uint32_t Region = 0;
			// setting: Setting Memory with Device type, not shareable, non-cacheable.   - FlexSPI
			extern const uint32_t __FlexSPI_segment_start__;
			extern const uint32_t __FlexSPI_segment_size__;
			extern const uint32_t __FlexSPI_segment_end__;
			MPU->RBAR = ARM_MPU_RBAR(Region, (uint32_t)&__FlexSPI_segment_start__);
			MPU->RASR = ARM_MPU_RASR(0, ARM_MPU_AP_RO, 1, 0, 0, 0, 0, CalculateMpuRegion ((uint32_t)&__FlexSPI_segment_size__));
			Region++;

			#if defined FLEXSPI2
				// setting: Setting Memory with Device type, not shareable, non-cacheable. - FlexSPI2
				extern const uint32_t __FlexSPI2_segment_start__;
				extern const uint32_t __FlexSPI2_segment_size__;
				extern const uint32_t __FlexSPI2_segment_end__;
				MPU->RBAR = ARM_MPU_RBAR(Region, (uint32_t)&__FlexSPI2_segment_start__);
				MPU->RASR = ARM_MPU_RASR(0, ARM_MPU_AP_RO, 1, 0, 0, 0, 0, CalculateMpuRegion ((uint32_t)&__FlexSPI2_segment_size__));
				Region++;
			#endif

			// setting: Memory with Normal type, not shareable, non-cacheable   - DTCM
			extern const uint32_t __DTCM_segment_start__;
			extern const uint32_t __DTCM_segment_size__;
			extern const uint32_t __DTCM_segment_end__;
			MPU->RBAR = ARM_MPU_RBAR(Region, (uint32_t)&__DTCM_segment_start__);
			MPU->RASR = ARM_MPU_RASR(0, ARM_MPU_AP_FULL, 1, 0, 0, 0, 0, CalculateMpuRegion ((uint32_t)&__DTCM_segment_size__));

			// Enable MPU
			ARM_MPU_Enable (MPU_CTRL_PRIVDEFENA_Msk);*/
		}
	#else
		// The M7 core has ARM specific cache
		void BOARD_ConfigMPU (void)
		{
			#if defined(__ICACHE_PRESENT) && __ICACHE_PRESENT
				// Disable I cache and D cache
				if (SCB_CCR_IC_Msk == (SCB_CCR_IC_Msk & SCB->CCR))
				{
					SCB_DisableICache();
				}
			#endif
			#if defined(__DCACHE_PRESENT) && __DCACHE_PRESENT
				if (SCB_CCR_DC_Msk == (SCB_CCR_DC_Msk & SCB->CCR))
				{
					SCB_DisableDCache();
				}
			#endif

			// Disable MPU
			ARM_MPU_Disable ();

			/* MPU configure:
			 * Use ARM_MPU_RASR(DisableExec, AccessPermission, TypeExtField, IsShareable, IsCacheable, IsBufferable,
			 * SubRegionDisable, Size)
			 * API in core_cm7.h.
			 * param DisableExec       Instruction access (XN) disable bit,0=instruction fetches enabled, 1=instruction fetches
			 * disabled.
			 * param AccessPermission  Data access permissions, allows you to configure read/write access for User and
			 * Privileged mode.
			 *      Use MACROS defined in core_cm7.h:
			 * ARM_MPU_AP_NONE/ARM_MPU_AP_PRIV/ARM_MPU_AP_URO/ARM_MPU_AP_FULL/ARM_MPU_AP_PRO/ARM_MPU_AP_RO
			 * Combine TypeExtField/IsShareable/IsCacheable/IsBufferable to configure MPU memory access attributes.
			 *  TypeExtField  IsShareable  IsCacheable  IsBufferable   Memory Attribtue    Shareability        Cache
			 *     0             x           0           0             Strongly Ordered    shareable
			 *     0             x           0           1              Device             shareable
			 *     0             0           1           0              Normal             not shareable   Outer and inner write through no write allocate
			 *     0             0           1           1              Normal             not shareable   Outer and inner write back no write allocate
			 *     0             1           1           0              Normal             shareable       Outer and inner write through no write allocate
			 *     0             1           1           1              Normal             shareable       Outer and inner write back no write allocate
			 *     1             0           0           0              Normal             not shareable   outer and inner noncache
			 *     1             1           0           0              Normal             shareable       outer and inner noncache
			 *     1             0           1           1              Normal             not shareable   outer and inner write back write/read acllocate
			 *     1             1           1           1              Normal             shareable       outer and inner write back write/read acllocate
			 *     2             x           0           0              Device              not shareable
			 *  Above are normal use settings, if your want to see more details or want to config different inner/outter cache
			 * policy.
			 *  please refer to Table 4-55 /4-56 in arm cortex-M7 generic user guide <dui0646b_cortex_m7_dgug.pdf>
			 * param SubRegionDisable  Sub-region disable field. 0=sub-region is enabled, 1=sub-region is disabled.
			 * param Size              Region size of the region to be configured. use ARM_MPU_REGION_SIZE_xxx MACRO in
			 * core_cm7.h. */

			/* MPU Configuration
			The M7 Core has 16 entires: http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0489d/Chdecfea.html
			Compare chapter "Chapter 32 ARM Cortex M7 Platform (M7)"

			The M4 Core has 8 entries: http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0439b/Chdecfea.html
			compare chapter "33.5.4 Memory Protection Unit (MPU)" */

/*			uint32_t Region = 0;
			// Region 1 setting: Memory with Device type, not shareable, non-cacheable.                - ITCM + ROM
			MPU->RBAR = ARM_MPU_RBAR(Region, 0x00000000U);
			MPU->RASR = ARM_MPU_RASR(0, ARM_MPU_AP_FULL, 2, 0, 0, 0, 0, ARM_MPU_REGION_SIZE_1GB);
			Region++;

			// Region 1 setting: Memory with Normal type, not shareable, outer/inner write back   - ITCM
			extern const uint32_t __ITCM_segment_start__;
			extern const uint32_t __ITCM_segment_size__;
			extern const uint32_t __ITCM_segment_end__;
			MPU->RBAR = ARM_MPU_RBAR(Region, (uint32_t)&__ITCM_segment_start__);
			MPU->RASR = ARM_MPU_RASR(0, ARM_MPU_AP_FULL, 0, 0, 1, 1, 0, CalculateMpuRegion ((uint32_t)&__ITCM_segment_size__));
			Region++;

			// Region 2 setting: Memory with Normal type, not shareable, outer/inner write back   - DTCM
			extern const uint32_t __DTCM_segment_start__;
			extern const uint32_t __DTCM_segment_size__;
			extern const uint32_t __DTCM_segment_end__;
			MPU->RBAR = ARM_MPU_RBAR(Region, (uint32_t)&__DTCM_segment_start__);
			MPU->RASR = ARM_MPU_RASR(0, ARM_MPU_AP_FULL, 0, 0, 1, 1, 0, CalculateMpuRegion ((uint32_t)&__DTCM_segment_size__));
			Region++;

			// Region 3 setting: Setting Memory with Device type, not shareable, non-cacheable.   - FlexSPI
			extern const uint32_t __FlexSPI_segment_start__;
			extern const uint32_t __FlexSPI_segment_size__;
			extern const uint32_t __FlexSPI_segment_end__;
			MPU->RBAR = ARM_MPU_RBAR(Region, (uint32_t)&__FlexSPI_segment_start__);
			MPU->RASR = ARM_MPU_RASR(0, ARM_MPU_AP_RO, 0, 0, 1, 1, 0, ARM_MPU_REGION_SIZE_16MB);
			Region++;

			#if defined FLEXSPI2
				// Region 4 setting: Setting Memory with Device type, not shareable, non-cacheable. - FlexSPI2
				extern const uint32_t __FlexSPI2_segment_start__;
				extern const uint32_t __FlexSPI2_segment_size__;
				extern const uint32_t __FlexSPI2_segment_end__;
				MPU->RBAR = ARM_MPU_RBAR(Region, (uint32_t)&__FlexSPI2_segment_start__);
				MPU->RASR = ARM_MPU_RASR(0, ARM_MPU_AP_RO, 0, 0, 1, 1, 0, ARM_MPU_REGION_SIZE_16MB);
				Region++;
			#endif

			// Enable MPU
			ARM_MPU_Enable(MPU_CTRL_PRIVDEFENA_Msk);

			// Enable I cache and D cache
			#if defined(__DCACHE_PRESENT) && __DCACHE_PRESENT
				SCB_EnableDCache();
			#endif
			#if defined(__ICACHE_PRESENT) && __ICACHE_PRESENT
				SCB_EnableICache();
			#endif*/
		}

	#endif
#else
	#warning " MPU for ARMv8 Architecture not supported"
	void BOARD_ConfigMPU (void)
	{
		// ToDo
	}
#endif



void BOARD_BootClockGate (void)
{
	#if defined CCM_CCGR0_CG0_SHIFT
		// Enable all peripheral clock
		CCM->CCGR0 = 0xFFFFFFFFU;
		CCM->CCGR1 = 0xFFFFFFFFU;
		CCM->CCGR2 = 0xFFFFFFFFU;
		CCM->CCGR3 = 0xFFFFFFFFU;
		CCM->CCGR4 = 0xFFFFFFFFU;
		CCM->CCGR5 = 0xFFFFFFFFU;
		CCM->CCGR6 = 0xFFFFFFFFU;
	#endif
}
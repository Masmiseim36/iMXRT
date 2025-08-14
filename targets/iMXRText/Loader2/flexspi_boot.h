/*
 * The Clear BSD License
 * Copyright 2017 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 * that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _FLEXSPI_BOOT_H_
#define _FLEXSPI_BOOT_H_

#include <stdint.h>
#include "fsl_device_registers.h"

/************************************* 
 *  IVT Data 
 *************************************/
typedef struct _ivt_
{
	/** @ref hdr with tag #HAB_TAG_IVT, length and HAB version fields
	 *  (see @ref data)
	 */
	uint32_t hdr;
	/** Absolute address of the first instruction to execute from the
	 *  image
	 */
	uint32_t entry;
	/** Reserved in this version of HAB: should be NULL. */
	uint32_t reserved1;
	/** Absolute address of the image DCD: may be NULL. */
	uint32_t dcd;
	/** Absolute address of the Boot Data: may be NULL, but not interpreted
	 *  any further by HAB
	 */
	uint32_t boot_data;
	/** Absolute address of the IVT.*/
	uint32_t self;
	/** Absolute address of the image CSF.*/
	uint32_t csf;
	/** Reserved in this version of HAB: should be zero. */
	uint32_t reserved2;
} ivt;

#define IVT_MAJOR_VERSION           0x4
#define IVT_MAJOR_VERSION_SHIFT     0x4
#define IVT_MAJOR_VERSION_MASK      0xF
#define IVT_MINOR_VERSION           0x1
#define IVT_MINOR_VERSION_SHIFT     0x0
#define IVT_MINOR_VERSION_MASK      0xF

#define IVT_VERSION(major, minor)   \
  ((((major) & IVT_MAJOR_VERSION_MASK) << IVT_MAJOR_VERSION_SHIFT) |  \
  (((minor) & IVT_MINOR_VERSION_MASK) << IVT_MINOR_VERSION_SHIFT))

/* IVT header */ 
#define IVT_TAG_HEADER        (0xD1)       /**< Image Vector Table */
#define IVT_SIZE              0x2000
#define IVT_PAR               IVT_VERSION(IVT_MAJOR_VERSION, IVT_MINOR_VERSION)

#define IVT_HEADER          (IVT_TAG_HEADER | (IVT_SIZE << 8) | (IVT_PAR << 24))
#define IVT_RSVD            (uint32_t)(0x00000000)

/* Set resume entry */
#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
	extern uint32_t __Vectors[];
	extern uint32_t Image$$RW_m_config_text$$Base[];
	#define IMAGE_ENTRY_ADDRESS ((uint32_t)__Vectors)
	#define FLASH_BASE ((uint32_t)Image$$RW_m_config_text$$Base)
#elif defined(__MCUXPRESSO)
	extern uint32_t __Vectors[];
	extern uint32_t __boot_hdr_start__[];
	#define IMAGE_ENTRY_ADDRESS ((uint32_t)__Vectors)
	#define FLASH_BASE          ((uint32_t)__boot_hdr_start__)
#elif defined(__ICCARM__)
	#define IMAGE_ENTRY_ADDRESS  ((uint32_t)__iar_program_start)
	#define FLASH_BASE           ((uint32_t)__section_begin("app_image"))
	#define FLASH_END            (((uint32_t)__section_begin("app_image")) + ((uint32_t )__section_size("app_image")))
	#define FLASH_SIZE            __section_size("app_image")
#elif defined __CROSSWORKS_ARM
	#if defined XIP_BOOT_FLEXSPI1 && defined FlexSPI_AMBA_BASE
		#define FLASH_BASE    FlexSPI_AMBA_BASE
	#elif defined XIP_BOOT_FLEXSPI1 && defined FlexSPI1_AMBA_BASE
		#define FLASH_BASE    FlexSPI1_AMBA_BASE
	#elif defined XIP_BOOT_FLEXSPI2 && defined FLEXSPI2
		#define FLASH_BASE    FlexSPI2_AMBA_BASE
	#else
		#define FLASH_BASE FlexSPI_AMBA_BASE
	#endif
	#define FLASH_END  (FLASH_BASE + 0x0800000)
	extern void reset_handler(void);
	#define IMAGE_ENTRY_ADDRESS ((uint32_t)reset_handler)
#elif defined __SES_ARM
	#define FLASH_BASE            0x60000000
	#define FLASH_END             0x70800000
	extern void Reset_Handler(void);
	#define IMAGE_ENTRY_ADDRESS ((uint32_t)Reset_Handler)
#elif defined(__GNUC__)
	extern uint32_t __VECTOR_TABLE[];
	extern uint32_t __FLASH_BASE[];
	#define IMAGE_ENTRY_ADDRESS ((uint32_t)__VECTOR_TABLE)
	#define FLASH_BASE ((uint32_t)__FLASH_BASE)

	extern void Reset_Handler (void);
	#define IMAGE_ENTRY_ADDRESS ((uint32_t)Reset_Handler)
#else
	#error "Unknown Compiler"
#endif


#ifndef FLASH_SIZE
	#define FLASH_SIZE (FLASH_END-FLASH_BASE)
#endif

#define DCD_ADDRESS           dcd_data
#define BOOT_DATA_ADDRESS     &boot_data
#define CSF_ADDRESS           0
#define IVT_RSVD             (uint32_t)(0x00000000)

/************************************* 
 *  Boot Data 
 *************************************/
typedef struct _boot_data_
{
  uint32_t start;           /* boot start location */
  uint32_t size;            /* size */
  uint32_t plugin;          /* plugin flag - 1 if downloaded application is plugin */
  uint32_t placeholder;     /* placeholder to make even 0x10 size */
} BOOT_DATA_T;


/************************************* 
 *  DCD Data 
 *************************************/
#define DCD_TAG_HEADER            (0xD2)
#define DCD_TAG_HEADER_SHIFT      (24)
#define DCD_VERSION               (0x40)
#define DCD_ARRAY_SIZE             1

#define SCLK 1

#define BOOT_DATA_ADDRESS     &boot_data
#define CSF_ADDRESS           0
#define PLUGIN_FLAG           (uint32_t)0

/* External Variables */
extern const BOOT_DATA_T boot_data;

#endif	// _FLEXSPI_BOOT_H_
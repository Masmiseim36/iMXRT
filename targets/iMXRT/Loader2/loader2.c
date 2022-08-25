/*****************************************************************************
 * Copyright (c) 2022 Rowley Associates Limited.                             *
 *                                                                           *
 * This file may be distributed under the terms of the License Agreement     *
 * provided with this software.                                              *
 *                                                                           *
 * THIS FILE IS PROVIDED AS IS WITH NO WARRANTY OF ANY KIND, INCLUDING THE   *
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. *
 *                                                                           *
 *****************************************************************************/

#include <libmem.h>
#include <libmem_loader.h>

#include <libmem.h>
#include "fsl_romapi.h"

#define FlexSpiInstance           1U
static serial_nor_config_option_t option = {
    .option0.U = 0xc0000007U,
    .option1.U = 0U,
};

/*! @brief FLEXSPI NOR flash driver Structure */
static flexspi_nor_config_t norConfig;

#ifndef FLASH_START
#error FLASH_START not defined
#endif
#define FLASH_SIZE        norConfig.memConfig.sflashA1Size
#define FLASH_PAGE_SIZE   norConfig.pageSize
#define FLASH_SECTOR_SIZE norConfig.sectorSize

typedef void *(*rom_table_lookup_fn)(uint16_t *table, uint32_t code);

static libmem_driver_paged_write_ctrlblk_t paged_write_ctrlblk;
static uint8_t write_buffer[1024];

static int
flash_write_page(libmem_driver_handle_t *h, uint8_t *dest, const uint8_t *src)
{
  status_t status = ROM_FLEXSPI_NorFlash_ProgramPage(FlexSpiInstance, &norConfig, (uint32_t)dest-FLASH_START, (const uint32_t *)src);  
  return status == kStatus_Success ? LIBMEM_STATUS_SUCCESS : LIBMEM_STATUS_ERROR;
}

static int
libmem_write_impl(libmem_driver_handle_t *h, uint8_t *dest, const uint8_t *src, size_t size)
{
  return libmem_driver_paged_write(h, dest, src, size, &paged_write_ctrlblk);
}

static int
libmem_erase_impl(libmem_driver_handle_t *h, uint8_t *start, size_t size, uint8_t **erase_start, size_t *erase_size)
{
  start = LIBMEM_ALIGNED_ADDRESS(start, FLASH_SECTOR_SIZE);
  if (start >= (uint8_t *)FLASH_START && start < (uint8_t *)FLASH_START + FLASH_SIZE)
    {
      uint8_t *end = LIBMEM_ALIGNED_ADDRESS(start + size + FLASH_SECTOR_SIZE - 1, FLASH_SECTOR_SIZE);
      if (end > (uint8_t *)FLASH_START + FLASH_SIZE)
        end = (uint8_t *)FLASH_START + FLASH_SIZE;
      size = end - start;
      if (erase_start)
        *erase_start = start;
      if (erase_size)
        *erase_size = size;
      status_t status = ROM_FLEXSPI_NorFlash_Erase(FlexSpiInstance, &norConfig, (uint32_t)start-FLASH_START, size);
      return status == kStatus_Success ? LIBMEM_STATUS_SUCCESS : LIBMEM_STATUS_ERROR;
    }
  return LIBMEM_STATUS_SUCCESS;
}

static int
libmem_flush_impl(libmem_driver_handle_t *h)
{
  return libmem_driver_paged_write_flush(h, &paged_write_ctrlblk);
}

static libmem_geometry_t geometry[2];

static const libmem_driver_functions_t driver_functions =
{
  libmem_write_impl,
  0,
  libmem_erase_impl,
  0,
  0,
  libmem_flush_impl
};

extern uint8_t __DTCM_segment_start__;
extern uint8_t __DTCM_segment_used_end__;
extern uint8_t __DTCM_segment_end__;

#if 0
unsigned char mem[] = "hello world today is rather sunny";
unsigned char mem2[64];
#endif

int
main(uint32_t flags, uint32_t param)
{
  int res;
  uint32_t fosc = flags & LIBMEM_RPC_LOADER_FLAG_PARAM ? param : 0;
  const char *error_string = 0;
  libmem_driver_handle_t flash1_handle;
  
  ROM_API_Init();
  status_t status = ROM_FLEXSPI_NorFlash_GetConfig(FlexSpiInstance, &norConfig, &option);
  if (status != kStatus_Success)
    return LIBMEM_STATUS_ERROR;
  status = ROM_FLEXSPI_NorFlash_Init(FlexSpiInstance, &norConfig);
  if (status != kStatus_Success)
    libmem_rpc_loader_exit(res, error_string);
  //ROM_FLEXSPI_NorFlash_ClearCache(FlexSpiInstance);
  //if (status != kStatus_Success)
  //  return LIBMEM_STATUS_ERROR;
  geometry[0].count = FLASH_SIZE / FLASH_SECTOR_SIZE;
  geometry[0].size = FLASH_SIZE;
  libmem_register_driver(&flash1_handle, (uint8_t *)FLASH_START, FLASH_SIZE, geometry, 0, &driver_functions, 0);
  libmem_driver_paged_write_init(&paged_write_ctrlblk, write_buffer, FLASH_PAGE_SIZE, flash_write_page, 4, 0);  

#if 0
  uint8_t *start;
  size_t size;
  res = libmem_erase((uint8_t*)FLASH_START+0x1000, sizeof(mem), &start, &size);
  res = libmem_write((uint8_t*)FLASH_START+0x1000, mem, sizeof(mem));
  res = libmem_flush(); 
#endif

  res = libmem_rpc_loader_start(&__DTCM_segment_used_end__, &__DTCM_segment_end__ - 1);

  /* Terminate loader */
  libmem_rpc_loader_exit(res, error_string);

  return 0;
}

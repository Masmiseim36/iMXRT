/*****************************************************************************
 * Copyright (c) 2018 Rowley Associates Limited.                             *
 *                                                                           *
 * This file may be distributed under the terms of the License Agreement     *
 * provided with this software.                                              *
 *                                                                           *
 * THIS FILE IS PROVIDED AS IS WITH NO WARRANTY OF ANY KIND, INCLUDING THE   *
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. *
 *                                                                           *
 *****************************************************************************/

#include <libmem_flm_driver.h>

#if defined(FLM_ALGORITHM_MIMXRT1021_QuadSPI_4KB_SEC)

// MIMXRT1020 8mB QuadSPI NOR Flash

#define FLM_START_ADDRESS 0x60000000
#define FLM_SIZE 0x00800000
#define FLM_PAGE_SIZE 0x00000100

static const libmem_geometry_t geometry[] =
{
  { 0x800, 0x1000 },
  { 0, 0 } 
};

#elif defined(FLM_ALGORITHM_MIMXRT105x_HYPER_256KB_SEC) || defined(FLM_ALGORITHM_MIMXRT106x_HYPER_256KB_SEC)

// MIMXRT105x 64mB Hyper Flash

#define FLM_START_ADDRESS 0x60000000
#define FLM_SIZE 0x04000000
#define FLM_PAGE_SIZE 0x00000200

static const libmem_geometry_t geometry[] =
{
  { 0x100, 0x40000 },
  { 0, 0 } 
};

#elif defined(FLM_ALGORITHM_MIMXRT105x_QuadSPI_4KB_SEC) || defined(FLM_ALGORITHM_MIMXRT105x_EcoXiP_4KB_SEC)

// MIMXRT105x 8mB QuadSPI NOR Flash

#define FLM_START_ADDRESS 0x60000000
#define FLM_SIZE 0x00800000
#define FLM_PAGE_SIZE 0x00000100

static const libmem_geometry_t geometry[] =
{
  { 0x800, 0x1000 },
  { 0, 0 } 
};

#elif defined(FLM_ALGORITHM_MIMXRT106x_QSPI_4KB_SEC) || defined(FLM_ALGORITHM_MIMXRT106x_EcoXiP_4KB_SEC)

// MIMXRT106x 8mB QuadSPI NOR Flash

#define FLM_START_ADDRESS 0x60000000
#define FLM_SIZE 0x00800000
#define FLM_PAGE_SIZE 0x00000100

static const libmem_geometry_t geometry[] =
{
  { 0x800, 0x1000 },
  { 0, 0 } 
};

#elif defined(FLM_ALGORITHM_MIMXRT1064_QSPI_4KB_SEC)

// MIMXRT1064 8mB QuadSPI NOR Flash

#define FLM_START_ADDRESS 0x70000000
#define FLM_SIZE 0x00800000
#define FLM_PAGE_SIZE 0x00000100

static const libmem_geometry_t geometry[] =
{
  { 0x800, 0x1000 },
  { 0, 0 } 
};

#else

#error unknown FLM algorithm

#endif

static uint8_t write_buffer[FLM_PAGE_SIZE];

int
libmem_loader_register_imxrt_flm_driver(libmem_driver_handle_t *h, uint32_t clk)
{
  return libmem_register_flm_driver(h, (uint8_t *)FLM_START_ADDRESS, FLM_SIZE, geometry, write_buffer, sizeof(write_buffer), clk);
}

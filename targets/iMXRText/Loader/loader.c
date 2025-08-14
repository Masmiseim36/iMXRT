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

#include <libmem.h>
#include <libmem_loader.h>

extern uint8_t __DTCM_segment_start__;
extern uint8_t __DTCM_segment_used_end__;
extern uint8_t __DTCM_segment_end__;

int libmem_loader_register_imxrt_flm_driver(libmem_driver_handle_t *h, uint32_t clk);

int
main(uint32_t flags, uint32_t param)
{
  int res;
  uint32_t fosc = flags & LIBMEM_RPC_LOADER_FLAG_PARAM ? param : 0;
  const char *error_string = 0;
  libmem_driver_handle_t flash1_handle;

  /* Register PAC52XX internal FLASH driver */
  res = libmem_loader_register_imxrt_flm_driver(&flash1_handle, fosc);

  /* Start loader */
  if (res == LIBMEM_STATUS_SUCCESS)
    res = libmem_rpc_loader_start(&__DTCM_segment_used_end__, &__DTCM_segment_end__ - 1);

  /* Terminate loader */
  libmem_rpc_loader_exit(res, error_string);

  return 0;
}

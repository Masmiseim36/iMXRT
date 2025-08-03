/*
 * Copyright 2014-2016 Freescale Semiconductor, Inc.
 * Copyright 2016-2025 NXP
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __FSL_DEVICE_REGISTERS_H__
#define __FSL_DEVICE_REGISTERS_H__

/*
 * Include the cpu specific register header files.
 *
 * The CPU macro should be declared in the project or makefile.
 */
#if (defined(CPU_MIMXRT533SFAWC) || defined(CPU_MIMXRT533SFFOC))
	#include "MIMXRT533S.h"
#elif (defined(CPU_MIMXRT555SFAWC) || defined(CPU_MIMXRT555SFFOC))
	#include "MIMXRT555S.h"
#elif (defined(CPU_MIMXRT595SFAWC_cm33) || defined(CPU_MIMXRT595SFFOC_cm33))
	#include "MIMXRT595S_cm33.h"
#else
	#error "No valid CPU defined!"
#endif

#endif /* __FSL_DEVICE_REGISTERS_H__ */

/*******************************************************************************
 * EOF
 ******************************************************************************/

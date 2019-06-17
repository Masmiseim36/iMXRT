/*
 * Copyright 2014-2016 Freescale Semiconductor, Inc.
 * Copyright 2016-2019 NXP
 * All rights reserved.
 *
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
#if (defined(CPU_MIMXRT1015CAF4A) || defined(CPU_MIMXRT1015DAF5A))
	#define MIMXRT1015_SERIES
	/* CMSIS-style register definitions */
	#include "MIMXRT1015.h"
	/* CPU specific feature definitions */
	#include "MIMXRT1015_features.h"
	
#elif (defined(CPU_MIMXRT1021CAF4A) || defined(CPU_MIMXRT1021CAG4A) || defined(CPU_MIMXRT1021DAF5A) || \
    defined(CPU_MIMXRT1021DAG5A))
	#define MIMXRT1021_SERIES
	/* CMSIS-style register definitions */
	#include "MIMXRT1021.h"
	/* CPU specific feature definitions */
	#include "MIMXRT1021_features.h"
	
#elif (defined(CPU_MIMXRT1051CVJ5B) || defined(CPU_MIMXRT1051CVL5B) || defined(CPU_MIMXRT1051DVL6B) || \
     defined(CPU_MIMXRT1051DVL6A) || defined(CPU_MIMXRT1051CVL5A))
	#define MIMXRT1051_SERIES
	/* CMSIS-style register definitions */
	#include "MIMXRT1051.h"
	/* CPU specific feature definitions */
	#include "MIMXRT1051_features.h"
	
#elif (defined(CPU_MIMXRT1052CVJ5B) || defined(CPU_MIMXRT1052CVL5B) || defined(CPU_MIMXRT1052DVJ6B) || \
     defined(CPU_MIMXRT1052DVL6B) || defined(CPU_MIMXRT1052DVL6A) || defined(CPU_MIMXRT1052CVL5A))
	#define MIMXRT1052_SERIES
	/* CMSIS-style register definitions */
	#include "MIMXRT1052.h"
	/* CPU specific feature definitions */
	#include "MIMXRT1052_features.h"
	
#elif (defined(CPU_MIMXRT1061CVJ5A) || defined(CPU_MIMXRT1061CVL5A) || defined(CPU_MIMXRT1061DVL6A))
	#define MIMXRT1061_SERIES
	/* CMSIS-style register definitions */
	#include "MIMXRT1061.h"
	/* CPU specific feature definitions */
	#include "MIMXRT1061_features.h"

#elif (defined(CPU_MIMXRT1062CVJ5A) || defined(CPU_MIMXRT1062CVL5A) || defined(CPU_MIMXRT1062DVJ6A) || \
    defined(CPU_MIMXRT1062DVL6A))
	#define MIMXRT1062_SERIES
	/* CMSIS-style register definitions */
	#include "MIMXRT1062.h"
	/* CPU specific feature definitions */
	#include "MIMXRT1062_features.h"

#elif (defined(CPU_MIMXRT1064CVL5A) || defined(CPU_MIMXRT1064DVL6A))
	#define MIMXRT1064_SERIES
	/* CMSIS-style register definitions */
	#include "MIMXRT1064.h"
	/* CPU specific feature definitions */
	#include "MIMXRT1064_features.h"

#else
    #error "No valid CPU defined!"
#endif

#endif /* __FSL_DEVICE_REGISTERS_H__ */

/*******************************************************************************
 * EOF
 ******************************************************************************/

/*
 * Copyright 2014-2016 Freescale Semiconductor, Inc.
 * Copyright 2016-2022 NXP
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
#if (defined(CPU_MIMXRT533SFAWC) || defined(CPU_MIMXRT533SFFOC))
	#define MIMXRT533S_SERIES
	/* CMSIS-style register definitions */
	#include "MIMXRT533S.h"
	/* CPU specific feature definitions */
	#include "MIMXRT533S_features.h"
#elif (defined(CPU_MIMXRT555SFAWC) || defined(CPU_MIMXRT555SFFOC))
	#define MIMXRT555S_SERIES
	/* CMSIS-style register definitions */
	#include "MIMXRT555S.h"
	/* CPU specific feature definitions */
	#include "MIMXRT555S_features.h"
#elif (defined(CPU_MIMXRT595SFAWC_cm33) || defined(CPU_MIMXRT595SFFOC_cm33))
	#define MIMXRT595S_cm33_SERIES
	/* CMSIS-style register definitions */
	#include "MIMXRT595S_cm33.h"
	/* CPU specific feature definitions */
	#include "MIMXRT595S_cm33_features.h"
#elif (defined(CPU_MIMXRT595SFAWC_dsp) || defined(CPU_MIMXRT595SFFOC_dsp))
	#define MIMXRT595S_dsp_SERIES
	/* CMSIS-style register definitions */
	#include "MIMXRT595S_dsp.h"
	/* CPU specific feature definitions */
	#include "MIMXRT595S_dsp_features.h"

#elif (defined(CPU_MIMXRT633SFAWBR) || defined(CPU_MIMXRT633SFFOB) || defined(CPU_MIMXRT633SFVKB))
	#define MIMXRT633S_SERIES
	/* CMSIS-style register definitions */
	#include "MIMXRT633S.h"
	/* CPU specific feature definitions */
	#include "MIMXRT633S_features.h"

#elif (defined(CPU_MIMXRT685SFAWBR_cm33) || defined(CPU_MIMXRT685SFFOB_cm33) || defined(CPU_MIMXRT685SFVKB_cm33))
	#define MIMXRT685S_cm33_SERIES
	/* CMSIS-style register definitions */
	#include "MIMXRT685S_cm33.h"
	/* CPU specific feature definitions */
	#include "MIMXRT685S_cm33_features.h"

#elif (defined(CPU_MIMXRT1011CAE4A) || defined(CPU_MIMXRT1011DAE5A))
	#define MIMXRT1011_SERIES
	/* CMSIS-style register definitions */
	#include "MIMXRT1011.h"
	/* CPU specific feature definitions */
	#include "MIMXRT1011_features.h"

#elif (defined(CPU_MIMXRT1015CAF4A) || defined(CPU_MIMXRT1015DAF5A))
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

#elif (defined(CPU_MIMXRT1024CAG4A) || defined(CPU_MIMXRT1024DAG5A))
	#define MIMXRT1024_SERIES
	/* CMSIS-style register definitions */
	#include "MIMXRT1024.h"
	/* CPU specific feature definitions */
	#include "MIMXRT1024_features.h"

#elif (defined(CPU_MIMXRT1041DFP6B) || defined(CPU_MIMXRT1041XFP5B) || defined(CPU_MIMXRT1041XJM5B))
	#define MIMXRT1041_SERIES
	/* CMSIS-style register definitions */
	#include "MIMXRT1041.h"
	/* CPU specific feature definitions */
	#include "MIMXRT1041_features.h"

#elif (defined(CPU_MIMXRT1042DFP6B) || defined(CPU_MIMXRT1042XFP5B) || defined(CPU_MIMXRT1042XJM5B))
	#define MIMXRT1042_SERIES
	/* CMSIS-style register definitions */
	#include "MIMXRT1042.h"
	/* CPU specific feature definitions */
	#include "MIMXRT1042_features.h"

#elif (defined(CPU_MIMXRT1051CVJ5B) || defined(CPU_MIMXRT1051CVL5B) || defined(CPU_MIMXRT1051DVJ6B) || \
	defined(CPU_MIMXRT1051DVL6B) || defined(CPU_MIMXRT1051DVL6A) || defined(CPU_MIMXRT1051CVL5A))
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

#elif (defined(CPU_MIMXRT1061CVJ5A) || defined(CPU_MIMXRT1061CVL5A) || defined(CPU_MIMXRT1061DVJ6A) || \
	defined(CPU_MIMXRT1061DVL6A))
	#define MIMXRT1061_SERIES
	/* CMSIS-style register definitions */
	#include "MIMXRT1061.h"
	/* CPU specific feature definitions */
	#include "MIMXRT1061_features.h"

#elif (defined(CPU_MIMXRT1061XVN5B))
	#define MIMXRT1061X_SERIES
	/* CMSIS-style register definitions */
	#include "MIMXRT1061X.h"
	/* CPU specific feature definitions */
	#include "MIMXRT1061X_features.h"

#elif (defined(CPU_MIMXRT1062CVJ5A) || defined(CPU_MIMXRT1062CVL5A) || defined(CPU_MIMXRT1062DVJ6A) || \
	defined(CPU_MIMXRT1062DVL6A))
	#define MIMXRT1062_SERIES
	/* CMSIS-style register definitions */
	#include "MIMXRT1062.h"
	/* CPU specific feature definitions */
	#include "MIMXRT1062_features.h"

#elif (defined(CPU_MIMXRT1062DVN6B) || defined(CPU_MIMXRT1062XVN5B))
	#define MIMXRT1062X_SERIES
	/* CMSIS-style register definitions */
	#include "MIMXRT1062X.h"
	/* CPU specific feature definitions */
	#include "MIMXRT1062X_features.h"

#elif (defined(CPU_MIMXRT1064CVJ5A) || defined(CPU_MIMXRT1064CVL5A) || defined(CPU_MIMXRT1064DVJ6A) || \
	defined(CPU_MIMXRT1064DVL6A))
	#define MIMXRT1064_SERIES
	/* CMSIS-style register definitions */
	#include "MIMXRT1064.h"
	/* CPU specific feature definitions */
	#include "MIMXRT1064_features.h"

#elif (defined(CPU_MIMXRT106ADVL6A))
	#define MIMXRT106A_SERIES
	/* CMSIS-style register definitions */
	#include "MIMXRT106A.h"
	/* CPU specific feature definitions */
	#include "MIMXRT106A_features.h"


#elif (defined(CPU_MIMXRT1165CVM5A_cm7) || defined(CPU_MIMXRT1165DVM6A_cm7) || defined(CPU_MIMXRT1165XVM5A_cm7))
	#define MIMXRT1165_cm7_SERIES
	/* CMSIS-style register definitions */
	#include "MIMXRT1165_cm7.h"
	/* CPU specific feature definitions */
	#include "MIMXRT1165_cm7_features.h"

#elif (defined(CPU_MIMXRT1165CVM5A_cm4) || defined(CPU_MIMXRT1165DVM6A_cm4) || defined(CPU_MIMXRT1165XVM5A_cm4))
	#define MIMXRT1165_cm4_SERIES
	/* CMSIS-style register definitions */
	#include "MIMXRT1165_cm4.h"
	/* CPU specific feature definitions */
	#include "MIMXRT1165_cm4_features.h"

#elif (defined(CPU_MIMXRT1166CVM5A_cm7) || defined(CPU_MIMXRT1166DVM6A_cm7) || defined(CPU_MIMXRT1166XVM5A_cm7))
	#define MIMXRT1166_cm7_SERIES
	/* CMSIS-style register definitions */
	#include "MIMXRT1166_cm7.h"
	/* CPU specific feature definitions */
	#include "MIMXRT1166_cm7_features.h"

#elif (defined(CPU_MIMXRT1166CVM5A_cm4) || defined(CPU_MIMXRT1166DVM6A_cm4) || defined(CPU_MIMXRT1166XVM5A_cm4))
	#define MIMXRT1166_cm4_SERIES
	/* CMSIS-style register definitions */
	#include "MIMXRT1166_cm4.h"
	/* CPU specific feature definitions */
	#include "MIMXRT1166_cm4_features.h"

#elif (defined(CPU_MIMXRT117HAVM8A_cm7) || defined(CPU_MIMXRT117HCVM8A_cm7) || defined(CPU_MIMXRT117HDVMAA_cm7))
	#define MIMXRT117H_cm7_SERIES
	/* CMSIS-style register definitions */
	#include "MIMXRT117H_cm7.h"
	/* CPU specific feature definitions */
	#include "MIMXRT117H_cm7_features.h"

#elif (defined(CPU_MIMXRT1171AVM8A) || defined(CPU_MIMXRT1171CVM8A) || defined(CPU_MIMXRT1171DVMAA))
	#define MIMXRT1171_SERIES
	/* CMSIS-style register definitions */
	#include "MIMXRT1171.h"
	/* CPU specific feature definitions */
	#include "MIMXRT1171_features.h"

#elif (defined(CPU_MIMXRT1172AVM8A) || defined(CPU_MIMXRT1172CVM8A) || defined(CPU_MIMXRT1172DVMAA))
	#define MIMXRT1172_SERIES
	/* CMSIS-style register definitions */
	#include "MIMXRT1172.h"
	/* CPU specific feature definitions */
	#include "MIMXRT1172_features.h"

#elif (defined(CPU_MIMXRT1173CVM8A_cm7))
	#define MIMXRT1173_cm7_SERIES
	/* CMSIS-style register definitions */
	#include "MIMXRT1173_cm7.h"
	/* CPU specific feature definitions */
	#include "MIMXRT1173_cm7_features.h"
#elif (defined(CPU_MIMXRT1173CVM8A_cm4))
	#define MIMXRT1173_cm4_SERIES
	/* CMSIS-style register definitions */
	#include "MIMXRT1173_cm4.h"
	/* CPU specific feature definitions */
	#include "MIMXRT1173_cm4_features.h"

#elif (defined(CPU_MIMXRT1175AVM8A_cm7) || defined(CPU_MIMXRT1175CVM8A_cm7) || defined(CPU_MIMXRT1175DVMAA_cm7))
	#define MIMXRT1175_cm7_SERIES
	/* CMSIS-style register definitions */
	#include "MIMXRT1175_cm7.h"
	/* CPU specific feature definitions */
	#include "MIMXRT1175_cm7_features.h"
#elif (defined(CPU_MIMXRT1175AVM8A_cm4) || defined(CPU_MIMXRT1175CVM8A_cm4) || defined(CPU_MIMXRT1175DVMAA_cm4))
	#define MIMXRT1175_cm4_SERIES
	/* CMSIS-style register definitions */
	#include "MIMXRT1175_cm4.h"
	/* CPU specific feature definitions */
	#include "MIMXRT1175_cm4_features.h"

#elif (defined(CPU_MIMXRT1176AVM8A_cm7) || defined(CPU_MIMXRT1176CVM8A_cm7) || defined(CPU_MIMXRT1176DVMAA_cm7))
	#define MIMXRT1176_cm7_SERIES
	/* CMSIS-style register definitions */
	#include "MIMXRT1176_cm7.h"
	/* CPU specific feature definitions */
	#include "MIMXRT1176_cm7_features.h"
#elif (defined(CPU_MIMXRT1176AVM8A_cm4) || defined(CPU_MIMXRT1176CVM8A_cm4) || defined(CPU_MIMXRT1176DVMAA_cm4))
	#define MIMXRT1176_cm4_SERIES
	/* CMSIS-style register definitions */
	#include "MIMXRT1176_cm4.h"
	/* CPU specific feature definitions */
	#include "MIMXRT1176_cm4_features.h"

#else
	#error "No valid CPU defined!"
#endif

#endif /* __FSL_DEVICE_REGISTERS_H__ */

/*******************************************************************************
 * EOF
 ******************************************************************************/

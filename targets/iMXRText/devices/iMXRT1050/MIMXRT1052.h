/*
** ###################################################################
**     Processors:          MIMXRT1052CVJ5B
**                          MIMXRT1052CVL5B
**                          MIMXRT1052DVJ6B
**                          MIMXRT1052DVL6B
**
**     Compilers:           Freescale C/C++ for Embedded ARM
**                          GNU C Compiler
**                          IAR ANSI C/C++ Compiler for ARM
**                          Keil ARM C/C++ Compiler
**                          MCUXpresso Compiler
**
**     Reference manual:    IMXRT1050RM Rev.5, 07/2021 | IMXRT1050SRM Rev.2
**     Version:             rev. 2.0, 2024-10-29
**     Build:               b250520
**
**     Abstract:
**         CMSIS Peripheral Access Layer for MIMXRT1052
**
**     Copyright 1997-2016 Freescale Semiconductor, Inc.
**     Copyright 2016-2025 NXP
**     SPDX-License-Identifier: BSD-3-Clause
**
**     http:                 www.nxp.com
**     mail:                 support@nxp.com
**
**     Revisions:
**     - rev. 0.1 (2017-01-10)
**         Initial version.
**     - rev. 1.0 (2018-09-21)
**         Update interrupt vector table and dma request source.
**         Update register BEE_ADDR_OFFSET1's bitfield name to ADDR_OFFSET1.
**         Split GPIO_COMBINED_IRQS to GPIO_COMBINED_LOW_IRQS and GPIO_COMBINED_HIGH_IRQS.
**     - rev. 1.1 (2018-11-16)
**         Update header files to align with IMXRT1050RM Rev.1.
**     - rev. 1.2 (2018-11-27)
**         Update header files to align with IMXRT1050RM Rev.2.1.
**     - rev. 1.3 (2019-04-29)
**         Add SET/CLR/TOG register group to register CTRL, STAT, CHANNELCTRL, CH0STAT, CH0OPTS, CH1STAT, CH1OPTS, CH2STAT, CH2OPTS, CH3STAT, CH3OPTS of DCP module.
**     - rev. 1.4 (2021-08-10)
**         Update header files to align with IMXRT1050RM Rev.5.
**     - rev. 2.0 (2024-10-29)
**         Change the device header file from single flat file to multiple files based on peripherals,
**         each peripheral with dedicated header file located in periphN folder.
**
** ###################################################################
*/

/*!
 * @file MIMXRT1052.h
 * @version 2.0
 * @date 2024-10-29
 * @brief CMSIS Peripheral Access Layer for MIMXRT1052
 *
 * CMSIS Peripheral Access Layer for MIMXRT1052
 */

#if !defined(MIMXRT1052_H_)  /* Check if memory map has not been already included */
#define MIMXRT1052_H_

/* IP Header Files List */
#include "./periph/PERI_ADC.h"
#include "./periph/PERI_ADC_ETC.h"
#include "./periph/PERI_AIPSTZ.h"
#include "./periph/PERI_AOI.h"
#include "./periph/PERI_BEE.h"
#include "./periph/PERI_CAN.h"
#include "./periph/PERI_CCM.h"
#include "./periph/PERI_CCM_ANALOG.h"
#include "./periph/PERI_CM7_MCM.h"
#include "./periph/PERI_CMP.h"
#include "./periph/PERI_CSI.h"
#include "./periph/PERI_CSU.h"
#include "./periph/PERI_DCDC.h"
#include "./periph/PERI_DCP.h"
#include "./periph/PERI_DMA.h"
#include "./periph/PERI_DMAMUX.h"
#include "./periph/PERI_ENC.h"
#include "./periph/PERI_ENET.h"
#include "./periph/PERI_EWM.h"
#include "./periph/PERI_FLEXIO.h"
#include "./periph/PERI_FLEXRAM.h"
#include "./periph/PERI_FLEXSPI.h"
#include "./periph/PERI_GPC.h"
#include "./periph/PERI_GPIO.h"
#include "./periph/PERI_GPT.h"
#include "./periph/PERI_I2S.h"
#include "./periph/PERI_IOMUXC.h"
#include "./periph/PERI_IOMUXC_GPR.h"
#include "./periph/PERI_IOMUXC_SNVS.h"
#include "./periph/PERI_IOMUXC_SNVS_GPR.h"
#include "./periph/PERI_KPP.h"
#include "./periph/PERI_LCDIF.h"
#include "./periph/PERI_LPI2C.h"
#include "./periph/PERI_LPSPI.h"
#include "./periph/PERI_LPUART.h"
#include "./periph/PERI_OCOTP.h"
#include "./periph/PERI_PGC.h"
#include "./periph/PERI_PIT.h"
#include "./periph/PERI_PMU.h"
#include "./periph/PERI_PWM.h"
#include "./periph/PERI_PXP.h"
#include "./periph/PERI_ROMC.h"
#include "./periph/PERI_RTWDOG.h"
#include "./periph/PERI_SEMC.h"
#include "./periph/PERI_SNVS.h"
#include "./periph/PERI_SPDIF.h"
#include "./periph/PERI_SRC.h"
#include "./periph/PERI_TEMPMON.h"
#include "./periph/PERI_TMR.h"
#include "./periph/PERI_TRNG.h"
#include "./periph/PERI_TSC.h"
#include "./periph/PERI_USB.h"
#include "./periph/PERI_USBNC.h"
#include "./periph/PERI_USBPHY.h"
#include "./periph/PERI_USB_ANALOG.h"
#include "./periph/PERI_USDHC.h"
#include "./periph/PERI_WDOG.h"
#include "./periph/PERI_XBARA.h"
#include "./periph/PERI_XBARB.h"
#include "./periph/PERI_XTALOSC24M.h"

#endif  /* #if !defined(MIMXRT1052_H_) */

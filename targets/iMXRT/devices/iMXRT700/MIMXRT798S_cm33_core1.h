/*
** ###################################################################
**     Processors:          MIMXRT798SGAWBR_cm33_core1
**                          MIMXRT798SGFOB_cm33_core1
**
**     Compilers:
**                          GNU C Compiler
**                          IAR ANSI C/C++ Compiler for ARM
**                          Keil ARM C/C++ Compiler
**                          MCUXpresso Compiler
**
**     Reference manual:    iMXRT700RM Rev.3, 05/2025
**     Version:             rev. 4.0, 2025-06-06
**     Build:               b250722
**
**     Abstract:
**         CMSIS Peripheral Access Layer for MIMXRT798S_cm33_core1
**
**     Copyright 1997-2016 Freescale Semiconductor, Inc.
**     Copyright 2016-2025 NXP
**     SPDX-License-Identifier: BSD-3-Clause
**
**     http:                 www.nxp.com
**     mail:                 support@nxp.com
**
**     Revisions:
**     - rev. 1.0 (2022-09-15)
**         Initial version.
**     - rev. 2.0 (2024-05-28)
**         Rev2 DraftA.
**     - rev. 3.0 (2024-10-29)
**         Change the device header file from single flat file to multiple files based on peripherals,
**         each peripheral with dedicated header file located in periphN folder.
**     - rev. 4.0 (2025-06-06)
**         B0 initial version
**
** ###################################################################
*/

/*!
 * @file MIMXRT798S_cm33_core1.h
 * @version 4.0
 * @date 2025-06-06
 * @brief CMSIS Peripheral Access Layer for MIMXRT798S_cm33_core1
 *
 * CMSIS Peripheral Access Layer for MIMXRT798S_cm33_core1
 */

#if !defined(MIMXRT798S_cm33_core1_H_)  /* Check if memory map has not been already included */
#define MIMXRT798S_cm33_core1_H_

/* IP Header Files List */
#include "./periph/PERI_ADC.h"
#include "./periph/PERI_AHBSC3.h"
#include "./periph/PERI_AHBSC4.h"
#include "./periph/PERI_CDOG.h"
#include "./periph/PERI_CLKCTL1.h"
#include "./periph/PERI_CLKCTL2.h"
#include "./periph/PERI_CLKCTL3.h"
#include "./periph/PERI_CLKCTL4.h"
#include "./periph/PERI_CMP.h"
#include "./periph/PERI_CTIMER.h"
#include "./periph/PERI_DMA.h"
#include "./periph/PERI_FLEXIO.h"
#include "./periph/PERI_FRO.h"
#include "./periph/PERI_GDET.h"
#include "./periph/PERI_GLIKEY.h"
#include "./periph/PERI_GPIO.h"
#include "./periph/PERI_I2S.h"
#include "./periph/PERI_I3C.h"
#include "./periph/PERI_INPUTMUX1.h"
#include "./periph/PERI_IOPCTL1.h"
#include "./periph/PERI_IOPCTL2.h"
#include "./periph/PERI_JPEGDEC.h"
#include "./periph/PERI_JPGDECWRP.h"
#include "./periph/PERI_LCDIF.h"
#include "./periph/PERI_LPI2C.h"
#include "./periph/PERI_LPSPI.h"
#include "./periph/PERI_LPUART.h"
#include "./periph/PERI_LP_FLEXCOMM.h"
#include "./periph/PERI_MIPI_DSI_HOST.h"
#include "./periph/PERI_MMU.h"
#include "./periph/PERI_MRT.h"
#include "./periph/PERI_MU.h"
#include "./periph/PERI_NIC.h"
#include "./periph/PERI_OSC32KNP.h"
#include "./periph/PERI_OSTIMER.h"
#include "./periph/PERI_PDM.h"
#include "./periph/PERI_PINT.h"
#include "./periph/PERI_PMC.h"
#include "./periph/PERI_PNGDEC.h"
#include "./periph/PERI_PVTS.h"
#include "./periph/PERI_RSTCTL1.h"
#include "./periph/PERI_RSTCTL2.h"
#include "./periph/PERI_RSTCTL3.h"
#include "./periph/PERI_RSTCTL4.h"
#include "./periph/PERI_RTC.h"
#include "./periph/PERI_SCT.h"
#include "./periph/PERI_SDADC.h"
#include "./periph/PERI_SEMA42.h"
#include "./periph/PERI_SLEEPCON1.h"
#include "./periph/PERI_SYSCON1.h"
#include "./periph/PERI_SYSCON2.h"
#include "./periph/PERI_SYSCON3.h"
#include "./periph/PERI_SYSCON4.h"
#include "./periph/PERI_USB.h"
#include "./periph/PERI_USBHSDCD.h"
#include "./periph/PERI_USBNC.h"
#include "./periph/PERI_USBPHY.h"
#include "./periph/PERI_USDHC.h"
#include "./periph/PERI_UTICK.h"
#include "./periph/PERI_WWDT.h"
#include "./periph/PERI_XSPI.h"

#endif  /* #if !defined(MIMXRT798S_cm33_core1_H_) */

/*
** ###################################################################
**     Processors:          MIMXRT595SFAWC_cm33
**                          MIMXRT595SFFOC_cm33
**
**     Compilers:           GNU C Compiler
**                          IAR ANSI C/C++ Compiler for ARM
**                          Keil ARM C/C++ Compiler
**                          MCUXpresso Compiler
**
**     Reference manual:    iMXRT500RM Rev.1, 07/2022
**     Version:             rev. 6.0, 2024-10-29
**     Build:               b250520
**
**     Abstract:
**         CMSIS Peripheral Access Layer for MIMXRT595S_cm33
**
**     Copyright 1997-2016 Freescale Semiconductor, Inc.
**     Copyright 2016-2025 NXP
**     SPDX-License-Identifier: BSD-3-Clause
**
**     http:                 www.nxp.com
**     mail:                 support@nxp.com
**
**     Revisions:
**     - rev. 1.0 (2019-04-19)
**         Initial version.
**     - rev. 2.0 (2019-07-22)
**         Base on rev 0.7 RM.
**     - rev. 3.0 (2020-03-16)
**         Base on Rev.A RM.
**     - rev. 4.0 (2020-05-18)
**         Base on Rev.B RM.
**     - rev. 5.0 (2020-08-27)
**         Base on Rev.C RM.
**     - rev. 6.0 (2024-10-29)
**         Change the device header file from single flat file to multiple files based on peripherals,
**         each peripheral with dedicated header file located in periphN folder.
**
** ###################################################################
*/

/*!
 * @file MIMXRT595S_cm33.h
 * @version 6.0
 * @date 2024-10-29
 * @brief CMSIS Peripheral Access Layer for MIMXRT595S_cm33
 *
 * CMSIS Peripheral Access Layer for MIMXRT595S_cm33
 */

#if !defined(MIMXRT595S_cm33_H_)  /* Check if memory map has not been already included */
#define MIMXRT595S_cm33_H_

/* IP Header Files List */
#include "periph/PERI_ADC.h"
#include "periph/PERI_AHB_SECURE_CTRL.h"
#include "periph/PERI_AXI_SWITCH_AMIB.h"
#include "periph/PERI_AXI_SWITCH_ASIB.h"
#include "periph/PERI_CACHE64_CTRL.h"
#include "periph/PERI_CACHE64_POLSEL.h"
#include "periph/PERI_CASPER.h"
#include "periph/PERI_CLKCTL0.h"
#include "periph/PERI_CLKCTL1.h"
#include "periph/PERI_CMP.h"
#include "periph/PERI_CRC.h"
#include "periph/PERI_CTIMER.h"
#include "periph/PERI_DEBUGGER_MAILBOX.h"
#include "periph/PERI_DMA.h"
#include "periph/PERI_DMIC.h"
#include "periph/PERI_FLEXCOMM.h"
#include "periph/PERI_FLEXIO.h"
#include "periph/PERI_FLEXSPI.h"
#include "periph/PERI_FREQME.h"
#include "periph/PERI_GPIO.h"
#include "periph/PERI_HASHCRYPT.h"
#include "periph/PERI_I2C.h"
#include "periph/PERI_I2S.h"
#include "periph/PERI_I3C.h"
#include "periph/PERI_INPUTMUX.h"
#include "periph/PERI_IOPCTL.h"
#include "periph/PERI_LCDIF.h"
#include "periph/PERI_MIPI_DSI_HOST.h"
#include "periph/PERI_MRT.h"
#include "periph/PERI_MU.h"
#include "periph/PERI_OCOTP.h"
#include "periph/PERI_OSTIMER.h"
#include "periph/PERI_OTFAD.h"
#include "periph/PERI_PINT.h"
#include "periph/PERI_PMC.h"
#include "periph/PERI_POWERQUAD.h"
#include "periph/PERI_PUF.h"
#include "periph/PERI_RSTCTL0.h"
#include "periph/PERI_RSTCTL1.h"
#include "periph/PERI_RTC.h"
#include "periph/PERI_SCT.h"
#include "periph/PERI_SEMA42.h"
#include "periph/PERI_SPI.h"
#include "periph/PERI_SYSCTL0.h"
#include "periph/PERI_SYSCTL1.h"
#include "periph/PERI_TRNG.h"
#include "periph/PERI_USART.h"
#include "periph/PERI_USBHSD.h"
#include "periph/PERI_USBHSDCD.h"
#include "periph/PERI_USBHSH.h"
#include "periph/PERI_USBPHY.h"
#include "periph/PERI_USDHC.h"
#include "periph/PERI_UTICK.h"
#include "periph/PERI_WWDT.h"

#endif  /* #if !defined(MIMXRT595S_cm33_H_) */

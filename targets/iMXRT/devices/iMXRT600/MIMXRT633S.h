/*
** ###################################################################
**     Processors:          MIMXRT633SFAWBR
**                          MIMXRT633SFFOB
**                          MIMXRT633SFVKB
**
**     Compilers:           GNU C Compiler
**                          IAR ANSI C/C++ Compiler for ARM
**                          Keil ARM C/C++ Compiler
**                          MCUXpresso Compiler
**
**     Reference manual:    MIMXRT685 User manual Rev. 1.8 21 November 2024
**     Version:             rev. 4.0, 2025-11-13
**     Build:               b260509
**
**     Abstract:
**         CMSIS Peripheral Access Layer for MIMXRT633S
**
**     Copyright 1997-2016 Freescale Semiconductor, Inc.
**     Copyright 2016-2026 NXP
**     SPDX-License-Identifier: BSD-3-Clause
**
**     http:                 www.nxp.com
**     mail:                 support@nxp.com
**
**     Revisions:
**     - rev. 1.0 (2018-06-19)
**         Initial version.
**     - rev. 2.0 (2019-11-12)
**         Base on rev 0.95 RM (B0 Header)
**     - rev. 3.0 (2024-10-29)
**         Change the device header file from single flat file to multiple files based on peripherals,
**         each peripheral with dedicated header file located in periphN folder.
**     - rev. 4.0 (2025-11-13)
**         Add puf irq and move some trng compatibility macros to common header.
**
** ###################################################################
*/

/*!
 * @file MIMXRT633S.h
 * @version 4.0
 * @date 2025-11-13
 * @brief CMSIS Peripheral Access Layer for MIMXRT633S
 *
 * CMSIS Peripheral Access Layer for MIMXRT633S
 */

#if !defined(MIMXRT633S_H_)  /* Check if memory map has not been already included */
#define MIMXRT633S_H_

/* IP Header Files List */
#include "./periph/PERI_ADC.h"
#include "./periph/PERI_AHB_SECURE_CTRL.h"
#include "./periph/PERI_CACHE64_CTRL.h"
#include "./periph/PERI_CACHE64_POLSEL.h"
#include "./periph/PERI_CASPER.h"
#include "./periph/PERI_CLKCTL0.h"
#include "./periph/PERI_CLKCTL1.h"
#include "./periph/PERI_CMP.h"
#include "./periph/PERI_CRC.h"
#include "./periph/PERI_CTIMER.h"
#include "./periph/PERI_DMA.h"
#include "./periph/PERI_DMIC.h"
#include "./periph/PERI_ESPI.h"
#include "./periph/PERI_FLEXCOMM.h"
#include "./periph/PERI_FLEXSPI.h"
#include "./periph/PERI_FREQME.h"
#include "./periph/PERI_GPIO.h"
#include "./periph/PERI_HASHCRYPT.h"
#include "./periph/PERI_I2C.h"
#include "./periph/PERI_I2S.h"
#include "./periph/PERI_I3C.h"
#include "./periph/PERI_INPUTMUX.h"
#include "./periph/PERI_IOPCTL.h"
#include "./periph/PERI_MRT.h"
#include "./periph/PERI_MU.h"
#include "./periph/PERI_OCOTP.h"
#include "./periph/PERI_OSTIMER.h"
#include "./periph/PERI_OTFAD.h"
#include "./periph/PERI_PINT.h"
#include "./periph/PERI_PMC.h"
#include "./periph/PERI_POWERQUAD.h"
#include "./periph/PERI_PUF.h"
#include "./periph/PERI_RSTCTL0.h"
#include "./periph/PERI_RSTCTL1.h"
#include "./periph/PERI_RTC.h"
#include "./periph/PERI_SCT.h"
#include "./periph/PERI_SEMA42.h"
#include "./periph/PERI_SPI.h"
#include "./periph/PERI_SYSCTL0.h"
#include "./periph/PERI_SYSCTL1.h"
#include "./periph/PERI_TRNG.h"
#include "./periph/PERI_USART.h"
#include "./periph/PERI_USBHSD.h"
#include "./periph/PERI_USBHSDCD.h"
#include "./periph/PERI_USBHSH.h"
#include "./periph/PERI_USBPHY.h"
#include "./periph/PERI_USDHC.h"
#include "./periph/PERI_UTICK.h"
#include "./periph/PERI_WWDT.h"

#endif  /* #if !defined(MIMXRT633S_H_) */

/*
** ###################################################################
**     Processors:          MIMXRT2663AHP8A
**                          MIMXRT2663CHPAA
**                          MIMXRT2663CVVAA
**                          MIMXRT2663DHPAA
**                          MIMXRT2663DVVAA
**                          MIMXRT2663XHP8A
**                          MIMXRT2663XVV8A
**
**     Compilers:
**                          GNU C Compiler
**                          IAR ANSI C/C++ Compiler for ARM
**                          Keil ARM C/C++ Compiler
**                          MCUXpresso Compiler
**
**     Reference manual:    iMXRT2660RM Rev.2 DraftA, 05/2024
**     Version:             rev. 1.0, 2024-11-05
**     Build:               b260603
**
**     Abstract:
**         CMSIS Peripheral Access Layer for MIMXRT2663
**
**     Copyright 1997-2016 Freescale Semiconductor, Inc.
**     Copyright 2016-2026 NXP
**     SPDX-License-Identifier: BSD-3-Clause
**
**     http:                 www.nxp.com
**     mail:                 support@nxp.com
**
**     Revisions:
**     - rev. 1.0 (2024-11-05)
**         Initial version.
**
** ###################################################################
*/

/*!
 * @file MIMXRT2663.h
 * @version 1.0
 * @date 2024-11-05
 * @brief CMSIS Peripheral Access Layer for MIMXRT2663
 *
 * CMSIS Peripheral Access Layer for MIMXRT2663
 */

#if !defined(MIMXRT2663_H_)  /* Check if memory map has not been already included */
#define MIMXRT2663_H_

/* IP Header Files List */
#include "./periph/PERI_ADC.h"
#include "./periph/PERI_AHB_SRAMC.h"
#include "./periph/PERI_ASRC.h"
#include "./periph/PERI_AUDIO_XCVR.h"
#include "./periph/PERI_AXBS.h"
#include "./periph/PERI_CAN.h"
#include "./periph/PERI_CCM.h"
#include "./periph/PERI_CGUANA.h"
#include "./periph/PERI_CMP.h"
#include "./periph/PERI_CRC.h"
#include "./periph/PERI_CSI.h"
#include "./periph/PERI_DCIF.h"
#include "./periph/PERI_DEBUGMAILBOX.h"
#include "./periph/PERI_DIGTMP.h"
#include "./periph/PERI_DMA.h"
#include "./periph/PERI_DMA5.h"
#include "./periph/PERI_DSI2_HOST.h"
#include "./periph/PERI_DSI2_HOST_APB_PKT_IF.h"
#include "./periph/PERI_DSI2_HOST_DBI_IF.h"
#include "./periph/PERI_DSI2_HOST_VID_IF.h"
#include "./periph/PERI_DSI2_TX_PHY.h"
#include "./periph/PERI_ENET.h"
#include "./periph/PERI_ENET_QOS.h"
#include "./periph/PERI_EQDC.h"
#include "./periph/PERI_EVTG.h"
#include "./periph/PERI_EWM.h"
#include "./periph/PERI_FLEXIO.h"
#include "./periph/PERI_FLEXSPI_SLV.h"
#include "./periph/PERI_FREQME.h"
#include "./periph/PERI_FSB.h"
#include "./periph/PERI_GPIO.h"
#include "./periph/PERI_I2S.h"
#include "./periph/PERI_I3C.h"
#include "./periph/PERI_INPUTMUX.h"
#include "./periph/PERI_IOMUXC.h"
#include "./periph/PERI_ISI.h"
#include "./periph/PERI_JPEGDEC.h"
#include "./periph/PERI_JPGDECWRP.h"
#include "./periph/PERI_LLC.h"
#include "./periph/PERI_LPDAC.h"
#include "./periph/PERI_LPI2C.h"
#include "./periph/PERI_LPIT.h"
#include "./periph/PERI_LPSPI.h"
#include "./periph/PERI_LPTMR.h"
#include "./periph/PERI_LPUART.h"
#include "./periph/PERI_MCM.h"
#include "./periph/PERI_MEMCON_GLOBAL.h"
#include "./periph/PERI_MEMCON_SLICE.h"
#include "./periph/PERI_MIPI_CSI2RX.h"
#include "./periph/PERI_MMU.h"
#include "./periph/PERI_MODCON.h"
#include "./periph/PERI_NIC.h"
#include "./periph/PERI_NPU.h"
#include "./periph/PERI_PDCON.h"
#include "./periph/PERI_PDM.h"
#include "./periph/PERI_PMU.h"
#include "./periph/PERI_POWERCON_CMC_CTRL.h"
#include "./periph/PERI_POWERCON_GLOBAL.h"
#include "./periph/PERI_POWERCON_SOC_CTRL.h"
#include "./periph/PERI_POWERCON_SYS_SLEEP_CTRL.h"
#include "./periph/PERI_PWM.h"
#include "./periph/PERI_REFORMATTER.h"
#include "./periph/PERI_RESETCON.h"
#include "./periph/PERI_ROMCP.h"
#include "./periph/PERI_RTC.h"
#include "./periph/PERI_S3MU.h"
#include "./periph/PERI_SINC.h"
#include "./periph/PERI_SRAM.h"
#include "./periph/PERI_SRAMCTL.h"
#include "./periph/PERI_STM.h"
#include "./periph/PERI_SWT.h"
#include "./periph/PERI_TENBASET_PHY.h"
#include "./periph/PERI_TINGPU.h"
#include "./periph/PERI_TMR.h"
#include "./periph/PERI_TPM.h"
#include "./periph/PERI_TRDC.h"
#include "./periph/PERI_TRDC_CMPT.h"
#include "./periph/PERI_TRGSYNC.h"
#include "./periph/PERI_USB.h"
#include "./periph/PERI_USBHS.h"
#include "./periph/PERI_USBHSDCD.h"
#include "./periph/PERI_USBNC.h"
#include "./periph/PERI_USBPHY.h"
#include "./periph/PERI_USDHC.h"
#include "./periph/PERI_VBATCON.h"
#include "./periph/PERI_XBAR0.h"
#include "./periph/PERI_XBAR1.h"
#include "./periph/PERI_XBAR2.h"
#include "./periph/PERI_XBAR3.h"
#include "./periph/PERI_XBAR4.h"
#include "./periph/PERI_XSPI.h"

#endif  /* #if !defined(MIMXRT2663_H_) */

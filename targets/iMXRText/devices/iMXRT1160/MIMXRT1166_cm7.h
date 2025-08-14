/*
** ###################################################################
**     Processors:          MIMXRT1166CVM5A_cm7
**                          MIMXRT1166DVM6A_cm7
**                          MIMXRT1166XVM5A_cm7
**
**     Compilers:           Freescale C/C++ for Embedded ARM
**                          GNU C Compiler
**                          IAR ANSI C/C++ Compiler for ARM
**                          Keil ARM C/C++ Compiler
**                          MCUXpresso Compiler
**
**     Reference manual:    IMXRT1160RM, Rev 0, 03/2021
**     Version:             rev. 1.0, 2024-10-29
**     Build:               b250520
**
**     Abstract:
**         CMSIS Peripheral Access Layer for MIMXRT1166_cm7
**
**     Copyright 1997-2016 Freescale Semiconductor, Inc.
**     Copyright 2016-2025 NXP
**     SPDX-License-Identifier: BSD-3-Clause
**
**     http:                 www.nxp.com
**     mail:                 support@nxp.com
**
**     Revisions:
**     - rev. 0.1 (2020-12-29)
**         Initial version.
**     - rev. 1.0 (2024-10-29)
**         Change the device header file from single flat file to multiple files based on peripherals,
**         each peripheral with dedicated header file located in periphN folder.
**
** ###################################################################
*/

/*!
 * @file MIMXRT1166_cm7.h
 * @version 1.0
 * @date 2024-10-29
 * @brief CMSIS Peripheral Access Layer for MIMXRT1166_cm7
 *
 * CMSIS Peripheral Access Layer for MIMXRT1166_cm7
 */

#if !defined(MIMXRT1166_cm7_H_)  /* Check if memory map has not been already included */
#define MIMXRT1166_cm7_H_

/* IP Header Files List */
#include "./periph/PERI_ADC.h"
#include "./periph/PERI_ADC_ETC.h"
#include "./periph/PERI_ANADIG_LDO_SNVS.h"
#include "./periph/PERI_ANADIG_LDO_SNVS_DIG.h"
#include "./periph/PERI_ANADIG_MISC.h"
#include "./periph/PERI_ANADIG_OSC.h"
#include "./periph/PERI_ANADIG_PLL.h"
#include "./periph/PERI_ANADIG_PMU.h"
#include "./periph/PERI_ANADIG_TEMPSENSOR.h"
#include "./periph/PERI_AOI.h"
#include "./periph/PERI_ASRC.h"
#include "./periph/PERI_AUDIO_PLL.h"
#include "./periph/PERI_CAAM.h"
#include "./periph/PERI_CAN.h"
#include "./periph/PERI_CAN_WRAPPER.h"
#include "./periph/PERI_CCM.h"
#include "./periph/PERI_CCM_OBS.h"
#include "./periph/PERI_CDOG.h"
#include "./periph/PERI_CMP.h"
#include "./periph/PERI_CSI.h"
#include "./periph/PERI_DAC.h"
#include "./periph/PERI_DCDC.h"
#include "./periph/PERI_DCIC.h"
#include "./periph/PERI_DMA.h"
#include "./periph/PERI_DMAMUX.h"
#include "./periph/PERI_DSI_HOST.h"
#include "./periph/PERI_DSI_HOST_APB_PKT_IF.h"
#include "./periph/PERI_DSI_HOST_DPI_INTFC.h"
#include "./periph/PERI_DSI_HOST_NXP_FDSOI28_DPHY_INTFC.h"
#include "./periph/PERI_EMVSIM.h"
#include "./periph/PERI_ENC.h"
#include "./periph/PERI_ENET.h"
#include "./periph/PERI_ETHERNET_PLL.h"
#include "./periph/PERI_EWM.h"
#include "./periph/PERI_FLEXIO.h"
#include "./periph/PERI_FLEXRAM.h"
#include "./periph/PERI_FLEXSPI.h"
#include "./periph/PERI_GPC_CPU_MODE_CTRL.h"
#include "./periph/PERI_GPC_SET_POINT_CTRL.h"
#include "./periph/PERI_GPC_STBY_CTRL.h"
#include "./periph/PERI_GPIO.h"
#include "./periph/PERI_GPT.h"
#include "./periph/PERI_I2S.h"
#include "./periph/PERI_IEE.h"
#include "./periph/PERI_IEE_APC.h"
#include "./periph/PERI_IOMUXC.h"
#include "./periph/PERI_IOMUXC_GPR.h"
#include "./periph/PERI_IOMUXC_LPSR.h"
#include "./periph/PERI_IOMUXC_LPSR_GPR.h"
#include "./periph/PERI_IOMUXC_SNVS.h"
#include "./periph/PERI_IOMUXC_SNVS_GPR.h"
#include "./periph/PERI_IPS_DOMAIN.h"
#include "./periph/PERI_KEY_MANAGER.h"
#include "./periph/PERI_KPP.h"
#include "./periph/PERI_LCDIF.h"
#include "./periph/PERI_LCDIFV2.h"
#include "./periph/PERI_LPI2C.h"
#include "./periph/PERI_LPSPI.h"
#include "./periph/PERI_LPUART.h"
#include "./periph/PERI_MCM.h"
#include "./periph/PERI_MECC.h"
#include "./periph/PERI_MIPI_CSI2RX.h"
#include "./periph/PERI_MU.h"
#include "./periph/PERI_OCOTP.h"
#include "./periph/PERI_OSC_RC_400M.h"
#include "./periph/PERI_OTFAD.h"
#include "./periph/PERI_PDM.h"
#include "./periph/PERI_PGMC_BPC.h"
#include "./periph/PERI_PGMC_CPC.h"
#include "./periph/PERI_PGMC_MIF.h"
#include "./periph/PERI_PGMC_PPC.h"
#include "./periph/PERI_PHY_LDO.h"
#include "./periph/PERI_PIT.h"
#include "./periph/PERI_PUF.h"
#include "./periph/PERI_PWM.h"
#include "./periph/PERI_PXP.h"
#include "./periph/PERI_RDC.h"
#include "./periph/PERI_RDC_SEMAPHORE.h"
#include "./periph/PERI_RTWDOG.h"
#include "./periph/PERI_SEMA4.h"
#include "./periph/PERI_SEMC.h"
#include "./periph/PERI_SNVS.h"
#include "./periph/PERI_SPDIF.h"
#include "./periph/PERI_SRAM.h"
#include "./periph/PERI_SRC.h"
#include "./periph/PERI_SSARC_HP.h"
#include "./periph/PERI_SSARC_LP.h"
#include "./periph/PERI_TMPSNS.h"
#include "./periph/PERI_TMR.h"
#include "./periph/PERI_USB.h"
#include "./periph/PERI_USBHSDCD.h"
#include "./periph/PERI_USBNC.h"
#include "./periph/PERI_USBPHY.h"
#include "./periph/PERI_USDHC.h"
#include "./periph/PERI_VIDEO_MUX.h"
#include "./periph/PERI_VIDEO_PLL.h"
#include "./periph/PERI_VMBANDGAP.h"
#include "./periph/PERI_WDOG.h"
#include "./periph/PERI_XBARA.h"
#include "./periph/PERI_XBARB.h"
#include "./periph/PERI_XECC.h"
#include "./periph/PERI_XRDC2.h"

#endif  /* #if !defined(MIMXRT1166_cm7_H_) */

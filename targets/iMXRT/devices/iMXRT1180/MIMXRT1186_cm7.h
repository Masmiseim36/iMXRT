/*
** ###################################################################
**     Processors:          MIMXRT1186CVJ8C_cm7
**                          MIMXRT1186XVJ8C_cm7
**
**     Compilers:           GNU C Compiler
**                          IAR ANSI C/C++ Compiler for ARM
**                          Keil ARM C/C++ Compiler
**                          MCUXpresso Compiler
**
**     Reference manual:    IMXRT1180RM, Rev 5, 01/2024
**     Version:             rev. 3.0, 2024-10-29
**     Build:               b250611
**
**     Abstract:
**         CMSIS Peripheral Access Layer for MIMXRT1186_cm7
**
**     Copyright 1997-2016 Freescale Semiconductor, Inc.
**     Copyright 2016-2025 NXP
**     SPDX-License-Identifier: BSD-3-Clause
**
**     http:                 www.nxp.com
**     mail:                 support@nxp.com
**
**     Revisions:
**     - rev. 1.0 (2021-03-09)
**         Initial version.
**     - rev. 2.0 (2024-01-18)
**         Header RFP.
**     - rev. 3.0 (2024-10-29)
**         Change the device header file from single flat file to multiple files based on peripherals,
**         each peripheral with dedicated header file located in periphN folder.
**
** ###################################################################
*/

/*!
 * @file MIMXRT1186_cm7.h
 * @version 3.0
 * @date 2024-10-29
 * @brief CMSIS Peripheral Access Layer for MIMXRT1186_cm7
 *
 * CMSIS Peripheral Access Layer for MIMXRT1186_cm7
 */

#if !defined(MIMXRT1186_cm7_H_)  /* Check if memory map has not been already included */
#define MIMXRT1186_cm7_H_

/* IP Header Files List */
#include "./periph/PERI_ADC.h"
#include "./periph/PERI_ANADIG.h"
#include "./periph/PERI_ANADIG_LDO_BBSM.h"
#include "./periph/PERI_ANADIG_OSC.h"
#include "./periph/PERI_ANADIG_PLL.h"
#include "./periph/PERI_ANADIG_PMU.h"
#include "./periph/PERI_ANADIG_TEMPSENSOR.h"
#include "./periph/PERI_AOI.h"
#include "./periph/PERI_ASRC.h"
#include "./periph/PERI_AXBS.h"
#include "./periph/PERI_BBNSM.h"
#include "./periph/PERI_BLK_CTRL_BBSMMIX.h"
#include "./periph/PERI_BLK_CTRL_NS_AONMIX.h"
#include "./periph/PERI_BLK_CTRL_S_AONMIX.h"
#include "./periph/PERI_BLK_CTRL_WAKEUPMIX.h"
#include "./periph/PERI_CACHE_ECC_MCM.h"
#include "./periph/PERI_CAN.h"
#include "./periph/PERI_CCM.h"
#include "./periph/PERI_CMP.h"
#include "./periph/PERI_DAC.h"
#include "./periph/PERI_DCDC.h"
#include "./periph/PERI_DMA.h"
#include "./periph/PERI_DMA4.h"
#include "./periph/PERI_ECAT.h"
#include "./periph/PERI_EIM.h"
#include "./periph/PERI_ENETC_GLOBAL.h"
#include "./periph/PERI_ENETC_PCI_TYPE0.h"
#include "./periph/PERI_ENETC_PF_EMDIO.h"
#include "./periph/PERI_ENETC_PF_TMR.h"
#include "./periph/PERI_ENETC_SI.h"
#include "./periph/PERI_ENETC_VF_PCI_TYPE0.h"
#include "./periph/PERI_EQDC.h"
#include "./periph/PERI_ERM.h"
#include "./periph/PERI_EWM.h"
#include "./periph/PERI_FLEXIO.h"
#include "./periph/PERI_FLEXSPI.h"
#include "./periph/PERI_FLEXSPI_SLV.h"
#include "./periph/PERI_GPC_CPU_CTRL.h"
#include "./periph/PERI_GPC_GLOBAL.h"
#include "./periph/PERI_GPC_SYS_SLEEP_CTRL.h"
#include "./periph/PERI_GPT.h"
#include "./periph/PERI_I2S.h"
#include "./periph/PERI_I3C.h"
#include "./periph/PERI_IEE.h"
#include "./periph/PERI_IEE_APC.h"
#include "./periph/PERI_IERC_IERB.h"
#include "./periph/PERI_IERC_PCI.h"
#include "./periph/PERI_IOMUXC.h"
#include "./periph/PERI_IOMUXC_AON.h"
#include "./periph/PERI_KPP.h"
#include "./periph/PERI_LPI2C.h"
#include "./periph/PERI_LPIT.h"
#include "./periph/PERI_LPSPI.h"
#include "./periph/PERI_LPTMR.h"
#include "./periph/PERI_LPUART.h"
#include "./periph/PERI_M7_MCM.h"
#include "./periph/PERI_MECC.h"
#include "./periph/PERI_MSGINTR.h"
#include "./periph/PERI_MU.h"
#include "./periph/PERI_NETC_ENETC.h"
#include "./periph/PERI_NETC_ETH_LINK.h"
#include "./periph/PERI_NETC_IERB.h"
#include "./periph/PERI_NETC_PORT.h"
#include "./periph/PERI_NETC_PRIV.h"
#include "./periph/PERI_NETC_PSEUDO_LINK.h"
#include "./periph/PERI_NETC_SW.h"
#include "./periph/PERI_NETC_SW_ENETC.h"
#include "./periph/PERI_OCOTP_FSB.h"
#include "./periph/PERI_OSC_RC_400M.h"
#include "./periph/PERI_OTFAD.h"
#include "./periph/PERI_PDM.h"
#include "./periph/PERI_PHY_LDO.h"
#include "./periph/PERI_PLL.h"
#include "./periph/PERI_PWM.h"
#include "./periph/PERI_RGPIO.h"
#include "./periph/PERI_RTWDOG.h"
#include "./periph/PERI_S3MU.h"
#include "./periph/PERI_SEMA42.h"
#include "./periph/PERI_SEMC.h"
#include "./periph/PERI_SINC.h"
#include "./periph/PERI_SPDIF.h"
#include "./periph/PERI_SRC_GENERAL.h"
#include "./periph/PERI_SRC_MIF_LN28FDSOI_SPLLRAM.h"
#include "./periph/PERI_SRC_MIF_S28SPREGH.h"
#include "./periph/PERI_SRC_MIF_S28SPREGH_PSWA.h"
#include "./periph/PERI_SRC_MIX_SLICE.h"
#include "./periph/PERI_SYSPM.h"
#include "./periph/PERI_SYS_CTR_COMPARE.h"
#include "./periph/PERI_SYS_CTR_CONTROL.h"
#include "./periph/PERI_SYS_CTR_READ.h"
#include "./periph/PERI_TCM_ECC_MCM.h"
#include "./periph/PERI_TMPSNS.h"
#include "./periph/PERI_TMR.h"
#include "./periph/PERI_TPM.h"
#include "./periph/PERI_TRDC.h"
#include "./periph/PERI_TSTMR.h"
#include "./periph/PERI_USB.h"
#include "./periph/PERI_USBHSDCD.h"
#include "./periph/PERI_USBNC.h"
#include "./periph/PERI_USBPHY.h"
#include "./periph/PERI_USDHC.h"
#include "./periph/PERI_VMBANDGAP.h"
#include "./periph/PERI_VREF.h"
#include "./periph/PERI_XBAR_NUM_OUT221.h"
#include "./periph/PERI_XBAR_NUM_OUT32.h"

#endif  /* #if !defined(MIMXRT1186_cm7_H_) */

/*
 * Copyright 2016 Freescale Semiconductor, Inc.
 * Copyright 2016-2021 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_IOMUXC_H_
#define _FSL_IOMUXC_H_

#include "fsl_common.h"

/*!
 * @addtogroup iomuxc_driver
 * @{
 */

/*! @file */

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.iomuxc"
#endif

/*! @name Driver version */
/*@{*/
/*! @brief IOMUXC driver version 2.0.3. */
#define FSL_IOMUXC_DRIVER_VERSION (MAKE_VERSION(2, 0, 4))
/*@}*/

/*!
 * @name Pin function ID
 * The pin function ID is a tuple of \<muxRegister muxMode inputRegister inputDaisy configRegister\>
 *
 * @{
 */
#define IOMUXC_GPIO_EMC_04_XBAR1_XBAR_INOUT04   0x401F8024U, 0x1U, 0, 0, 0x401F8198U
#define IOMUXC_GPIO_EMC_04_SPDIF_OUT            0x401F8024U, 0x2U, 0, 0, 0x401F8198U
#define IOMUXC_GPIO_EMC_04_SAI2_TX_BCLK         0x401F8024U, 0x3U, 0x401F8464U, 0x1U, 0x401F8198U
#define IOMUXC_GPIO_EMC_04_FLEXIO1_FLEXIO16     0x401F8024U, 0x4U, 0, 0, 0x401F8198U
#define IOMUXC_GPIO_EMC_04_GPIO2_IO04           0x401F8024U, 0x5U, 0, 0, 0x401F8198U
#define IOMUXC_GPIO_EMC_04_SJC_JTAG_ACT         0x401F8024U, 0x7U, 0, 0, 0x401F8198U

#define IOMUXC_GPIO_EMC_05_XBAR1_XBAR_INOUT05   0x401F8028U, 0x1U, 0, 0, 0x401F819CU
#define IOMUXC_GPIO_EMC_05_SPDIF_IN             0x401F8028U, 0x2U, 0x401F8488U, 0x0U, 0x401F819CU
#define IOMUXC_GPIO_EMC_05_SAI2_TX_SYNC         0x401F8028U, 0x3U, 0x401F8468U, 0x1U, 0x401F819CU
#define IOMUXC_GPIO_EMC_05_FLEXIO1_FLEXIO17     0x401F8028U, 0x4U, 0, 0, 0x401F819CU
#define IOMUXC_GPIO_EMC_05_GPIO2_IO05           0x401F8028U, 0x5U, 0, 0, 0x401F819CU
#define IOMUXC_GPIO_EMC_05_SJC_DE_B             0x401F8028U, 0x7U, 0, 0, 0x401F819CU

#define IOMUXC_GPIO_EMC_06_XBAR1_XBAR_INOUT06   0x401F802CU, 0x1U, 0, 0, 0x401F81A0U
#define IOMUXC_GPIO_EMC_06_LPUART3_TX           0x401F802CU, 0x2U, 0x401F83DCU, 0x0U, 0x401F81A0U
#define IOMUXC_GPIO_EMC_06_SAI2_TX_DATA         0x401F802CU, 0x3U, 0, 0, 0x401F81A0U
#define IOMUXC_GPIO_EMC_06_FLEXIO1_FLEXIO18     0x401F802CU, 0x4U, 0, 0, 0x401F81A0U
#define IOMUXC_GPIO_EMC_06_GPIO2_IO06           0x401F802CU, 0x5U, 0, 0, 0x401F81A0U

#define IOMUXC_GPIO_EMC_07_XBAR1_XBAR_INOUT07   0x401F8030U, 0x1U, 0, 0, 0x401F81A4U
#define IOMUXC_GPIO_EMC_07_LPUART3_RX           0x401F8030U, 0x2U, 0x401F83D8U, 0x0U, 0x401F81A4U
#define IOMUXC_GPIO_EMC_07_SAI2_RX_SYNC         0x401F8030U, 0x3U, 0x401F8460U, 0x1U, 0x401F81A4U
#define IOMUXC_GPIO_EMC_07_FLEXIO1_FLEXIO19     0x401F8030U, 0x4U, 0, 0, 0x401F81A4U
#define IOMUXC_GPIO_EMC_07_GPIO2_IO07           0x401F8030U, 0x5U, 0, 0, 0x401F81A4U

#define IOMUXC_GPIO_EMC_08_XBAR1_XBAR_INOUT08   0x401F8034U, 0x1U, 0, 0, 0x401F81A8U
#define IOMUXC_GPIO_EMC_08_SAI2_RX_DATA         0x401F8034U, 0x3U, 0x401F845CU, 0x1U, 0x401F81A8U
#define IOMUXC_GPIO_EMC_08_FLEXIO1_FLEXIO20     0x401F8034U, 0x4U, 0, 0, 0x401F81A8U
#define IOMUXC_GPIO_EMC_08_GPIO2_IO08           0x401F8034U, 0x5U, 0, 0, 0x401F81A8U

#define IOMUXC_GPIO_EMC_09_XBAR1_XBAR_INOUT09   0x401F8038U, 0x1U, 0, 0, 0x401F81ACU
#define IOMUXC_GPIO_EMC_09_SAI2_RX_BCLK         0x401F8038U, 0x3U, 0x401F8458U, 0x1U, 0x401F81ACU
#define IOMUXC_GPIO_EMC_09_FLEXIO1_FLEXIO21     0x401F8038U, 0x4U, 0, 0, 0x401F81ACU
#define IOMUXC_GPIO_EMC_09_GPIO2_IO09           0x401F8038U, 0x5U, 0, 0, 0x401F81ACU

#define IOMUXC_GPIO_EMC_16_MQS_RIGHT            0x401F8054U, 0x2U, 0, 0, 0x401F81C8U
#define IOMUXC_GPIO_EMC_16_SAI2_MCLK            0x401F8054U, 0x3U, 0x401F8454U, 0x1U, 0x401F81C8U
#define IOMUXC_GPIO_EMC_16_GPIO2_IO16           0x401F8054U, 0x5U, 0, 0, 0x401F81C8U
#define IOMUXC_GPIO_EMC_16_SRC_BOOT_MODE00      0x401F8054U, 0x6U, 0, 0, 0x401F81C8U

#define IOMUXC_GPIO_EMC_17_MQS_LEFT             0x401F8058U, 0x2U, 0, 0, 0x401F81CCU
#define IOMUXC_GPIO_EMC_17_SAI3_MCLK            0x401F8058U, 0x3U, 0x401F846CU, 0x1U, 0x401F81CCU
#define IOMUXC_GPIO_EMC_17_GPIO2_IO17           0x401F8058U, 0x5U, 0, 0, 0x401F81CCU
#define IOMUXC_GPIO_EMC_17_SRC_BOOT_MODE01      0x401F8058U, 0x6U, 0, 0, 0x401F81CCU

#define IOMUXC_GPIO_EMC_18_XBAR1_XBAR_INOUT16   0x401F805CU, 0x1U, 0x401F84A8U, 0x1U, 0x401F81D0U
#define IOMUXC_GPIO_EMC_18_LPI2C2_SDA           0x401F805CU, 0x2U, 0x401F8388U, 0x1U, 0x401F81D0U
#define IOMUXC_GPIO_EMC_18_SAI1_RX_SYNC         0x401F805CU, 0x3U, 0x401F8448U, 0x2U, 0x401F81D0U
#define IOMUXC_GPIO_EMC_18_FLEXIO1_FLEXIO22     0x401F805CU, 0x4U, 0, 0, 0x401F81D0U
#define IOMUXC_GPIO_EMC_18_GPIO2_IO18           0x401F805CU, 0x5U, 0, 0, 0x401F81D0U
#define IOMUXC_GPIO_EMC_18_SRC_BT_CFG00         0x401F805CU, 0x6U, 0, 0, 0x401F81D0U

#define IOMUXC_GPIO_EMC_19_XBAR1_XBAR_INOUT17   0x401F8060U, 0x1U, 0x401F84ACU, 0x1U, 0x401F81D4U
#define IOMUXC_GPIO_EMC_19_LPI2C2_SCL           0x401F8060U, 0x2U, 0x401F8384U, 0x1U, 0x401F81D4U
#define IOMUXC_GPIO_EMC_19_SAI1_RX_BCLK         0x401F8060U, 0x3U, 0x401F8434U, 0x2U, 0x401F81D4U
#define IOMUXC_GPIO_EMC_19_FLEXIO1_FLEXIO23     0x401F8060U, 0x4U, 0, 0, 0x401F81D4U
#define IOMUXC_GPIO_EMC_19_GPIO2_IO19           0x401F8060U, 0x5U, 0, 0, 0x401F81D4U
#define IOMUXC_GPIO_EMC_19_SRC_BT_CFG01         0x401F8060U, 0x6U, 0, 0, 0x401F81D4U

#define IOMUXC_GPIO_EMC_20_FLEXPWM1_PWMA03      0x401F8064U, 0x1U, 0x401F8334U, 0x1U, 0x401F81D8U
#define IOMUXC_GPIO_EMC_20_LPUART2_CTS_B        0x401F8064U, 0x2U, 0x401F83CCU, 0x1U, 0x401F81D8U
#define IOMUXC_GPIO_EMC_20_SAI1_MCLK            0x401F8064U, 0x3U, 0x401F8430U, 0x3U, 0x401F81D8U
#define IOMUXC_GPIO_EMC_20_FLEXIO1_FLEXIO24     0x401F8064U, 0x4U, 0, 0, 0x401F81D8U
#define IOMUXC_GPIO_EMC_20_GPIO2_IO20           0x401F8064U, 0x5U, 0, 0, 0x401F81D8U
#define IOMUXC_GPIO_EMC_20_SRC_BT_CFG02         0x401F8064U, 0x6U, 0, 0, 0x401F81D8U

#define IOMUXC_GPIO_EMC_21_FLEXPWM1_PWMB03      0x401F8068U, 0x1U, 0x401F8344U, 0x1U, 0x401F81DCU
#define IOMUXC_GPIO_EMC_21_LPUART2_RTS_B        0x401F8068U, 0x2U, 0, 0, 0x401F81DCU
#define IOMUXC_GPIO_EMC_21_SAI1_RX_DATA00       0x401F8068U, 0x3U, 0x401F8438U, 0x2U, 0x401F81DCU
#define IOMUXC_GPIO_EMC_21_FLEXIO1_FLEXIO25     0x401F8068U, 0x4U, 0, 0, 0x401F81DCU
#define IOMUXC_GPIO_EMC_21_GPIO2_IO21           0x401F8068U, 0x5U, 0, 0, 0x401F81DCU
#define IOMUXC_GPIO_EMC_21_SRC_BT_CFG03         0x401F8068U, 0x6U, 0, 0, 0x401F81DCU

#define IOMUXC_GPIO_EMC_22_FLEXPWM1_PWMA02      0x401F806CU, 0x1U, 0x401F8330U, 0x1U, 0x401F81E0U
#define IOMUXC_GPIO_EMC_22_LPUART2_TX           0x401F806CU, 0x2U, 0x401F83D4U, 0x1U, 0x401F81E0U
#define IOMUXC_GPIO_EMC_22_SAI1_TX_DATA03       0x401F806CU, 0x3U, 0x401F843CU, 0x1U, 0x401F81E0U
#define IOMUXC_GPIO_EMC_22_FLEXIO1_FLEXIO26     0x401F806CU, 0x4U, 0, 0, 0x401F81E0U
#define IOMUXC_GPIO_EMC_22_GPIO2_IO22           0x401F806CU, 0x5U, 0, 0, 0x401F81E0U
#define IOMUXC_GPIO_EMC_22_SRC_BT_CFG04         0x401F806CU, 0x6U, 0, 0, 0x401F81E0U

#define IOMUXC_GPIO_EMC_23_FLEXPWM1_PWMB02      0x401F8070U, 0x1U, 0x401F8340U, 0x1U, 0x401F81E4U
#define IOMUXC_GPIO_EMC_23_LPUART2_RX           0x401F8070U, 0x2U, 0x401F83D0U, 0x1U, 0x401F81E4U
#define IOMUXC_GPIO_EMC_23_SAI1_TX_DATA02       0x401F8070U, 0x3U, 0x401F8440U, 0x1U, 0x401F81E4U
#define IOMUXC_GPIO_EMC_23_FLEXIO1_FLEXIO27     0x401F8070U, 0x4U, 0, 0, 0x401F81E4U
#define IOMUXC_GPIO_EMC_23_GPIO2_IO23           0x401F8070U, 0x5U, 0, 0, 0x401F81E4U
#define IOMUXC_GPIO_EMC_23_SRC_BT_CFG05         0x401F8070U, 0x6U, 0, 0, 0x401F81E4U

#define IOMUXC_GPIO_EMC_24_FLEXPWM1_PWMA01      0x401F8074U, 0x1U, 0x401F832CU, 0x1U, 0x401F81E8U
#define IOMUXC_GPIO_EMC_24_SAI1_TX_DATA01       0x401F8074U, 0x3U, 0x401F8444U, 0x1U, 0x401F81E8U
#define IOMUXC_GPIO_EMC_24_FLEXIO1_FLEXIO28     0x401F8074U, 0x4U, 0, 0, 0x401F81E8U
#define IOMUXC_GPIO_EMC_24_GPIO2_IO24           0x401F8074U, 0x5U, 0, 0, 0x401F81E8U
#define IOMUXC_GPIO_EMC_24_SRC_BT_CFG06         0x401F8074U, 0x6U, 0, 0, 0x401F81E8U

#define IOMUXC_GPIO_EMC_25_FLEXPWM1_PWMB01      0x401F8078U, 0x1U, 0x401F833CU, 0x1U, 0x401F81ECU
#define IOMUXC_GPIO_EMC_25_SAI1_TX_DATA00       0x401F8078U, 0x3U, 0, 0, 0x401F81ECU
#define IOMUXC_GPIO_EMC_25_FLEXIO1_FLEXIO29     0x401F8078U, 0x4U, 0, 0, 0x401F81ECU
#define IOMUXC_GPIO_EMC_25_GPIO2_IO25           0x401F8078U, 0x5U, 0, 0, 0x401F81ECU
#define IOMUXC_GPIO_EMC_25_SRC_BT_CFG07         0x401F8078U, 0x6U, 0, 0, 0x401F81ECU

#define IOMUXC_GPIO_EMC_26_FLEXPWM1_PWMA00      0x401F807CU, 0x1U, 0x401F8328U, 0x1U, 0x401F81F0U
#define IOMUXC_GPIO_EMC_26_SAI1_TX_BCLK         0x401F807CU, 0x3U, 0x401F844CU, 0x2U, 0x401F81F0U
#define IOMUXC_GPIO_EMC_26_FLEXIO1_FLEXIO30     0x401F807CU, 0x4U, 0, 0, 0x401F81F0U
#define IOMUXC_GPIO_EMC_26_GPIO2_IO26           0x401F807CU, 0x5U, 0, 0, 0x401F81F0U
#define IOMUXC_GPIO_EMC_26_SRC_BT_CFG08         0x401F807CU, 0x6U, 0, 0, 0x401F81F0U

#define IOMUXC_GPIO_EMC_27_FLEXPWM1_PWMB00      0x401F8080U, 0x1U, 0x401F8338U, 0x1U, 0x401F81F4U
#define IOMUXC_GPIO_EMC_27_SAI1_TX_SYNC         0x401F8080U, 0x3U, 0x401F8450U, 0x2U, 0x401F81F4U
#define IOMUXC_GPIO_EMC_27_FLEXIO1_FLEXIO31     0x401F8080U, 0x4U, 0, 0, 0x401F81F4U
#define IOMUXC_GPIO_EMC_27_GPIO2_IO27           0x401F8080U, 0x5U, 0, 0, 0x401F81F4U
#define IOMUXC_GPIO_EMC_27_SRC_BT_CFG09         0x401F8080U, 0x6U, 0, 0, 0x401F81F4U

#define IOMUXC_GPIO_EMC_32_QTIMER1_TIMER0       0x401F8094U, 0x1U, 0x401F8410U, 0x1U, 0x401F8208U
#define IOMUXC_GPIO_EMC_32_LPUART4_TX           0x401F8094U, 0x2U, 0x401F83E8U, 0x2U, 0x401F8208U
#define IOMUXC_GPIO_EMC_32_SAI3_TX_DATA         0x401F8094U, 0x3U, 0, 0, 0x401F8208U
#define IOMUXC_GPIO_EMC_32_GPIO3_IO00           0x401F8094U, 0x5U, 0, 0, 0x401F8208U
#define IOMUXC_GPIO_EMC_32_REF_24M_OUT          0x401F8094U, 0x7U, 0, 0, 0x401F8208U

#define IOMUXC_GPIO_EMC_33_QTIMER1_TIMER1       0x401F8098U, 0x1U, 0x401F8414U, 0x1U, 0x401F820CU
#define IOMUXC_GPIO_EMC_33_LPUART4_RX           0x401F8098U, 0x2U, 0x401F83E4U, 0x2U, 0x401F820CU
#define IOMUXC_GPIO_EMC_33_SAI3_TX_BCLK         0x401F8098U, 0x3U, 0x401F847CU, 0x1U, 0x401F820CU
#define IOMUXC_GPIO_EMC_33_GPIO3_IO01           0x401F8098U, 0x5U, 0, 0, 0x401F820CU

#define IOMUXC_GPIO_EMC_34_QTIMER1_TIMER2       0x401F809CU, 0x1U, 0x401F8418U, 0x1U, 0x401F8210U
#define IOMUXC_GPIO_EMC_34_SAI3_TX_SYNC         0x401F809CU, 0x3U, 0x401F8480U, 0x1U, 0x401F8210U
#define IOMUXC_GPIO_EMC_34_GPIO3_IO02           0x401F809CU, 0x5U, 0, 0, 0x401F8210U

#define IOMUXC_GPIO_EMC_35_QTIMER1_TIMER3       0x401F80A0U, 0x1U, 0x401F841CU, 0x1U, 0x401F8214U
#define IOMUXC_GPIO_EMC_35_GPIO3_IO03           0x401F80A0U, 0x5U, 0, 0, 0x401F8214U

#define IOMUXC_GPIO_AD_B0_00_JTAG_MUX_TMS       0x401F80BCU, 0x0U, 0, 0, 0x401F8230U
#define IOMUXC_GPIO_AD_B0_00_GPIO1_IO00         0x401F80BCU, 0x5U, 0, 0, 0x401F8230U
#define IOMUXC_GPIO_AD_B0_00_GPT1_COMPARE1      0x401F80BCU, 0x7U, 0, 0, 0x401F8230U

#define IOMUXC_GPIO_AD_B0_01_JTAG_MUX_TCK       0x401F80C0U, 0x0U, 0, 0, 0x401F8234U
#define IOMUXC_GPIO_AD_B0_01_GPIO1_IO01         0x401F80C0U, 0x5U, 0, 0, 0x401F8234U
#define IOMUXC_GPIO_AD_B0_01_GPT1_CAPTURE2      0x401F80C0U, 0x7U, 0, 0, 0x401F8234U

#define IOMUXC_GPIO_AD_B0_02_JTAG_MUX_MOD       0x401F80C4U, 0x0U, 0, 0, 0x401F8238U
#define IOMUXC_GPIO_AD_B0_02_GPIO1_IO02         0x401F80C4U, 0x5U, 0, 0, 0x401F8238U
#define IOMUXC_GPIO_AD_B0_02_GPT1_CAPTURE1      0x401F80C4U, 0x7U, 0, 0, 0x401F8238U

#define IOMUXC_GPIO_AD_B0_03_JTAG_MUX_TDI       0x401F80C8U, 0x0U, 0, 0, 0x401F823CU
#define IOMUXC_GPIO_AD_B0_03_WDOG1_WDOG_B       0x401F80C8U, 0x2U, 0, 0, 0x401F823CU
#define IOMUXC_GPIO_AD_B0_03_SAI1_MCLK          0x401F80C8U, 0x3U, 0x401F8430U, 0x1U, 0x401F823CU
#define IOMUXC_GPIO_AD_B0_03_GPIO1_IO03         0x401F80C8U, 0x5U, 0, 0, 0x401F823CU
#define IOMUXC_GPIO_AD_B0_03_USB_OTG1_OC        0x401F80C8U, 0x6U, 0x401F848CU, 0x0U, 0x401F823CU
#define IOMUXC_GPIO_AD_B0_03_CCM_PMIC_RDY       0x401F80C8U, 0x7U, 0x401F8300U, 0x2U, 0x401F823CU

#define IOMUXC_GPIO_AD_B0_04_JTAG_MUX_TDO       0x401F80CCU, 0x0U, 0, 0, 0x401F8240U
#define IOMUXC_GPIO_AD_B0_04_GPIO1_IO04         0x401F80CCU, 0x5U, 0, 0, 0x401F8240U
#define IOMUXC_GPIO_AD_B0_04_USB_OTG1_PWR       0x401F80CCU, 0x6U, 0, 0, 0x401F8240U
#define IOMUXC_GPIO_AD_B0_04_EWM_EWM_OUT_B      0x401F80CCU, 0x7U, 0, 0, 0x401F8240U

#define IOMUXC_GPIO_AD_B0_05_JTAG_MUX_TRSTB     0x401F80D0U, 0x0U, 0, 0, 0x401F8244U
#define IOMUXC_GPIO_AD_B0_05_GPIO1_IO05         0x401F80D0U, 0x5U, 0, 0, 0x401F8244U
#define IOMUXC_GPIO_AD_B0_05_USB_OTG1_ID        0x401F80D0U, 0x6U, 0x401F82FCU, 0x0U, 0x401F8244U
#define IOMUXC_GPIO_AD_B0_05_NMI_GLUE_NMI       0x401F80D0U, 0x7U, 0x401F840CU, 0x0U, 0x401F8244U

#define IOMUXC_GPIO_AD_B0_06_PIT_TRIGGER00      0x401F80D4U, 0x0U, 0, 0, 0x401F8248U
#define IOMUXC_GPIO_AD_B0_06_MQS_RIGHT          0x401F80D4U, 0x1U, 0, 0, 0x401F8248U
#define IOMUXC_GPIO_AD_B0_06_LPUART1_TX         0x401F80D4U, 0x2U, 0, 0, 0x401F8248U
#define IOMUXC_GPIO_AD_B0_06_GPIO1_IO06         0x401F80D4U, 0x5U, 0, 0, 0x401F8248U
#define IOMUXC_GPIO_AD_B0_06_REF_32K_OUT        0x401F80D4U, 0x6U, 0, 0, 0x401F8248U

#define IOMUXC_GPIO_AD_B0_07_PIT_TRIGGER01      0x401F80D8U, 0x0U, 0, 0, 0x401F824CU
#define IOMUXC_GPIO_AD_B0_07_MQS_LEFT           0x401F80D8U, 0x1U, 0, 0, 0x401F824CU
#define IOMUXC_GPIO_AD_B0_07_LPUART1_RX         0x401F80D8U, 0x2U, 0, 0, 0x401F824CU
#define IOMUXC_GPIO_AD_B0_07_GPIO1_IO07         0x401F80D8U, 0x5U, 0, 0, 0x401F824CU
#define IOMUXC_GPIO_AD_B0_07_REF_24M_OUT        0x401F80D8U, 0x6U, 0, 0, 0x401F824CU

#define IOMUXC_GPIO_AD_B0_08_LPUART1_CTS_B      0x401F80DCU, 0x2U, 0, 0, 0x401F8250U
#define IOMUXC_GPIO_AD_B0_08_KPP_COL00          0x401F80DCU, 0x3U, 0, 0, 0x401F8250U
#define IOMUXC_GPIO_AD_B0_08_GPIO1_IO08         0x401F80DCU, 0x5U, 0, 0, 0x401F8250U
#define IOMUXC_GPIO_AD_B0_08_ARM_CM7_TXEV       0x401F80DCU, 0x6U, 0, 0, 0x401F8250U

#define IOMUXC_GPIO_AD_B0_09_LPUART1_RTS_B      0x401F80E0U, 0x2U, 0, 0, 0x401F8254U
#define IOMUXC_GPIO_AD_B0_09_KPP_ROW00          0x401F80E0U, 0x3U, 0, 0, 0x401F8254U
#define IOMUXC_GPIO_AD_B0_09_CSU_CSU_INT_DEB    0x401F80E0U, 0x4U, 0, 0, 0x401F8254U
#define IOMUXC_GPIO_AD_B0_09_GPIO1_IO09         0x401F80E0U, 0x5U, 0, 0, 0x401F8254U
#define IOMUXC_GPIO_AD_B0_09_ARM_CM7_RXEV       0x401F80E0U, 0x6U, 0, 0, 0x401F8254U

#define IOMUXC_GPIO_AD_B0_10_LPSPI1_SCK         0x401F80E4U, 0x1U, 0x401F83A0U, 0x1U, 0x401F8258U
#define IOMUXC_GPIO_AD_B0_10_KPP_COL01          0x401F80E4U, 0x3U, 0, 0, 0x401F8258U
#define IOMUXC_GPIO_AD_B0_10_GPIO1_IO10         0x401F80E4U, 0x5U, 0, 0, 0x401F8258U
#define IOMUXC_GPIO_AD_B0_10_ARM_CM7_TRACE_CLK  0x401F80E4U, 0x6U, 0, 0, 0x401F8258U

#define IOMUXC_GPIO_AD_B0_11_LPSPI1_PCS0        0x401F80E8U, 0x1U, 0x401F839CU, 0x1U, 0x401F825CU
#define IOMUXC_GPIO_AD_B0_11_KPP_ROW01          0x401F80E8U, 0x3U, 0, 0, 0x401F825CU
#define IOMUXC_GPIO_AD_B0_11_GPIO1_IO11         0x401F80E8U, 0x5U, 0, 0, 0x401F825CU
#define IOMUXC_GPIO_AD_B0_11_ARM_CM7_TRACE_SWO  0x401F80E8U, 0x6U, 0, 0, 0x401F825CU

#define IOMUXC_GPIO_AD_B0_12_LPSPI1_SDO         0x401F80ECU, 0x1U, 0x401F83A8U, 0x1U, 0x401F8260U
#define IOMUXC_GPIO_AD_B0_12_LPUART3_CTS_B      0x401F80ECU, 0x2U, 0, 0, 0x401F8260U
#define IOMUXC_GPIO_AD_B0_12_KPP_COL02          0x401F80ECU, 0x3U, 0, 0, 0x401F8260U
#define IOMUXC_GPIO_AD_B0_12_GPIO1_IO12         0x401F80ECU, 0x5U, 0, 0, 0x401F8260U
#define IOMUXC_GPIO_AD_B0_12_ARM_CM7_TRACE00    0x401F80ECU, 0x6U, 0, 0, 0x401F8260U
#define IOMUXC_GPIO_AD_B0_12_SNVS_HP_VIO_5_CTL  0x401F80ECU, 0x7U, 0, 0, 0x401F8260U

#define IOMUXC_GPIO_AD_B0_13_LPSPI1_SDI         0x401F80F0U, 0x1U, 0x401F83A4U, 0x1U, 0x401F8264U
#define IOMUXC_GPIO_AD_B0_13_LPUART3_RTS_B      0x401F80F0U, 0x2U, 0, 0, 0x401F8264U
#define IOMUXC_GPIO_AD_B0_13_KPP_ROW02          0x401F80F0U, 0x3U, 0, 0, 0x401F8264U
#define IOMUXC_GPIO_AD_B0_13_GPIO1_IO13         0x401F80F0U, 0x5U, 0, 0, 0x401F8264U
#define IOMUXC_GPIO_AD_B0_13_ARM_CM7_TRACE01    0x401F80F0U, 0x6U, 0, 0, 0x401F8264U
#define IOMUXC_GPIO_AD_B0_13_SNVS_HP_VIO_5_B    0x401F80F0U, 0x7U, 0, 0, 0x401F8264U

#define IOMUXC_GPIO_AD_B0_14_LPUART3_TX         0x401F80F4U, 0x2U, 0x401F83DCU, 0x1U, 0x401F8268U
#define IOMUXC_GPIO_AD_B0_14_KPP_COL03          0x401F80F4U, 0x3U, 0, 0, 0x401F8268U
#define IOMUXC_GPIO_AD_B0_14_GPIO1_IO14         0x401F80F4U, 0x5U, 0, 0, 0x401F8268U
#define IOMUXC_GPIO_AD_B0_14_ARM_CM7_TRACE02    0x401F80F4U, 0x6U, 0, 0, 0x401F8268U
#define IOMUXC_GPIO_AD_B0_14_WDOG1_WDOG_ANY     0x401F80F4U, 0x7U, 0, 0, 0x401F8268U

#define IOMUXC_GPIO_AD_B0_15_LPUART3_RX         0x401F80F8U, 0x2U, 0x401F83D8U, 0x1U, 0x401F826CU
#define IOMUXC_GPIO_AD_B0_15_KPP_ROW03          0x401F80F8U, 0x3U, 0, 0, 0x401F826CU
#define IOMUXC_GPIO_AD_B0_15_GPIO1_IO15         0x401F80F8U, 0x5U, 0, 0, 0x401F826CU
#define IOMUXC_GPIO_AD_B0_15_ARM_CM7_TRACE03    0x401F80F8U, 0x6U, 0, 0, 0x401F826CU

#define IOMUXC_GPIO_AD_B1_10_USB_OTG1_PWR       0x401F8124U, 0x0U, 0, 0, 0x401F8298U
#define IOMUXC_GPIO_AD_B1_10_FLEXPWM1_PWMA02    0x401F8124U, 0x1U, 0x401F8330U, 0x0U, 0x401F8298U
#define IOMUXC_GPIO_AD_B1_10_LPUART4_TX         0x401F8124U, 0x2U, 0x401F83E8U, 0x1U, 0x401F8298U
#define IOMUXC_GPIO_AD_B1_10_FLEXIO1_FLEXIO05   0x401F8124U, 0x4U, 0, 0, 0x401F8298U
#define IOMUXC_GPIO_AD_B1_10_GPIO1_IO26         0x401F8124U, 0x5U, 0, 0, 0x401F8298U
#define IOMUXC_GPIO_AD_B1_10_GPT2_CAPTURE1      0x401F8124U, 0x6U, 0, 0, 0x401F8298U

#define IOMUXC_GPIO_AD_B1_11_USB_OTG1_ID        0x401F8128U, 0x0U, 0x401F82FCU, 0x1U, 0x401F829CU
#define IOMUXC_GPIO_AD_B1_11_FLEXPWM1_PWMB02    0x401F8128U, 0x1U, 0x401F8340U, 0x0U, 0x401F829CU
#define IOMUXC_GPIO_AD_B1_11_LPUART4_RX         0x401F8128U, 0x2U, 0x401F83E4U, 0x1U, 0x401F829CU
#define IOMUXC_GPIO_AD_B1_11_FLEXIO1_FLEXIO04   0x401F8128U, 0x4U, 0, 0, 0x401F829CU
#define IOMUXC_GPIO_AD_B1_11_GPIO1_IO27         0x401F8128U, 0x5U, 0, 0, 0x401F829CU
#define IOMUXC_GPIO_AD_B1_11_GPT2_COMPARE1      0x401F8128U, 0x6U, 0, 0, 0x401F829CU

#define IOMUXC_GPIO_AD_B1_12_USB_OTG1_OC        0x401F812CU, 0x0U, 0x401F848CU, 0x1U, 0x401F82A0U
#define IOMUXC_GPIO_AD_B1_12_ACMP_OUT00         0x401F812CU, 0x1U, 0, 0, 0x401F82A0U
#define IOMUXC_GPIO_AD_B1_12_FLEXIO1_FLEXIO03   0x401F812CU, 0x4U, 0, 0, 0x401F82A0U
#define IOMUXC_GPIO_AD_B1_12_GPIO1_IO28         0x401F812CU, 0x5U, 0, 0, 0x401F82A0U
#define IOMUXC_GPIO_AD_B1_12_FLEXPWM1_PWMA03    0x401F812CU, 0x6U, 0x401F8334U, 0x0U, 0x401F82A0U

#define IOMUXC_GPIO_AD_B1_13_LPI2C1_HREQ        0x401F8130U, 0x0U, 0, 0, 0x401F82A4U
#define IOMUXC_GPIO_AD_B1_13_ACMP_OUT01         0x401F8130U, 0x1U, 0, 0, 0x401F82A4U
#define IOMUXC_GPIO_AD_B1_13_FLEXIO1_FLEXIO02   0x401F8130U, 0x4U, 0, 0, 0x401F82A4U
#define IOMUXC_GPIO_AD_B1_13_GPIO1_IO29         0x401F8130U, 0x5U, 0, 0, 0x401F82A4U
#define IOMUXC_GPIO_AD_B1_13_FLEXPWM1_PWMB03    0x401F8130U, 0x6U, 0x401F8344U, 0x0U, 0x401F82A4U

#define IOMUXC_GPIO_AD_B1_14_LPI2C1_SCL         0x401F8134U, 0x0U, 0x401F837CU, 0x1U, 0x401F82A8U
#define IOMUXC_GPIO_AD_B1_14_ACMP_OUT02         0x401F8134U, 0x1U, 0, 0, 0x401F82A8U
#define IOMUXC_GPIO_AD_B1_14_FLEXIO1_FLEXIO01   0x401F8134U, 0x4U, 0, 0, 0x401F82A8U
#define IOMUXC_GPIO_AD_B1_14_GPIO1_IO30         0x401F8134U, 0x5U, 0, 0, 0x401F82A8U

#define IOMUXC_GPIO_AD_B1_15_LPI2C1_SDA         0x401F8138U, 0x0U, 0x401F8380U, 0x1U, 0x401F82ACU
#define IOMUXC_GPIO_AD_B1_15_ACMP_OUT03         0x401F8138U, 0x1U, 0, 0, 0x401F82ACU
#define IOMUXC_GPIO_AD_B1_15_FLEXIO1_FLEXIO00   0x401F8138U, 0x4U, 0, 0, 0x401F82ACU
#define IOMUXC_GPIO_AD_B1_15_GPIO1_IO31         0x401F8138U, 0x5U, 0, 0, 0x401F82ACU
#define IOMUXC_GPIO_AD_B1_15_CCM_DI0_EXT_CLK    0x401F8138U, 0x6U, 0, 0, 0x401F82ACU

#define IOMUXC_GPIO_SD_B1_00_FLEXSPI_B_DATA03   0x401F8158U, 0x1U, 0, 0, 0x401F82CCU
#define IOMUXC_GPIO_SD_B1_00_XBAR1_XBAR_INOUT10 0x401F8158U, 0x3U, 0x401F84B0U, 0x1U, 0x401F82CCU
#define IOMUXC_GPIO_SD_B1_00_GPIO3_IO20         0x401F8158U, 0x5U, 0, 0, 0x401F82CCU

#define IOMUXC_GPIO_SD_B1_01_FLEXSPI_B_SCLK     0x401F815CU, 0x1U, 0, 0, 0x401F82D0U
#define IOMUXC_GPIO_SD_B1_01_FLEXSPI_A_SS1_B    0x401F815CU, 0x3U, 0, 0, 0x401F82D0U
#define IOMUXC_GPIO_SD_B1_01_GPIO3_IO21         0x401F815CU, 0x5U, 0, 0, 0x401F82D0U

#define IOMUXC_GPIO_SD_B1_02_FLEXSPI_B_DATA00   0x401F8160U, 0x1U, 0, 0, 0x401F82D4U
#define IOMUXC_GPIO_SD_B1_02_GPIO3_IO22         0x401F8160U, 0x5U, 0, 0, 0x401F82D4U
#define IOMUXC_GPIO_SD_B1_02_CCM_CLKO1          0x401F8160U, 0x6U, 0, 0, 0x401F82D4U

#define IOMUXC_GPIO_SD_B1_03_FLEXSPI_B_DATA02   0x401F8164U, 0x1U, 0, 0, 0x401F82D8U
#define IOMUXC_GPIO_SD_B1_03_GPIO3_IO23         0x401F8164U, 0x5U, 0, 0, 0x401F82D8U
#define IOMUXC_GPIO_SD_B1_03_CCM_CLKO2          0x401F8164U, 0x6U, 0, 0, 0x401F82D8U

#define IOMUXC_GPIO_SD_B1_04_FLEXSPI_B_DATA01   0x401F8168U, 0x1U, 0, 0, 0x401F82DCU
#define IOMUXC_GPIO_SD_B1_04_EWM_EWM_OUT_B      0x401F8168U, 0x4U, 0, 0, 0x401F82DCU
#define IOMUXC_GPIO_SD_B1_04_GPIO3_IO24         0x401F8168U, 0x5U, 0, 0, 0x401F82DCU
#define IOMUXC_GPIO_SD_B1_04_CCM_WAIT           0x401F8168U, 0x6U, 0, 0, 0x401F82DCU

#define IOMUXC_GPIO_SD_B1_05_FLEXSPI_A_DQS      0x401F816CU, 0x1U, 0, 0, 0x401F82E0U
#define IOMUXC_GPIO_SD_B1_05_SAI3_MCLK          0x401F816CU, 0x3U, 0x401F846CU, 0x0U, 0x401F82E0U
#define IOMUXC_GPIO_SD_B1_05_FLEXSPI_B_SS0_B    0x401F816CU, 0x4U, 0, 0, 0x401F82E0U
#define IOMUXC_GPIO_SD_B1_05_GPIO3_IO25         0x401F816CU, 0x5U, 0, 0, 0x401F82E0U
#define IOMUXC_GPIO_SD_B1_05_CCM_PMIC_RDY       0x401F816CU, 0x6U, 0x401F8300U, 0x1U, 0x401F82E0U

#define IOMUXC_GPIO_SD_B1_06_FLEXSPI_A_DATA03   0x401F8170U, 0x1U, 0x401F8374U, 0x0U, 0x401F82E4U
#define IOMUXC_GPIO_SD_B1_06_SAI3_TX_BCLK       0x401F8170U, 0x3U, 0x401F847CU, 0x0U, 0x401F82E4U
#define IOMUXC_GPIO_SD_B1_06_LPSPI2_PCS0        0x401F8170U, 0x4U, 0x401F83ACU, 0x2U, 0x401F82E4U
#define IOMUXC_GPIO_SD_B1_06_GPIO3_IO26         0x401F8170U, 0x5U, 0, 0, 0x401F82E4U
#define IOMUXC_GPIO_SD_B1_06_CCM_STOP           0x401F8170U, 0x6U, 0, 0, 0x401F82E4U

#define IOMUXC_GPIO_SD_B1_07_FLEXSPI_A_SCLK     0x401F8174U, 0x1U, 0x401F8378U, 0x0U, 0x401F82E8U
#define IOMUXC_GPIO_SD_B1_07_SAI3_TX_SYNC       0x401F8174U, 0x3U, 0x401F8480U, 0x0U, 0x401F82E8U
#define IOMUXC_GPIO_SD_B1_07_LPSPI2_SCK         0x401F8174U, 0x4U, 0x401F83B0U, 0x2U, 0x401F82E8U
#define IOMUXC_GPIO_SD_B1_07_GPIO3_IO27         0x401F8174U, 0x5U, 0, 0, 0x401F82E8U

#define IOMUXC_GPIO_SD_B1_08_FLEXSPI_A_DATA00   0x401F8178U, 0x1U, 0x401F8368U, 0x0U, 0x401F82ECU
#define IOMUXC_GPIO_SD_B1_08_SAI3_TX_DATA       0x401F8178U, 0x3U, 0, 0, 0x401F82ECU
#define IOMUXC_GPIO_SD_B1_08_LPSPI2_SDO         0x401F8178U, 0x4U, 0x401F83B8U, 0x2U, 0x401F82ECU
#define IOMUXC_GPIO_SD_B1_08_GPIO3_IO28         0x401F8178U, 0x5U, 0, 0, 0x401F82ECU

#define IOMUXC_GPIO_SD_B1_09_FLEXSPI_A_DATA02   0x401F817CU, 0x1U, 0x401F8370U, 0x0U, 0x401F82F0U
#define IOMUXC_GPIO_SD_B1_09_SAI3_RX_BCLK       0x401F817CU, 0x3U, 0x401F8470U, 0x0U, 0x401F82F0U
#define IOMUXC_GPIO_SD_B1_09_LPSPI2_SDI         0x401F817CU, 0x4U, 0x401F83B4U, 0x2U, 0x401F82F0U
#define IOMUXC_GPIO_SD_B1_09_GPIO3_IO29         0x401F817CU, 0x5U, 0, 0, 0x401F82F0U
#define IOMUXC_GPIO_SD_B1_09_CCM_REF_EN_B       0x401F817CU, 0x6U, 0, 0, 0x401F82F0U

#define IOMUXC_GPIO_SD_B1_10_FLEXSPI_A_DATA01   0x401F8180U, 0x1U, 0x401F836CU, 0x0U, 0x401F82F4U
#define IOMUXC_GPIO_SD_B1_10_SAI3_RX_SYNC       0x401F8180U, 0x3U, 0x401F8478U, 0x0U, 0x401F82F4U
#define IOMUXC_GPIO_SD_B1_10_LPSPI2_PCS2        0x401F8180U, 0x4U, 0, 0, 0x401F82F4U
#define IOMUXC_GPIO_SD_B1_10_GPIO3_IO30         0x401F8180U, 0x5U, 0, 0, 0x401F82F4U

#define IOMUXC_GPIO_SD_B1_11_FLEXSPI_A_SS0_B    0x401F8184U, 0x1U, 0, 0, 0x401F82F8U
#define IOMUXC_GPIO_SD_B1_11_SAI3_RX_DATA       0x401F8184U, 0x3U, 0x401F8474U, 0x0U, 0x401F82F8U
#define IOMUXC_GPIO_SD_B1_11_LPSPI2_PCS3        0x401F8184U, 0x4U, 0, 0, 0x401F82F8U
#define IOMUXC_GPIO_SD_B1_11_GPIO3_IO31         0x401F8184U, 0x5U, 0, 0, 0x401F82F8U

#define IOMUXC_SNVS_PMIC_ON_REQ_SNVS_LP_PMIC_ON_REQ 0x400A8004U, 0x0U, 0, 0, 0x400A801CU
#define IOMUXC_SNVS_PMIC_ON_REQ_GPIO5_IO01          0x400A8004U, 0x5U, 0, 0, 0x400A801CU

#define IOMUXC_SNVS_TEST_MODE 0, 0, 0, 0, 0x400A800CU

#define IOMUXC_SNVS_POR_B 0, 0, 0, 0, 0x400A8010U

#define IOMUXC_SNVS_ONOFF 0, 0, 0, 0, 0x400A8014U

/*@}*/

#define IOMUXC_GPR_SAIMCLK_LOWBITMASK (0x7U)
#define IOMUXC_GPR_SAIMCLK_HIGHBITMASK (0x3U)

typedef enum _iomuxc_gpr_mode
{
    kIOMUXC_GPR_GlobalInterruptRequest    = IOMUXC_GPR_GPR1_GINT_MASK,
    kIOMUXC_GPR_SAI1MClkOutputDir         = IOMUXC_GPR_GPR1_SAI1_MCLK_DIR_MASK,
    kIOMUXC_GPR_SAI2MClkOutputDir         = IOMUXC_GPR_GPR1_SAI2_MCLK_DIR_MASK,
    kIOMUXC_GPR_SAI3MClkOutputDir         = IOMUXC_GPR_GPR1_SAI3_MCLK_DIR_MASK,
    kIOMUXC_GPR_ExcMonitorSlavErrResponse = IOMUXC_GPR_GPR1_EXC_MON_MASK,
    kIOMUXC_GPR_AHBClockEnable            = (int)IOMUXC_GPR_GPR1_CM7_FORCE_HCLK_EN_MASK,
} iomuxc_gpr_mode_t;

typedef enum _iomuxc_gpr_saimclk
{
    kIOMUXC_GPR_SAI1MClk1Sel = IOMUXC_GPR_GPR1_SAI1_MCLK1_SEL_SHIFT,
    kIOMUXC_GPR_SAI1MClk2Sel = IOMUXC_GPR_GPR1_SAI1_MCLK2_SEL_SHIFT,
    kIOMUXC_GPR_SAI1MClk3Sel = IOMUXC_GPR_GPR1_SAI1_MCLK3_SEL_SHIFT,
    kIOMUXC_GPR_SAI2MClk3Sel = IOMUXC_GPR_GPR1_SAI2_MCLK3_SEL_SHIFT,
    kIOMUXC_GPR_SAI3MClk3Sel = IOMUXC_GPR_GPR1_SAI3_MCLK3_SEL_SHIFT,
} iomuxc_gpr_saimclk_t;

typedef enum _iomuxc_mqs_pwm_oversample_rate
{
    kIOMUXC_MqsPwmOverSampleRate32 = 0, /* MQS PWM over sampling rate 32. */
    kIOMUXC_MqsPwmOverSampleRate64 = 1  /* MQS PWM over sampling rate 64. */
} iomuxc_mqs_pwm_oversample_rate_t;

#if defined(__cplusplus)
extern "C" {
#endif /*_cplusplus */

/*! @name Configuration */
/*@{*/

/*!
 * @brief Sets the IOMUXC pin mux mode.
 * @note The first five parameters can be filled with the pin function ID macros.
 *
 * This is an example to set the PTA6 as the lpuart0_tx:
 * @code
 * IOMUXC_SetPinMux(IOMUXC_PTA6_LPUART0_TX, 0);
 * @endcode
 *
 * This is an example to set the PTA0 as GPIOA0:
 * @code
 * IOMUXC_SetPinMux(IOMUXC_PTA0_GPIOA0, 0);
 * @endcode
 *
 * @param muxRegister  The pin mux register.
 * @param muxMode      The pin mux mode.
 * @param inputRegister The select input register.
 * @param inputDaisy   The input daisy.
 * @param configRegister  The config register.
 * @param inputOnfield   Software input on field.
 */
static inline void IOMUXC_SetPinMux(uint32_t muxRegister,
                                    uint32_t muxMode,
                                    uint32_t inputRegister,
                                    uint32_t inputDaisy,
                                    uint32_t configRegister,
                                    uint32_t inputOnfield)
{
    (void)configRegister;
    *((volatile uint32_t *)muxRegister) =
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE(muxMode) | IOMUXC_SW_MUX_CTL_PAD_SION(inputOnfield);

    if (inputRegister != 0UL)
    {
        *((volatile uint32_t *)inputRegister) = inputDaisy;
    }
}

/*!
 * @brief Sets the IOMUXC pin configuration.
 * @note The previous five parameters can be filled with the pin function ID macros.
 *
 * This is an example to set pin configuration for IOMUXC_PTA3_LPI2C0_SCLS:
 * @code
 * IOMUXC_SetPinConfig(IOMUXC_PTA3_LPI2C0_SCLS,IOMUXC_SW_PAD_CTL_PAD_PUS_MASK|IOMUXC_SW_PAD_CTL_PAD_PUS(2U))
 * @endcode
 *
 * @param muxRegister  The pin mux register.
 * @param muxMode      The pin mux mode.
 * @param inputRegister The select input register.
 * @param inputDaisy   The input daisy.
 * @param configRegister  The config register.
 * @param configValue   The pin config value.
 */
static inline void IOMUXC_SetPinConfig(uint32_t muxRegister,
                                       uint32_t muxMode,
                                       uint32_t inputRegister,
                                       uint32_t inputDaisy,
                                       uint32_t configRegister,
                                       uint32_t configValue)
{
    (void)muxRegister;
    (void)muxMode;
    (void)inputRegister;
    (void)inputDaisy;
    if (configRegister != 0UL)
    {
        *((volatile uint32_t *)configRegister) = configValue;
    }
}

/*!
 * @brief Sets IOMUXC general configuration for some mode.
 *
 * @param base     The IOMUXC GPR base address.
 * @param mode     The mode for setting. the mode is the logical OR of "iomuxc_gpr_mode"
 * @param enable   True enable false disable.
 */
static inline void IOMUXC_EnableMode(IOMUXC_GPR_Type *base, uint32_t mode, bool enable)
{
    mode &= ~(IOMUXC_GPR_GPR1_SAI1_MCLK1_SEL_MASK | IOMUXC_GPR_GPR1_SAI1_MCLK2_SEL_MASK |
              IOMUXC_GPR_GPR1_SAI1_MCLK3_SEL_MASK | IOMUXC_GPR_GPR1_SAI2_MCLK3_SEL_MASK |
              IOMUXC_GPR_GPR1_SAI3_MCLK3_SEL_MASK);

    if (enable)
    {
        base->GPR1 |= mode;
    }
    else
    {
        base->GPR1 &= ~mode;
    }
}

/*!
 * @brief Sets IOMUXC general configuration for SAI MCLK selection.
 *
 * @param base     The IOMUXC GPR base address.
 * @param mclk     The SAI MCLK.
 * @param clkSrc   The clock source. Take refer to register setting details for the clock source in RM.
 */
static inline void IOMUXC_SetSaiMClkClockSource(IOMUXC_GPR_Type *base, iomuxc_gpr_saimclk_t mclk, uint8_t clkSrc)
{
    uint32_t gpr;

    if (mclk > kIOMUXC_GPR_SAI1MClk2Sel)
    {
        gpr = base->GPR1 & ~((uint32_t)IOMUXC_GPR_SAIMCLK_HIGHBITMASK << (uint32_t)mclk);
        base->GPR1 = (((uint32_t)clkSrc & IOMUXC_GPR_SAIMCLK_HIGHBITMASK) << (uint32_t)mclk) | gpr;
    }
    else
    {
        gpr = base->GPR1 & ~((uint32_t)IOMUXC_GPR_SAIMCLK_LOWBITMASK << (uint32_t)mclk);
        base->GPR1 = (((uint32_t)clkSrc & IOMUXC_GPR_SAIMCLK_LOWBITMASK) << (uint32_t)mclk) | gpr;
    }
}

/*!
 * @brief Enters or exit MQS software reset.
 *
 * @param base     The IOMUXC GPR base address.
 * @param enable   Enter or exit MQS software reset.
 */
static inline void IOMUXC_MQSEnterSoftwareReset(IOMUXC_GPR_Type *base, bool enable)
{
    if (enable)
    {
        base->GPR2 |= IOMUXC_GPR_GPR2_MQS_SW_RST_MASK;
    }
    else
    {
        base->GPR2 &= ~IOMUXC_GPR_GPR2_MQS_SW_RST_MASK;
    }
}

/*!
 * @brief Enables or disables MQS.
 *
 * @param base     The IOMUXC GPR base address.
 * @param enable   Enable or disable the MQS.
 */
static inline void IOMUXC_MQSEnable(IOMUXC_GPR_Type *base, bool enable)
{
    if (enable)
    {
        base->GPR2 |= IOMUXC_GPR_GPR2_MQS_EN_MASK;
    }
    else
    {
        base->GPR2 &= ~IOMUXC_GPR_GPR2_MQS_EN_MASK;
    }
}

/*!
 * @brief Configure MQS PWM oversampling rate compared with mclk and divider ratio control for mclk from hmclk.
 *
 * @param base     The IOMUXC GPR base address.
 * @param rate     The MQS PWM oversampling rate, refer to "iomuxc_mqs_pwm_oversample_rate_t".
 * @param divider  The divider ratio control for mclk from hmclk. mclk freq = 1 /(divider + 1) * hmclk freq.
 */

static inline void IOMUXC_MQSConfig(IOMUXC_GPR_Type *base, iomuxc_mqs_pwm_oversample_rate_t rate, uint8_t divider)
{
    uint32_t gpr = base->GPR2 & ~(IOMUXC_GPR_GPR2_MQS_OVERSAMPLE_MASK | IOMUXC_GPR_GPR2_MQS_CLK_DIV_MASK);
    base->GPR2 = gpr | IOMUXC_GPR_GPR2_MQS_OVERSAMPLE(rate) | IOMUXC_GPR_GPR2_MQS_CLK_DIV(divider);
}

/*@}*/

#if defined(__cplusplus)
}
#endif /*_cplusplus */

/*! @}*/

#endif /* _FSL_IOMUXC_H_ */

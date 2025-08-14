/*
** ###################################################################
**     Processors:          MIMXRT2660CHPAA
**                          MIMXRT2660CVVAA
**                          MIMXRT2660DHPAA
**                          MIMXRT2660DVVAA
**                          MIMXRT2660XHP8A
**                          MIMXRT2660XVV8A
**                          MIMXRT2661CHPAA
**                          MIMXRT2661CVVAA
**                          MIMXRT2661DHPAA
**                          MIMXRT2661DVJ8A
**                          MIMXRT2661DVM8A
**                          MIMXRT2661DVVAA
**                          MIMXRT2661XHP8A
**                          MIMXRT2661XVV8A
**                          MIMXRT2662AHP8A
**                          MIMXRT2662CHPAA
**                          MIMXRT2662CVVAA
**                          MIMXRT2662DHPAA
**                          MIMXRT2662DVVAA
**                          MIMXRT2662XHP8A
**                          MIMXRT2662XVV8A
**                          MIMXRT2663AHP8A
**                          MIMXRT2663CHPAA
**                          MIMXRT2663CVVAA
**                          MIMXRT2663DHPAA
**                          MIMXRT2663DVVAA
**                          MIMXRT2663XHP8A
**                          MIMXRT2663XVV8A
**
**     Version:             rev. 1.0, 2024-11-05
**     Build:               b260603
**
**     Abstract:
**         CMSIS Peripheral Access Layer for INPUTMUX
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
 * @file PERI_INPUTMUX.h
 * @version 1.0
 * @date 2024-11-05
 * @brief CMSIS Peripheral Access Layer for INPUTMUX
 *
 * CMSIS Peripheral Access Layer for INPUTMUX
 */

#if !defined(PERI_INPUTMUX_H_)
#define PERI_INPUTMUX_H_                         /**< Symbol preventing repeated inclusion */

#if (defined(CPU_MIMXRT2660CHPAA) || defined(CPU_MIMXRT2660CVVAA) || defined(CPU_MIMXRT2660DHPAA) || defined(CPU_MIMXRT2660DVVAA) || defined(CPU_MIMXRT2660XHP8A) || defined(CPU_MIMXRT2660XVV8A) || defined(CPU_MIMXRT2661DVM8A))
#include "../MIMXRT2660_COMMON.h"
#elif (defined(CPU_MIMXRT2661CHPAA) || defined(CPU_MIMXRT2661CVVAA) || defined(CPU_MIMXRT2661DHPAA) || defined(CPU_MIMXRT2661DVJ8A) || defined(CPU_MIMXRT2661DVVAA) || defined(CPU_MIMXRT2661XHP8A) || defined(CPU_MIMXRT2661XVV8A))
#include "../MIMXRT2661_COMMON.h"
#elif (defined(CPU_MIMXRT2662AHP8A) || defined(CPU_MIMXRT2662CHPAA) || defined(CPU_MIMXRT2662CVVAA) || defined(CPU_MIMXRT2662DHPAA) || defined(CPU_MIMXRT2662DVVAA) || defined(CPU_MIMXRT2662XHP8A) || defined(CPU_MIMXRT2662XVV8A))
#include "../MIMXRT2662_COMMON.h"
#elif (defined(CPU_MIMXRT2663AHP8A) || defined(CPU_MIMXRT2663CHPAA) || defined(CPU_MIMXRT2663CVVAA) || defined(CPU_MIMXRT2663DHPAA) || defined(CPU_MIMXRT2663DVVAA) || defined(CPU_MIMXRT2663XHP8A) || defined(CPU_MIMXRT2663XVV8A))
#include "../MIMXRT2663_COMMON.h"
#else
  #error "No valid CPU defined!"
#endif

/* ----------------------------------------------------------------------------
   -- Device Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup Peripheral_access_layer Device Peripheral Access Layer
 * @{
 */


/*
** Start of section using anonymous unions
*/

#if defined(__ARMCC_VERSION)
  #if (__ARMCC_VERSION >= 6010050)
    #pragma clang diagnostic push
  #else
    #pragma push
    #pragma anon_unions
  #endif
#elif defined(__GNUC__)
  /* anonymous unions are enabled by default */
#elif defined(__IAR_SYSTEMS_ICC__)
  #pragma language=extended
#else
  #error Not supported compiler type
#endif

/* ----------------------------------------------------------------------------
   -- INPUTMUX Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup INPUTMUX_Peripheral_Access_Layer INPUTMUX Peripheral Access Layer
 * @{
 */

/** INPUTMUX - Register Layout Typedef */
typedef struct {
  __IO uint32_t FREQME_REF;                        /**< FREQME Reference Clock Selection, offset: 0x0 */
  __IO uint32_t FREQME_TAR;                        /**< FREQME Target Clock Selection, offset: 0x4 */
} INPUTMUX_Type;

/* ----------------------------------------------------------------------------
   -- INPUTMUX Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup INPUTMUX_Register_Masks INPUTMUX Register Masks
 * @{
 */

/*! @name FREQME_REF - FREQME Reference Clock Selection */
/*! @{ */

#define INPUTMUX_FREQME_REF_INP_MASK             (0x7FU)  /* Merged from fields with different position or width, of widths (5, 6, 7), largest definition used */
#define INPUTMUX_FREQME_REF_INP_SHIFT            (0U)
/*! INP - Clock Source Input
 *  0b0000000..
 *  0b0000001..hsp_ss_xbar0 out9 input is selected
 *  0b0000010..
 *  0b0000011..
 *  0b0000100..
 *  0b0000101..
 *  0b0000110..
 *  0b0000111..
 *  0b0001000..
 *  0b0001001..
 *  0b0001010..
 *  0b0001011..
 *  0b0001100..
 *  0b0001101..
 *  0b0001110..
 *  0b0001111..
 *  0b0010000..SYSCON_CGU cgu_ss_sxosc_rootclk input is selected
 *  0b0010001..SYSCON_CGU cgu_ss_base_clk input is selected
 *  0b0010010..SYSCON_CGU cgu_ss_low_clk input is selected
 *  0b0010011..SYSCON_CGU cgu_ss_mainpll_divx input is selected
 *  0b0010100..SYSCON_CGU cgu_ss_syspll_divx input is selected
 *  0b0010101..SYSCON_CGU cgu_ss_pll_pfdx input is selected
 *  0b0010110..SYSCON_CGU cgu_ss_media_pfdx input is selected
 *  0b0010111..SYSCON_CGU cgu_ss_mainpfdx_rootclk input is selected
 *  0b0011000..SYSCON_CGU cgu_ss_commpfdx_rootclk input is selected
 *  0b0011001..SYSCON_CGU cgu_ss_maindivx_rootclk input is selected
 *  0b0011010..SYSCON_CGU cgu_ss_saimclk_rootclk input is selected
 *  0b0011011..SYSCON_CGU cgu_ss_saimclk0_rootclk input is selected
 *  0b0011100..SYSCON_CGU cgu_ss_saimclk1_rootclk input is selected
 *  0b0011101..SYSCON_CGU cgu_ss_saimclk2_rootclk input is selected
 *  0b0011110..SYSCON_CGU cgu_ss_lp12m_core_rootclk input is selected
 *  0b0011111..SYSCON_CGU cgu_ss_lp1m_core_rootclk input is selected
 *  0b0100000..SYSCON_CGU cgu_ss_ulp32k_rootclk input is selected
 *  0b0100001..SYSCON_CGU cgu_ss_fro192m_rootclk input is selected
 *  0b0100010..SYSCON_CGU cgu_ss_fro96m_rootclk input is selected
 *  0b0100011..SYSCON_CGU cgu_ss_fro48m_rootclk input is selected
 *  0b0100100..SYSCON_CGU cgu_ss_fro24m_rootclk input is selected
 *  0b0100101..SYSCON_CGU cgu_ss_sysplldiv4_rootclk input is selected
 *  0b0100110..SYSCON_CGU cgu_ss_sysplldiv5_rootclk input is selected
 *  0b0100111..SYSCON_CGU cgu_ss_sysplldivx_rootclk input is selected
 *  0b0101000..SYSCON_CGU cgu_ss_mainplldivx_rootclk input is selected
 *  0b0101001..SYSCON_CGU cgu_ss_mainplldiv8_rootclk input is selected
 *  0b0101010..SYSCON_CGU cgu_ss_mainplldiv10_rootclk input is selected
 *  0b0101011..SYSCON_CGU cgu_ss_mainplldiv20_rootclk input is selected
 *  0b0101100..SYSCON_CGU cgu_ss_audiopll_rootclk input is selected
 *  0b0101101..SYSCON_CGU cgu_ss_videopll_rootclk input is selected
 *  0b0101110..SYSCON_CGU cgu_ss_cpu_rootclk input is selected
 *  0b0101111..SYSCON_CGU cgu_ss_cpu_rootclk_divided input is selected
 *  0b0110000..
 *  0b0110001..
 *  0b0110010..SYSCON_CGU cgu_ss_npu_rootclk input is selected
 *  0b0110011..SYSCON_CGU cgu_ss_mediabus_rootclk input is selected
 *  0b0110100..SYSCON_CGU cgu_ss_audiobus_rootclk input is selected
 *  0b0110101..SYSCON_CGU cgu_ss_commbus_rootclk input is selected
 *  0b0110110..SYSCON_CGU cgu_ss_wakebus_rootclk input is selected
 *  0b0110111..SYSCON_CGU cgu_ss_syscon_pdmain_clk input is selected
 *  0b0111000..SYSCON_CGU cgu_ss_peri_rootclk0 input is selected
 *  0b0111001..SYSCON_CGU cgu_ss_peri_rootclk1 input is selected
 *  0b0111010..SYSCON_CGU cgu_ss_peri_rootclk2 input is selected
 *  0b0111011..SYSCON_CGU cgu_ss_peri_rootclk3 input is selected
 *  0b0111100..SYSCON_CGU cgu_ss_peri_rootclk4 input is selected
 *  0b0111101..SYSCON_CGU cgu_ss_peri_rootclk5 input is selected
 *  0b0111110..SYSCON_CGU cgu_ss_peri_rootclk6 input is selected
 *  0b0111111..SYSCON_CGU cgu_ss_peri_rootclk7 input is selected
 *  0b1000000..SYSCON_CGU cgu_ss_audio_rootclk input is selected
 *  0b1000001..SYSCON_CGU cgu_ss_video_rootclk input is selected
 *  0b1000010..SYSCON_CGU cgu_ss_usb1_rootclk input is selected
 *  0b1000011..SYSCON_CGU cgu_ss_eth_rootclk input is selected
 *  0b1000100..SYSCON_CGU cgu_ss_test_rootclk input is selected
 */
#define INPUTMUX_FREQME_REF_INP(x)               (((uint32_t)(((uint32_t)(x)) << INPUTMUX_FREQME_REF_INP_SHIFT)) & INPUTMUX_FREQME_REF_INP_MASK)  /* Merged from fields with different position or width, of widths (5, 6, 7), largest definition used */
/*! @} */

/*! @name FREQME_TAR - FREQME Target Clock Selection */
/*! @{ */

#define INPUTMUX_FREQME_TAR_INP_MASK             (0x7FU)  /* Merged from fields with different position or width, of widths (5, 6, 7), largest definition used */
#define INPUTMUX_FREQME_TAR_INP_SHIFT            (0U)
/*! INP - Clock Source Input
 *  0b0000000..
 *  0b0000001..hsp_ss_xbar0 out9 input is selected
 *  0b0000010..
 *  0b0000011..
 *  0b0000100..
 *  0b0000101..
 *  0b0000110..
 *  0b0000111..
 *  0b0001000..
 *  0b0001001..
 *  0b0001010..
 *  0b0001011..
 *  0b0001100..
 *  0b0001101..
 *  0b0001110..
 *  0b0001111..
 *  0b0010000..SYSCON_CGU cgu_ss_sxosc_rootclk input is selected
 *  0b0010001..SYSCON_CGU cgu_ss_base_clk input is selected
 *  0b0010010..SYSCON_CGU cgu_ss_low_clk input is selected
 *  0b0010011..SYSCON_CGU cgu_ss_mainpll_divx input is selected
 *  0b0010100..SYSCON_CGU cgu_ss_syspll_divx input is selected
 *  0b0010101..SYSCON_CGU cgu_ss_pll_pfdx input is selected
 *  0b0010110..SYSCON_CGU cgu_ss_media_pfdx input is selected
 *  0b0010111..SYSCON_CGU cgu_ss_mainpfdx_rootclk input is selected
 *  0b0011000..SYSCON_CGU cgu_ss_commpfdx_rootclk input is selected
 *  0b0011001..SYSCON_CGU cgu_ss_maindivx_rootclk input is selected
 *  0b0011010..SYSCON_CGU cgu_ss_saimclk_rootclk input is selected
 *  0b0011011..SYSCON_CGU cgu_ss_saimclk0_rootclk input is selected
 *  0b0011100..SYSCON_CGU cgu_ss_saimclk1_rootclk input is selected
 *  0b0011101..SYSCON_CGU cgu_ss_saimclk2_rootclk input is selected
 *  0b0011110..SYSCON_CGU cgu_ss_lp12m_core_rootclk input is selected
 *  0b0011111..SYSCON_CGU cgu_ss_lp1m_core_rootclk input is selected
 *  0b0100000..SYSCON_CGU cgu_ss_ulp32k_rootclk input is selected
 *  0b0100001..SYSCON_CGU cgu_ss_fro192m_rootclk input is selected
 *  0b0100010..SYSCON_CGU cgu_ss_fro96m_rootclk input is selected
 *  0b0100011..SYSCON_CGU cgu_ss_fro48m_rootclk input is selected
 *  0b0100100..SYSCON_CGU cgu_ss_fro24m_rootclk input is selected
 *  0b0100101..SYSCON_CGU cgu_ss_sysplldiv4_rootclk input is selected
 *  0b0100110..SYSCON_CGU cgu_ss_sysplldiv5_rootclk input is selected
 *  0b0100111..SYSCON_CGU cgu_ss_sysplldivx_rootclk input is selected
 *  0b0101000..SYSCON_CGU cgu_ss_mainplldivx_rootclk input is selected
 *  0b0101001..SYSCON_CGU cgu_ss_mainplldiv8_rootclk input is selected
 *  0b0101010..SYSCON_CGU cgu_ss_mainplldiv10_rootclk input is selected
 *  0b0101011..SYSCON_CGU cgu_ss_mainplldiv20_rootclk input is selected
 *  0b0101100..SYSCON_CGU cgu_ss_audiopll_rootclk input is selected
 *  0b0101101..SYSCON_CGU cgu_ss_videopll_rootclk input is selected
 *  0b0101110..SYSCON_CGU cgu_ss_cpu_rootclk input is selected
 *  0b0101111..SYSCON_CGU cgu_ss_cpu_rootclk_divided input is selected
 *  0b0110000..
 *  0b0110001..
 *  0b0110010..SYSCON_CGU cgu_ss_npu_rootclk input is selected
 *  0b0110011..SYSCON_CGU cgu_ss_mediabus_rootclk input is selected
 *  0b0110100..SYSCON_CGU cgu_ss_audiobus_rootclk input is selected
 *  0b0110101..SYSCON_CGU cgu_ss_commbus_rootclk input is selected
 *  0b0110110..SYSCON_CGU cgu_ss_wakebus_rootclk input is selected
 *  0b0110111..SYSCON_CGU cgu_ss_syscon_pdmain_clk input is selected
 *  0b0111000..SYSCON_CGU cgu_ss_peri_rootclk0 input is selected
 *  0b0111001..SYSCON_CGU cgu_ss_peri_rootclk1 input is selected
 *  0b0111010..SYSCON_CGU cgu_ss_peri_rootclk2 input is selected
 *  0b0111011..SYSCON_CGU cgu_ss_peri_rootclk3 input is selected
 *  0b0111100..SYSCON_CGU cgu_ss_peri_rootclk4 input is selected
 *  0b0111101..SYSCON_CGU cgu_ss_peri_rootclk5 input is selected
 *  0b0111110..SYSCON_CGU cgu_ss_peri_rootclk6 input is selected
 *  0b0111111..SYSCON_CGU cgu_ss_peri_rootclk7 input is selected
 *  0b1000000..SYSCON_CGU cgu_ss_audio_rootclk input is selected
 *  0b1000001..SYSCON_CGU cgu_ss_video_rootclk input is selected
 *  0b1000010..SYSCON_CGU cgu_ss_usb1_rootclk input is selected
 *  0b1000011..SYSCON_CGU cgu_ss_eth_rootclk input is selected
 *  0b1000100..SYSCON_CGU cgu_ss_test_rootclk input is selected
 */
#define INPUTMUX_FREQME_TAR_INP(x)               (((uint32_t)(((uint32_t)(x)) << INPUTMUX_FREQME_TAR_INP_SHIFT)) & INPUTMUX_FREQME_TAR_INP_MASK)  /* Merged from fields with different position or width, of widths (5, 6, 7), largest definition used */
/*! @} */


/*!
 * @}
 */ /* end of group INPUTMUX_Register_Masks */


/*!
 * @}
 */ /* end of group INPUTMUX_Peripheral_Access_Layer */


/*
** End of section using anonymous unions
*/

#if defined(__ARMCC_VERSION)
  #if (__ARMCC_VERSION >= 6010050)
    #pragma clang diagnostic pop
  #else
    #pragma pop
  #endif
#elif defined(__GNUC__)
  /* leave anonymous unions enabled */
#elif defined(__IAR_SYSTEMS_ICC__)
  #pragma language=default
#else
  #error Not supported compiler type
#endif

/*!
 * @}
 */ /* end of group Peripheral_access_layer */


#endif  /* PERI_INPUTMUX_H_ */


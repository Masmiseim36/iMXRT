/*****************************************************************************
 * Copyright (c) 2018 Rowley Associates Limited.                             *
 * Modified work Copyright (C) 2021-2026 Markus Klein                        *
 *                                                                           *
 * This file may be distributed under the terms of the License Agreement     *
 * provided with this software.                                              *
 *                                                                           *
 * THIS FILE IS PROVIDED AS IS WITH NO WARRANTY OF ANY KIND, INCLUDING THE   *
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. *
 *****************************************************************************/
/*****************************************************************************
 *                           Preprocessor Definitions
 *                           ------------------------
 *
 * STARTUP_FROM_RESET
 *
 *   If defined, the program will startup from power-on/reset. If not defined
 *   the program will just loop endlessly from power-on/reset.
 *
 *   This definition is not defined by default on this target because the
 *   debugger is unable to reset this target and maintain control of it over the
 *   JTAG interface. The advantage of doing this is that it allows the debugger
 *   to reset the CPU and run programs from a known reset CPU state on each run.
 *   It also acts as a safety net if you accidently download a program in FLASH
 *   that crashes and prevents the debugger from taking control over JTAG
 *   rendering the target unusable over JTAG. The obvious disadvantage of doing
 *   this is that your application will not startup without the debugger.
 *
 *   We advise that on this target you keep STARTUP_FROM_RESET undefined whilst
 *   you are developing and only define STARTUP_FROM_RESET when development is
 *   complete.
 *
 * __NO_SYSTEM_INIT
 *
 *   If defined, the SystemInit() function will NOT be called. By default SystemInit()
 *   is called after reset to enable the clocks and memories to be initialised
 *   prior to any C startup initialisation.
 *
 * VECTORS_IN_RAM
 *
 *   If defined then the exception vectors are copied from Flash to RAM
 *
 * DISABLE_ECC_ON_STARTUP
 *
 *   If defined then the ECC functionality is disabled on start-up
 *
 * __NO_FPU
 *
 *   If defined do NOT turn on the FPU
 *
 * __NO_RUNFAST_MODE
 *
 *   If defined do NOT turn on flush-to-zero and default NaN modes
 *
 *****************************************************************************/

.macro ISR_HANDLER name=
  .section .vectors, "ax"
  .word \name
  .section .init, "ax"
  .thumb_func
  .weak \name
\name:
1: b 1b /* endless loop */
.endm

.macro ISR_HANDLER2 name=
  .section .vectors, "ax"
  .word \name\()_IRQHandler
  .section .init, "ax"
  .thumb_func
  .weak \name\()_IRQHandler
\name\()_IRQHandler:
1: b \name\()_DriverIRQHandler
  .thumb_func
  .weak \name\()_DriverIRQHandler
\name\()_DriverIRQHandler:
1: b 1b
.endm

.macro ISR_RESERVED
  .section .vectors, "ax"
  .word 0
.endm

  .syntax unified
  .global reset_handler
  .global Reset_Handler

  .section .vectors, "ax"
  .code 16
  .global _vectors
_vectors:
  .global __Vectors
__Vectors:
  .global g_pfnVectors
g_pfnVectors:
   .long   __stack_end__
#ifdef STARTUP_FROM_RESET
  .word reset_handler
#else
  .word reset_wait
#endif /* STARTUP_FROM_RESET */
ISR_HANDLER NMI_Handler                         // The NMI handler
ISR_HANDLER HardFault_Handler                   // The hard fault handler
ISR_HANDLER MemManage_Handler                   // The MPU fault handler
ISR_HANDLER BusFault_Handler                    // The bus fault handler
ISR_HANDLER UsageFault_Handler                  // The usage fault handler
ISR_HANDLER SecureFault_Handler                 // Secure Fault Handler
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_HANDLER SVC_Handler
ISR_HANDLER DebugMon_Handler
ISR_RESERVED
ISR_HANDLER PendSV_Handler
ISR_HANDLER SysTick_Handler
  // External Interrupts
ISR_HANDLER2 CMPT_CM85_CTI        // 16 : CMPT_CM85_CTI_IRQHandler         - CM85: Cti Interrupt Request 0/Cti Interrupt Request 1
ISR_RESERVED                      // 17 : Reserved17_IRQHandler            - Reserved interrupt
ISR_RESERVED                      // 18 : Reserved18_IRQHandler            - Reserved interrupt
ISR_HANDLER2 CMPT_CM85_ECC_ERROR  // 19 : CMPT_CM85_ECC_ERROR_IRQHandler   - CM85: Single / Multi Bit Ecc Error From L1$ Or Tcm
ISR_RESERVED                      // 20 : Reserved20_IRQHandler            - Reserved interrupt
ISR_RESERVED                      // 21 : Reserved21_IRQHandler            - Reserved interrupt
ISR_HANDLER2 CMPT_CM85_MCM        // 22 : CMPT_CM85_MCM_IRQHandler         - CMPT CM85 MCM: Interrupt request
ISR_HANDLER2 CMPT_TRDC            // 23 : CMPT_TRDC_IRQHandler             - CMPT TRDC: Interrupt request
ISR_HANDLER2 CMPT_NPU             // 24 : CMPT_NPU_IRQHandler              - CMPT NPU: Interrupt request
ISR_HANDLER2 CMPT_SRAMCTL_0       // 25 : CMPT_SRAMCTL_0_IRQHandler        - CMPT SRAMCTL0: Interrupt request
ISR_HANDLER2 CMPT_SRAMCTL_1       // 26 : CMPT_SRAMCTL_1_IRQHandler        - CMPT SRAMCTL1: Interrupt request
ISR_HANDLER2 CMPT_SRAMCTL_2       // 27 : CMPT_SRAMCTL_2_IRQHandler        - CMPT SRAMCTL2: Interrupt request
ISR_RESERVED                      // 28 : Reserved28_IRQHandler            - Reserved interrupt
ISR_RESERVED                      // 29 : Reserved29_IRQHandler            - Reserved interrupt
ISR_RESERVED                      // 30 : Reserved30_IRQHandler            - Reserved interrupt
ISR_RESERVED                      // 31 : Reserved31_IRQHandler            - Reserved interrupt
ISR_HANDLER2 CMPT_FREQME          // 32 : CMPT_FREQME_IRQHandler           - CMPT FREQME: Interrupt request
ISR_RESERVED                      // 33 : Reserved33_IRQHandler            - Reserved interrupt
ISR_RESERVED                      // 34 : Reserved34_IRQHandler            - Reserved interrupt
ISR_RESERVED                      // 35 : Reserved35_IRQHandler            - Reserved interrupt
ISR_HANDLER2 CMPT_LLC             // 36 : CMPT_LLC_IRQHandler              - CMPT LLC: Interrupt request
ISR_RESERVED                      // 37 : Reserved37_IRQHandler            - Reserved interrupt
ISR_RESERVED                      // 38 : Reserved38_IRQHandler            - Reserved interrupt
ISR_RESERVED                      // 39 : Reserved39_IRQHandler            - Reserved interrupt
ISR_HANDLER2 CMPT_DAP             // 40 : CMPT_DAP_IRQHandler              - CMPT DAP: Interrupt request
ISR_HANDLER2 MAIN_TRDC            // 41 : MAIN_TRDC_IRQHandler             - MAIN TRDC: Interrupt request
ISR_HANDLER2 MAIN_AHB2AXI         // 42 : MAIN_AHB2AXI_IRQHandler          - XHB500: Interrupt On Error Response for a Buffered Or Early Terminated Write
ISR_RESERVED                      // 43 : Reserved43_IRQHandler            - Reserved interrupt
ISR_HANDLER2 MAIN_MMU             // 44 : MAIN_MMU_IRQHandler              - MMU: Interrupt request
ISR_HANDLER2 MAIN_EDMA3_CH0       // 45 : MAIN_EDMA3_CH0_IRQHandler        - MAIN EDMA3: Channel 0 interrupt
ISR_HANDLER2 MAIN_EDMA3_CH1       // 46 : MAIN_EDMA3_CH1_IRQHandler        - MAIN EDMA3: Channel 1 interrupt
ISR_HANDLER2 MAIN_EDMA3_CH2       // 47 : MAIN_EDMA3_CH2_IRQHandler        - MAIN EDMA3: Channel 2 interrupt
ISR_HANDLER2 MAIN_EDMA3_CH3       // 48 : MAIN_EDMA3_CH3_IRQHandler        - MAIN EDMA3: Channel 3 interrupt
ISR_HANDLER2 MAIN_EDMA3_CH4       // 49 : MAIN_EDMA3_CH4_IRQHandler        - MAIN EDMA3: Channel 4 interrupt
ISR_HANDLER2 MAIN_EDMA3_CH5       // 50 : MAIN_EDMA3_CH5_IRQHandler        - MAIN EDMA3: Channel 5 interrupt
ISR_HANDLER2 MAIN_EDMA3_CH6       // 51 : MAIN_EDMA3_CH6_IRQHandler        - MAIN EDMA3: Channel 6 interrupt
ISR_HANDLER2 MAIN_EDMA3_CH7       // 52 : MAIN_EDMA3_CH7_IRQHandler        - MAIN EDMA3: Channel 7 interrupt
ISR_HANDLER2 MAIN_EDMA3_CH8       // 53 : MAIN_EDMA3_CH8_IRQHandler        - MAIN EDMA3: Channel 8 interrupt
ISR_HANDLER2 MAIN_EDMA3_CH9       // 54 : MAIN_EDMA3_CH9_IRQHandler        - MAIN EDMA3: Channel 9 interrupt
ISR_HANDLER2 MAIN_EDMA3_CH10      // 55 : MAIN_EDMA3_CH10_IRQHandler       - MAIN EDMA3: Channel 10 interrupt
ISR_HANDLER2 MAIN_EDMA3_CH11      // 56 : MAIN_EDMA3_CH11_IRQHandler       - MAIN EDMA3: Channel 11 interrupt
ISR_HANDLER2 MAIN_EDMA3_CH12      // 57 : MAIN_EDMA3_CH12_IRQHandler       - MAIN EDMA3: Channel 12 interrupt
ISR_HANDLER2 MAIN_EDMA3_CH13      // 58 : MAIN_EDMA3_CH13_IRQHandler       - MAIN EDMA3: Channel 13 interrupt
ISR_HANDLER2 MAIN_EDMA3_CH14      // 59 : MAIN_EDMA3_CH14_IRQHandler       - MAIN EDMA3: Channel 14 interrupt
ISR_HANDLER2 MAIN_EDMA3_CH15      // 60 : MAIN_EDMA3_CH15_IRQHandler       - MAIN EDMA3: Channel 15 interrupt
ISR_HANDLER2 MAIN_EDMA3_CH16      // 61 : MAIN_EDMA3_CH16_IRQHandler       - MAIN EDMA3: Channel 16 interrupt
ISR_HANDLER2 MAIN_EDMA3_CH17      // 62 : MAIN_EDMA3_CH17_IRQHandler       - MAIN EDMA3: Channel 17 interrupt
ISR_HANDLER2 MAIN_EDMA3_CH18      // 63 : MAIN_EDMA3_CH18_IRQHandler       - MAIN EDMA3: Channel 18 interrupt
ISR_HANDLER2 MAIN_EDMA3_CH19      // 64 : MAIN_EDMA3_CH19_IRQHandler       - MAIN EDMA3: Channel 19 interrupt
ISR_HANDLER2 MAIN_EDMA3_CH20      // 65 : MAIN_EDMA3_CH20_IRQHandler       - MAIN EDMA3: Channel 20 interrupt
ISR_HANDLER2 MAIN_EDMA3_CH21      // 66 : MAIN_EDMA3_CH21_IRQHandler       - MAIN EDMA3: Channel 21 interrupt
ISR_HANDLER2 MAIN_EDMA3_CH22      // 67 : MAIN_EDMA3_CH22_IRQHandler       - MAIN EDMA3: Channel 22 interrupt
ISR_HANDLER2 MAIN_EDMA3_CH23      // 68 : MAIN_EDMA3_CH23_IRQHandler       - MAIN EDMA3: Channel 23 interrupt
ISR_HANDLER2 MAIN_EDMA3_CH24      // 69 : MAIN_EDMA3_CH24_IRQHandler       - MAIN EDMA3: Channel 24 interrupt
ISR_HANDLER2 MAIN_EDMA3_CH25      // 70 : MAIN_EDMA3_CH25_IRQHandler       - MAIN EDMA3: Channel 25 interrupt
ISR_HANDLER2 MAIN_EDMA3_CH26      // 71 : MAIN_EDMA3_CH26_IRQHandler       - MAIN EDMA3: Channel 26 interrupt
ISR_HANDLER2 MAIN_EDMA3_CH27      // 72 : MAIN_EDMA3_CH27_IRQHandler       - MAIN EDMA3: Channel 27 interrupt
ISR_HANDLER2 MAIN_EDMA3_CH28      // 73 : MAIN_EDMA3_CH28_IRQHandler       - MAIN EDMA3: Channel 28 interrupt
ISR_HANDLER2 MAIN_EDMA3_CH29      // 74 : MAIN_EDMA3_CH29_IRQHandler       - MAIN EDMA3: Channel 29 interrupt
ISR_HANDLER2 MAIN_EDMA3_CH30      // 75 : MAIN_EDMA3_CH30_IRQHandler       - MAIN EDMA3: Channel 30 interrupt
ISR_HANDLER2 MAIN_EDMA3_CH31      // 76 : MAIN_EDMA3_CH31_IRQHandler       - MAIN EDMA3: Channel 31 interrupt
ISR_HANDLER2 MAIN_EDMA5_CH0       // 77 : MAIN_EDMA5_CH0_IRQHandler        - MAIN EDMA5: Channel 0 interrupt
ISR_HANDLER2 MAIN_EDMA5_CH1       // 78 : MAIN_EDMA5_CH1_IRQHandler        - MAIN EDMA5: Channel 1 interrupt
ISR_HANDLER2 MAIN_EDMA5_CH2       // 79 : MAIN_EDMA5_CH2_IRQHandler        - MAIN EDMA5: Channel 2 interrupt
ISR_HANDLER2 MAIN_EDMA5_CH3       // 80 : MAIN_EDMA5_CH3_IRQHandler        - MAIN EDMA5: Channel 3 interrupt
ISR_HANDLER2 MAIN_EDMA5_CH4       // 81 : MAIN_EDMA5_CH4_IRQHandler        - MAIN EDMA5: Channel 4 interrupt
ISR_HANDLER2 MAIN_EDMA5_CH5       // 82 : MAIN_EDMA5_CH5_IRQHandler        - MAIN EDMA5: Channel 5 interrupt
ISR_HANDLER2 MAIN_EDMA5_CH6       // 83 : MAIN_EDMA5_CH6_IRQHandler        - MAIN EDMA5: Channel 6 interrupt
ISR_HANDLER2 MAIN_EDMA5_CH7       // 84 : MAIN_EDMA5_CH7_IRQHandler        - MAIN EDMA5: Channel 7 interrupt
ISR_HANDLER2 MAIN_EDMA5_CH8       // 85 : MAIN_EDMA5_CH8_IRQHandler        - MAIN EDMA5: Channel 8 interrupt
ISR_HANDLER2 MAIN_EDMA5_CH9       // 86 : MAIN_EDMA5_CH9_IRQHandler        - MAIN EDMA5: Channel 9 interrupt
ISR_HANDLER2 MAIN_EDMA5_CH10      // 87 : MAIN_EDMA5_CH10_IRQHandler       - MAIN EDMA5: Channel 10 interrupt
ISR_HANDLER2 MAIN_EDMA5_CH11      // 88 : MAIN_EDMA5_CH11_IRQHandler       - MAIN EDMA5: Channel 11 interrupt
ISR_HANDLER2 MAIN_EDMA5_CH12      // 89 : MAIN_EDMA5_CH12_IRQHandler       - MAIN EDMA5: Channel 12 interrupt
ISR_HANDLER2 MAIN_EDMA5_CH13      // 90 : MAIN_EDMA5_CH13_IRQHandler       - MAIN EDMA5: Channel 13 interrupt
ISR_HANDLER2 MAIN_EDMA5_CH14      // 91 : MAIN_EDMA5_CH14_IRQHandler       - MAIN EDMA5: Channel 14 interrupt
ISR_HANDLER2 MAIN_EDMA5_CH15      // 92 : MAIN_EDMA5_CH15_IRQHandler       - MAIN EDMA5: Channel 15 interrupt
ISR_HANDLER2 MAIN_FREQME          // 93 : MAIN_FREQME_IRQHandler           - MAIN FREQME: interrupt request
ISR_HANDLER2 HSP_STM              // 94 : HSP_STM_IRQHandler               - HSP STM: Interrupt request
ISR_RESERVED                      // 95 : Reserved95_IRQHandler            - Reserved interrupt
ISR_RESERVED                      // 96 : Reserved96_IRQHandler            - Reserved interrupt
ISR_RESERVED                      // 97 : Reserved97_IRQHandler            - Reserved interrupt
ISR_HANDLER2 MAIN_XSPI0           // 98 : MAIN_XSPI0_IRQHandler            - MAIN XSPI0: interrupt request
ISR_HANDLER2 MAIN_XSPI1           // 99 : MAIN_XSPI1_IRQHandler            - MAIN XSPI1: interrupt request
ISR_RESERVED                      // 100: Reserved100_IRQHandler           - Reserved interrupt
ISR_RESERVED                      // 101: Reserved101_IRQHandler           - Reserved interrupt
ISR_HANDLER2 SYSCON_CGU_INT0      // 102: SYSCON_CGU_INT0_IRQHandler       - CGU: Vdda 0V8 Status Interrupt
ISR_HANDLER2 SYSCON_CGU_INT1      // 103: SYSCON_CGU_INT1_IRQHandler       - CGU: Audiopll Dnum Update Done Interrupt
ISR_HANDLER2 SYSCON_CGU_INT2      // 104: SYSCON_CGU_INT2_IRQHandler       - CGU: Videopll Dnum Update Done Interrupt
ISR_HANDLER2 MAIN_CSSI            // 105: MAIN_CSSI_IRQHandler             - CSSI:Status/Error Interrupt/Active Interrupt
ISR_HANDLER2 MAIN_CSSI_SENTMU0A   // 106: MAIN_CSSI_SENTMU0A_IRQHandler    - CSSI_MU: MU0A Ored Tx Interrupt To Mua
ISR_HANDLER2 MAIN_CSSI_SENTMU1A   // 107: MAIN_CSSI_SENTMU1A_IRQHandler    - CSSI_MU: Mu1A Ored Tx Interrupt To Mua
ISR_RESERVED                      // 108: Reserved108_IRQHandler           - Reserved interrupt
ISR_HANDLER2 SYSCON_PMU_INT0      // 109: SYSCON_PMU_INT0_IRQHandler       - PMU: Ored All Sensor And Monitor Interrupts
ISR_HANDLER2 SYSCON_PMU_INT1      // 110: SYSCON_PMU_INT1_IRQHandler       - PMU: Body Bias Interrupt
ISR_HANDLER2 SYSCON_RESETCON      // 111: SYSCON_RESETCON_IRQHandler       - SYSCON RESETCON: Interrupt request
ISR_HANDLER2 SYSCON_FREQME        // 112: SYSCON_FREQME_IRQHandler         - SYSCON FREQME: Interrupt request
ISR_RESERVED                      // 113: Reserved113_IRQHandler           - Reserved interrupt
ISR_RESERVED                      // 114: Reserved114_IRQHandler           - Reserved interrupt
ISR_RESERVED                      // 115: Reserved115_IRQHandler           - Reserved interrupt
ISR_HANDLER2 HSP_XBAR0_CH0        // 116: HSP_XBAR0_CH0_IRQHandler         - HSP XBAR0 Channel 0 interrupt
ISR_HANDLER2 HSP_XBAR0_CH1        // 117: HSP_XBAR0_CH1_IRQHandler         - HSP XBAR0 Channel 1 interrupt
ISR_HANDLER2 HSP_XBAR0_CH2        // 118: HSP_XBAR0_CH2_IRQHandler         - HSP XBAR0 Channel 2 interrupt
ISR_HANDLER2 HSP_XBAR0_CH3        // 119: HSP_XBAR0_CH3_IRQHandler         - HSP XBAR0 Channel 3 interrupt
ISR_HANDLER2 HSP_XBAR0_CH4        // 120: HSP_XBAR0_CH4_IRQHandler         - HSP XBAR0 Channel 4 interrupt
ISR_HANDLER2 HSP_XBAR0_CH5        // 121: HSP_XBAR0_CH5_IRQHandler         - HSP XBAR0 Channel 5 interrupt
ISR_HANDLER2 HSP_XBAR0_CH6        // 122: HSP_XBAR0_CH6_IRQHandler         - HSP XBAR0 Channel 6 interrupt
ISR_HANDLER2 HSP_XBAR0_CH7        // 123: HSP_XBAR0_CH7_IRQHandler         - HSP XBAR0 Channel 7 interrupt
ISR_RESERVED                      // 124: Reserved124_IRQHandler           - Reserved interrupt
ISR_RESERVED                      // 125: Reserved125_IRQHandler           - Reserved interrupt
ISR_RESERVED                      // 126: Reserved126_IRQHandler           - Reserved interrupt
ISR_RESERVED                      // 127: Reserved127_IRQHandler           - Reserved interrupt
ISR_HANDLER2 HSP_ADC0             // 128: HSP_ADC0_IRQHandler              - HSP ADC0 interrupt
ISR_HANDLER2 HSP_ADC1             // 129: HSP_ADC1_IRQHandler              - HSP ADC1 interrupt
ISR_HANDLER2 HSP_DAC              // 130: HSP_DAC_IRQHandler               - HSP DAC interrupt
ISR_HANDLER2 HSP_LPIT0            // 131: HSP_LPIT0_IRQHandler             - HSP LPIT0 interrupt
ISR_HANDLER2 HSP_LPIT1            // 132: HSP_LPIT1_IRQHandler             - HSP LPIT1 interrupt
ISR_HANDLER2 HSP_EFLEXPWM0_CH0    // 133: HSP_EFLEXPWM0_CH0_IRQHandler     - HSP EFLEXPWM0 channel 0 interrupt
ISR_HANDLER2 HSP_EFLEXPWM0_CH1    // 134: HSP_EFLEXPWM0_CH1_IRQHandler     - HSP EFLEXPWM0 channel 1 interrupt
ISR_HANDLER2 HSP_EFLEXPWM0_CH2    // 135: HSP_EFLEXPWM0_CH2_IRQHandler     - HSP EFLEXPWM0 channel 2 interrupt
ISR_HANDLER2 HSP_EFLEXPWM0_CH3    // 136: HSP_EFLEXPWM0_CH3_IRQHandler     - HSP EFLEXPWM0 channel 3 interrupt
ISR_HANDLER2 HSP_EFLEXPWM0_ERROR  // 137: HSP_EFLEXPWM0_ERROR_IRQHandler   - HSP EFLEXPWM0 fault interrupt
ISR_HANDLER2 HSP_EFLEXPWM1_CH0    // 138: HSP_EFLEXPWM1_CH0_IRQHandler     - HSP EFLEXPWM1 channel 0 interrupt
ISR_HANDLER2 HSP_EFLEXPWM1_CH1    // 139: HSP_EFLEXPWM1_CH1_IRQHandler     - HSP EFLEXPWM1 channel 1 interrupt
ISR_HANDLER2 HSP_EFLEXPWM1_CH2    // 140: HSP_EFLEXPWM1_CH2_IRQHandler     - HSP EFLEXPWM1 channel 2 interrupt
ISR_HANDLER2 HSP_EFLEXPWM1_CH3    // 141: HSP_EFLEXPWM1_CH3_IRQHandler     - HSP EFLEXPWM1 channel 3 interrupt
ISR_HANDLER2 HSP_EFLEXPWM1_ERROR  // 142: HSP_EFLEXPWM1_ERROR_IRQHandler   - HSP EFLEXPWM1 fault interrupt
ISR_HANDLER2 HSP_EFLEXPWM2_CH0    // 143: HSP_EFLEXPWM2_CH0_IRQHandler     - HSP EFLEXPWM2 channel 0 interrupt
ISR_HANDLER2 HSP_EFLEXPWM2_CH1    // 144: HSP_EFLEXPWM2_CH1_IRQHandler     - HSP EFLEXPWM2 channel 1 interrupt
ISR_HANDLER2 HSP_EFLEXPWM2_CH2    // 145: HSP_EFLEXPWM2_CH2_IRQHandler     - HSP EFLEXPWM2 channel 2 interrupt
ISR_HANDLER2 HSP_EFLEXPWM2_CH3    // 146: HSP_EFLEXPWM2_CH3_IRQHandler     - HSP EFLEXPWM2 channel 3 interrupt
ISR_HANDLER2 HSP_EFLEXPWM2_ERROR  // 147: HSP_EFLEXPWM2_ERROR_IRQHandler   - HSP EFLEXPWM2 fault interrupt
ISR_HANDLER2 HSP_EFLEXPWM3_CH0    // 148: HSP_EFLEXPWM3_CH0_IRQHandler     - HSP EFLEXPWM3 channel 0 interrupt
ISR_HANDLER2 HSP_EFLEXPWM3_CH1    // 149: HSP_EFLEXPWM3_CH1_IRQHandler     - HSP EFLEXPWM3 channel 1 interrupt
ISR_HANDLER2 HSP_EFLEXPWM3_CH2    // 150: HSP_EFLEXPWM3_CH2_IRQHandler     - HSP EFLEXPWM3 channel 2 interrupt
ISR_HANDLER2 HSP_EFLEXPWM3_CH3    // 151: HSP_EFLEXPWM3_CH3_IRQHandler     - HSP EFLEXPWM3 channel 3 interrupt
ISR_HANDLER2 HSP_EFLEXPWM3_ERROR  // 152: HSP_EFLEXPWM3_ERROR_IRQHandler   - HSP EFLEXPWM3 fault interrupt
ISR_HANDLER2 HSP_QTPM0_CH0        // 153: HSP_QTPM0_CH0_IRQHandler         - HSP QUADTPM0 channel 0 interrupt
ISR_HANDLER2 HSP_QTPM0_CH1        // 154: HSP_QTPM0_CH1_IRQHandler         - HSP QUADTPM0 channel 1 interrupt
ISR_HANDLER2 HSP_QTPM0_CH2        // 155: HSP_QTPM0_CH2_IRQHandler         - HSP QUADTPM0 channel 2 interrupt
ISR_HANDLER2 HSP_QTPM0_CH3        // 156: HSP_QTPM0_CH3_IRQHandler         - HSP QUADTPM0 channel 3 interrupt
ISR_RESERVED                      // 157: Reserved157_IRQHandler           - Reserved interrupt
ISR_RESERVED                      // 158: Reserved158_IRQHandler           - Reserved interrupt
ISR_RESERVED                      // 159: Reserved159_IRQHandler           - Reserved interrupt
ISR_RESERVED                      // 160: Reserved160_IRQHandler           - Reserved interrupt
ISR_RESERVED                      // 161: Reserved161_IRQHandler           - Reserved interrupt
ISR_RESERVED                      // 162: Reserved162_IRQHandler           - Reserved interrupt
ISR_RESERVED                      // 163: Reserved163_IRQHandler           - Reserved interrupt
ISR_RESERVED                      // 164: Reserved164_IRQHandler           - Reserved interrupt
ISR_HANDLER2 HSP_QTMR0            // 165: HSP_QTMR0_IRQHandler             - HSP QUADTMR0 interrupt
ISR_HANDLER2 HSP_QTMR1            // 166: HSP_QTMR1_IRQHandler             - HSP QUADTMR1 interrupt
ISR_HANDLER2 HSP_QTMR2            // 167: HSP_QTMR2_IRQHandler             - HSP QUADTMR2 interrupt
ISR_HANDLER2 HSP_QTMR3            // 168: HSP_QTMR3_IRQHandler             - HSP QUADTMR3 interrupt
ISR_HANDLER2 HSP_EQDC0            // 169: HSP_EQDC0_IRQHandler             - HSP EQDC0 interrupt
ISR_HANDLER2 HSP_EQDC1            // 170: HSP_EQDC1_IRQHandler             - HSP EQDC1 interrupt
ISR_HANDLER2 HSP_EQDC2            // 171: HSP_EQDC2_IRQHandler             - HSP EQDC2 interrupt
ISR_HANDLER2 HSP_EQDC3            // 172: HSP_EQDC3_IRQHandler             - HSP EQDC3 interrupt
ISR_HANDLER2 HSP_SINC0_CH0        // 173: HSP_SINC0_CH0_IRQHandler         - HSP SINC0 channel 0 interrupt
ISR_HANDLER2 HSP_SINC0_CH1        // 174: HSP_SINC0_CH1_IRQHandler         - HSP SINC0 channel 1 interrupt
ISR_HANDLER2 HSP_SINC0_CH2        // 175: HSP_SINC0_CH2_IRQHandler         - HSP SINC0 channel 2 interrupt
ISR_HANDLER2 HSP_SINC0_CH3        // 176: HSP_SINC0_CH3_IRQHandler         - HSP SINC0 channel 3 interrupt
ISR_HANDLER2 HSP_SINC1_CH0        // 177: HSP_SINC1_CH0_IRQHandler         - HSP SINC1 channel 0 interrupt
ISR_HANDLER2 HSP_SINC1_CH1        // 178: HSP_SINC1_CH1_IRQHandler         - HSP SINC1 channel 1 interrupt
ISR_HANDLER2 HSP_SINC1_CH2        // 179: HSP_SINC1_CH2_IRQHandler         - HSP SINC1 channel 2 interrupt
ISR_HANDLER2 HSP_SINC1_CH3        // 180: HSP_SINC1_CH3_IRQHandler         - HSP SINC1 channel 3 interrupt
ISR_HANDLER2 HSP_FLEXCAN0         // 181: HSP_FLEXCAN0_IRQHandler          - HSP FLEXCAN0 interrupt
ISR_HANDLER2 HSP_FLEXCAN0_ERROR   // 182: HSP_FLEXCAN0_ERROR_IRQHandler    - HSP FLEXCAN0 error interrupt
ISR_HANDLER2 HSP_FLEXCAN1         // 183: HSP_FLEXCAN1_IRQHandler          - HSP FLEXCAN1 interrupt
ISR_HANDLER2 HSP_FLEXCAN1_ERROR   // 184: HSP_FLEXCAN1_ERROR_IRQHandler    - HSP FLEXCAN1 error interrupt
ISR_HANDLER2 HSP_FLEXCAN2         // 185: HSP_FLEXCAN2_IRQHandler          - HSP FLEXCAN2 interrupt
ISR_HANDLER2 HSP_FLEXCAN2_ERROR   // 186: HSP_FLEXCAN2_ERROR_IRQHandler    - HSP FLEXCAN2 error interrupt
ISR_HANDLER2 HSP_FLEXIO0          // 187: HSP_FLEXIO0_IRQHandler           - HSP FLEXIO0 interrupt
ISR_HANDLER2 HSP_FLEXIO1          // 188: HSP_FLEXIO1_IRQHandler           - HSP FLEXIO1 interrupt
ISR_HANDLER2 HSP_FLEXIO2          // 189: HSP_FLEXIO2_IRQHandler           - HSP FLEXIO2 interrupt
ISR_HANDLER2 HSP_I3C              // 190: HSP_I3C_IRQHandler               - HSP I3C interrupt
ISR_HANDLER2 HSP_LPI2C0           // 191: HSP_LPI2C0_IRQHandler            - HSP LPI2C0 interrupt
ISR_HANDLER2 HSP_LPI2C1           // 192: HSP_LPI2C1_IRQHandler            - HSP LPI2C1 interrupt
ISR_RESERVED                      // 193: Reserved193_IRQHandler           - Reserved interrupt
ISR_RESERVED                      // 194: Reserved194_IRQHandler           - Reserved interrupt
ISR_HANDLER2 HSP_LPSPI0           // 195: HSP_LPSPI0_IRQHandler            - HSP LPSPI0 interrupt
ISR_HANDLER2 HSP_LPSPI1           // 196: HSP_LPSPI1_IRQHandler            - HSP LPSPI1 interrupt
ISR_HANDLER2 HSP_LPSPI2           // 197: HSP_LPSPI2_IRQHandler            - HSP LPSPI2 interrupt
ISR_HANDLER2 HSP_LPSPI3           // 198: HSP_LPSPI3_IRQHandler            - HSP LPSPI3 interrupt
ISR_HANDLER2 HSP_LPSPI4           // 199: HSP_LPSPI4_IRQHandler            - HSP LPSPI4 interrupt
ISR_HANDLER2 HSP_LPUART0          // 200: HSP_LPUART0_IRQHandler           - HSP LPUART0 interrupt
ISR_HANDLER2 HSP_LPUART1          // 201: HSP_LPUART1_IRQHandler           - HSP LPUART1 interrupt
ISR_HANDLER2 HSP_LPUART2          // 202: HSP_LPUART2_IRQHandler           - HSP LPUART2 interrupt
ISR_HANDLER2 HSP_LPUART3          // 203: HSP_LPUART3_IRQHandler           - HSP LPUART3 interrupt
ISR_HANDLER2 HSP_LPUART4          // 204: HSP_LPUART4_IRQHandler           - HSP LPUART4 interrupt
ISR_HANDLER2 HSP_LPUART5          // 205: HSP_LPUART5_IRQHandler           - HSP LPUART5 interrupt
ISR_HANDLER2 HSP_GPIO0_CH0        // 206: HSP_GPIO0_CH0_IRQHandler         - HSP GPIO0 channel 0 interrupt
ISR_HANDLER2 HSP_GPIO0_CH1        // 207: HSP_GPIO0_CH1_IRQHandler         - HSP GPIO0 channel 1 interrupt
ISR_HANDLER2 HSP_GPIO1_CH0        // 208: HSP_GPIO1_CH0_IRQHandler         - HSP GPIO1 channel 0 interrupt
ISR_HANDLER2 HSP_GPIO1_CH1        // 209: HSP_GPIO1_CH1_IRQHandler         - HSP GPIO1 channel 1 interrupt
ISR_HANDLER2 HSP_GPIO2_CH0        // 210: HSP_GPIO2_CH0_IRQHandler         - HSP GPIO2 channel 0 interrupt
ISR_HANDLER2 HSP_GPIO2_CH1        // 211: HSP_GPIO2_CH1_IRQHandler         - HSP GPIO2 channel 1 interrupt
ISR_HANDLER2 HSP_GPIO3_CH0        // 212: HSP_GPIO3_CH0_IRQHandler         - HSP GPIO3 channel 0 interrupt
ISR_HANDLER2 HSP_GPIO3_CH1        // 213: HSP_GPIO3_CH1_IRQHandler         - HSP GPIO3 channel 1 interrupt
ISR_HANDLER2 HSP_GPIO4_CH0        // 214: HSP_GPIO4_CH0_IRQHandler         - HSP GPIO4 channel 0 interrupt
ISR_HANDLER2 HSP_GPIO4_CH1        // 215: HSP_GPIO4_CH1_IRQHandler         - HSP GPIO4 channel 1 interrupt
ISR_RESERVED                      // 216: Reserved216_IRQHandler           - Reserved interrupt
ISR_RESERVED                      // 217: Reserved217_IRQHandler           - Reserved interrupt
ISR_RESERVED                      // 218: Reserved218_IRQHandler           - Reserved interrupt
ISR_RESERVED                      // 219: Reserved219_IRQHandler           - Reserved interrupt
ISR_RESERVED                      // 220: Reserved220_IRQHandler           - Reserved interrupt
ISR_HANDLER2 WAKE_TRDC            // 221: WAKE_TRDC_IRQHandler             - WAKE TRDC interrupt
ISR_HANDLER2 WAKE_XBAR_CH0        // 222: WAKE_XBAR_CH0_IRQHandler         - WAKE XBAR channel 0 interrupt
ISR_HANDLER2 WAKE_XBAR_CH1        // 223: WAKE_XBAR_CH1_IRQHandler         - WAKE XBAR channel 1 interrupt
ISR_HANDLER2 WAKE_EDMA3_CH0       // 224: WAKE_EDMA3_CH0_IRQHandler        - WAKE EDMA3: Channel 0 interrupt
ISR_HANDLER2 WAKE_EDMA3_CH1       // 225: WAKE_EDMA3_CH1_IRQHandler        - WAKE EDMA3: Channel 1 interrupt
ISR_HANDLER2 WAKE_EDMA3_CH2       // 226: WAKE_EDMA3_CH2_IRQHandler        - WAKE EDMA3: Channel 2 interrupt
ISR_HANDLER2 WAKE_EDMA3_CH3       // 227: WAKE_EDMA3_CH3_IRQHandler        - WAKE EDMA3: Channel 3 interrupt
ISR_HANDLER2 WAKE_EDMA3_CH4       // 228: WAKE_EDMA3_CH4_IRQHandler        - WAKE EDMA3: Channel 4 interrupt
ISR_HANDLER2 WAKE_EDMA3_CH5       // 229: WAKE_EDMA3_CH5_IRQHandler        - WAKE EDMA3: Channel 5 interrupt
ISR_HANDLER2 WAKE_EDMA3_CH6       // 230: WAKE_EDMA3_CH6_IRQHandler        - WAKE EDMA3: Channel 6 interrupt
ISR_HANDLER2 WAKE_EDMA3_CH7       // 231: WAKE_EDMA3_CH7_IRQHandler        - WAKE EDMA3: Channel 7 interrupt
ISR_HANDLER2 WAKE_ACMP0           // 232: WAKE_ACMP0_IRQHandler            - WAKE CMP0 interrupt
ISR_HANDLER2 WAKE_ACMP1           // 233: WAKE_ACMP1_IRQHandler            - WAKE CMP1 interrupt
ISR_HANDLER2 WAKE_ACMP2           // 234: WAKE_ACMP2_IRQHandler            - WAKE CMP2 interrupt
ISR_HANDLER2 WAKE_ACMP3           // 235: WAKE_ACMP3_IRQHandler            - WAKE CMP3 interrupt
ISR_HANDLER2 WAKE_EWM             // 236: WAKE_EWM_IRQHandler              - WAKE EWM interrupt
ISR_HANDLER2 WAKE_FREQME          // 237: WAKE_FREQME_IRQHandler           - WAKE FREQME interrupt
ISR_HANDLER2 WAKE_LPTMR0          // 238: WAKE_LPTMR0_IRQHandler           - WAKE LPTMR0 interrupt
ISR_HANDLER2 WAKE_LPTMR1          // 239: WAKE_LPTMR1_IRQHandler           - WAKE LPTMR1 interrupt
ISR_HANDLER2 WAKE_QTPM_CH0        // 240: WAKE_QTPM_CH0_IRQHandler         - WAKE QTPM channel 0 interrupt
ISR_HANDLER2 WAKE_QTPM_CH1        // 241: WAKE_QTPM_CH1_IRQHandler         - WAKE QTPM channel 1 interrupt
ISR_HANDLER2 WAKE_QTPM_CH2        // 242: WAKE_QTPM_CH2_IRQHandler         - WAKE QTPM channel 2 interrupt
ISR_HANDLER2 WAKE_QTPM_CH3        // 243: WAKE_QTPM_CH3_IRQHandler         - WAKE QTPM channel 3 interrupt
ISR_HANDLER2 WAKE_SWT0            // 244: WAKE_SWT0_IRQHandler             - WAKE SWT0 interrupt
ISR_HANDLER2 WAKE_SWT1            // 245: WAKE_SWT1_IRQHandler             - WAKE SWT1 interrupt
ISR_RESERVED                      // 246: Reserved246_IRQHandler           - Reserved interrupt
ISR_RESERVED                      // 247: Reserved247_IRQHandler           - Reserved interrupt
ISR_RESERVED                      // 248: Reserved248_IRQHandler           - Reserved interrupt
ISR_RESERVED                      // 249: Reserved249_IRQHandler           - Reserved interrupt
ISR_HANDLER2 WAKE_I3C             // 250: WAKE_I3C_IRQHandler              - WAKE I3C interrupt
ISR_HANDLER2 WAKE_LPI2C0          // 251: WAKE_LPI2C0_IRQHandler           - WAKE LPI2C0 interrupt
ISR_HANDLER2 WAKE_LPI2C1          // 252: WAKE_LPI2C1_IRQHandler           - WAKE LPI2C1 interrupt
ISR_HANDLER2 WAKE_LPSPI           // 253: WAKE_LPSPI_IRQHandler            - WAKE LPSPI interrupt
ISR_HANDLER2 WAKE_LPUART0         // 254: WAKE_LPUART0_IRQHandler          - WAKE LPUART0 interrupt
ISR_HANDLER2 WAKE_LPUART1         // 255: WAKE_LPUART1_IRQHandler          - WAKE LPUART1 interrupt
ISR_HANDLER2 WAKE_GPIO_CH0        // 256: WAKE_GPIO_CH0_IRQHandler         - WAKE GPIO channel 0 interrupt
ISR_HANDLER2 WAKE_GPIO_CH1        // 257: WAKE_GPIO_CH1_IRQHandler         - WAKE GPIO channel 1 interrupt
ISR_HANDLER2 WAKE_PDM_EVENT       // 258: WAKE_PDM_EVENT_IRQHandler        - WAKE MICIFL  Filter Results Available (Fifo) Interrupt/
ISR_HANDLER2 WAKE_PDM_ERROR       // 259: WAKE_PDM_ERROR_IRQHandler        - WAKE MICIFL Error interrupt
ISR_HANDLER2 WAKE_PDM_HWVAD_EVENT // 260: WAKE_PDM_HWVAD_EVENT_IRQHandler  - WAKE MICIFL Hwvad Activation interrupt
ISR_HANDLER2 WAKE_PDM_HWVAD_ERROR // 261: WAKE_PDM_HWVAD_ERROR_IRQHandler  - WAKE MICIFL Hwvad Exception Interrupt
ISR_RESERVED                      // 262: Reserved262_IRQHandler           - Reserved interrupt
ISR_HANDLER2 VBAT_BBSM_INT0       // 263: VBAT_BBSM_INT0_IRQHandler        - BBSM: Alarm interrupt0
ISR_HANDLER2 VBAT_BBSM_INT1       // 264: VBAT_BBSM_INT1_IRQHandler        - BBSM: Alarm interrupt1
ISR_HANDLER2 VBAT_TDET            // 265: VBAT_TDET_IRQHandler             - VBAT TDET interrupt
ISR_HANDLER2 VBAT_LPTMR           // 266: VBAT_LPTMR_IRQHandler            - VBAT LPTMR interrupt
ISR_HANDLER2 VBAT_RTC             // 267: VBAT_RTC_IRQHandler              - VBAT RTC interrupt
ISR_HANDLER2 VBAT_GPIO_CH0        // 268: VBAT_GPIO_CH0_IRQHandler         - VBAT GPIO channel 0 interrupt
ISR_HANDLER2 VBAT_GPIO_CH1        // 269: VBAT_GPIO_CH1_IRQHandler         - VBAT GPIO channel 1 interrupt
ISR_HANDLER2 VBAT_VBATCON         // 270: VBAT_VBATCON_IRQHandler          - vabtcon Interrupt Request
ISR_HANDLER2 AUDIO_TRDC           // 271: AUDIO_TRDC_IRQHandler            - AUDIO TRDC interrupt
ISR_HANDLER2 AUDIO_SAI0           // 272: AUDIO_SAI0_IRQHandler            - AUDIO SAI0 interrupt
ISR_HANDLER2 AUDIO_SAI1           // 273: AUDIO_SAI1_IRQHandler            - AUDIO SAI1 interrupt
ISR_HANDLER2 AUDIO_SAI2           // 274: AUDIO_SAI2_IRQHandler            - AUDIO SAI2 interrupt
ISR_RESERVED                      // 275: Reserved275_IRQHandler           - Reserved interrupt
ISR_RESERVED                      // 276: Reserved276_IRQHandler           - Reserved interrupt
ISR_RESERVED                      // 277: Reserved277_IRQHandler           - Reserved interrupt
ISR_HANDLER2 AUDIO_PDM_EVENT      // 278: AUDIO_PDM_EVENT_IRQHandler       - AUDIO MICDIL Filter Results Available (Fifo) Interrupt
ISR_HANDLER2 AUDIO_PDM_ERROR      // 279: AUDIO_PDM_ERROR_IRQHandler       - AUDIO MICDIL Error interrupt
ISR_RESERVED                      // 280: Reserved280_IRQHandler           - Reserved interrupt
ISR_RESERVED                      // 281: Reserved281_IRQHandler           - Reserved interrupt
ISR_RESERVED                      // 282: Reserved282_IRQHandler           - Reserved interrupt
ISR_RESERVED                      // 283: Reserved283_IRQHandler           - Reserved interrupt
ISR_RESERVED                      // 284: Reserved284_IRQHandler           - Reserved interrupt
ISR_RESERVED                      // 285: Reserved285_IRQHandler           - Reserved interrupt
ISR_HANDLER2 AUDIO_SPDIF          // 286: AUDIO_SPDIF_IRQHandler           - AUDIO SPDIF interrupt
ISR_HANDLER2 AUDIO_EDMA3_CH0      // 287: AUDIO_EDMA3_CH0_IRQHandler       - AUDIO EDMA3: Channel 0 interrupt
ISR_HANDLER2 AUDIO_EDMA3_CH1      // 288: AUDIO_EDMA3_CH1_IRQHandler       - AUDIO EDMA3: Channel 1 interrupt
ISR_HANDLER2 AUDIO_EDMA3_CH2      // 289: AUDIO_EDMA3_CH2_IRQHandler       - AUDIO EDMA3: Channel 2 interrupt
ISR_HANDLER2 AUDIO_EDMA3_CH3      // 290: AUDIO_EDMA3_CH3_IRQHandler       - AUDIO EDMA3: Channel 3 interrupt
ISR_HANDLER2 AUDIO_EDMA3_CH4      // 291: AUDIO_EDMA3_CH4_IRQHandler       - AUDIO EDMA3: Channel 4 interrupt
ISR_HANDLER2 AUDIO_EDMA3_CH5      // 292: AUDIO_EDMA3_CH5_IRQHandler       - AUDIO EDMA3: Channel 5 interrupt
ISR_HANDLER2 AUDIO_EDMA3_CH6      // 293: AUDIO_EDMA3_CH6_IRQHandler       - AUDIO EDMA3: Channel 6 interrupt
ISR_HANDLER2 AUDIO_EDMA3_CH7      // 294: AUDIO_EDMA3_CH7_IRQHandler       - AUDIO EDMA3: Channel 7 interrupt
ISR_RESERVED                      // 295: Reserved295_IRQHandler           - Reserved interrupt
ISR_RESERVED                      // 296: Reserved296_IRQHandler           - Reserved interrupt
ISR_RESERVED                      // 297: Reserved297_IRQHandler           - Reserved interrupt
ISR_RESERVED                      // 298: Reserved298_IRQHandler           - Reserved interrupt
ISR_RESERVED                      // 299: Reserved299_IRQHandler           - Reserved interrupt
ISR_RESERVED                      // 300: Reserved300_IRQHandler           - Reserved interrupt
ISR_RESERVED                      // 301: Reserved301_IRQHandler           - Reserved interrupt
ISR_RESERVED                      // 302: Reserved302_IRQHandler           - Reserved interrupt
ISR_RESERVED                      // 303: Reserved303_IRQHandler           - Reserved interrupt
ISR_HANDLER2 AUDIO_ASRC           // 304: AUDIO_ASRC_IRQHandler            - AUDIO ASRC interrupt
ISR_HANDLER2 AUDIO_FREQME         // 305: AUDIO_FREQME_IRQHandler          - AUDIO FREQME interrupt
ISR_RESERVED                      // 306: Reserved306_IRQHandler           - Reserved interrupt
ISR_RESERVED                      // 307: Reserved307_IRQHandler           - Reserved interrupt
ISR_RESERVED                      // 308: Reserved308_IRQHandler           - Reserved interrupt
ISR_RESERVED                      // 309: Reserved309_IRQHandler           - Reserved interrupt
ISR_RESERVED                      // 310: Reserved310_IRQHandler           - Reserved interrupt
ISR_HANDLER2 COMM_TRDC            // 311: COMM_TRDC_IRQHandler             - COMM TRDC interrupt
ISR_HANDLER2 COMM_ENET_QOS        // 312: COMM_ENET_QOS_IRQHandler         - COMM ENET_QOS interrupt
ISR_HANDLER2 COMM_ENET_QOS_TX     // 313: COMM_ENET_QOS_TX_IRQHandler      - ENET_QOS: Per Channel Transmit Interrupt Signal To Host System
ISR_HANDLER2 COMM_ENET_QOS_RX     // 314: COMM_ENET_QOS_RX_IRQHandler      - ENET_QOS: Per Channel Receive Interrupt Signal To Host System
ISR_HANDLER2 COMM_ENET_TX         // 315: COMM_ENET_TX_IRQHandler          - COMM ENET RX interrupt
ISR_HANDLER2 COMM_ENET_RX         // 316: COMM_ENET_RX_IRQHandler          - COMM ENET TX interrupt
ISR_HANDLER2 COMM_ENET_OTHER      // 317: COMM_ENET_OTHER_IRQHandler       - ENET_1G_MAC: ETH OTHER_INT0_17
ISR_HANDLER2 COMM_ENET_1588       // 318: COMM_ENET_1588_IRQHandler        - ENET_1G_MAC: IEEE-1588 Timer Interrupt
ISR_HANDLER2 COMM_FLEXSPI_SLV     // 319: COMM_FLEXSPI_SLV_IRQHandler      - COMM FlexSPI follower interrupt
ISR_HANDLER2 COMM_USB0            // 320: COMM_USB0_IRQHandler             - COMM USB0 interrupt
ISR_HANDLER2 COMM_USBPHY          // 321: COMM_USBPHY_IRQHandler           - COMM USBPHY interrupt
ISR_HANDLER2 COMM_USB1            // 322: COMM_USB1_IRQHandler             - COMM USB1 interrupt
ISR_HANDLER2 COMM_USDHC0          // 323: COMM_USDHC0_IRQHandler           - COMM USDHC0 interrupt
ISR_HANDLER2 COMM_USDHC1          // 324: COMM_USDHC1_IRQHandler           - COMM USDHC1 interrupt
ISR_RESERVED                      // 325: Reserved325_IRQHandler           - Reserved interrupt
ISR_HANDLER2 COMM_TENBASET_PHY0   // 326: COMM_TENBASET_PHY0_IRQHandler    - XENO_PHY0:Functional IRQ1 interrupt request
ISR_RESERVED                      // 327: Reserved327_IRQHandler           - Reserved interrupt
ISR_HANDLER2 COMM_TENBASET_PHY1   // 328: COMM_TENBASET_PHY1_IRQHandler    - XENO_PHY1:Functional IRQ1 interrupt request
ISR_RESERVED                      // 329: Reserved329_IRQHandler           - Reserved interrupt
ISR_HANDLER2 COMM_FREQME          // 330: COMM_FREQME_IRQHandler           - COMM FREQME interrupt
ISR_HANDLER2 MEDIA_TRDC           // 331: MEDIA_TRDC_IRQHandler            - MEDIA TRDC interrupt
ISR_RESERVED                      // 332: Reserved332_IRQHandler           - Reserved interrupt
ISR_HANDLER2 MEDIA_DCIF_CH0       // 333: MEDIA_DCIF_CH0_IRQHandler        - MEDIA DCIF channel 0 interrupt
ISR_HANDLER2 MEDIA_DCIF_CH1       // 334: MEDIA_DCIF_CH1_IRQHandler        - MEDIA DCIF channel 1 interrupt
ISR_HANDLER2 MEDIA_DCIF_CH2       // 335: MEDIA_DCIF_CH2_IRQHandler        - MEDIA DCIF channel 2 interrupt
ISR_HANDLER2 MEDIA_DCIF_CH3       // 336: MEDIA_DCIF_CH3_IRQHandler        - MEDIA DCIF channel 3 interrupt
ISR_HANDLER2 MEDIA_DCIF_CH4       // 337: MEDIA_DCIF_CH4_IRQHandler        - MEDIA DCIF channel 4 interrupt
ISR_HANDLER2 MEDIA_DCIF_CH5       // 338: MEDIA_DCIF_CH5_IRQHandler        - MEDIA DCIF channel 5 interrupt
ISR_HANDLER2 MEDIA_DCIF_CH6       // 339: MEDIA_DCIF_CH6_IRQHandler        - MEDIA DCIF channel 6 interrupt
ISR_HANDLER2 MEDIA_DCIF_CH7       // 340: MEDIA_DCIF_CH7_IRQHandler        - MEDIA DCIF channel 7 interrupt
ISR_HANDLER2 MEDIA_DCIF_CH8       // 341: MEDIA_DCIF_CH8_IRQHandler        - MEDIA DCIF channel 8 interrupt
ISR_HANDLER2 MEDIA_CSI            // 342: MEDIA_CSI_IRQHandler             - CSI:CSI Interrupt Request
ISR_HANDLER2 MEDIA_REFORMATTER    // 343: MEDIA_REFORMATTER_IRQHandler     - MEDIA REFORMATTER interrupt
ISR_HANDLER2 MEDIA_ISI            // 344: MEDIA_ISI_IRQHandler             - MEDIA ISI interrupt
ISR_HANDLER2 MEDIA_TINGPU         // 345: MEDIA_TINGPU_IRQHandler          - MEDIA TINGPU interrupt
ISR_HANDLER2 MEDIA_JPEGDEC        // 346: MEDIA_JPEGDEC_IRQHandler         - MEDIA JPEGDEC interrupt
ISR_HANDLER2 MEDIA_MIPI_CSI       // 347: MEDIA_MIPI_CSI_IRQHandler        - MIPI_CSI: Interrupt Request
ISR_HANDLER2 MEDIA_MIPI_DSI       // 348: MEDIA_MIPI_DSI_IRQHandler        - MIPI_DSI: Interrupt Request
ISR_HANDLER2 MEDIA_FREQME         // 349: MEDIA_FREQME_IRQHandler          - MEDIA FREQME interrupt

  .section .vectors, "ax"
_vectors_end:

#if defined VECTORS_IN_RAM && defined XIP_EXTERNAL_FLASH
  .section .vectors_ram, "ax"
_vectors_ram:
  .space _vectors_end-_vectors, 0
#endif

  .section .init, "ax"
  .thumb_func

reset_handler:
Reset_Handler:
  ldr r0, =__stack_start__
  msr msplim, r0
#ifndef __NO_SYSTEM_INIT
  ldr r0, =__stack_end__
  mov sp, r0
  bl SystemInit
#endif

#if defined VECTORS_IN_RAM && defined XIP_EXTERNAL_FLASH
  ldr r0, =__vectors_load_start__
  ldr r1, =__vectors_load_end__
  ldr r2, =_vectors_ram
l0:
  cmp r0, r1
  beq l1
  ldr r3, [r0]
  str r3, [r2]
  adds r0, r0, #4
  adds r2, r2, #4
  b l0
l1:
#endif

#if !defined(__NO_FPU) && !defined(__SOFTFP__)
  // Enable CP11 and CP10 with CPACR |= (0xf<<20)
  movw r0, 0xED88
  movt r0, 0xE000
  ldr r1, [r0]
  orrs r1, r1, #(0xf << 20)
  str r1, [r0]
#ifndef __NO_RUNFAST_MODE
  isb
  dsb
  vmrs r0, fpscr
  orrs r0, r0, #(0x3 << 24) // FZ and DN
  vmsr fpscr, r0
  // clear the CONTROL.FPCA bit
  mov r0, #0
  msr control, r0
  // FPDSCR similarly
  movw r1, 0xEF3C
  movt r1, 0xE000
  ldr r0, [r1]
  orrs r0, r0, #(0x3 << 24) // FZ and DN
  str r0, [r1]
#endif
#endif

  /* Configure vector table offset register */
  ldr r0, =0xE000ED08
#if defined VECTORS_IN_RAM && defined XIP_EXTERNAL_FLASH
  ldr r1, =_vectors_ram
#else
  ldr r1, =_vectors
#endif
  str r1, [r0]

  b _start

#ifndef __NO_SYSTEM_INIT
  .thumb_func
  .weak SystemInit
SystemInit:
  bx lr
#endif

#ifndef STARTUP_FROM_RESET
  .thumb_func
reset_wait:
1: b 1b /* endless loop */
#endif /* STARTUP_FROM_RESET */

/* The ROM expects the 512-byte FlexSPI NOR configuration parameters to be present at offset 0x400 in Serial NOR flash.
   Compare Chapter "12.5.1.2 FlexSPI Serial NOR Flash Boot Operation" in the reference manual */
// Use this only when the M33-core is your boot-core, which is the default
.section .boot_offset
  .fill 0x400, 1, 0

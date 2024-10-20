/*****************************************************************************
 * Copyright (c) 2018 Rowley Associates Limited.                             *
 * Modified work Copyright (C) 2021-2024 Markus Klein                        *
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
ISR_HANDLER TMR1                                 // TMR1_IRQHandler                                 - TMR1 interrupt
ISR_HANDLER DAP                                  // DAP_IRQHandler                                  - DAP interrupt
ISR_HANDLER M7_CTI_TRIGGER_OUTPUT                // M7_CTI_TRIGGER_OUTPUT_IRQHandler                - CTI trigger outputs from CM7
ISR_HANDLER M33_CTI_TRIGGER_OUTPUT               // M33_CTI_TRIGGER_OUTPUT_IRQHandler               - CTI trigger outputs from CM33
ISR_RESERVED                                     // Reserved20_IRQHandler                           - Reserved interrupt
ISR_RESERVED                                     // Reserved21_IRQHandler                           - Reserved interrupt
ISR_RESERVED                                     // Reserved22_IRQHandler                           - Reserved interrupt
ISR_RESERVED                                     // Reserved23_IRQHandler                           - Reserved interrupt
ISR_HANDLER CAN1                                 // CAN1_IRQHandler                                 - CAN1 interrupt
ISR_HANDLER CAN1_ERROR                           // CAN1_ERROR_IRQHandler                           - CAN1 error interrupt
ISR_HANDLER GPIO1_0                              // GPIO1_0_IRQHandler                              - GPIO1 interrupt 0
ISR_HANDLER GPIO1_1                              // GPIO1_1_IRQHandler                              - GPIO1 interrupt 1
ISR_HANDLER I3C1                                 // I3C1_IRQHandler                                 - I3C1 interrupt
ISR_HANDLER LPI2C1                               // LPI2C1_IRQHandler                               - LPI2C1 interrupt
ISR_HANDLER LPI2C2                               // LPI2C2_IRQHandler                               - LPI2C2 interrupt
ISR_HANDLER LPIT1                                // LPIT1_IRQHandler                                - LPIT1 interrupt
ISR_HANDLER LPSPI1                               // LPSPI1_IRQHandler                               - LPSPI1 interrupt
ISR_HANDLER LPSPI2                               // LPSPI2_IRQHandler                               - LPSPI2 interrupt
ISR_HANDLER LPTMR1                               // LPTMR1_IRQHandler                               - LPTMR1 interrupt
ISR_HANDLER LPUART1                              // LPUART1_IRQHandler                              - LPUART1 interrupt
ISR_HANDLER LPUART2                              // LPUART2_IRQHandler                              - LPUART2 interrupt
ISR_HANDLER MU1                                  // MU1_IRQHandler                                  - MU1 interrupt
ISR_HANDLER MU2                                  // MU2_IRQHandler                                  - MU2 interrupt
ISR_HANDLER PWM1_FAULT                           // PWM1_FAULT_IRQHandler                           - PWM1 fault or reload error interrupt
ISR_HANDLER PWM1_0                               // PWM1_0_IRQHandler                               - PWM1 capture 0, compare 0, or reload 0 interrupt
ISR_HANDLER PWM1_1                               // PWM1_1_IRQHandler                               - PWM1 capture 1, compare 1, or reload 1 interrupt
ISR_HANDLER PWM1_2                               // PWM1_2_IRQHandler                               - PWM1 capture 2, compare 2, or reload 2 interrupt
ISR_HANDLER PWM1_3                               // PWM1_3_IRQHandler                               - PWM1 capture 3, compare 3, or reload 3 interrupt
ISR_HANDLER EDGELOCK_TRUST_MUA_RX_FULL           // EDGELOCK_TRUST_MUA_RX_FULL_IRQHandler           - Edgelock Trust MUA RX full interrupt
ISR_HANDLER EDGELOCK_TRUST_MUA_TX_EMPTY          // EDGELOCK_TRUST_MUA_TX_EMPTY_IRQHandler          - Edgelock Trust MUA TX empty interrupt
ISR_HANDLER EDGELOCK_APPS_CORE_MUA_RX_FULL       // EDGELOCK_APPS_CORE_MUA_RX_FULL_IRQHandler       - Edgelock Apps Core MUA RX full interrupt
ISR_HANDLER EDGELOCK_APPS_CORE_MUA_TX_EMPTY      // EDGELOCK_APPS_CORE_MUA_TX_EMPTY_IRQHandler      - Edgelock Apps Core MUA TX empty interrupt
ISR_HANDLER EDGELOCK_REALTIME_CORE_MUA_RX_FULL   // EDGELOCK_REALTIME_CORE_MUA_RX_FULL_IRQHandler   - Edgelock Realtime Core MUA RX full interrupt
ISR_HANDLER EDGELOCK_REALTIME_CORE_MUA_TX_EMPTY  // EDGELOCK_REALTIME_CORE_MUA_TX_EMPTY_IRQHandler  - Edgelock Realtime Core MUA TX empty interrupt
ISR_HANDLER EDGELOCK_SECURE                      // EDGELOCK_SECURE_IRQHandler                      - Edgelock secure interrupt
ISR_HANDLER EDGELOCK_NONSECURE                   // EDGELOCK_NONSECURE_IRQHandler                   - Edgelock non-secure interrupt
ISR_HANDLER TPM1_XBAR1_CH0_CH1                   // TPM1_XBAR1_CH0_CH1_IRQHandler                   - TPM1 interrupt, XBAR channel 0/1 interrupt
ISR_HANDLER TPM2_XBAR1_CH2_CH3                   // TPM2_XBAR1_CH2_CH3_IRQHandler                   - TPM2 interrupt, XBAR channel 2/3 interrupt
ISR_HANDLER RTWDOG1                              // RTWDOG1_IRQHandler                              - RTWDOG1 interrupt
ISR_HANDLER RTWDOG2                              // RTWDOG2_IRQHandler                              - RTWDOG2 interrupt
ISR_HANDLER TRDC_MGR_AON                         // TRDC_MGR_AON_IRQHandler                         - AONMIX TRDC transfer error interrupt
ISR_HANDLER PDM_HWVAD_EVENT                      // PDM_HWVAD_EVENT_IRQHandler                      - HWVAD event interrupt
ISR_HANDLER PDM_HWVAD_ERROR                      // PDM_HWVAD_ERROR_IRQHandler                      - HWVAD error interrupt
ISR_HANDLER PDM_EVENT                            // PDM_EVENT_IRQHandler                            - PDM event interrupt
ISR_HANDLER PDM_ERROR                            // PDM_ERROR_IRQHandler                            - PDM error interrupt
ISR_HANDLER SAI1                                 // SAI1_IRQHandler                                 - SAI interrupt
ISR_HANDLER CM33_PS                              // CM33_PS_IRQHandler                              - M33 PS Tag/Data Parity Error
ISR_HANDLER CM33_TCM_ECC                         // CM33_TCM_ECC_IRQHandler                         - M33 TCM ECC interrupt
ISR_HANDLER CM33_TCM_ERROR                       // CM33_TCM_ERROR_IRQHandler                       - M33 TCM Error interrupt
ISR_HANDLER CM7_TCM_ECC                          // CM7_TCM_ECC_IRQHandler                          - M7 TCM ECC interrupt
ISR_HANDLER CM7_TCM_ERROR                        // CM7_TCM_ERROR_IRQHandler                        - M7 TCM Error interrupt
ISR_HANDLER CAN2                                 // CAN2_IRQHandler                                 - CAN2 interrupt
ISR_HANDLER CAN2_ERROR                           // CAN2_ERROR_IRQHandler                           - CAN2 error interrupt
ISR_HANDLER FLEXIO1                              // FLEXIO1_IRQHandler                              - FLEXIO1 interrupt
ISR_HANDLER FLEXIO2                              // FLEXIO2_IRQHandler                              - FLEXIO2 interrupt
ISR_HANDLER FLEXSPI1                             // FLEXSPI1_IRQHandler                             - FLEXSPI1 interrupt
ISR_HANDLER FLEXSPI2                             // FLEXSPI2_IRQHandler                             - FLEXSPI2 interrupt
ISR_HANDLER GPIO2_0                              // GPIO2_0_IRQHandler                              - GPIO2 interrupt 0
ISR_HANDLER GPIO2_1                              // GPIO2_1_IRQHandler                              - GPIO2 interrupt 1
ISR_HANDLER GPIO3_0                              // GPIO3_0_IRQHandler                              - GPIO3 interrupt 0
ISR_HANDLER GPIO3_1                              // GPIO3_1_IRQHandler                              - GPIO3 interrupt 1
ISR_HANDLER I3C2                                 // I3C2_IRQHandler                                 - I3C2 interrupt
ISR_HANDLER LPI2C3                               // LPI2C3_IRQHandler                               - LPI2C3 interrupt
ISR_RESERVED                                     // Reserved79_IRQHandler                           - Reserved interrupt
ISR_HANDLER LPIT2                                // LPIT2_IRQHandler                                - LPIT2 interrupt
ISR_HANDLER LPSPI3                               // LPSPI3_IRQHandler                               - LPSPI3 interrupt
ISR_HANDLER LPSPI4                               // LPSPI4_IRQHandler                               - LPSPI4 interrupt
ISR_RESERVED                                     // Reserved83_IRQHandler                           - Reserved interrupt
ISR_HANDLER LPUART3                              // LPUART3_IRQHandler                              - LPUART3 interrupt
ISR_HANDLER LPUART4                              // LPUART4_IRQHandler                              - LPUART4 interrupt
ISR_HANDLER LPUART5                              // LPUART5_IRQHandler                              - LPUART5 interrupt
ISR_HANDLER LPUART6                              // LPUART6_IRQHandler                              - LPUART6 interrupt
ISR_HANDLER ASRC                                 // ASRC_IRQHandler                                 - ASRC interrupt
ISR_HANDLER BBNSM                                // BBNSM_IRQHandler                                - BBNSM iterrupt
ISR_HANDLER SYS_CTR1                             // SYS_CTR1_IRQHandler                             - System Counter compare interrupt 0 and 1
ISR_HANDLER TPM3_EWM                             // TPM3_EWM_IRQHandler                             - TPM3 interrupt, EWM reset out interrupt
ISR_RESERVED                                     // Reserved92_IRQHandler                           - Reserved interrupt
ISR_RESERVED                                     // Reserved93_IRQHandler                           - Reserved interrupt
ISR_RESERVED                                     // Reserved94_IRQHandler                           - Reserved interrupt
ISR_HANDLER RTWDOG3                              // RTWDOG3_IRQHandler                              - RTWDOG3 interrupt
ISR_HANDLER RTWDOG4                              // RTWDOG4_IRQHandler                              - RTWDOG4 interrupt
ISR_HANDLER RTWDOG5                              // RTWDOG5_IRQHandler                              - RTWDOG5 interrupt
ISR_HANDLER TRDC_MGR_WKUP                        // TRDC_MGR_WKUP_IRQHandler                        - WAKEUPMIX TRDC transfer error interrupt
ISR_HANDLER TMPSNS_INT                           // TMPSNS_INT_IRQHandler                           - Temperature alarm interrupt
ISR_HANDLER BBSM                                 // BBSM_IRQHandler                                 - BBSM wakeup alarm interrupt
ISR_HANDLER LDO_AON_ANA                          // LDO_AON_ANA_IRQHandler                          - Brown out interrupt
ISR_HANDLER USDHC1                               // USDHC1_IRQHandler                               - USDHC1
ISR_RESERVED                                     // Reserved103_IRQHandler                          - Reserved interrupt
ISR_HANDLER TRDC_MGR_MEGA                        // TRDC_MGR_MEGA_IRQHandler                        - MEGAMIX TRDC transfer error interrupt
ISR_HANDLER SFA_TMR2                             // SFA_TMR2_IRQHandler                             - Signal Frequency Analyzer interrupt, TMR2 interrupt
ISR_HANDLER LDO_AON_DIG                          // LDO_AON_DIG_IRQHandler                          - Brown out interrupt
ISR_HANDLER MECC1                                // MECC1_IRQHandler                                - MECC1 interrupt
ISR_HANDLER MECC2                                // MECC2_IRQHandler                                - MECC2 interrupt
ISR_HANDLER ADC1                                 // ADC1_IRQHandler                                 - ADC1 interrupt
ISR_HANDLER DMA_ERROR                            // DMA_ERROR_IRQHandler                            - AON Domain eDMA error interrupt
ISR_HANDLER DMA3_CH0                             // DMA3_CH0_IRQHandler                             - AON Domain eDMA channel 0 interrupt
ISR_HANDLER DMA3_CH1                             // DMA3_CH1_IRQHandler                             - AON Domain eDMA channel 1 interrupt
ISR_HANDLER DMA3_CH2                             // DMA3_CH2_IRQHandler                             - AON Domain eDMA channel 2 interrupt
ISR_HANDLER DMA3_CH3                             // DMA3_CH3_IRQHandler                             - AON Domain eDMA channel 3 interrupt
ISR_HANDLER DMA3_CH4                             // DMA3_CH4_IRQHandler                             - AON Domain eDMA channel 4 interrupt
ISR_HANDLER DMA3_CH5                             // DMA3_CH5_IRQHandler                             - AON Domain eDMA channel 5 interrupt
ISR_HANDLER DMA3_CH6                             // DMA3_CH6_IRQHandler                             - AON Domain eDMA channel 6 interrupt
ISR_HANDLER DMA3_CH7                             // DMA3_CH7_IRQHandler                             - AON Domain eDMA channel 7 interrupt
ISR_HANDLER DMA3_CH8                             // DMA3_CH8_IRQHandler                             - AON Domain eDMA channel 8 interrupt
ISR_HANDLER DMA3_CH9                             // DMA3_CH9_IRQHandler                             - AON Domain eDMA channel 9 interrupt
ISR_HANDLER DMA3_CH10                            // DMA3_CH10_IRQHandler                            - AON Domain eDMA channel 10 interrupt
ISR_HANDLER DMA3_CH11                            // DMA3_CH11_IRQHandler                            - AON Domain eDMA channel 11 interrupt
ISR_HANDLER DMA3_CH12                            // DMA3_CH12_IRQHandler                            - AON Domain eDMA channel 12 interrupt
ISR_HANDLER DMA3_CH13                            // DMA3_CH13_IRQHandler                            - AON Domain eDMA channel 13 interrupt
ISR_HANDLER DMA3_CH14                            // DMA3_CH14_IRQHandler                            - AON Domain eDMA channel 14 interrupt
ISR_HANDLER DMA3_CH15                            // DMA3_CH15_IRQHandler                            - AON Domain eDMA channel 15 interrupt
ISR_HANDLER DMA3_CH16                            // DMA3_CH16_IRQHandler                            - AON Domain eDMA channel 16 interrupt
ISR_HANDLER DMA3_CH17                            // DMA3_CH17_IRQHandler                            - AON Domain eDMA channel 17 interrupt
ISR_HANDLER DMA3_CH18                            // DMA3_CH18_IRQHandler                            - AON Domain eDMA channel 18 interrupt
ISR_HANDLER DMA3_CH19                            // DMA3_CH19_IRQHandler                            - AON Domain eDMA channel 19 interrupt
ISR_HANDLER DMA3_CH20                            // DMA3_CH20_IRQHandler                            - AON Domain eDMA channel 20 interrupt
ISR_HANDLER DMA3_CH21                            // DMA3_CH21_IRQHandler                            - AON Domain eDMA channel 21 interrupt
ISR_HANDLER DMA3_CH22                            // DMA3_CH22_IRQHandler                            - AON Domain eDMA channel 22 interrupt
ISR_HANDLER DMA3_CH23                            // DMA3_CH23_IRQHandler                            - AON Domain eDMA channel 23 interrupt
ISR_HANDLER DMA3_CH24                            // DMA3_CH24_IRQHandler                            - AON Domain eDMA channel 24 interrupt
ISR_HANDLER DMA3_CH25                            // DMA3_CH25_IRQHandler                            - AON Domain eDMA channel 25 interrupt
ISR_HANDLER DMA3_CH26                            // DMA3_CH26_IRQHandler                            - AON Domain eDMA channel 26 interrupt
ISR_HANDLER DMA3_CH27                            // DMA3_CH27_IRQHandler                            - AON Domain eDMA channel 27 interrupt
ISR_HANDLER DMA3_CH28_SINC3_CH0                  // DMA3_CH28_SINC3_CH0_IRQHandler                  - AON Domain eDMA channel 28 interrupt
ISR_HANDLER DMA3_CH29_SINC3_CH1                  // DMA3_CH29_SINC3_CH1_IRQHandler                  - AON Domain eDMA channel 29 interrupt
ISR_HANDLER DMA3_CH30_SINC3_CH2                  // DMA3_CH30_SINC3_CH2_IRQHandler                  - AON Domain eDMA channel 30 interrupt
ISR_HANDLER DMA3_CH31_SINC3_CH3                  // DMA3_CH31_SINC3_CH3_IRQHandler                  - AON Domain eDMA channel 31 interrupt
ISR_HANDLER DMA4_ERROR                           // DMA4_ERROR_IRQHandler                           - WAKEUP Domain eDMA error interrupt
ISR_HANDLER DMA4_CH0_CH1                         // DMA4_CH0_CH1_IRQHandler                         - WAKEUP Domain eDMA channel 0/1 interrupt
ISR_HANDLER DMA4_CH2_CH3                         // DMA4_CH2_CH3_IRQHandler                         - WAKEUP Domain eDMA channel 2/3 interrupt
ISR_HANDLER DMA4_CH4_CH5                         // DMA4_CH4_CH5_IRQHandler                         - WAKEUP Domain eDMA channel 4/5 interrupt
ISR_HANDLER DMA4_CH6_CH7                         // DMA4_CH6_CH7_IRQHandler                         - WAKEUP Domain eDMA channel 6/7 interrupt
ISR_HANDLER DMA4_CH8_CH9                         // DMA4_CH8_CH9_IRQHandler                         - WAKEUP Domain eDMA channel 8/9 interrupt
ISR_HANDLER DMA4_CH10_CH11                       // DMA4_CH10_CH11_IRQHandler                       - WAKEUP Domain eDMA channel 10/11 interrupt
ISR_HANDLER DMA4_CH12_CH13                       // DMA4_CH12_CH13_IRQHandler                       - WAKEUP Domain eDMA channel 12/13 interrupt
ISR_HANDLER DMA4_CH14_CH15                       // DMA4_CH14_CH15_IRQHandler                       - WAKEUP Domain eDMA channel 14/15 interrupt
ISR_HANDLER DMA4_CH16_CH17                       // DMA4_CH16_CH17_IRQHandler                       - WAKEUP Domain eDMA channel 16/17 interrupt
ISR_HANDLER DMA4_CH18_CH19                       // DMA4_CH18_CH19_IRQHandler                       - WAKEUP Domain eDMA channel 18/19 interrupt
ISR_HANDLER DMA4_CH20_CH21                       // DMA4_CH20_CH21_IRQHandler                       - WAKEUP Domain eDMA channel 20/21 interrupt
ISR_HANDLER DMA4_CH22_CH23                       // DMA4_CH22_CH23_IRQHandler                       - WAKEUP Domain eDMA channel 22/23 interrupt
ISR_HANDLER DMA4_CH24_CH25                       // DMA4_CH24_CH25_IRQHandler                       - WAKEUP Domain eDMA channel 24/25 interrupt
ISR_HANDLER DMA4_CH26_CH27                       // DMA4_CH26_CH27_IRQHandler                       - WAKEUP Domain eDMA channel 26/27 interrupt
ISR_HANDLER DMA4_CH28_CH29                       // DMA4_CH28_CH29_IRQHandler                       - WAKEUP Domain eDMA channel 28/29 interrupt
ISR_HANDLER DMA4_CH30_CH31                       // DMA4_CH30_CH31_IRQHandler                       - WAKEUP Domain eDMA channel 30/31 interrupt
ISR_HANDLER DMA4_CH32_CH33                       // DMA4_CH32_CH33_IRQHandler                       - WAKEUP Domain eDMA channel 32/33 interrupt
ISR_HANDLER DMA4_CH34_CH35                       // DMA4_CH34_CH35_IRQHandler                       - WAKEUP Domain eDMA channel 34/35 interrupt
ISR_HANDLER DMA4_CH36_CH37                       // DMA4_CH36_CH37_IRQHandler                       - WAKEUP Domain eDMA channel 36/37 interrupt
ISR_HANDLER DMA4_CH38_CH39                       // DMA4_CH38_CH39_IRQHandler                       - WAKEUP Domain eDMA channel 38/39 interrupt
ISR_HANDLER DMA4_CH40_CH41                       // DMA4_CH40_CH41_IRQHandler                       - WAKEUP Domain eDMA channel 40/41 interrupt
ISR_HANDLER DMA4_CH42_CH43                       // DMA4_CH42_CH43_IRQHandler                       - WAKEUP Domain eDMA channel 42/43 interrupt
ISR_HANDLER DMA4_CH44_CH45                       // DMA4_CH44_CH45_IRQHandler                       - WAKEUP Domain eDMA channel 44/45 interrupt
ISR_HANDLER DMA4_CH46_CH47                       // DMA4_CH46_CH47_IRQHandler                       - WAKEUP Domain eDMA channel 46/47 interrupt
ISR_HANDLER DMA4_CH48_CH49_LPI2C5                // DMA4_CH48_CH49_LPI2C5_IRQHandler                - WAKEUP Domain eDMA channel 48/49 interrupt, LPI2C5 interrupt
ISR_HANDLER DMA4_CH50_CH51_LPI2C6                // DMA4_CH50_CH51_LPI2C6_IRQHandler                - WAKEUP Domain eDMA channel 50/51 interrupt, LPI2C6 interrupt
ISR_HANDLER DMA4_CH52_CH53_SAI4                  // DMA4_CH52_CH53_SAI4_IRQHandler                  - WAKEUP Domain eDMA channel 46/47 interrupt, SAI4 interrupt
ISR_HANDLER DMA4_CH54_CH55_SPDIF                 // DMA4_CH54_CH55_SPDIF_IRQHandler                 - WAKEUP Domain eDMA channel 46/47 interrupt, SPDIF interrupt
ISR_HANDLER DMA4_CH56_CH57_LPUART9               // DMA4_CH56_CH57_LPUART9_IRQHandler               - WAKEUP Domain eDMA channel 46/47 interrupt, LPUART9 interrupt
ISR_HANDLER DMA4_CH58_CH59_LPUART10              // DMA4_CH58_CH59_LPUART10_IRQHandler              - WAKEUP Domain eDMA channel 46/47 interrupt, LPUART10 interrupt
ISR_HANDLER DMA4_CH60_CH61_LPUART11              // DMA4_CH60_CH61_LPUART11_IRQHandler              - WAKEUP Domain eDMA channel 46/47 interrupt, LPUART11 interrupt
ISR_HANDLER DMA4_CH62_CH63_LPUART12              // DMA4_CH62_CH63_LPUART12_IRQHandler              - WAKEUP Domain eDMA channel 46/47 interrupt, LPUART12 interrupt
ISR_HANDLER INTG_BOOTROM_DEBUG_CTRL              // INTG_BOOTROM_DEBUG_CTRL_IRQHandler              - CM33, CM7, DAP access IRQ
ISR_HANDLER EDGELOCK_REQ1                        // EDGELOCK_REQ1_IRQHandler                        - 
ISR_HANDLER EDGELOCK_REQ2                        // EDGELOCK_REQ2_IRQHandler                        - 
ISR_HANDLER EDGELOCK_REQ3                        // EDGELOCK_REQ3_IRQHandler                        - 
ISR_HANDLER DBG_TRACE_TMR3                       // DBG_TRACE_TMR3_IRQHandler                       - TMR3 interrupt
ISR_HANDLER JTAGC_TMR4                           // JTAGC_TMR4_IRQHandler                           - TMR4 interrupt
ISR_HANDLER M33_SYSRESET_REQ                     // M33_SYSRESET_REQ_IRQHandler                     - CM33 SYSREQRST SRC reset source
ISR_HANDLER M33_LOCKUP                           // M33_LOCKUP_IRQHandler                           - CM33 LOCKUP SRC reset source
ISR_HANDLER M7_SYSRESET_REQ                      // M7_SYSRESET_REQ_IRQHandler                      - CM33 SYSREQRST SRC reset source
ISR_HANDLER M7_LOCKUP                            // M7_LOCKUP_IRQHandler                            - CM33 LOCKUP SRC reset source
ISR_HANDLER PWM2_FAULT                           // PWM2_FAULT_IRQHandler                           - PWM2 fault or reload error interrupt
ISR_HANDLER PWM2_0                               // PWM2_0_IRQHandler                               - PWM2 capture 0, compare 0, or reload 0 interrupt
ISR_HANDLER PWM2_1                               // PWM2_1_IRQHandler                               - PWM2 capture 1, compare 1, or reload 1 interrupt
ISR_HANDLER PWM2_2                               // PWM2_2_IRQHandler                               - PWM2 capture 2, compare 2, or reload 2 interrupt
ISR_HANDLER PWM2_3                               // PWM2_3_IRQHandler                               - PWM2 capture 3, compare 3, or reload 3 interrupt
ISR_RESERVED                                     // Reserved191_IRQHandler                          - Reserved interrupt
ISR_RESERVED                                     // Reserved192_IRQHandler                          - Reserved interrupt
ISR_RESERVED                                     // Reserved193_IRQHandler                          - Reserved interrupt
ISR_RESERVED                                     // Reserved194_IRQHandler                          - Reserved interrupt
ISR_RESERVED                                     // Reserved195_IRQHandler                          - Reserved interrupt
ISR_RESERVED                                     // Reserved196_IRQHandler                          - Reserved interrupt
ISR_RESERVED                                     // Reserved197_IRQHandler                          - Reserved interrupt
ISR_RESERVED                                     // Reserved198_IRQHandler                          - Reserved interrupt
ISR_RESERVED                                     // Reserved199_IRQHandler                          - Reserved interrupt
ISR_RESERVED                                     // Reserved200_IRQHandler                          - Reserved interrupt
ISR_HANDLER EQDC1                                // EQDC1_IRQHandler                                - EQDC1 interrupt
ISR_HANDLER EQDC2                                // EQDC2_IRQHandler                                - EQDC2 interrupt
ISR_RESERVED                                     // Reserved203_IRQHandler                          - Reserved interrupt
ISR_RESERVED                                     // Reserved204_IRQHandler                          - Reserved interrupt
ISR_RESERVED                                     // Reserved205_IRQHandler                          - Reserved interrupt
ISR_HANDLER DCDC                                 // DCDC_IRQHandler                                 - DCDC brown out interrupt
ISR_RESERVED                                     // Reserved207_IRQHandler                          - Reserved interrupt
ISR_RESERVED                                     // Reserved208_IRQHandler                          - Reserved interrupt
ISR_HANDLER DAC                                  // DAC_IRQHandler                                  - DAC interrupt
ISR_RESERVED                                     // Reserved210_IRQHandler                          - Reserved interrupt
ISR_RESERVED                                     // Reserved211_IRQHandler                          - Reserved interrupt
ISR_HANDLER LPUART7                              // LPUART7_IRQHandler                              - LPUART7 interrupt
ISR_HANDLER LPUART8                              // LPUART8_IRQHandler                              - LPUART8 interrupt
ISR_HANDLER SAI2                                 // SAI2_IRQHandler                                 - SAI2 interrupt
ISR_HANDLER SAI3                                 // SAI3_IRQHandler                                 - SAI3 interrupt
ISR_HANDLER ACMP1                                // ACMP1_IRQHandler                                - CMP1 interrupt
ISR_HANDLER ACMP2                                // ACMP2_IRQHandler                                - CMP2 interrupt
ISR_HANDLER ACMP3                                // ACMP3_IRQHandler                                - CMP3 interrupt
ISR_HANDLER ACMP4                                // ACMP4_IRQHandler                                - CMP4 interrupt
ISR_HANDLER CM7_PS                               // CM7_PS_IRQHandler                               - M7 PS Tag/Data Parity Error
ISR_HANDLER CM7_MCM                              // CM7_MCM_IRQHandler                              - M7 MCM interrupt
ISR_HANDLER CM33_MCM                             // CM33_MCM_IRQHandler                             - M33 MCM interrupt
ISR_HANDLER ECAT_INT                             // ECAT_INT_IRQHandler                             - EtherCAT interrupt
ISR_HANDLER SAFETY_CLK_MON                       // SAFETY_CLK_MON_IRQHandler                       - Safety clock monitor interrupt
ISR_HANDLER GPT1                                 // GPT1_IRQHandler                                 - GPT1 interrupt
ISR_HANDLER GPT2                                 // GPT2_IRQHandler                                 - GPT2 interrupt
ISR_HANDLER KPP                                  // KPP_IRQHandler                                  - KPP interrupt
ISR_RESERVED                                     // Reserved228_IRQHandler                          - Reserved interrupt
ISR_RESERVED                                     // Reserved229_IRQHandler                          - Reserved interrupt
ISR_RESERVED                                     // Reserved230_IRQHandler                          - Reserved interrupt
ISR_RESERVED                                     // Reserved231_IRQHandler                          - Reserved interrupt
ISR_HANDLER FLEXSPI_SLV                          // FLEXSPI_SLV_IRQHandler                          - FLEXSPI follower interrupt
ISR_HANDLER NETC                                 // NETC_IRQHandler                                 - NETC interrupt
ISR_HANDLER MSGINTR1                             // MSGINTR1_IRQHandler                             - MSGINTR1 interrupt
ISR_HANDLER MSGINTR2                             // MSGINTR2_IRQHandler                             - MSGINTR2 interrupt
ISR_HANDLER MSGINTR3                             // MSGINTR3_IRQHandler                             - MSGINTR3 interrupt
ISR_HANDLER MSGINTR4                             // MSGINTR4_IRQHandler                             - MSGINTR4 interrupt
ISR_HANDLER MSGINTR5                             // MSGINTR5_IRQHandler                             - MSGINTR5 interrupt
ISR_HANDLER MSGINTR6                             // MSGINTR6_IRQHandler                             - MSGINTR6 interrupt
ISR_RESERVED                                     // Reserved240_IRQHandler                          - Reserved interrupt
ISR_RESERVED                                     // Reserved241_IRQHandler                          - Reserved interrupt
ISR_RESERVED                                     // Reserved242_IRQHandler                          - Reserved interrupt
ISR_RESERVED                                     // Reserved243_IRQHandler                          - Reserved interrupt
ISR_RESERVED                                     // Reserved244_IRQHandler                          - Reserved interrupt
ISR_RESERVED                                     // Reserved245_IRQHandler                          - Reserved interrupt
ISR_RESERVED                                     // Reserved246_IRQHandler                          - Reserved interrupt
ISR_RESERVED                                     // Reserved247_IRQHandler                          - Reserved interrupt
ISR_HANDLER GPIO4_0                              // GPIO4_0_IRQHandler                              - GPIO4 interrupt 0
ISR_HANDLER GPIO4_1                              // GPIO4_1_IRQHandler                              - GPIO4 interrupt 1
ISR_HANDLER GPIO5_0                              // GPIO5_0_IRQHandler                              - GPIO5 interrupt 0
ISR_HANDLER GPIO5_1                              // GPIO5_1_IRQHandler                              - GPIO5 interrupt 1
ISR_HANDLER GPIO6_0                              // GPIO6_0_IRQHandler                              - GPIO6 interrupt 0
ISR_HANDLER GPIO6_1                              // GPIO6_1_IRQHandler                              - GPIO6 interrupt 1
ISR_RESERVED                                     // Reserved254_IRQHandler                          - Reserved interrupt
ISR_RESERVED                                     // 255

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

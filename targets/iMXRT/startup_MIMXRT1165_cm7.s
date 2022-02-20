/*****************************************************************************
 * Copyright (c) 2018 Rowley Associates Limited.                             *
 * Modified work Copyright (C) 2021 Markus Klein                             *
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
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_HANDLER SVC_Handler
ISR_HANDLER DebugMon_Handler
ISR_RESERVED
ISR_HANDLER PendSV_Handler
ISR_HANDLER SysTick_Handler
  // External Interrupts
ISR_HANDLER2 DMA0_DMA16                           // DMA0_DMA16_IRQHandler              - DMA channel 0/16 transfer complete
ISR_HANDLER2 DMA1_DMA17                           // DMA1_DMA17_IRQHandler              - DMA channel 1/17 transfer complete
ISR_HANDLER2 DMA2_DMA18                           // DMA2_DMA18_IRQHandler              - DMA channel 2/18 transfer complete
ISR_HANDLER2 DMA3_DMA19                           // DMA3_DMA19_IRQHandler              - DMA channel 3/19 transfer complete
ISR_HANDLER2 DMA4_DMA20                           // DMA4_DMA20_IRQHandler              - DMA channel 4/20 transfer complete
ISR_HANDLER2 DMA5_DMA21                           // DMA5_DMA21_IRQHandler              - DMA channel 5/21 transfer complete
ISR_HANDLER2 DMA6_DMA22                           // DMA6_DMA22_IRQHandler              - DMA channel 6/22 transfer complete
ISR_HANDLER2 DMA7_DMA23                           // DMA7_DMA23_IRQHandler              - DMA channel 7/23 transfer complete
ISR_HANDLER2 DMA8_DMA24                           // DMA8_DMA24_IRQHandler              - DMA channel 8/24 transfer complete
ISR_HANDLER2 DMA9_DMA25                           // DMA9_DMA25_IRQHandler              - DMA channel 9/25 transfer complete
ISR_HANDLER2 DMA10_DMA26                          // DMA10_DMA26_IRQHandler             - DMA channel 10/26 transfer complete
ISR_HANDLER2 DMA11_DMA27                          // DMA11_DMA27_IRQHandler             - DMA channel 11/27 transfer complete
ISR_HANDLER2 DMA12_DMA28                          // DMA12_DMA28_IRQHandler             - DMA channel 12/28 transfer complete
ISR_HANDLER2 DMA13_DMA29                          // DMA13_DMA29_IRQHandler             - DMA channel 13/29 transfer complete
ISR_HANDLER2 DMA14_DMA30                          // DMA14_DMA30_IRQHandler             - DMA channel 14/30 transfer complete
ISR_HANDLER2 DMA15_DMA31                          // DMA15_DMA31_IRQHandler             - DMA channel 15/31 transfer complete
ISR_HANDLER2 DMA_ERROR                            // DMA_ERROR_IRQHandler               - DMA error interrupt channels 0-15 / 16-31
ISR_HANDLER2 CTI_TRIGGER_OUT0                     // CTI_TRIGGER_OUT0_IRQHandler        - CTI_TRIGGER_OUT0
ISR_HANDLER2 CTI_TRIGGER_OUT1                     // CTI_TRIGGER_OUT1_IRQHandler        - CTI_TRIGGER_OUT1
ISR_HANDLER2 CORE                                 // CORE_IRQHandler                    - CorePlatform exception IRQ
ISR_HANDLER2 LPUART1                              // LPUART1_IRQHandler                 - LPUART1 TX interrupt and RX interrupt
ISR_HANDLER2 LPUART2                              // LPUART2_IRQHandler                 - LPUART2 TX interrupt and RX interrupt
ISR_HANDLER2 LPUART3                              // LPUART3_IRQHandler                 - LPUART3 TX interrupt and RX interrupt
ISR_HANDLER2 LPUART4                              // LPUART4_IRQHandler                 - LPUART4 TX interrupt and RX interrupt
ISR_HANDLER2 LPUART5                              // LPUART5_IRQHandler                 - LPUART5 TX interrupt and RX interrupt
ISR_HANDLER2 LPUART6                              // LPUART6_IRQHandler                 - LPUART6 TX interrupt and RX interrupt
ISR_HANDLER2 LPUART7                              // LPUART7_IRQHandler                 - LPUART7 TX interrupt and RX interrupt
ISR_HANDLER2 LPUART8                              // LPUART8_IRQHandler                 - LPUART8 TX interrupt and RX interrupt
ISR_HANDLER2 LPUART9                              // LPUART9_IRQHandler                 - LPUART9 TX interrupt and RX interrupt
ISR_HANDLER2 LPUART10                             // LPUART10_IRQHandler                - LPUART10 TX interrupt and RX interrupt
ISR_HANDLER2 LPUART11                             // LPUART11_IRQHandler                - LPUART11 TX interrupt and RX interrupt
ISR_HANDLER2 LPUART12                             // LPUART12_IRQHandler                - LPUART12 TX interrupt and RX interrupt
ISR_HANDLER2 LPI2C1                               // LPI2C1_IRQHandler                  - LPI2C1 interrupt
ISR_HANDLER2 LPI2C2                               // LPI2C2_IRQHandler                  - LPI2C2 interrupt
ISR_HANDLER2 LPI2C3                               // LPI2C3_IRQHandler                  - LPI2C3 interrupt
ISR_HANDLER2 LPI2C4                               // LPI2C4_IRQHandler                  - LPI2C4 interrupt
ISR_HANDLER2 LPI2C5                               // LPI2C5_IRQHandler                  - LPI2C5 interrupt
ISR_HANDLER2 LPI2C6                               // LPI2C6_IRQHandler                  - LPI2C6 interrupt
ISR_HANDLER2 LPSPI1                               // LPSPI1_IRQHandler                  - LPSPI1 interrupt request line to the core
ISR_HANDLER2 LPSPI2                               // LPSPI2_IRQHandler                  - LPSPI2 interrupt request line to the core
ISR_HANDLER2 LPSPI3                               // LPSPI3_IRQHandler                  - LPSPI3 interrupt request line to the core
ISR_HANDLER2 LPSPI4                               // LPSPI4_IRQHandler                  - LPSPI4 interrupt request line to the core
ISR_HANDLER2 LPSPI5                               // LPSPI5_IRQHandler                  - LPSPI5 interrupt request line to the core
ISR_HANDLER2 LPSPI6                               // LPSPI6_IRQHandler                  - LPSPI6 interrupt request line to the core
ISR_HANDLER2 CAN1                                 // CAN1_IRQHandler                    - CAN1 interrupt
ISR_HANDLER2 CAN1_ERROR                           // CAN1_ERROR_IRQHandler              - CAN1 error interrupt
ISR_HANDLER2 CAN2                                 // CAN2_IRQHandler                    - CAN2 interrupt
ISR_HANDLER2 CAN2_ERROR                           // CAN2_ERROR_IRQHandler              - CAN2 error interrupt
ISR_HANDLER2 CAN3                                 // CAN3_IRQHandler                    - CAN3 interrupt
ISR_HANDLER2 CAN3_ERROR                           // CAN3_ERROR_IRQHandler              - CAN3 erro interrupt
ISR_HANDLER2 FLEXRAM                              // FLEXRAM_IRQHandler                 - FlexRAM address out of range Or access hit IRQ
ISR_HANDLER2 KPP                                  // KPP_IRQHandler                     - Keypad nterrupt
ISR_RESERVED                                      // Reserved68_IRQHandler              - Reserved interrupt
ISR_HANDLER2 GPR_IRQ                              // GPR_IRQ_IRQHandler                 - GPR interrupt
ISR_RESERVED                                      // Reserved70_IRQHandler              - Reserved interrupt
ISR_HANDLER2 LCDIFv2                              // LCDIFv2_IRQHandler                 - LCDIFv2 interrupt
ISR_RESERVED                                      // Reserved72_IRQHandler              - Reserved interrupt
ISR_RESERVED                                      // Reserved73_IRQHandler              - Reserved interrupt
ISR_HANDLER2 MIPI_CSI                             // MIPI_CSI_IRQHandler                - MIPI_CSI interrupt
ISR_HANDLER2 MIPI_DSI                             // MIPI_DSI_IRQHandler                - MIPI_DSI interrupt
ISR_HANDLER2 GPU2D                                // GPU2D_IRQHandler                   - GPU2D interrupt
ISR_HANDLER2 GPIO6_Combined_0_15                  // GPIO6_Combined_0_15_IRQHandler     - Combined interrupt indication for GPIO6 signal 0 throughout 15
ISR_HANDLER2 GPIO6_Combined_16_31                 // GPIO6_Combined_16_31_IRQHandler    - Combined interrupt indication for GPIO6 signal 16 throughout 31
ISR_HANDLER2 DAC                                  // DAC_IRQHandler                     - DAC interrupt
ISR_HANDLER2 KEY_MANAGER                          // KEY_MANAGER_IRQHandler             - PUF interrupt
ISR_HANDLER2 WDOG2                                // WDOG2_IRQHandler                   - WDOG2 interrupt
ISR_HANDLER2 SNVS_HP_NON_TZ                       // SNVS_HP_NON_TZ_IRQHandler          - SRTC Consolidated Interrupt. Non TZ
ISR_HANDLER2 SNVS_HP_TZ                           // SNVS_HP_TZ_IRQHandler              - SRTC Security Interrupt. TZ
ISR_HANDLER2 SNVS_PULSE_EVENT                     // SNVS_PULSE_EVENT_IRQHandler        - ON-OFF button press shorter than 5 secs (pulse event)
ISR_HANDLER2 CAAM_IRQ0                            // CAAM_IRQ0_IRQHandler               - CAAM interrupt queue for JQ0
ISR_HANDLER2 CAAM_IRQ1                            // CAAM_IRQ1_IRQHandler               - CAAM interrupt queue for JQ1
ISR_HANDLER2 CAAM_IRQ2                            // CAAM_IRQ2_IRQHandler               - CAAM interrupt queue for JQ2
ISR_HANDLER2 CAAM_IRQ3                            // CAAM_IRQ3_IRQHandler               - CAAM interrupt queue for JQ3
ISR_HANDLER2 CAAM_RECORVE_ERRPR                   // CAAM_RECORVE_ERRPR_IRQHandler      - CAAM interrupt for recoverable error
ISR_HANDLER2 CAAM_RTIC                            // CAAM_RTIC_IRQHandler               - CAAM interrupt for RTIC
ISR_HANDLER2 CDOG                                 // CDOG_IRQHandler                    - CDOG interrupt
ISR_HANDLER2 SAI1                                 // SAI1_IRQHandler                    - SAI1 interrupt
ISR_HANDLER2 SAI2                                 // SAI2_IRQHandler                    - SAI1 interrupt
ISR_HANDLER2 SAI3_RX                              // SAI3_RX_IRQHandler                 - SAI3 interrupt
ISR_HANDLER2 SAI3_TX                              // SAI3_TX_IRQHandler                 - SAI3 interrupt
ISR_HANDLER2 SAI4_RX                              // SAI4_RX_IRQHandler                 - SAI4 interrupt
ISR_HANDLER2 SAI4_TX                              // SAI4_TX_IRQHandler                 - SAI4 interrupt
ISR_HANDLER2 SPDIF                                // SPDIF_IRQHandler                   - SPDIF interrupt
ISR_HANDLER2 TMPSNS_INT                           // TMPSNS_INT_IRQHandler              - TMPSNS interrupt
ISR_HANDLER2 TMPSNS_LOW_HIGH                      // TMPSNS_LOW_HIGH_IRQHandler         - TMPSNS low high interrupt
ISR_HANDLER2 TMPSNS_PANIC                         // TMPSNS_PANIC_IRQHandler            - TMPSNS panic interrupt
ISR_HANDLER2 LPSR_LP8_BROWNOUT                    // LPSR_LP8_BROWNOUT_IRQHandler       - LPSR 1p8 brownout interrupt
ISR_HANDLER2 LPSR_LP0_BROWNOUT                    // LPSR_LP0_BROWNOUT_IRQHandler       - LPSR 1p0 brownout interrupt
ISR_HANDLER2 ADC1                                 // ADC1_IRQHandler                    - ADC1 interrupt
ISR_HANDLER2 ADC2                                 // ADC2_IRQHandler                    - ADC2 interrupt
ISR_HANDLER2 USBPHY1                              // USBPHY1_IRQHandler                 - USBPHY1 interrupt
ISR_HANDLER2 USBPHY2                              // USBPHY2_IRQHandler                 - USBPHY2 interrupt
ISR_HANDLER2 RDC                                  // RDC_IRQHandler                     - RDC interrupt
ISR_HANDLER2 GPIO13_Combined_0_31                 // GPIO13_Combined_0_31_IRQHandler    - Combined interrupt indication for GPIO13 signal 0 throughout 31
ISR_RESERVED                                      // Reserved110_IRQHandler             - Reserved interrupt
ISR_HANDLER2 DCIC1                                // DCIC1_IRQHandler                   - DCIC1 interrupt
ISR_HANDLER2 DCIC2                                // DCIC2_IRQHandler                   - DCIC2 interrupt
ISR_HANDLER2 ASRC                                 // ASRC_IRQHandler                    - ASRC interrupt
ISR_HANDLER2 FLEXRAM_ECC                          // FLEXRAM_ECC_IRQHandler             - FlexRAM ECC fatal interrupt
ISR_HANDLER2 CM7_GPIO2_3                          // CM7_GPIO2_3_IRQHandler             - CM7_GPIO2,CM7_GPIO3 interrupt
ISR_HANDLER2 GPIO1_Combined_0_15                  // GPIO1_Combined_0_15_IRQHandler     - Combined interrupt indication for GPIO1 signal 0 throughout 15
ISR_HANDLER2 GPIO1_Combined_16_31                 // GPIO1_Combined_16_31_IRQHandler    - Combined interrupt indication for GPIO1 signal 16 throughout 31
ISR_HANDLER2 GPIO2_Combined_0_15                  // GPIO2_Combined_0_15_IRQHandler     - Combined interrupt indication for GPIO2 signal 0 throughout 15
ISR_HANDLER2 GPIO2_Combined_16_31                 // GPIO2_Combined_16_31_IRQHandler    - Combined interrupt indication for GPIO2 signal 16 throughout 31
ISR_HANDLER2 GPIO3_Combined_0_15                  // GPIO3_Combined_0_15_IRQHandler     - Combined interrupt indication for GPIO3 signal 0 throughout 15
ISR_HANDLER2 GPIO3_Combined_16_31                 // GPIO3_Combined_16_31_IRQHandler    - Combined interrupt indication for GPIO3 signal 16 throughout 31
ISR_HANDLER2 GPIO4_Combined_0_15                  // GPIO4_Combined_0_15_IRQHandler     - Combined interrupt indication for GPIO4 signal 0 throughout 15
ISR_HANDLER2 GPIO4_Combined_16_31                 // GPIO4_Combined_16_31_IRQHandler    - Combined interrupt indication for GPIO4 signal 16 throughout 31
ISR_HANDLER2 GPIO5_Combined_0_15                  // GPIO5_Combined_0_15_IRQHandler     - Combined interrupt indication for GPIO5 signal 0 throughout 15
ISR_HANDLER2 GPIO5_Combined_16_31                 // GPIO5_Combined_16_31_IRQHandler    - Combined interrupt indication for GPIO5 signal 16 throughout 31
ISR_HANDLER2 FLEXIO1                              // FLEXIO1_IRQHandler                 - FLEXIO1 interrupt
ISR_HANDLER2 FLEXIO2                              // FLEXIO2_IRQHandler                 - FLEXIO2 interrupt
ISR_HANDLER2 WDOG1                                // WDOG1_IRQHandler                   - WDOG1 interrupt
ISR_HANDLER2 RTWDOG3                              // RTWDOG3_IRQHandler                 - RTWDOG3 interrupt
ISR_HANDLER2 EWM                                  // EWM_IRQHandler                     - EWM interrupt
ISR_HANDLER2 OCOTP_READ_FUSE_ERROR                // OCOTP_READ_FUSE_ERROR_IRQHandler   - OCOTP read fuse error interrupt
ISR_HANDLER2 OCOTP_READ_DONE_ERROR                // OCOTP_READ_DONE_ERROR_IRQHandler   - OCOTP read fuse done interrupt
ISR_HANDLER2 GPC                                  // GPC_IRQHandler                     - GPC interrupt
ISR_HANDLER2 MUA                                  // MUA_IRQHandler                     - MUA interrupt
ISR_HANDLER2 GPT1                                 // GPT1_IRQHandler                    - GPT1 interrupt
ISR_HANDLER2 GPT2                                 // GPT2_IRQHandler                    - GPT2 interrupt
ISR_HANDLER2 GPT3                                 // GPT3_IRQHandler                    - GPT3 interrupt
ISR_HANDLER2 GPT4                                 // GPT4_IRQHandler                    - GPT4 interrupt
ISR_HANDLER2 GPT5                                 // GPT5_IRQHandler                    - GPT5 interrupt
ISR_HANDLER2 GPT6                                 // GPT6_IRQHandler                    - GPT6 interrupt
ISR_HANDLER2 PWM1_0                               // PWM1_0_IRQHandler                  - PWM1 capture 0, compare 0, or reload 0 interrupt
ISR_HANDLER2 PWM1_1                               // PWM1_1_IRQHandler                  - PWM1 capture 1, compare 1, or reload 0 interrupt
ISR_HANDLER2 PWM1_2                               // PWM1_2_IRQHandler                  - PWM1 capture 2, compare 2, or reload 0 interrupt
ISR_HANDLER2 PWM1_3                               // PWM1_3_IRQHandler                  - PWM1 capture 3, compare 3, or reload 0 interrupt
ISR_HANDLER2 PWM1_FAULT                           // PWM1_FAULT_IRQHandler              - PWM1 fault or reload error interrupt
ISR_HANDLER2 FLEXSPI1                             // FLEXSPI1_IRQHandler                - FlexSPI1 interrupt
ISR_HANDLER2 FLEXSPI2                             // FLEXSPI2_IRQHandler                - FlexSPI2 interrupt
ISR_HANDLER2 SEMC                                 // SEMC_IRQHandler                    - SEMC interrupt
ISR_HANDLER2 USDHC1                               // USDHC1_IRQHandler                  - USDHC1 interrupt
ISR_HANDLER2 USDHC2                               // USDHC2_IRQHandler                  - USDHC2 interrupt
ISR_HANDLER2 USB_OTG2                             // USB_OTG2_IRQHandler                - USBO2 USB OTG2
ISR_HANDLER2 USB_OTG1                             // USB_OTG1_IRQHandler                - USBO2 USB OTG1
ISR_HANDLER2 ENET                                 // ENET_IRQHandler                    - ENET interrupt
ISR_HANDLER2 ENET_1588_Timer                      // ENET_1588_Timer_IRQHandler         - ENET_1588_Timer interrupt
ISR_HANDLER2 ENET_1G_MAC0_Tx_Rx_1                 // ENET_1G_MAC0_Tx_Rx_1_IRQHandler    - ENET 1G MAC0 transmit/receive 1
ISR_HANDLER2 ENET_1G_MAC0_Tx_Rx_2                 // ENET_1G_MAC0_Tx_Rx_2_IRQHandler    - ENET 1G MAC0 transmit/receive 2
ISR_HANDLER2 ENET_1G                              // ENET_1G_IRQHandler                 - ENET 1G interrupt
ISR_HANDLER2 ENET_1G_1588_Timer                   // ENET_1G_1588_Timer_IRQHandler      - ENET_1G_1588_Timer interrupt
ISR_HANDLER2 XBAR1_IRQ_0_1                        // XBAR1_IRQ_0_1_IRQHandler           - XBAR1 interrupt
ISR_HANDLER2 XBAR1_IRQ_2_3                        // XBAR1_IRQ_2_3_IRQHandler           - XBAR1 interrupt
ISR_HANDLER2 ADC_ETC_IRQ0                         // ADC_ETC_IRQ0_IRQHandler            - ADCETC IRQ0 interrupt
ISR_HANDLER2 ADC_ETC_IRQ1                         // ADC_ETC_IRQ1_IRQHandler            - ADCETC IRQ1 interrupt
ISR_HANDLER2 ADC_ETC_IRQ2                         // ADC_ETC_IRQ2_IRQHandler            - ADCETC IRQ2 interrupt
ISR_HANDLER2 ADC_ETC_IRQ3                         // ADC_ETC_IRQ3_IRQHandler            - ADCETC IRQ3 interrupt
ISR_HANDLER2 ADC_ETC_ERROR_IRQ                    // ADC_ETC_ERROR_IRQ_IRQHandler       - ADCETC Error IRQ interrupt
ISR_RESERVED                                      // Reserved166_IRQHandler             - Reserved interrupt
ISR_RESERVED                                      // Reserved167_IRQHandler             - Reserved interrupt
ISR_RESERVED                                      // Reserved168_IRQHandler             - Reserved interrupt
ISR_RESERVED                                      // Reserved169_IRQHandler             - Reserved interrupt
ISR_RESERVED                                      // Reserved170_IRQHandler             - Reserved interrupt
ISR_HANDLER2 PIT1                                 // PIT1_IRQHandler                    - PIT1 interrupt
ISR_HANDLER2 PIT2                                 // PIT2_IRQHandler                    - PIT2 interrupt
ISR_HANDLER2 ACMP1                                // ACMP1_IRQHandler                   - ACMP interrupt
ISR_HANDLER2 ACMP2                                // ACMP2_IRQHandler                   - ACMP interrupt
ISR_HANDLER2 ACMP3                                // ACMP3_IRQHandler                   - ACMP interrupt
ISR_HANDLER2 ACMP4                                // ACMP4_IRQHandler                   - ACMP interrupt
ISR_RESERVED                                      // Reserved177_IRQHandler             - Reserved interrupt
ISR_RESERVED                                      // Reserved178_IRQHandler             - Reserved interrupt
ISR_RESERVED                                      // Reserved179_IRQHandler             - Reserved interrupt
ISR_RESERVED                                      // Reserved180_IRQHandler             - Reserved interrupt
ISR_HANDLER2 ENC1                                 // ENC1_IRQHandler                    - ENC1 interrupt
ISR_HANDLER2 ENC2                                 // ENC2_IRQHandler                    - ENC2 interrupt
ISR_HANDLER2 ENC3                                 // ENC3_IRQHandler                    - ENC3 interrupt
ISR_HANDLER2 ENC4                                 // ENC4_IRQHandler                    - ENC4 interrupt
ISR_RESERVED                                      // Reserved185_IRQHandler             - Reserved interrupt
ISR_RESERVED                                      // Reserved186_IRQHandler             - Reserved interrupt
ISR_HANDLER2 TMR1                                 // TMR1_IRQHandler                    - TMR1 interrupt
ISR_HANDLER2 TMR2                                 // TMR2_IRQHandler                    - TMR2 interrupt
ISR_HANDLER2 TMR3                                 // TMR3_IRQHandler                    - TMR3 interrupt
ISR_HANDLER2 TMR4                                 // TMR4_IRQHandler                    - TMR4 interrupt
ISR_HANDLER2 SEMA4_CP0                            // SEMA4_CP0_IRQHandler               - SEMA4 CP0 interrupt
ISR_HANDLER2 SEMA4_CP1                            // SEMA4_CP1_IRQHandler               - SEMA4 CP1 interrupt
ISR_HANDLER2 PWM2_0                               // PWM2_0_IRQHandler                  - PWM2 capture 0, compare 0, or reload 0 interrupt
ISR_HANDLER2 PWM2_1                               // PWM2_1_IRQHandler                  - PWM2 capture 1, compare 1, or reload 0 interrupt
ISR_HANDLER2 PWM2_2                               // PWM2_2_IRQHandler                  - PWM2 capture 2, compare 2, or reload 0 interrupt
ISR_HANDLER2 PWM2_3                               // PWM2_3_IRQHandler                  - PWM2 capture 3, compare 3, or reload 0 interrupt
ISR_HANDLER2 PWM2_FAULT                           // PWM2_FAULT_IRQHandler              - PWM2 fault or reload error interrupt
ISR_HANDLER2 PWM3_0                               // PWM3_0_IRQHandler                  - PWM3 capture 0, compare 0, or reload 0 interrupt
ISR_HANDLER2 PWM3_1                               // PWM3_1_IRQHandler                  - PWM3 capture 1, compare 1, or reload 0 interrupt
ISR_HANDLER2 PWM3_2                               // PWM3_2_IRQHandler                  - PWM3 capture 2, compare 2, or reload 0 interrupt
ISR_HANDLER2 PWM3_3                               // PWM3_3_IRQHandler                  - PWM3 capture 3, compare 3, or reload 0 interrupt
ISR_HANDLER2 PWM3_FAULT                           // PWM3_FAULT_IRQHandler              - PWM3 fault or reload error interrupt
ISR_HANDLER2 PWM4_0                               // PWM4_0_IRQHandler                  - PWM4 capture 0, compare 0, or reload 0 interrupt
ISR_HANDLER2 PWM4_1                               // PWM4_1_IRQHandler                  - PWM4 capture 1, compare 1, or reload 0 interrupt
ISR_HANDLER2 PWM4_2                               // PWM4_2_IRQHandler                  - PWM4 capture 2, compare 2, or reload 0 interrupt
ISR_HANDLER2 PWM4_3                               // PWM4_3_IRQHandler                  - PWM4 capture 3, compare 3, or reload 0 interrupt
ISR_HANDLER2 PWM4_FAULT                           // PWM4_FAULT_IRQHandler              - PWM4 fault or reload error interrupt
ISR_RESERVED                                      // Reserved208_IRQHandler             - Reserved interrupt
ISR_RESERVED                                      // Reserved209_IRQHandler             - Reserved interrupt
ISR_RESERVED                                      // Reserved210_IRQHandler             - Reserved interrupt
ISR_RESERVED                                      // Reserved211_IRQHandler             - Reserved interrupt
ISR_RESERVED                                      // Reserved212_IRQHandler             - Reserved interrupt
ISR_RESERVED                                      // Reserved213_IRQHandler             - Reserved interrupt
ISR_RESERVED                                      // Reserved214_IRQHandler             - Reserved interrupt
ISR_RESERVED                                      // Reserved215_IRQHandler             - Reserved interrupt
ISR_HANDLER2 HWVAD_EVENT                          // HWVAD_EVENT_IRQHandler             - HWVAD event interrupt
ISR_HANDLER2 HWVAD_ERROR                          // HWVAD_ERROR_IRQHandler             - HWVAD error interrupt
ISR_HANDLER2 PDM_EVENT                            // PDM_EVENT_IRQHandler               - PDM event interrupt
ISR_HANDLER2 PDM_ERROR                            // PDM_ERROR_IRQHandler               - PDM error interrupt
ISR_HANDLER2 EMVSIM1                              // EMVSIM1_IRQHandler                 - EMVSIM1 interrupt
ISR_HANDLER2 EMVSIM2                              // EMVSIM2_IRQHandler                 - EMVSIM2 interrupt
ISR_HANDLER2 MECC1_INT                            // MECC1_INT_IRQHandler               - MECC1 int
ISR_HANDLER2 MECC1_FATAL_INT                      // MECC1_FATAL_INT_IRQHandler         - MECC1 fatal int
ISR_HANDLER2 MECC2_INT                            // MECC2_INT_IRQHandler               - MECC2 int
ISR_HANDLER2 MECC2_FATAL_INT                      // MECC2_FATAL_INT_IRQHandler         - MECC2 fatal int
ISR_HANDLER2 XECC_FLEXSPI1_INT                    // XECC_FLEXSPI1_INT_IRQHandler       - XECC int
ISR_HANDLER2 XECC_FLEXSPI1_FATAL_INT              // XECC_FLEXSPI1_FATAL_INT_IRQHandler - XECC fatal int
ISR_HANDLER2 XECC_FLEXSPI2_INT                    // XECC_FLEXSPI2_INT_IRQHandler       - XECC int
ISR_HANDLER2 XECC_FLEXSPI2_FATAL_INT              // XECC_FLEXSPI2_FATAL_INT_IRQHandler - XECC fatal int
ISR_HANDLER2 XECC_SEMC_INT                        // XECC_SEMC_INT_IRQHandler           - XECC int
ISR_HANDLER2 XECC_SEMC_FATAL_INT                  // XECC_SEMC_FATAL_INT_IRQHandler     - XECC fatal int
ISR_RESERVED                                      // DefaultISR                         - 232
ISR_RESERVED                                      // DefaultISR                         - 233
ISR_RESERVED                                      // DefaultISR                         - 234
ISR_RESERVED                                      // DefaultISR                         - 235
ISR_RESERVED                                      // DefaultISR                         - 236
ISR_RESERVED                                      // DefaultISR                         - 237
ISR_RESERVED                                      // DefaultISR                         - 238
ISR_RESERVED                                      // DefaultISR                         - 239
ISR_RESERVED                                      // DefaultISR                         - 240
ISR_RESERVED                                      // DefaultISR                         - 241
ISR_RESERVED                                      // DefaultISR                         - 242
ISR_RESERVED                                      // DefaultISR                         - 243
ISR_RESERVED                                      // DefaultISR                         - 244
ISR_RESERVED                                      // DefaultISR                         - 245
ISR_RESERVED                                      // DefaultISR                         - 246
ISR_RESERVED                                      // DefaultISR                         - 247
ISR_RESERVED                                      // DefaultISR                         - 248
ISR_RESERVED                                      // DefaultISR                         - 249
ISR_RESERVED                                      // DefaultISR                         - 250
ISR_RESERVED                                      // DefaultISR                         - 251
ISR_RESERVED                                      // DefaultISR                         - 252
ISR_RESERVED                                      // DefaultISR                         - 253
ISR_RESERVED                                      // DefaultISR                         - 254
.long 0xFFFFFFFF                                  // Reserved for user TRIM value

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

#ifdef DISABLE_ECC_ON_STARTUP
  // disable FLEXRAM-ECC (enabled by fuse or ROM) and restore the factory default
  // stack memory is not initalized for ECC at this point, temp. disable ECC is required to avoid ECC errors
  ldr r0, =0x40028000+0x108
  ldr r1, =0
  str r1, [r0]
#endif
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

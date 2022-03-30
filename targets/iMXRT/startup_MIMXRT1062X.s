/*****************************************************************************
 * Copyright (c) 2018 Rowley Associates Limited.                             *
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
ISR_HANDLER NMI_Handler                // NMI Handler
ISR_HANDLER HardFault_Handler          // Hard Fault Handler
ISR_HANDLER MemManage_Handler          // MPU Fault Handler
ISR_HANDLER BusFault_Handler           // Bus Fault Handler
ISR_HANDLER UsageFault_Handler         // Usage Fault Handler
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_HANDLER SVC_Handler                // SVCall Handler
ISR_HANDLER DebugMon_Handler           // Debug Monitor Handler
ISR_RESERVED
ISR_HANDLER PendSV_Handler             // PendSV Handler
ISR_HANDLER SysTick_Handler            // SysTick Handler
  // External Interrupts
ISR_HANDLER2 DMA0_DMA16                // DMA0_DMA16_IRQHandler           - DMA channel 0/16 transfer complete
ISR_HANDLER2 DMA1_DMA17                // DMA1_DMA17_IRQHandler           - DMA channel 1/17 transfer complete
ISR_HANDLER2 DMA2_DMA18                // DMA2_DMA18_IRQHandler           - DMA channel 2/18 transfer complete
ISR_HANDLER2 DMA3_DMA19                // DMA3_DMA19_IRQHandler           - DMA channel 3/19 transfer complete
ISR_HANDLER2 DMA4_DMA20                // DMA4_DMA20_IRQHandler           - DMA channel 4/20 transfer complete
ISR_HANDLER2 DMA5_DMA21                // DMA5_DMA21_IRQHandler           - DMA channel 5/21 transfer complete
ISR_HANDLER2 DMA6_DMA22                // DMA6_DMA22_IRQHandler           - DMA channel 6/22 transfer complete
ISR_HANDLER2 DMA7_DMA23                // DMA7_DMA23_IRQHandler           - DMA channel 7/23 transfer complete
ISR_HANDLER2 DMA8_DMA24                // DMA8_DMA24_IRQHandler           - DMA channel 8/24 transfer complete
ISR_HANDLER2 DMA9_DMA25                // DMA9_DMA25_IRQHandler           - DMA channel 9/25 transfer complete
ISR_HANDLER2 DMA10_DMA26               // DMA10_DMA26_IRQHandler          - DMA channel 10/26 transfer complete
ISR_HANDLER2 DMA11_DMA27               // DMA11_DMA27_IRQHandler          - DMA channel 11/27 transfer complete
ISR_HANDLER2 DMA12_DMA28               // DMA12_DMA28_IRQHandler          - DMA channel 12/28 transfer complete
ISR_HANDLER2 DMA13_DMA29               // DMA13_DMA29_IRQHandler          - DMA channel 13/29 transfer complete
ISR_HANDLER2 DMA14_DMA30               // DMA14_DMA30_IRQHandler          - DMA channel 14/30 transfer complete
ISR_HANDLER2 DMA15_DMA31               // DMA15_DMA31_IRQHandler          - DMA channel 15/31 transfer complete
ISR_HANDLER2 DMA_ERROR                 // DMA_ERROR_IRQHandler            - DMA error interrupt channels 0-15 / 16-31
ISR_HANDLER2 CTI0_ERROR                // CTI0_ERROR_IRQHandler           - CTI0_Error
ISR_HANDLER2 CTI1_ERROR                // CTI1_ERROR_IRQHandler           - CTI1_Error
ISR_HANDLER2 CORE                      // CORE_IRQHandler                 - CorePlatform exception IRQ
ISR_HANDLER2 LPUART1                   // LPUART1_IRQHandler              - LPUART1 TX interrupt and RX interrupt
ISR_HANDLER2 LPUART2                   // LPUART2_IRQHandler              - LPUART2 TX interrupt and RX interrupt
ISR_HANDLER2 LPUART3                   // LPUART3_IRQHandler              - LPUART3 TX interrupt and RX interrupt
ISR_HANDLER2 LPUART4                   // LPUART4_IRQHandler              - LPUART4 TX interrupt and RX interrupt
ISR_HANDLER2 LPUART5                   // LPUART5_IRQHandler              - LPUART5 TX interrupt and RX interrupt
ISR_HANDLER2 LPUART6                   // LPUART6_IRQHandler              - LPUART6 TX interrupt and RX interrupt
ISR_HANDLER2 LPUART7                   // LPUART7_IRQHandler              - LPUART7 TX interrupt and RX interrupt
ISR_HANDLER2 LPUART8                   // LPUART8_IRQHandler              - LPUART8 TX interrupt and RX interrupt
ISR_HANDLER2 LPI2C1                    // LPI2C1_IRQHandler               - LPI2C1 interrupt
ISR_HANDLER2 LPI2C2                    // LPI2C2_IRQHandler               - LPI2C2 interrupt
ISR_HANDLER2 LPI2C3                    // LPI2C3_IRQHandler               - LPI2C3 interrupt
ISR_HANDLER2 LPI2C4                    // LPI2C4_IRQHandler               - LPI2C4 interrupt
ISR_HANDLER2 LPSPI1                    // LPSPI1_IRQHandler               - LPSPI1 single interrupt vector for all sources
ISR_HANDLER2 LPSPI2                    // LPSPI2_IRQHandler               - LPSPI2 single interrupt vector for all sources
ISR_HANDLER2 LPSPI3                    // LPSPI3_IRQHandler               - LPSPI3 single interrupt vector for all sources
ISR_HANDLER2 LPSPI4                    // LPSPI4_IRQHandler               - LPSPI4  single interrupt vector for all sources
ISR_HANDLER2 CAN1                      // CAN1_IRQHandler                 - CAN1 interrupt
ISR_HANDLER2 CAN2                      // CAN2_IRQHandler                 - CAN2 interrupt
ISR_HANDLER2 FLEXRAM                   // FLEXRAM_IRQHandler              - FlexRAM address out of range Or access hit IRQ
ISR_HANDLER2 KPP                       // KPP_IRQHandler                  - Keypad nterrupt
ISR_HANDLER2 TSC_DIG                   // TSC_DIG_IRQHandler              - TSC interrupt
ISR_HANDLER2 GPR_IRQ                   // GPR_IRQ_IRQHandler              - GPR interrupt
ISR_HANDLER2 LCDIF                     // LCDIF_IRQHandler                - LCDIF interrupt
ISR_HANDLER2 CSI                       // CSI_IRQHandler                  - CSI interrupt
ISR_HANDLER2 PXP                       // PXP_IRQHandler                  - PXP interrupt
ISR_HANDLER2 WDOG2                     // WDOG2_IRQHandler                - WDOG2 interrupt
ISR_HANDLER2 SNVS_HP_WRAPPER           // SNVS_HP_WRAPPER_IRQHandler      - SRTC Consolidated Interrupt. Non TZ
ISR_HANDLER2 SNVS_HP_WRAPPER_TZ        // SNVS_HP_WRAPPER_TZ_IRQHandler   - SRTC Security Interrupt. TZ
ISR_HANDLER2 SNVS_LP_WRAPPER           // SNVS_LP_WRAPPER_IRQHandler      - ON-OFF button press shorter than 5 secs (pulse event)
ISR_HANDLER2 CSU                       // CSU_IRQHandler                  - CSU interrupt
ISR_HANDLER2 DCP                       // DCP_IRQHandler                  - DCP_IRQ interrupt
ISR_HANDLER2 DCP_VMI                   // DCP_VMI_IRQHandler              - DCP_VMI_IRQ interrupt
ISR_RESERVED                           //                                 - Reserved interrupt
ISR_HANDLER2 TRNG                      // TRNG_IRQHandler                 - TRNG interrupt
ISR_HANDLER2 SJC                       // SJC_IRQHandler                  - SJC interrupt
ISR_HANDLER2 BEE                       // BEE_IRQHandler                  - BEE interrupt
ISR_HANDLER2 SAI1                      // SAI1_IRQHandler                 - SAI1 interrupt
ISR_HANDLER2 SAI2                      // SAI2_IRQHandler                 - SAI1 interrupt
ISR_HANDLER2 SAI3_RX                   // SAI3_RX_IRQHandler              - SAI3 interrupt
ISR_HANDLER2 SAI3_TX                   // SAI3_TX_IRQHandler              - SAI3 interrupt
ISR_HANDLER2 SPDIF                     // SPDIF_IRQHandler                - SPDIF interrupt
ISR_HANDLER2 PMU_EVENT                 // PMU_EVENT_IRQHandler            - Brown-out event interrupt
ISR_RESERVED                           //                                 - Reserved interrupt
ISR_HANDLER2 TEMP_LOW_HIGH             // TEMP_LOW_HIGH_IRQHandler        - TempSensor low/high interrupt
ISR_HANDLER2 TEMP_PANIC                // TEMP_PANIC_IRQHandler           - TempSensor panic interrupt
ISR_HANDLER2 USB_PHY1                  // USB_PHY1_IRQHandler             - USBPHY (UTMI0), Interrupt
ISR_HANDLER2 USB_PHY2                  // USB_PHY2_IRQHandler             - USBPHY (UTMI1), Interrupt
ISR_HANDLER2 ADC1                      // ADC1_IRQHandler                 - ADC1 interrupt
ISR_HANDLER2 ADC2                      // ADC2_IRQHandler                 - ADC2 interrupt
ISR_HANDLER2 DCDC                      // DCDC_IRQHandler                 - DCDC interrupt
ISR_RESERVED                           // Reserved86_IRQHandler           - Reserved interrupt
ISR_HANDLER2 GPIO10_Combined_0_31      // GPIO10_Combined_0_31_IRQHandler - Combined interrupt indication for GPIO10 signal 0 throughout 31
ISR_HANDLER2 GPIO1_INT0                // GPIO1_INT0_IRQHandler           - Active HIGH Interrupt from INT0 from GPIO
ISR_HANDLER2 GPIO1_INT1                // GPIO1_INT1_IRQHandler           - Active HIGH Interrupt from INT1 from GPIO
ISR_HANDLER2 GPIO1_INT2                // GPIO1_INT2_IRQHandler           - Active HIGH Interrupt from INT2 from GPIO
ISR_HANDLER2 GPIO1_INT3                // GPIO1_INT3_IRQHandler           - Active HIGH Interrupt from INT3 from GPIO
ISR_HANDLER2 GPIO1_INT4                // GPIO1_INT4_IRQHandler           - Active HIGH Interrupt from INT4 from GPIO
ISR_HANDLER2 GPIO1_INT5                // GPIO1_INT5_IRQHandler           - Active HIGH Interrupt from INT5 from GPIO
ISR_HANDLER2 GPIO1_INT6                // GPIO1_INT6_IRQHandler           - Active HIGH Interrupt from INT6 from GPIO
ISR_HANDLER2 GPIO1_INT7                // GPIO1_INT7_IRQHandler           - Active HIGH Interrupt from INT7 from GPIO
ISR_HANDLER2 GPIO1_Combined_0_15       // GPIO1_Combined_0_15_IRQHandler  - Combined interrupt indication for GPIO1 signal 0 throughout 15
ISR_HANDLER2 GPIO1_Combined_16_31      // GPIO1_Combined_16_31_IRQHandler - Combined interrupt indication for GPIO1 signal 16 throughout 31
ISR_HANDLER2 GPIO2_Combined_0_15       // GPIO2_Combined_0_15_IRQHandler  - Combined interrupt indication for GPIO2 signal 0 throughout 15
ISR_HANDLER2 GPIO2_Combined_16_31      // GPIO2_Combined_16_31_IRQHandler - Combined interrupt indication for GPIO2 signal 16 throughout 31
ISR_HANDLER2 GPIO3_Combined_0_15       // GPIO3_Combined_0_15_IRQHandler  - Combined interrupt indication for GPIO3 signal 0 throughout 15
ISR_HANDLER2 GPIO3_Combined_16_31      // GPIO3_Combined_16_31_IRQHandler - Combined interrupt indication for GPIO3 signal 16 throughout 31
ISR_HANDLER2 GPIO4_Combined_0_15       // GPIO4_Combined_0_15_IRQHandler  - Combined interrupt indication for GPIO4 signal 0 throughout 15
ISR_HANDLER2 GPIO4_Combined_16_31      // GPIO4_Combined_16_31_IRQHandler - Combined interrupt indication for GPIO4 signal 16 throughout 31
ISR_HANDLER2 GPIO5_Combined_0_15       // GPIO5_Combined_0_15_IRQHandler  - Combined interrupt indication for GPIO5 signal 0 throughout 15
ISR_HANDLER2 GPIO5_Combined_16_31      // GPIO5_Combined_16_31_IRQHandler - Combined interrupt indication for GPIO5 signal 16 throughout 31
ISR_HANDLER2 FLEXIO1                   // FLEXIO1_IRQHandler              - FLEXIO1 interrupt
ISR_HANDLER2 FLEXIO2                   // FLEXIO2_IRQHandler              - FLEXIO2 interrupt
ISR_HANDLER2 WDOG1                     // WDOG1_IRQHandler                - WDOG1 interrupt
ISR_HANDLER2 RTWDOG                    // RTWDOG_IRQHandler               - RTWDOG interrupt
ISR_HANDLER2 EWM                       // EWM_IRQHandler                  - EWM interrupt
ISR_HANDLER2 CCM_1                     // CCM_1_IRQHandler                - CCM IRQ1 interrupt
ISR_HANDLER2 CCM_2                     // CCM_2_IRQHandler                - CCM IRQ2 interrupt
ISR_HANDLER2 GPC                       // GPC_IRQHandler                  - GPC interrupt
ISR_HANDLER2 SRC                       // SRC_IRQHandler                  - SRC interrupt
ISR_RESERVED                           //                                 - Reserved interrupt
ISR_HANDLER2 GPT1                      // GPT1_IRQHandler                 - GPT1 interrupt
ISR_HANDLER2 GPT2                      // GPT2_IRQHandler                 - GPT2 interrupt
ISR_HANDLER2 PWM1_0                    // PWM1_0_IRQHandler               - PWM1 capture 0, compare 0, or reload 0 interrupt
ISR_HANDLER2 PWM1_1                    // PWM1_1_IRQHandler               - PWM1 capture 1, compare 1, or reload 0 interrupt
ISR_HANDLER2 PWM1_2                    // PWM1_2_IRQHandler               - PWM1 capture 2, compare 2, or reload 0 interrupt
ISR_HANDLER2 PWM1_3                    // PWM1_3_IRQHandler               - PWM1 capture 3, compare 3, or reload 0 interrupt
ISR_HANDLER2 PWM1_FAULT                // PWM1_FAULT_IRQHandler           - PWM1 fault or reload error interrupt
ISR_HANDLER2 FLEXSPI2                  // FLEXSPI2_IRQHandler             - FlexSPI2 interrupt
ISR_HANDLER2 FLEXSPI                   // FLEXSPI_IRQHandler              - FlexSPI0 interrupt
ISR_HANDLER2 SEMC                      // SEMC_IRQHandler                 - SEMC interrupt
ISR_HANDLER2 USDHC1                    // USDHC1_IRQHandler               - USDHC1 interrupt
ISR_HANDLER2 USDHC2                    // USDHC2_IRQHandler               - USDHC2 interrupt
ISR_HANDLER2 USB_OTG2                  // USB_OTG2_IRQHandler             - USBO2 USB OTG2
ISR_HANDLER2 USB_OTG1                  // USB_OTG1_IRQHandler             - USBO2 USB OTG1
ISR_HANDLER2 ENET                      // ENET_IRQHandler                 - ENET interrupt
ISR_HANDLER2 ENET_1588_Timer           // ENET_1588_Timer_IRQHandler      - ENET_1588_Timer interrupt
ISR_HANDLER2 XBAR1_IRQ_0_1             // XBAR1_IRQ_0_1_IRQHandler        - XBAR1 interrupt
ISR_HANDLER2 XBAR1_IRQ_2_3             // XBAR1_IRQ_2_3_IRQHandler        - XBAR1 interrupt
ISR_HANDLER2 ADC_ETC_IRQ0              // ADC_ETC_IRQ0_IRQHandler         - ADCETC IRQ0 interrupt
ISR_HANDLER2 ADC_ETC_IRQ1              // ADC_ETC_IRQ1_IRQHandler         - ADCETC IRQ1 interrupt
ISR_HANDLER2 ADC_ETC_IRQ2              // ADC_ETC_IRQ2_IRQHandler         - ADCETC IRQ2 interrupt
ISR_HANDLER2 ADC_ETC_ERROR_IRQ         // ADC_ETC_ERROR_IRQ_IRQHandler    - ADCETC Error IRQ interrupt
ISR_HANDLER2 PIT                       // PIT_IRQHandler                  - PIT interrupt
ISR_HANDLER2 ACMP1                     // ACMP1_IRQHandler                - ACMP interrupt
ISR_HANDLER2 ACMP2                     // ACMP2_IRQHandler                - ACMP interrupt
ISR_HANDLER2 ACMP3                     // ACMP3_IRQHandler                - ACMP interrupt
ISR_HANDLER2 ACMP4                     // ACMP4_IRQHandler                - ACMP interrupt
ISR_RESERVED                           //                                 - Reserved interrupt
ISR_RESERVED                           //                                 - Reserved interrupt
ISR_HANDLER2 ENC1                      // ENC1_IRQHandler                 - ENC1 interrupt
ISR_HANDLER2 ENC2                      // ENC2_IRQHandler                 - ENC2 interrupt
ISR_HANDLER2 ENC3                      // ENC3_IRQHandler                 - ENC3 interrupt
ISR_HANDLER2 ENC4                      // ENC4_IRQHandler                 - ENC4 interrupt
ISR_HANDLER2 TMR1                      // TMR1_IRQHandler                 - TMR1 interrupt
ISR_HANDLER2 TMR2                      // TMR2_IRQHandler                 - TMR2 interrupt
ISR_HANDLER2 TMR3                      // TMR3_IRQHandler                 - TMR3 interrupt
ISR_HANDLER2 TMR4                      // TMR4_IRQHandler                 - TMR4 interrupt
ISR_HANDLER2 PWM2_0                    // PWM2_0_IRQHandler               - PWM2 capture 0, compare 0, or reload 0 interrupt
ISR_HANDLER2 PWM2_1                    // PWM2_1_IRQHandler               - PWM2 capture 1, compare 1, or reload 0 interrupt
ISR_HANDLER2 PWM2_2                    // PWM2_2_IRQHandler               - PWM2 capture 2, compare 2, or reload 0 interrupt
ISR_HANDLER2 PWM2_3                    // PWM2_3_IRQHandler               - PWM2 capture 3, compare 3, or reload 0 interrupt
ISR_HANDLER2 PWM2_FAULT                // PWM2_FAULT_IRQHandler           - PWM2 fault or reload error interrupt
ISR_HANDLER2 PWM3_0                    // PWM3_0_IRQHandler               - PWM3 capture 0, compare 0, or reload 0 interrupt
ISR_HANDLER2 PWM3_1                    // PWM3_1_IRQHandler               - PWM3 capture 1, compare 1, or reload 0 interrupt
ISR_HANDLER2 PWM3_2                    // PWM3_2_IRQHandler               - PWM3 capture 2, compare 2, or reload 0 interrupt
ISR_HANDLER2 PWM3_3                    // PWM3_3_IRQHandler               - PWM3 capture 3, compare 3, or reload 0 interrupt
ISR_HANDLER2 PWM3_FAULT                // PWM3_FAULT_IRQHandler           - PWM3 fault or reload error interrupt
ISR_HANDLER2 PWM4_0                    // PWM4_0_IRQHandler               - PWM4 capture 0, compare 0, or reload 0 interrupt
ISR_HANDLER2 PWM4_1                    // PWM4_1_IRQHandler               - PWM4 capture 1, compare 1, or reload 0 interrupt
ISR_HANDLER2 PWM4_2                    // PWM4_2_IRQHandler               - PWM4 capture 2, compare 2, or reload 0 interrupt
ISR_HANDLER2 PWM4_3                    // PWM4_3_IRQHandler               - PWM4 capture 3, compare 3, or reload 0 interrupt
ISR_HANDLER2 PWM4_FAULT                // PWM4_FAULT_IRQHandler           - PWM4 fault or reload error interrupt
ISR_HANDLER2 ENET2                     // ENET2_IRQHandler                - ENET2 interrupt
ISR_HANDLER2 ENET2_1588_Timer          // ENET2_1588_Timer_IRQHandler     - ENET2_1588_Timer interrupt
ISR_HANDLER2 CAN3                      // CAN3_IRQHandler                 - CAN3 interrupt
ISR_RESERVED                           //                                 - Reserved interrupt
ISR_HANDLER2 FLEXIO3                   // FLEXIO3_IRQHandler              - FLEXIO3 interrupt
ISR_HANDLER2 GPIO6_7_8_9               // GPIO6_7_8_9_IRQHandler          - GPIO6, GPIO7, GPIO8, GPIO9 interrupt
ISR_RESERVED                           // 174
ISR_RESERVED                           // 175
ISR_RESERVED                           // 176
ISR_RESERVED                           // 177
ISR_RESERVED                           // 178
ISR_RESERVED                           // 179
ISR_RESERVED                           // 180
ISR_RESERVED                           // 181
ISR_RESERVED                           // 182
ISR_RESERVED                           // 183
ISR_RESERVED                           // 184
ISR_RESERVED                           // 185
ISR_RESERVED                           // 186
ISR_RESERVED                           // 187
ISR_RESERVED                           // 188
ISR_RESERVED                           // 189
ISR_RESERVED                           // 190
ISR_RESERVED                           // 191
ISR_RESERVED                           // 192
ISR_RESERVED                           // 193
ISR_RESERVED                           // 194
ISR_RESERVED                           // 195
ISR_RESERVED                           // 196
ISR_RESERVED                           // 197
ISR_RESERVED                           // 198
ISR_RESERVED                           // 199
ISR_RESERVED                           // 200
ISR_RESERVED                           // 201
ISR_RESERVED                           // 202
ISR_RESERVED                           // 203
ISR_RESERVED                           // 204
ISR_RESERVED                           // 205
ISR_RESERVED                           // 206
ISR_RESERVED                           // 207
ISR_RESERVED                           // 208
ISR_RESERVED                           // 209
ISR_RESERVED                           // 210
ISR_RESERVED                           // 211
ISR_RESERVED                           // 212
ISR_RESERVED                           // 213
ISR_RESERVED                           // 214
ISR_RESERVED                           // 215
ISR_RESERVED                           // 216
ISR_RESERVED                           // 217
ISR_RESERVED                           // 218
ISR_RESERVED                           // 219
ISR_RESERVED                           // 220
ISR_RESERVED                           // 221
ISR_RESERVED                           // 222
ISR_RESERVED                           // 223
ISR_RESERVED                           // 224
ISR_RESERVED                           // 225
ISR_RESERVED                           // 226
ISR_RESERVED                           // 227
ISR_RESERVED                           // 228
ISR_RESERVED                           // 229
ISR_RESERVED                           // 230
ISR_RESERVED                           // 231
ISR_RESERVED                           // 232
ISR_RESERVED                           // 233
ISR_RESERVED                           // 234
ISR_RESERVED                           // 235
ISR_RESERVED                           // 236
ISR_RESERVED                           // 237
ISR_RESERVED                           // 238
ISR_RESERVED                           // 239
ISR_RESERVED                           // 240
ISR_RESERVED                           // 241
ISR_RESERVED                           // 242
ISR_RESERVED                           // 243
ISR_RESERVED                           // 244
ISR_RESERVED                           // 245
ISR_RESERVED                           // 246
ISR_RESERVED                           // 247
ISR_RESERVED                           // 248
ISR_RESERVED                           // 249
ISR_RESERVED                           // 250
ISR_RESERVED                           // 251
ISR_RESERVED                           // 252
ISR_RESERVED                           // 253
ISR_RESERVED                           // 254
.long 0xFFFFFFFF                       // Reserved for user TRIM value

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

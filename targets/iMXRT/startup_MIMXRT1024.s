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
ISR_HANDLER NMI_Handler           // NMI Handler
ISR_HANDLER HardFault_Handler     // Hard Fault Handler
ISR_HANDLER MemManage_Handler     // MPU Fault Handler
ISR_HANDLER BusFault_Handler      // Bus Fault Handler
ISR_HANDLER UsageFault_Handler    // Usage Fault Handler
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_HANDLER SVC_Handler           // SVCall Handler
ISR_HANDLER DebugMon_Handler      // Debug Monitor Handler
ISR_RESERVED
ISR_HANDLER PendSV_Handler        // PendSV Handler
ISR_HANDLER SysTick_Handler       // SysTick Handler
  // External Interrupts
ISR_HANDLER2 DMA0_DMA16           // 16 : DMA0_DMA16_IRQHandler           - DMA channel 0/16 transfer complete
ISR_HANDLER2 DMA1_DMA17           // 17 : DMA1_DMA17_IRQHandler           - DMA channel 1/17 transfer complete
ISR_HANDLER2 DMA2_DMA18           // 18 : DMA2_DMA18_IRQHandler           - DMA channel 2/18 transfer complete
ISR_HANDLER2 DMA3_DMA19           // 19 : DMA3_DMA19_IRQHandler           - DMA channel 3/19 transfer complete
ISR_HANDLER2 DMA4_DMA20           // 20 : DMA4_DMA20_IRQHandler           - DMA channel 4/20 transfer complete
ISR_HANDLER2 DMA5_DMA21           // 21 : DMA5_DMA21_IRQHandler           - DMA channel 5/21 transfer complete
ISR_HANDLER2 DMA6_DMA22           // 22 : DMA6_DMA22_IRQHandler           - DMA channel 6/22 transfer complete
ISR_HANDLER2 DMA7_DMA23           // 23 : DMA7_DMA23_IRQHandler           - DMA channel 7/23 transfer complete
ISR_HANDLER2 DMA8_DMA24           // 24 : DMA8_DMA24_IRQHandler           - DMA channel 8/24 transfer complete
ISR_HANDLER2 DMA9_DMA25           // 25 : DMA9_DMA25_IRQHandler           - DMA channel 9/25 transfer complete
ISR_HANDLER2 DMA10_DMA26          // 26 : DMA10_DMA26_IRQHandler          - DMA channel 10/26 transfer complete
ISR_HANDLER2 DMA11_DMA27          // 27 : DMA11_DMA27_IRQHandler          - DMA channel 11/27 transfer complete
ISR_HANDLER2 DMA12_DMA28          // 28 : DMA12_DMA28_IRQHandler          - DMA channel 12/28 transfer complete
ISR_HANDLER2 DMA13_DMA29          // 29 : DMA13_DMA29_IRQHandler          - DMA channel 13/29 transfer complete
ISR_HANDLER2 DMA14_DMA30          // 30 : DMA14_DMA30_IRQHandler          - DMA channel 14/30 transfer complete
ISR_HANDLER2 DMA15_DMA31          // 31 : DMA15_DMA31_IRQHandler          - DMA channel 15/31 transfer complete
ISR_HANDLER2 DMA_ERROR            // 32 : DMA_ERROR_IRQHandler            - DMA error interrupt channels 0-15 / 16-31
ISR_HANDLER2 CTI0_ERROR           // 33 : CTI0_ERROR_IRQHandler           - CTI trigger outputs
ISR_HANDLER2 CTI1_ERROR           // 34 : CTI1_ERROR_IRQHandler           - CTI trigger outputs
ISR_HANDLER2 CORE                 // 35 : CORE_IRQHandler                 - CorePlatform exception IRQ
ISR_HANDLER2 LPUART1              // 36 : LPUART1_IRQHandler              - LPUART1 TX interrupt and RX interrupt
ISR_HANDLER2 LPUART2              // 37 : LPUART2_IRQHandler              - LPUART2 TX interrupt and RX interrupt
ISR_HANDLER2 LPUART3              // 38 : LPUART3_IRQHandler              - LPUART3 TX interrupt and RX interrupt
ISR_HANDLER2 LPUART4              // 39 : LPUART4_IRQHandler              - LPUART4 TX interrupt and RX interrupt
ISR_HANDLER2 LPUART5              // 40 : LPUART5_IRQHandler              - LPUART5 TX interrupt and RX interrupt
ISR_HANDLER2 LPUART6              // 41 : LPUART6_IRQHandler              - LPUART6 TX interrupt and RX interrupt
ISR_HANDLER2 LPUART7              // 42 : LPUART7_IRQHandler              - LPUART7 TX interrupt and RX interrupt
ISR_HANDLER2 LPUART8              // 43 : LPUART8_IRQHandler              - LPUART8 TX interrupt and RX interrupt
ISR_HANDLER2 LPI2C1               // 44 : LPI2C1_IRQHandler               - LPI2C1 interrupt
ISR_HANDLER2 LPI2C2               // 45 : LPI2C2_IRQHandler               - LPI2C2 interrupt
ISR_HANDLER2 LPI2C3               // 46 : LPI2C3_IRQHandler               - LPI2C3 interrupt
ISR_HANDLER2 LPI2C4               // 47 : LPI2C4_IRQHandler               - LPI2C4 interrupt
ISR_HANDLER2 LPSPI1               // 48 : LPSPI1_IRQHandler               - LPSPI1 single interrupt vector for all sources
ISR_HANDLER2 LPSPI2               // 49 : LPSPI2_IRQHandler               - LPSPI2 single interrupt vector for all sources
ISR_HANDLER2 LPSPI3               // 50 : LPSPI3_IRQHandler               - LPSPI3 single interrupt vector for all sources
ISR_HANDLER2 LPSPI4               // 51 : LPSPI4_IRQHandler               - LPSPI4  single interrupt vector for all sources
ISR_HANDLER2 CAN1                 // 52 : CAN1_IRQHandler                 - CAN1 interrupt
ISR_HANDLER2 CAN2                 // 53 : CAN2_IRQHandler                 - CAN2 interrupt
ISR_HANDLER2 FLEXRAM              // 54 : FLEXRAM_IRQHandler              - FlexRAM address out of range Or access hit IRQ
ISR_HANDLER2 KPP                  // 55 : KPP_IRQHandler                  - Keypad nterrupt
ISR_RESERVED                      // 56 :                                 - Reserved interrupt
ISR_HANDLER2 GPR_IRQ              // 57 : GPR_IRQ_IRQHandler              - Used to notify cores on exception condition while boot
ISR_RESERVED                      // 58 :                                 - Reserved interrupt
ISR_RESERVED                      // 59 :                                 - Reserved interrupt
ISR_RESERVED                      // 60 :                                 - Reserved interrupt
ISR_HANDLER2 WDOG2                // 61 : WDOG2_IRQHandler                - WDOG2 interrupt
ISR_HANDLER2 SNVS_HP_WRAPPER      // 62 : SNVS_HP_WRAPPER_IRQHandler      - SNVS Functional Interrupt
ISR_HANDLER2 SNVS_HP_WRAPPER_TZ   // 63 : SNVS_HP_WRAPPER_TZ_IRQHandler   - SNVS Security Interrupt
ISR_HANDLER2 SNVS_LP_HP_WRAPPER   // 64 : SNVS_LP_HP_WRAPPER_IRQHandler   - ON-OFF button press shorter than 5 secs (pulse event)
ISR_HANDLER2 CSU                  // 65 : CSU_IRQHandler                  - CSU interrupt
ISR_HANDLER2 DCP                  // 66 : DCP_IRQHandler                  - Combined DCP channel interrupts(except channel 0) and CRC interrupt
ISR_HANDLER2 DCP_VMI              // 67 : DCP_VMI_IRQHandler              - IRQ of DCP channel 0
ISR_RESERVED                      // 68 :                                 - Reserved interrupt
ISR_HANDLER2 TRNG                 // 69 : TRNG_IRQHandler                 - TRNG interrupt
ISR_RESERVED                      // 70 :                                 - Reserved interrupt
ISR_HANDLER2 BEE                  // 71 : BEE_IRQHandler                  - BEE interrupt
ISR_HANDLER2 SAI1                 // 72 : SAI1_IRQHandler                 - SAI1 interrupt
ISR_HANDLER2 SAI2                 // 73 : SAI2_IRQHandler                 - SAI1 interrupt
ISR_HANDLER2 SAI3_RX              // 74 : SAI3_RX_IRQHandler              - SAI3 interrupt
ISR_HANDLER2 SAI3_TX              // 75 : SAI3_TX_IRQHandler              - SAI3 interrupt
ISR_HANDLER2 SPDIF                // 76 : SPDIF_IRQHandler                - SPDIF interrupt
ISR_HANDLER2 PMU                  // 77 : PMU_IRQHandler                  - PMU interrupt
ISR_RESERVED                      // 78 :                                 - Reserved interrupt
ISR_HANDLER2 TEMP_LOW_HIGH        // 79 : TEMP_LOW_HIGH_IRQHandler        - TEMPMON interrupt
ISR_HANDLER2 TEMP_PANIC           // 80 : TEMP_PANIC_IRQHandler           - TEMPMON interrupt
ISR_HANDLER2 USB_PHY              // 81 : USB_PHY_IRQHandler              - USBPHY (OTG1 UTMI), Interrupt
ISR_RESERVED                      // 82 :                                 - Reserved interrupt
ISR_HANDLER2 ADC1                 // 83 : ADC1_IRQHandler                 - ADC1 interrupt
ISR_HANDLER2 ADC2                 // 84 : ADC2_IRQHandler                 - ADC2 interrupt
ISR_HANDLER2 DCDC                 // 85 : DCDC_IRQHandler                 - DCDC interrupt
ISR_RESERVED                      // 86 :                                 - Reserved interrupt
ISR_RESERVED                      // 87 :                                 - Reserved interrupt
ISR_HANDLER2 GPIO1_INT0           // 88 : GPIO1_INT0_IRQHandler           - Active HIGH Interrupt from INT0 from GPIO
ISR_HANDLER2 GPIO1_INT1           // 89 : GPIO1_INT1_IRQHandler           - Active HIGH Interrupt from INT1 from GPIO
ISR_HANDLER2 GPIO1_INT2           // 90 : GPIO1_INT2_IRQHandler           - Active HIGH Interrupt from INT2 from GPIO
ISR_HANDLER2 GPIO1_INT3           // 91 : GPIO1_INT3_IRQHandler           - Active HIGH Interrupt from INT3 from GPIO
ISR_HANDLER2 GPIO1_INT4           // 92 : GPIO1_INT4_IRQHandler           - Active HIGH Interrupt from INT4 from GPIO
ISR_HANDLER2 GPIO1_INT5           // 93 : GPIO1_INT5_IRQHandler           - Active HIGH Interrupt from INT5 from GPIO
ISR_HANDLER2 GPIO1_INT6           // 94 : GPIO1_INT6_IRQHandler           - Active HIGH Interrupt from INT6 from GPIO
ISR_HANDLER2 GPIO1_INT7           // 95 : GPIO1_INT7_IRQHandler           - Active HIGH Interrupt from INT7 from GPIO
ISR_HANDLER2 GPIO1_Combined_0_15  // 96 : GPIO1_Combined_0_15_IRQHandler  - Combined interrupt indication for GPIO1 signal 0 throughout 15
ISR_HANDLER2 GPIO1_Combined_16_31 // 97 : GPIO1_Combined_16_31_IRQHandler - Combined interrupt indication for GPIO1 signal 16 throughout 31
ISR_HANDLER2 GPIO2_Combined_0_15  // 98 : GPIO2_Combined_0_15_IRQHandler  - Combined interrupt indication for GPIO2 signal 0 throughout 15
ISR_HANDLER2 GPIO2_Combined_16_31 // 99 : GPIO2_Combined_16_31_IRQHandler - Combined interrupt indication for GPIO2 signal 16 throughout 31
ISR_HANDLER2 GPIO3_Combined_0_15  // 100: GPIO3_Combined_0_15_IRQHandler  - Combined interrupt indication for GPIO3 signal 0 throughout 15
ISR_HANDLER2 GPIO3_Combined_16_31 // 101: GPIO3_Combined_16_31_IRQHandler - Combined interrupt indication for GPIO3 signal 16 throughout 31
ISR_RESERVED                      // 102:                                 - Reserved interrupt
ISR_RESERVED                      // 103:                                 - Reserved interrupt
ISR_HANDLER2 GPIO5_Combined_0_15  // 104: GPIO5_Combined_0_15_IRQHandler  - Combined interrupt indication for GPIO5 signal 0 throughout 15
ISR_HANDLER2 GPIO5_Combined_16_31 // 105: GPIO5_Combined_16_31_IRQHandler - Combined interrupt indication for GPIO5 signal 16 throughout 31
ISR_HANDLER2 FLEXIO1              // 106: FLEXIO1_IRQHandler              - FLEXIO1 interrupt
ISR_RESERVED                      // 107:                                 - Reserved interrupt
ISR_HANDLER2 WDOG1                // 108: WDOG1_IRQHandler                - WDOG1 interrupt
ISR_HANDLER2 RTWDOG               // 109: RTWDOG_IRQHandler               - RTWDOG interrupt
ISR_HANDLER2 EWM                  // 110: EWM_IRQHandler                  - EWM interrupt
ISR_HANDLER2 CCM_1                // 111: CCM_1_IRQHandler                - CCM IRQ1 interrupt
ISR_HANDLER2 CCM_2                // 112: CCM_2_IRQHandler                - CCM IRQ2 interrupt
ISR_HANDLER2 GPC                  // 113: GPC_IRQHandler                  - GPC interrupt
ISR_HANDLER2 SRC                  // 114: SRC_IRQHandler                  - SRC interrupt
ISR_RESERVED                      // 115:                                 - Reserved interrupt
ISR_HANDLER2 GPT1                 // 116: GPT1_IRQHandler                 - GPT1 interrupt
ISR_HANDLER2 GPT2                 // 117: GPT2_IRQHandler                 - GPT2 interrupt
ISR_HANDLER2 PWM1_0               // 118: PWM1_0_IRQHandler               - PWM1 capture 0, compare 0, or reload 0 interrupt
ISR_HANDLER2 PWM1_1               // 119: PWM1_1_IRQHandler               - PWM1 capture 1, compare 1, or reload 0 interrupt
ISR_HANDLER2 PWM1_2               // 120: PWM1_2_IRQHandler               - PWM1 capture 2, compare 2, or reload 0 interrupt
ISR_HANDLER2 PWM1_3               // 121: PWM1_3_IRQHandler               - PWM1 capture 3, compare 3, or reload 0 interrupt
ISR_HANDLER2 PWM1_FAULT           // 122: PWM1_FAULT_IRQHandler           - PWM1 fault or reload error interrupt
ISR_RESERVED                      // 123:                                 - Reserved interrupt
ISR_HANDLER2 FLEXSPI              // 124: FLEXSPI_IRQHandler              - FlexSPI0 interrupt
ISR_HANDLER2 SEMC                 // 125: SEMC_IRQHandler                 - Reserved interrupt
ISR_HANDLER2 USDHC1               // 126: USDHC1_IRQHandler               - USDHC1 interrupt
ISR_HANDLER2 USDHC2               // 127: USDHC2_IRQHandler               - USDHC2 interrupt
ISR_RESERVED                      // 128:                                 - Reserved interrupt
ISR_HANDLER2 USB_OTG1             // 129: USB_OTG1_IRQHandler             - USBO2 USB OTG1
ISR_HANDLER2 ENET                 // 130: ENET_IRQHandler                 - ENET interrupt
ISR_HANDLER2 ENET_1588_Timer      // 131: ENET_1588_Timer_IRQHandler      - ENET_1588_Timer interrupt
ISR_HANDLER2 XBAR1_IRQ_0_1        // 132: XBAR1_IRQ_0_1_IRQHandler        - XBAR1 interrupt
ISR_HANDLER2 XBAR1_IRQ_2_3        // 133: XBAR1_IRQ_2_3_IRQHandler        - XBAR1 interrupt
ISR_HANDLER2 ADC_ETC_IRQ0         // 134: ADC_ETC_IRQ0_IRQHandler         - ADCETC IRQ0 interrupt
ISR_HANDLER2 ADC_ETC_IRQ1         // 135: ADC_ETC_IRQ1_IRQHandler         - ADCETC IRQ1 interrupt
ISR_HANDLER2 ADC_ETC_IRQ2         // 136: ADC_ETC_IRQ2_IRQHandler         - ADCETC IRQ2 interrupt
ISR_HANDLER2 ADC_ETC_ERROR_IRQ    // 137: ADC_ETC_ERROR_IRQ_IRQHandler    - ADCETC Error IRQ interrupt
ISR_HANDLER2 PIT                  // 138: PIT_IRQHandler                  - PIT interrupt
ISR_HANDLER2 ACMP1                // 139: ACMP1_IRQHandler                - ACMP interrupt
ISR_HANDLER2 ACMP2                // 140: ACMP2_IRQHandler                - ACMP interrupt
ISR_HANDLER2 ACMP3                // 141: ACMP3_IRQHandler                - ACMP interrupt
ISR_HANDLER2 ACMP4                // 142: ACMP4_IRQHandler                - ACMP interrupt
ISR_RESERVED                      // 143:                                 - Reserved interrupt
ISR_RESERVED                      // 144:                                 - Reserved interrupt
ISR_HANDLER2 ENC1                 // 145: ENC1_IRQHandler                 - ENC1 interrupt
ISR_HANDLER2 ENC2                 // 146: ENC2_IRQHandler                 - ENC2 interrupt
ISR_RESERVED                      // 147:                                 - Reserved interrupt
ISR_RESERVED                      // 148:                                 - Reserved interrupt
ISR_HANDLER2 TMR1                 // 149: TMR1_IRQHandler                 - TMR1 interrupt
ISR_HANDLER2 TMR2                 // 150: TMR2_IRQHandler                 - TMR2 interrupt
ISR_RESERVED                      // 151:                                 - Reserved interrupt
ISR_RESERVED                      // 152:                                 - Reserved interrupt
ISR_HANDLER2 PWM2_0               // 153: PWM2_0_IRQHandler               - PWM2 capture 0, compare 0, or reload 0 interrupt
ISR_HANDLER2 PWM2_1               // 154: PWM2_1_IRQHandler               - PWM2 capture 1, compare 1, or reload 0 interrupt
ISR_HANDLER2 PWM2_2               // 155: PWM2_2_IRQHandler               - PWM2 capture 2, compare 2, or reload 0 interrupt
ISR_HANDLER2 PWM2_3               // 156: PWM2_3_IRQHandler               - PWM2 capture 3, compare 3, or reload 0 interrupt
ISR_HANDLER2 PWM2_FAULT           // 157: PWM2_FAULT_IRQHandler           - PWM2 fault or reload error interrupt
ISR_RESERVED                      // 158
ISR_RESERVED                      // 159
ISR_RESERVED                      // 160
ISR_RESERVED                      // 161
ISR_RESERVED                      // 162
ISR_RESERVED                      // 163
ISR_RESERVED                      // 164
ISR_RESERVED                      // 165
ISR_RESERVED                      // 166
ISR_RESERVED                      // 167
ISR_RESERVED                      // 168
ISR_RESERVED                      // 169
ISR_RESERVED                      // 170
ISR_RESERVED                      // 171
ISR_RESERVED                      // 172
ISR_RESERVED                      // 173
ISR_RESERVED                      // 174
ISR_RESERVED                      // 175
ISR_RESERVED                      // 176
ISR_RESERVED                      // 177
ISR_RESERVED                      // 178
ISR_RESERVED                      // 179
ISR_RESERVED                      // 180
ISR_RESERVED                      // 181
ISR_RESERVED                      // 182
ISR_RESERVED                      // 183
ISR_RESERVED                      // 184
ISR_RESERVED                      // 185
ISR_RESERVED                      // 186
ISR_RESERVED                      // 187
ISR_RESERVED                      // 188
ISR_RESERVED                      // 189
ISR_RESERVED                      // 190
ISR_RESERVED                      // 191
ISR_RESERVED                      // 192
ISR_RESERVED                      // 193
ISR_RESERVED                      // 194
ISR_RESERVED                      // 195
ISR_RESERVED                      // 196
ISR_RESERVED                      // 197
ISR_RESERVED                      // 198
ISR_RESERVED                      // 199
ISR_RESERVED                      // 200
ISR_RESERVED                      // 201
ISR_RESERVED                      // 202
ISR_RESERVED                      // 203
ISR_RESERVED                      // 204
ISR_RESERVED                      // 205
ISR_RESERVED                      // 206
ISR_RESERVED                      // 207
ISR_RESERVED                      // 208
ISR_RESERVED                      // 209
ISR_RESERVED                      // 210
ISR_RESERVED                      // 211
ISR_RESERVED                      // 212
ISR_RESERVED                      // 213
ISR_RESERVED                      // 214
ISR_RESERVED                      // 215
ISR_RESERVED                      // 216
ISR_RESERVED                      // 217
ISR_RESERVED                      // 218
ISR_RESERVED                      // 219
ISR_RESERVED                      // 220
ISR_RESERVED                      // 221
ISR_RESERVED                      // 222
ISR_RESERVED                      // 223
ISR_RESERVED                      // 224
ISR_RESERVED                      // 225
ISR_RESERVED                      // 226
ISR_RESERVED                      // 227
ISR_RESERVED                      // 228
ISR_RESERVED                      // 229
ISR_RESERVED                      // 230
ISR_RESERVED                      // 231
ISR_RESERVED                      // 232
ISR_RESERVED                      // 233
ISR_RESERVED                      // 234
ISR_RESERVED                      // 235
ISR_RESERVED                      // 236
ISR_RESERVED                      // 237
ISR_RESERVED                      // 238
ISR_RESERVED                      // 239
ISR_RESERVED                      // 240
ISR_RESERVED                      // 241
ISR_RESERVED                      // 242
ISR_RESERVED                      // 243
ISR_RESERVED                      // 244
ISR_RESERVED                      // 245
ISR_RESERVED                      // 246
ISR_RESERVED                      // 247
ISR_RESERVED                      // 248
ISR_RESERVED                      // 249
ISR_RESERVED                      // 250
ISR_RESERVED                      // 251
ISR_RESERVED                      // 252
ISR_RESERVED                      // 253
ISR_RESERVED                      // 254
.long 0xFFFFFFFF                  // Reserved for user TRIM value

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

/* The ROM expects the 512-byte FlexSPI NOR configuration parameters to be present at offset 0x0 in Serial NOR flash.
   Compare Chapter "9.6.1.2 FlexSPI Serial NOR Flash Boot Operation" in the reference manual of the iMXRT1021 / iMXRT1024 */
//.section .boot_offset
//  .fill 0, 1, 0

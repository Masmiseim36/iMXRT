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

.macro ISR_RESERVED
  .section .vectors, "ax"
  .word 0
.endm

  .syntax unified
  .global reset_handler

  .section .vectors, "ax"
  .code 16
  .global _vectors
_vectors:
   .long   __stack_end__
#ifdef STARTUP_FROM_RESET
  .word reset_handler
#else
  .word reset_wait
#endif /* STARTUP_FROM_RESET */
ISR_HANDLER NMI_Handler                      // NMI Handler
ISR_HANDLER HardFault_Handler                // Hard Fault Handler
ISR_HANDLER MemManage_Handler                // MPU Fault Handler
ISR_HANDLER BusFault_Handler                 // Bus Fault Handler
ISR_HANDLER UsageFault_Handler               // Usage Fault Handler
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_HANDLER SVC_Handler                      // SVCall Handler
ISR_HANDLER DebugMon_Handler                 // Debug Monitor Handler
ISR_RESERVED
ISR_HANDLER PendSV_Handler                   // PendSV Handler
ISR_HANDLER SysTick_Handler                  // SysTick Handler
  // External Interrupts
ISR_HANDLER DMA0_DMA16_IRQHandler            // 16 : DMA channel 0/16 transfer complete
ISR_HANDLER DMA1_DMA17_IRQHandler            // 17 : DMA channel 1/17 transfer complete
ISR_HANDLER DMA2_DMA18_IRQHandler            // 18 : DMA channel 2/18 transfer complete
ISR_HANDLER DMA3_DMA19_IRQHandler            // 19 : DMA channel 3/19 transfer complete
ISR_HANDLER DMA4_DMA20_IRQHandler            // 20 : DMA channel 4/20 transfer complete
ISR_HANDLER DMA5_DMA21_IRQHandler            // 21 : DMA channel 5/21 transfer complete
ISR_HANDLER DMA6_DMA22_IRQHandler            // 22 : DMA channel 6/22 transfer complete
ISR_HANDLER DMA7_DMA23_IRQHandler            // 23 : DMA channel 7/23 transfer complete
ISR_HANDLER DMA8_DMA24_IRQHandler            // 24 : DMA channel 8/24 transfer complete
ISR_HANDLER DMA9_DMA25_IRQHandler            // 25 : DMA channel 9/25 transfer complete
ISR_HANDLER DMA10_DMA26_IRQHandler           // 26 : DMA channel 10/26 transfer complete
ISR_HANDLER DMA11_DMA27_IRQHandler           // 27 : DMA channel 11/27 transfer complete
ISR_HANDLER DMA12_DMA28_IRQHandler           // 28 : DMA channel 12/28 transfer complete
ISR_HANDLER DMA13_DMA29_IRQHandler           // 29 : DMA channel 13/29 transfer complete
ISR_HANDLER DMA14_DMA30_IRQHandler           // 30 : DMA channel 14/30 transfer complete
ISR_HANDLER DMA15_DMA31_IRQHandler           // 31 : DMA channel 15/31 transfer complete
ISR_HANDLER DMA_ERROR_IRQHandler             // 32 : DMA error interrupt channels 0-15 / 16-31
ISR_HANDLER CTI0_ERROR_IRQHandler            // 33 : CTI trigger outputs
ISR_HANDLER CTI1_ERROR_IRQHandler            // 34 : CTI trigger outputs
ISR_HANDLER CORE_IRQHandler                  // 35 : CorePlatform exception IRQ
ISR_HANDLER LPUART1_IRQHandler               // 36 : LPUART1 TX interrupt and RX interrupt
ISR_HANDLER LPUART2_IRQHandler               // 37 : LPUART2 TX interrupt and RX interrupt
ISR_HANDLER LPUART3_IRQHandler               // 38 : LPUART3 TX interrupt and RX interrupt
ISR_HANDLER LPUART4_IRQHandler               // 39 : LPUART4 TX interrupt and RX interrupt
ISR_HANDLER LPUART5_IRQHandler               // 40 : LPUART5 TX interrupt and RX interrupt
ISR_HANDLER LPUART6_IRQHandler               // 41 : LPUART6 TX interrupt and RX interrupt
ISR_HANDLER LPUART7_IRQHandler               // 42 : LPUART7 TX interrupt and RX interrupt
ISR_HANDLER LPUART8_IRQHandler               // 43 : LPUART8 TX interrupt and RX interrupt
ISR_HANDLER LPI2C1_IRQHandler                // 44 : LPI2C1 interrupt
ISR_HANDLER LPI2C2_IRQHandler                // 45 : LPI2C2 interrupt
ISR_HANDLER LPI2C3_IRQHandler                // 46 : LPI2C3 interrupt
ISR_HANDLER LPI2C4_IRQHandler                // 47 : LPI2C4 interrupt
ISR_HANDLER LPSPI1_IRQHandler                // 48 : LPSPI1 single interrupt vector for all sources
ISR_HANDLER LPSPI2_IRQHandler                // 49 : LPSPI2 single interrupt vector for all sources
ISR_HANDLER LPSPI3_IRQHandler                // 50 : LPSPI3 single interrupt vector for all sources
ISR_HANDLER LPSPI4_IRQHandler                // 51 : LPSPI4  single interrupt vector for all sources
ISR_HANDLER CAN1_IRQHandler                  // 52 : CAN1 interrupt
ISR_HANDLER CAN2_IRQHandler                  // 53 : CAN2 interrupt
ISR_HANDLER FLEXRAM_IRQHandler               // 54 : FlexRAM address out of range Or access hit IRQ
ISR_HANDLER KPP_IRQHandler                   // 55 : Keypad nterrupt
ISR_RESERVED                                  // 56 : Reserved interrupt
ISR_HANDLER GPR_IRQ_IRQHandler               // 57 : Used to notify cores on exception condition while boot
ISR_RESERVED                                  // 58 : Reserved interrupt
ISR_RESERVED                                  // 59 : Reserved interrupt
ISR_RESERVED                                  // 60 : Reserved interrupt
ISR_HANDLER WDOG2_IRQHandler                 // 61 : WDOG2 interrupt
ISR_HANDLER SNVS_HP_WRAPPER_IRQHandler       // 62 : SNVS Functional Interrupt
ISR_HANDLER SNVS_HP_WRAPPER_TZ_IRQHandler    // 63 : SNVS Security Interrupt
ISR_HANDLER SNVS_LP_HP_WRAPPER_IRQHandler    // 64 : ON-OFF button press shorter than 5 secs (pulse event)
ISR_HANDLER CSU_IRQHandler                   // 65 : CSU interrupt
ISR_HANDLER DCP_IRQHandler                   // 66 : Combined DCP channel interrupts(except channel 0) and CRC interrupt
ISR_HANDLER DCP_VMI_IRQHandler               // 67 : IRQ of DCP channel 0
ISR_RESERVED                                  // 68 : Reserved interrupt
ISR_HANDLER TRNG_IRQHandler                  // 69 : TRNG interrupt
ISR_RESERVED                                  // 70 : Reserved interrupt
ISR_HANDLER BEE_IRQHandler                   // 71 : BEE interrupt
ISR_HANDLER SAI1_IRQHandler                  // 72 : SAI1 interrupt
ISR_HANDLER SAI2_IRQHandler                  // 73 : SAI1 interrupt
ISR_HANDLER SAI3_RX_IRQHandler               // 74 : SAI3 interrupt
ISR_HANDLER SAI3_TX_IRQHandler               // 75 : SAI3 interrupt
ISR_HANDLER SPDIF_IRQHandler                 // 76 : SPDIF interrupt
ISR_HANDLER PMU_IRQHandler                   // 77 : PMU interrupt
ISR_RESERVED                                  // 78 : Reserved interrupt
ISR_HANDLER TEMP_LOW_HIGH_IRQHandler         // 79 : TEMPMON interrupt
ISR_HANDLER TEMP_PANIC_IRQHandler            // 80 : TEMPMON interrupt
ISR_HANDLER USB_PHY_IRQHandler               // 81 : USBPHY (OTG1 UTMI), Interrupt
ISR_RESERVED                                  // 82 : Reserved interrupt
ISR_HANDLER ADC1_IRQHandler                  // 83 : ADC1 interrupt
ISR_HANDLER ADC2_IRQHandler                  // 84 : ADC2 interrupt
ISR_HANDLER DCDC_IRQHandler                  // 85 : DCDC interrupt
ISR_RESERVED                                  // 86 : Reserved interrupt
ISR_RESERVED                                  // 87 : Reserved interrupt
ISR_HANDLER GPIO1_INT0_IRQHandler            // 88 : Active HIGH Interrupt from INT0 from GPIO
ISR_HANDLER GPIO1_INT1_IRQHandler            // 89 : Active HIGH Interrupt from INT1 from GPIO
ISR_HANDLER GPIO1_INT2_IRQHandler            // 90 : Active HIGH Interrupt from INT2 from GPIO
ISR_HANDLER GPIO1_INT3_IRQHandler            // 91 : Active HIGH Interrupt from INT3 from GPIO
ISR_HANDLER GPIO1_INT4_IRQHandler            // 92 : Active HIGH Interrupt from INT4 from GPIO
ISR_HANDLER GPIO1_INT5_IRQHandler            // 93 : Active HIGH Interrupt from INT5 from GPIO
ISR_HANDLER GPIO1_INT6_IRQHandler            // 94 : Active HIGH Interrupt from INT6 from GPIO
ISR_HANDLER GPIO1_INT7_IRQHandler            // 95 : Active HIGH Interrupt from INT7 from GPIO
ISR_HANDLER GPIO1_Combined_0_15_IRQHandler   // 96 : Combined interrupt indication for GPIO1 signal 0 throughout 15
ISR_HANDLER GPIO1_Combined_16_31_IRQHandler  // 97 : Combined interrupt indication for GPIO1 signal 16 throughout 31
ISR_HANDLER GPIO2_Combined_0_15_IRQHandler   // 98 : Combined interrupt indication for GPIO2 signal 0 throughout 15
ISR_HANDLER GPIO2_Combined_16_31_IRQHandler  // 99 : Combined interrupt indication for GPIO2 signal 16 throughout 31
ISR_HANDLER GPIO3_Combined_0_15_IRQHandler   // 100: Combined interrupt indication for GPIO3 signal 0 throughout 15
ISR_HANDLER GPIO3_Combined_16_31_IRQHandler  // 101: Combined interrupt indication for GPIO3 signal 16 throughout 31
ISR_RESERVED                                  // 102: Reserved interrupt
ISR_RESERVED                                  // 103: Reserved interrupt
ISR_HANDLER GPIO5_Combined_0_15_IRQHandler   // 104: Combined interrupt indication for GPIO5 signal 0 throughout 15
ISR_HANDLER GPIO5_Combined_16_31_IRQHandler  // 105: Combined interrupt indication for GPIO5 signal 16 throughout 31
ISR_HANDLER FLEXIO1_IRQHandler               // 106: FLEXIO1 interrupt
ISR_RESERVED                                  // 107: Reserved interrupt
ISR_HANDLER WDOG1_IRQHandler                 // 108: WDOG1 interrupt
ISR_HANDLER RTWDOG_IRQHandler                // 109: RTWDOG interrupt
ISR_HANDLER EWM_IRQHandler                   // 110: EWM interrupt
ISR_HANDLER CCM_1_IRQHandler                 // 111: CCM IRQ1 interrupt
ISR_HANDLER CCM_2_IRQHandler                 // 112: CCM IRQ2 interrupt
ISR_HANDLER GPC_IRQHandler                   // 113: GPC interrupt
ISR_HANDLER SRC_IRQHandler                   // 114: SRC interrupt
ISR_RESERVED                                  // 115: Reserved interrupt
ISR_HANDLER GPT1_IRQHandler                  // 116: GPT1 interrupt
ISR_HANDLER GPT2_IRQHandler                  // 117: GPT2 interrupt
ISR_HANDLER PWM1_0_IRQHandler                // 118: PWM1 capture 0, compare 0, or reload 0 interrupt
ISR_HANDLER PWM1_1_IRQHandler                // 119: PWM1 capture 1, compare 1, or reload 0 interrupt
ISR_HANDLER PWM1_2_IRQHandler                // 120: PWM1 capture 2, compare 2, or reload 0 interrupt
ISR_HANDLER PWM1_3_IRQHandler                // 121: PWM1 capture 3, compare 3, or reload 0 interrupt
ISR_HANDLER PWM1_FAULT_IRQHandler            // 122: PWM1 fault or reload error interrupt
ISR_RESERVED                                  // 123: Reserved interrupt
ISR_HANDLER FLEXSPI_IRQHandler               // 124: FlexSPI0 interrupt
ISR_HANDLER SEMC_IRQHandler                  // 125: Reserved interrupt
ISR_HANDLER USDHC1_IRQHandler                // 126: USDHC1 interrupt
ISR_HANDLER USDHC2_IRQHandler                // 127: USDHC2 interrupt
ISR_RESERVED                                  // 128: Reserved interrupt
ISR_HANDLER USB_OTG1_IRQHandler              // 129: USBO2 USB OTG1
ISR_HANDLER ENET_IRQHandler                  // 130: ENET interrupt
ISR_HANDLER ENET_1588_Timer_IRQHandler       // 131: ENET_1588_Timer interrupt
ISR_HANDLER XBAR1_IRQ_0_1_IRQHandler         // 132: XBAR1 interrupt
ISR_HANDLER XBAR1_IRQ_2_3_IRQHandler         // 133: XBAR1 interrupt
ISR_HANDLER ADC_ETC_IRQ0_IRQHandler          // 134: ADCETC IRQ0 interrupt
ISR_HANDLER ADC_ETC_IRQ1_IRQHandler          // 135: ADCETC IRQ1 interrupt
ISR_HANDLER ADC_ETC_IRQ2_IRQHandler          // 136: ADCETC IRQ2 interrupt
ISR_HANDLER ADC_ETC_ERROR_IRQ_IRQHandler     // 137: ADCETC Error IRQ interrupt
ISR_HANDLER PIT_IRQHandler                   // 138: PIT interrupt
ISR_HANDLER ACMP1_IRQHandler                 // 139: ACMP interrupt
ISR_HANDLER ACMP2_IRQHandler                 // 140: ACMP interrupt
ISR_HANDLER ACMP3_IRQHandler                 // 141: ACMP interrupt
ISR_HANDLER ACMP4_IRQHandler                 // 142: ACMP interrupt
ISR_RESERVED                                  // 143: Reserved interrupt
ISR_RESERVED                                  // 144: Reserved interrupt
ISR_HANDLER ENC1_IRQHandler                  // 145: ENC1 interrupt
ISR_HANDLER ENC2_IRQHandler                  // 146: ENC2 interrupt
ISR_RESERVED                                  // 147: Reserved interrupt
ISR_RESERVED                                  // 148: Reserved interrupt
ISR_HANDLER TMR1_IRQHandler                  // 149: TMR1 interrupt
ISR_HANDLER TMR2_IRQHandler                  // 150: TMR2 interrupt
ISR_RESERVED                                  // 151: Reserved interrupt
ISR_RESERVED                                  // 152: Reserved interrupt
ISR_HANDLER PWM2_0_IRQHandler                // 153: PWM2 capture 0, compare 0, or reload 0 interrupt
ISR_HANDLER PWM2_1_IRQHandler                // 154: PWM2 capture 1, compare 1, or reload 0 interrupt
ISR_HANDLER PWM2_2_IRQHandler                // 155: PWM2 capture 2, compare 2, or reload 0 interrupt
ISR_HANDLER PWM2_3_IRQHandler                // 156: PWM2 capture 3, compare 3, or reload 0 interrupt
ISR_HANDLER PWM2_FAULT_IRQHandler            // 157: PWM2 fault or reload error interrupt
ISR_RESERVED                                 // 158
ISR_RESERVED                                 // 159
ISR_RESERVED                                 // 160
ISR_RESERVED                                 // 161
ISR_RESERVED                                 // 162
ISR_RESERVED                                 // 163
ISR_RESERVED                                 // 164
ISR_RESERVED                                 // 165
ISR_RESERVED                                 // 166
ISR_RESERVED                                 // 167
ISR_RESERVED                                 // 168
ISR_RESERVED                                 // 169
ISR_RESERVED                                 // 170
ISR_RESERVED                                 // 171
ISR_RESERVED                                 // 172
ISR_RESERVED                                 // 173
ISR_RESERVED                                 // 174
ISR_RESERVED                                 // 175
ISR_RESERVED                                 // 176
ISR_RESERVED                                 // 177
ISR_RESERVED                                 // 178
ISR_RESERVED                                 // 179
ISR_RESERVED                                 // 180
ISR_RESERVED                                 // 181
ISR_RESERVED                                 // 182
ISR_RESERVED                                 // 183
ISR_RESERVED                                 // 184
ISR_RESERVED                                 // 185
ISR_RESERVED                                 // 186
ISR_RESERVED                                 // 187
ISR_RESERVED                                 // 188
ISR_RESERVED                                 // 189
ISR_RESERVED                                 // 190
ISR_RESERVED                                 // 191
ISR_RESERVED                                 // 192
ISR_RESERVED                                 // 193
ISR_RESERVED                                 // 194
ISR_RESERVED                                 // 195
ISR_RESERVED                                 // 196
ISR_RESERVED                                 // 197
ISR_RESERVED                                 // 198
ISR_RESERVED                                 // 199
ISR_RESERVED                                 // 200
ISR_RESERVED                                 // 201
ISR_RESERVED                                 // 202
ISR_RESERVED                                 // 203
ISR_RESERVED                                 // 204
ISR_RESERVED                                 // 205
ISR_RESERVED                                 // 206
ISR_RESERVED                                 // 207
ISR_RESERVED                                 // 208
ISR_RESERVED                                 // 209
ISR_RESERVED                                 // 210
ISR_RESERVED                                 // 211
ISR_RESERVED                                 // 212
ISR_RESERVED                                 // 213
ISR_RESERVED                                 // 214
ISR_RESERVED                                 // 215
ISR_RESERVED                                 // 216
ISR_RESERVED                                 // 217
ISR_RESERVED                                 // 218
ISR_RESERVED                                 // 219
ISR_RESERVED                                 // 220
ISR_RESERVED                                 // 221
ISR_RESERVED                                 // 222
ISR_RESERVED                                 // 223
ISR_RESERVED                                 // 224
ISR_RESERVED                                 // 225
ISR_RESERVED                                 // 226
ISR_RESERVED                                 // 227
ISR_RESERVED                                 // 228
ISR_RESERVED                                 // 229
ISR_RESERVED                                 // 230
ISR_RESERVED                                 // 231
ISR_RESERVED                                 // 232
ISR_RESERVED                                 // 233
ISR_RESERVED                                 // 234
ISR_RESERVED                                 // 235
ISR_RESERVED                                 // 236
ISR_RESERVED                                 // 237
ISR_RESERVED                                 // 238
ISR_RESERVED                                 // 239
ISR_RESERVED                                 // 240
ISR_RESERVED                                 // 241
ISR_RESERVED                                 // 242
ISR_RESERVED                                 // 243
ISR_RESERVED                                 // 244
ISR_RESERVED                                 // 245
ISR_RESERVED                                 // 246
ISR_RESERVED                                 // 247
ISR_RESERVED                                 // 248
ISR_RESERVED                                 // 249
ISR_RESERVED                                 // 250
ISR_RESERVED                                 // 251
ISR_RESERVED                                 // 252
ISR_RESERVED                                 // 253
ISR_RESERVED                                 // 254
.long 0xFFFFFFFF   /*  Reserved for user TRIM value*/

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

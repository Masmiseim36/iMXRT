/*****************************************************************************
 * Copyright (c) 2018 Rowley Associates Limited.                      *
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
ISR_HANDLER NMI_Handler
ISR_HANDLER HardFault_Handler
ISR_HANDLER MemManage_Handler
ISR_HANDLER BusFault_Handler
ISR_HANDLER UsageFault_Handler
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
ISR_HANDLER2 DMA0_DMA16
ISR_HANDLER2 DMA1_DMA17
ISR_HANDLER2 DMA2_DMA18
ISR_HANDLER2 DMA3_DMA19
ISR_HANDLER2 DMA4_DMA20
ISR_HANDLER2 DMA5_DMA21
ISR_HANDLER2 DMA6_DMA22
ISR_HANDLER2 DMA7_DMA23
ISR_HANDLER2 DMA8_DMA24
ISR_HANDLER2 DMA9_DMA25
ISR_HANDLER2 DMA10_DMA26
ISR_HANDLER2 DMA11_DMA27
ISR_HANDLER2 DMA12_DMA28
ISR_HANDLER2 DMA13_DMA29
ISR_HANDLER2 DMA14_DMA30
ISR_HANDLER2 DMA15_DMA31
ISR_HANDLER2 DMA_ERROR
ISR_RESERVED // 17
ISR_RESERVED // 18
ISR_HANDLER2 CORE
ISR_HANDLER2 LPUART1
ISR_HANDLER2 LPUART2
ISR_HANDLER2 LPUART3
ISR_HANDLER2 LPUART4
ISR_HANDLER2 LPUART5
ISR_HANDLER2 LPUART6
ISR_HANDLER2 LPUART7
ISR_HANDLER2 LPUART8
ISR_HANDLER2 LPUART9
ISR_HANDLER2 LPUART10
ISR_HANDLER2 LPUART11
ISR_HANDLER2 LPUART12
ISR_HANDLER2 LPI2C1
ISR_HANDLER2 LPI2C2
ISR_HANDLER2 LPI2C3
ISR_HANDLER2 LPI2C4
ISR_HANDLER2 LPI2C5
ISR_HANDLER2 LPI2C6
ISR_HANDLER2 LPSPI1
ISR_HANDLER2 LPSPI2
ISR_HANDLER2 LPSPI3
ISR_HANDLER2 LPSPI4
ISR_HANDLER2 LPSPI5
ISR_HANDLER2 LPSPI6
ISR_HANDLER2 CAN1
ISR_HANDLER2 CAN1_ERROR
ISR_HANDLER2 CAN2
ISR_HANDLER2 CAN2_ERROR
ISR_HANDLER2 CAN3
ISR_HANDLER2 CAN3_ERROR
ISR_RESERVED // 50
ISR_HANDLER2 KPP
ISR_RESERVED // 52
ISR_HANDLER2 GPR_IRQ
ISR_HANDLER2 eLCDIF
ISR_HANDLER2 LCDIFv2
ISR_HANDLER2 CSI
ISR_HANDLER2 PXP
ISR_HANDLER2 MIPI_CSI
ISR_HANDLER2 MIPI_DSI
ISR_HANDLER2 GPU2D
ISR_HANDLER2 GPIO12_Combined_0_15
ISR_HANDLER2 GPIO12_Combined_16_31
ISR_HANDLER2 DAC
ISR_HANDLER2 KEY_MANAGER
ISR_HANDLER2 WDOG2
ISR_HANDLER2 SNVS_HP_NON_TZ
ISR_HANDLER2 SNVS_HP_TZ
ISR_HANDLER2 SNVS_PULSE_EVENT
ISR_HANDLER2 CAAM_IRQ0
ISR_HANDLER2 CAAM_IRQ1
ISR_HANDLER2 CAAM_IRQ2
ISR_HANDLER2 CAAM_IRQ3
ISR_HANDLER2 CAAM_RECORVE_ERRPR
ISR_HANDLER2 CAAM_RTIC
ISR_HANDLER2 CDOG
ISR_HANDLER2 SAI1
ISR_HANDLER2 SAI2
ISR_HANDLER2 SAI3_RX
ISR_HANDLER2 SAI3_TX
ISR_HANDLER2 SAI4_RX
ISR_HANDLER2 SAI4_TX
ISR_HANDLER2 SPDIF
ISR_HANDLER2 TMPSNS_INT
ISR_HANDLER2 TMPSNS_LOW_HIGH
ISR_HANDLER2 TMPSNS_PANIC
ISR_HANDLER2 LPSR_LP8_BROWNOUT
ISR_HANDLER2 LPSR_LP0_BROWNOUT
ISR_HANDLER2 ADC1
ISR_HANDLER2 ADC2
ISR_HANDLER2 USBPHY1
ISR_HANDLER2 USBPHY2
ISR_HANDLER2 RDC
ISR_HANDLER2 GPIO13_Combined_0_31
ISR_RESERVED // 94
ISR_HANDLER2 DCIC1
ISR_HANDLER2 DCIC2
ISR_HANDLER2 ASRC
ISR_HANDLER2 FLEXRAM_ECC
ISR_HANDLER2 GPIO7_8_9_10_11
ISR_HANDLER2 GPIO1_Combined_0_15
ISR_HANDLER2 GPIO1_Combined_16_31
ISR_HANDLER2 GPIO2_Combined_0_15
ISR_HANDLER2 GPIO2_Combined_16_31
ISR_HANDLER2 GPIO3_Combined_0_15
ISR_HANDLER2 GPIO3_Combined_16_31
ISR_HANDLER2 GPIO4_Combined_0_15
ISR_HANDLER2 GPIO4_Combined_16_31
ISR_HANDLER2 GPIO5_Combined_0_15
ISR_HANDLER2 GPIO5_Combined_16_31
ISR_HANDLER2 FLEXIO1
ISR_HANDLER2 FLEXIO2
ISR_HANDLER2 WDOG1
ISR_HANDLER2 RTWDOG4
ISR_HANDLER2 EWM
ISR_HANDLER2 OCOTP_READ_FUSE_ERROR
ISR_HANDLER2 OCOTP_READ_DONE_ERROR
ISR_HANDLER2 GPC
ISR_HANDLER2 MUB
ISR_HANDLER2 GPT1
ISR_HANDLER2 GPT2
ISR_HANDLER2 GPT3
ISR_HANDLER2 GPT4
ISR_HANDLER2 GPT5
ISR_HANDLER2 GPT6
ISR_HANDLER2 PWM1_0
ISR_HANDLER2 PWM1_1
ISR_HANDLER2 PWM1_2
ISR_HANDLER2 PWM1_3
ISR_HANDLER2 PWM1_FAULT
ISR_HANDLER2 FLEXSPI1
ISR_HANDLER2 FLEXSPI2
ISR_HANDLER2 SEMC
ISR_HANDLER2 USDHC1
ISR_HANDLER2 USDHC2
ISR_HANDLER2 USB_OTG2
ISR_HANDLER2 USB_OTG1
ISR_HANDLER2 ENET
ISR_HANDLER2 ENET_1588_Timer
ISR_HANDLER2 ENET_1G_MAC0_Tx_Rx_1
ISR_HANDLER2 ENET_1G_MAC0_Tx_Rx_2
ISR_HANDLER2 ENET_1G
ISR_HANDLER2 ENET_1G_1588_Timer
ISR_HANDLER2 XBAR1_IRQ_0_1
ISR_HANDLER2 XBAR1_IRQ_2_3
ISR_HANDLER2 ADC_ETC_IRQ0
ISR_HANDLER2 ADC_ETC_IRQ1
ISR_HANDLER2 ADC_ETC_IRQ2
ISR_HANDLER2 ADC_ETC_IRQ3
ISR_HANDLER2 ADC_ETC_ERROR_IRQ
ISR_RESERVED // 150
ISR_RESERVED // 151
ISR_RESERVED // 152
ISR_RESERVED // 153
ISR_RESERVED // 154
ISR_HANDLER2 PIT1
ISR_HANDLER2 PIT2
ISR_HANDLER2 ACMP1
ISR_HANDLER2 ACMP2
ISR_HANDLER2 ACMP3
ISR_HANDLER2 ACMP4
ISR_RESERVED // 161
ISR_RESERVED // 162
ISR_RESERVED // 163
ISR_RESERVED // 164
ISR_HANDLER2 ENC1
ISR_HANDLER2 ENC2
ISR_HANDLER2 ENC3
ISR_HANDLER2 ENC4
ISR_RESERVED // 169
ISR_RESERVED // 170
ISR_HANDLER2 TMR1
ISR_HANDLER2 TMR2
ISR_HANDLER2 TMR3
ISR_HANDLER2 TMR4
ISR_HANDLER2 SEMA4_CP0
ISR_HANDLER2 SEMA4_CP1
ISR_HANDLER2 PWM2_0
ISR_HANDLER2 PWM2_1
ISR_HANDLER2 PWM2_2
ISR_HANDLER2 PWM2_3
ISR_HANDLER2 PWM2_FAULT
ISR_HANDLER2 PWM3_0
ISR_HANDLER2 PWM3_1
ISR_HANDLER2 PWM3_2
ISR_HANDLER2 PWM3_3
ISR_HANDLER2 PWM3_FAULT
ISR_HANDLER2 PWM4_0
ISR_HANDLER2 PWM4_1
ISR_HANDLER2 PWM4_2
ISR_HANDLER2 PWM4_3
ISR_HANDLER2 PWM4_FAULT
ISR_RESERVED // 192
ISR_RESERVED // 193
ISR_RESERVED // 194
ISR_RESERVED // 195
ISR_RESERVED // 196
ISR_RESERVED // 197
ISR_RESERVED // 198
ISR_RESERVED // 199
ISR_HANDLER2 PDM_HWVAD_EVENT
ISR_HANDLER2 PDM_HWVAD_ERROR
ISR_HANDLER2 PDM_EVENT
ISR_HANDLER2 PDM_ERROR
ISR_HANDLER2 EMVSIM1
ISR_HANDLER2 EMVSIM2
ISR_HANDLER2 MECC1_INT
ISR_HANDLER2 MECC1_FATAL_INT
ISR_HANDLER2 MECC2_INT
ISR_HANDLER2 MECC2_FATAL_INT
ISR_HANDLER2 XECC_FLEXSPI1_INT
ISR_HANDLER2 XECC_FLEXSPI1_FATAL_INT
ISR_HANDLER2 XECC_FLEXSPI2_INT
ISR_HANDLER2 XECC_FLEXSPI2_FATAL_INT
ISR_HANDLER2 XECC_SEMC_INT
ISR_HANDLER2 XECC_SEMC_FATAL_INT
ISR_HANDLER2 ENET_QOS
ISR_HANDLER2 ENET_QOS_PMT
ISR_RESERVED // 218
ISR_RESERVED // 219
ISR_RESERVED // 220
ISR_RESERVED // 221
ISR_RESERVED // 222
ISR_RESERVED // 223
ISR_RESERVED // 224
ISR_RESERVED // 225
ISR_RESERVED // 226
ISR_RESERVED // 227
ISR_RESERVED // 228
ISR_RESERVED // 229
ISR_RESERVED // 230
ISR_RESERVED // 231
ISR_RESERVED // 232
ISR_RESERVED // 233
ISR_RESERVED // 234
ISR_RESERVED // 235
ISR_RESERVED // 236
ISR_RESERVED // 237
ISR_RESERVED // 238
.long 0xFFFFFFFF   /*  Reserved for user TRIM value*/

  .section .vectors, "ax"
_vectors_end:

#ifdef VECTORS_IN_RAM
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

#ifdef VECTORS_IN_RAM
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
#ifdef VECTORS_IN_RAM
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
/*****************************************************************************
 * Copyright (c) 2022 Rowley Associates Limited.                             *
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
ISR_HANDLER2 DMA0
ISR_HANDLER2 DMA1
ISR_HANDLER2 DMA2
ISR_HANDLER2 DMA3
ISR_HANDLER2 DMA4
ISR_HANDLER2 DMA5
ISR_HANDLER2 DMA6
ISR_HANDLER2 DMA7
ISR_HANDLER2 DMA8
ISR_HANDLER2 DMA9
ISR_HANDLER2 DMA10
ISR_HANDLER2 DMA11
ISR_HANDLER2 DMA12
ISR_HANDLER2 DMA13
ISR_HANDLER2 DMA14
ISR_HANDLER2 DMA15
ISR_HANDLER2 DMA_ERROR
ISR_HANDLER2 CTI0_ERROR
ISR_HANDLER2 CTI1_ERROR
ISR_HANDLER2 CORE
ISR_HANDLER2 LPUART1
ISR_HANDLER2 LPUART2
ISR_HANDLER2 LPUART3
ISR_HANDLER2 LPUART4
ISR_HANDLER2 PIT
ISR_HANDLER2 USB_OTG1
ISR_HANDLER2 FLEXSPI
ISR_HANDLER2 FLEXRAM
ISR_HANDLER2 LPI2C1
ISR_HANDLER2 LPI2C2
ISR_HANDLER2 GPT1
ISR_HANDLER2 GPT2
ISR_HANDLER2 LPSPI1
ISR_HANDLER2 LPSPI2
ISR_HANDLER2 PWM1_0
ISR_HANDLER2 PWM1_1
ISR_HANDLER2 PWM1_2
ISR_HANDLER2 PWM1_3
ISR_HANDLER2 PWM1_FAULT
ISR_HANDLER2 KPP
ISR_HANDLER2 SRC
ISR_HANDLER2 GPR_IRQ
ISR_HANDLER2 CCM_1
ISR_HANDLER2 CCM_2
ISR_HANDLER2 EWM
ISR_HANDLER2 WDOG2
ISR_HANDLER2 SNVS_HP_WRAPPER
ISR_HANDLER2 SNVS_HP_WRAPPER_TZ
ISR_HANDLER2 SNVS_LP_WRAPPER
ISR_HANDLER2 CSU
ISR_HANDLER2 DCP
ISR_HANDLER2 DCP_VMI
ISR_RESERVED // 52
ISR_HANDLER2 TRNG
ISR_RESERVED // 54
ISR_RESERVED // 55
ISR_HANDLER2 SAI1
ISR_HANDLER2 RTWDOG
ISR_HANDLER2 SAI3_RX
ISR_HANDLER2 SAI3_TX
ISR_HANDLER2 SPDIF
ISR_HANDLER2 PMU
ISR_HANDLER2 XBAR1_IRQ_0_1_2_3
ISR_HANDLER2 TEMP_LOW_HIGH
ISR_HANDLER2 TEMP_PANIC
ISR_HANDLER2 USB_PHY
ISR_HANDLER2 GPC
ISR_HANDLER2 ADC1
ISR_HANDLER2 FLEXIO1
ISR_HANDLER2 DCDC
ISR_HANDLER2 GPIO1_Combined_0_15
ISR_HANDLER2 GPIO1_Combined_16_31
ISR_HANDLER2 GPIO2_Combined_0_15
ISR_HANDLER2 GPIO5_Combined_0_15
ISR_HANDLER2 WDOG1
ISR_HANDLER2 ADC_ETC_IRQ0
ISR_HANDLER2 ADC_ETC_IRQ1
ISR_HANDLER2 ADC_ETC_IRQ2
ISR_HANDLER2 ADC_ETC_IRQ3
ISR_HANDLER2 ADC_ETC_ERROR_IRQ
ISR_RESERVED // 80
ISR_RESERVED // 81
ISR_RESERVED // 82
ISR_RESERVED // 83
ISR_RESERVED // 84
ISR_RESERVED // 85
ISR_RESERVED // 86
ISR_RESERVED // 87
ISR_RESERVED // 88
ISR_RESERVED // 89
ISR_RESERVED // 90
ISR_RESERVED // 91
ISR_RESERVED // 92
ISR_RESERVED // 93
ISR_RESERVED // 94
ISR_RESERVED // 95
ISR_RESERVED // 96
ISR_RESERVED // 97
ISR_RESERVED // 98
ISR_RESERVED // 99
ISR_RESERVED // 100
ISR_RESERVED // 101
ISR_RESERVED // 102
ISR_RESERVED // 103
ISR_RESERVED // 104
ISR_RESERVED // 105
ISR_RESERVED // 106
ISR_RESERVED // 107
ISR_RESERVED // 108
ISR_RESERVED // 109
ISR_RESERVED // 110
ISR_RESERVED // 111
ISR_RESERVED // 112
ISR_RESERVED // 113
ISR_RESERVED // 114
ISR_RESERVED // 115
ISR_RESERVED // 116
ISR_RESERVED // 117
ISR_RESERVED // 118
ISR_RESERVED // 119
ISR_RESERVED // 120
ISR_RESERVED // 121
ISR_RESERVED // 122
ISR_RESERVED // 123
ISR_RESERVED // 124
ISR_RESERVED // 125
ISR_RESERVED // 126
ISR_RESERVED // 127
ISR_RESERVED // 128
ISR_RESERVED // 129
ISR_RESERVED // 130
ISR_RESERVED // 131
ISR_RESERVED // 132
ISR_RESERVED // 133
ISR_RESERVED // 134
ISR_RESERVED // 135
ISR_RESERVED // 136
ISR_RESERVED // 137
ISR_RESERVED // 138
ISR_RESERVED // 139
ISR_RESERVED // 140
ISR_RESERVED // 141
ISR_RESERVED // 142
ISR_RESERVED // 143
ISR_RESERVED // 144
ISR_RESERVED // 145
ISR_RESERVED // 146
ISR_RESERVED // 147
ISR_RESERVED // 148
ISR_RESERVED // 149
ISR_RESERVED // 150
ISR_RESERVED // 151
ISR_RESERVED // 152
ISR_RESERVED // 153
ISR_RESERVED // 154
ISR_RESERVED // 155
ISR_RESERVED // 156
ISR_RESERVED // 157
ISR_RESERVED // 158
ISR_RESERVED // 159
ISR_RESERVED // 160
ISR_RESERVED // 161
ISR_RESERVED // 162
ISR_RESERVED // 163
ISR_RESERVED // 164
ISR_RESERVED // 165
ISR_RESERVED // 166
ISR_RESERVED // 167
ISR_RESERVED // 168
ISR_RESERVED // 169
ISR_RESERVED // 170
ISR_RESERVED // 171
ISR_RESERVED // 172
ISR_RESERVED // 173
ISR_RESERVED // 174
ISR_RESERVED // 175
ISR_RESERVED // 176
ISR_RESERVED // 177
ISR_RESERVED // 178
ISR_RESERVED // 179
ISR_RESERVED // 180
ISR_RESERVED // 181
ISR_RESERVED // 182
ISR_RESERVED // 183
ISR_RESERVED // 184
ISR_RESERVED // 185
ISR_RESERVED // 186
ISR_RESERVED // 187
ISR_RESERVED // 188
ISR_RESERVED // 189
ISR_RESERVED // 190
ISR_RESERVED // 191
ISR_RESERVED // 192
ISR_RESERVED // 193
ISR_RESERVED // 194
ISR_RESERVED // 195
ISR_RESERVED // 196
ISR_RESERVED // 197
ISR_RESERVED // 198
ISR_RESERVED // 199
ISR_RESERVED // 200
ISR_RESERVED // 201
ISR_RESERVED // 202
ISR_RESERVED // 203
ISR_RESERVED // 204
ISR_RESERVED // 205
ISR_RESERVED // 206
ISR_RESERVED // 207
ISR_RESERVED // 208
ISR_RESERVED // 209
ISR_RESERVED // 210
ISR_RESERVED // 211
ISR_RESERVED // 212
ISR_RESERVED // 213
ISR_RESERVED // 214
ISR_RESERVED // 215
ISR_RESERVED // 216
ISR_RESERVED // 217
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

.section .fill
  .fill 0x400, 1, 0

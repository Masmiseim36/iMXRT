/*****************************************************************************
 * Copyright (c) 2018 Rowley Associates Limited.                             *
 * Modified work Copyright (C) 2021-2023 Markus Klein                        *
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
ISR_HANDLER NMI_Handler                 // The NMI handler
ISR_HANDLER HardFault_Handler           // The hard fault handler
ISR_HANDLER MemManage_Handler           // The MPU fault handler
ISR_HANDLER BusFault_Handler            // The bus fault handler
ISR_HANDLER UsageFault_Handler          // The usage fault handler
ISR_HANDLER SecureFault_Handler         // The secure fault handler
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_HANDLER SVC_Handler
ISR_HANDLER DebugMon_Handler
ISR_RESERVED
ISR_HANDLER PendSV_Handler
ISR_HANDLER SysTick_Handler
// External Interrupts
ISR_HANDLER WDT0_IRQHandler
ISR_HANDLER DMA0_IRQHandler
ISR_HANDLER GPIO_INTA_IRQHandler
ISR_HANDLER GPIO_INTB_IRQHandler
ISR_HANDLER PIN_INT0_IRQHandler
ISR_HANDLER PIN_INT1_IRQHandler
ISR_HANDLER PIN_INT2_IRQHandler
ISR_HANDLER PIN_INT3_IRQHandler
ISR_HANDLER UTICK0_IRQHandler
ISR_HANDLER MRT0_IRQHandler
ISR_HANDLER CTIMER0_IRQHandler
ISR_HANDLER CTIMER1_IRQHandler
ISR_HANDLER SCT0_IRQHandler
ISR_HANDLER CTIMER3_IRQHandler
ISR_HANDLER FLEXCOMM0_IRQHandler
ISR_HANDLER FLEXCOMM1_IRQHandler
ISR_HANDLER FLEXCOMM2_IRQHandler
ISR_HANDLER FLEXCOMM3_IRQHandler
ISR_HANDLER FLEXCOMM4_IRQHandler
ISR_HANDLER FLEXCOMM5_IRQHandler
ISR_HANDLER FLEXCOMM14_IRQHandler
ISR_HANDLER FLEXCOMM15_IRQHandler
ISR_HANDLER ADC0_IRQHandler
ISR_HANDLER MRT1_IRQHandler
ISR_HANDLER ACMP_IRQHandler
ISR_HANDLER DMIC0_IRQHandler
ISR_RESERVED
ISR_HANDLER HYPERVISOR_IRQHandler
ISR_HANDLER SECUREVIOLATION_IRQHandler
ISR_HANDLER HWVAD0_IRQHandler
ISR_RESERVED
ISR_HANDLER RNG_IRQHandler
ISR_HANDLER RTC_IRQHandler
ISR_HANDLER DSPWAKE_IRQHandler
ISR_HANDLER MU_A_IRQHandler
ISR_HANDLER PIN_INT4_IRQHandler
ISR_HANDLER PIN_INT5_IRQHandler
ISR_HANDLER PIN_INT6_IRQHandler
ISR_HANDLER PIN_INT7_IRQHandler
ISR_HANDLER CTIMER2_IRQHandler
ISR_HANDLER CTIMER4_IRQHandler
ISR_HANDLER OS_EVENT_IRQHandler
ISR_HANDLER FLEXSPI_IRQHandler
ISR_HANDLER FLEXCOMM6_IRQHandler
ISR_HANDLER FLEXCOMM7_IRQHandler
ISR_HANDLER USDHC0_IRQHandler
ISR_HANDLER USDHC1_IRQHandler
ISR_HANDLER SGPIO_INTA_IRQHandler
ISR_HANDLER SGPIO_INTB_IRQHandler
ISR_HANDLER I3C0_IRQHandler
ISR_HANDLER USB_IRQHandler
ISR_HANDLER USB_WAKEUP_IRQHandler
ISR_HANDLER WDT1_IRQHandler
ISR_HANDLER USBPHY_DCD_IRQHandler
ISR_HANDLER DMA1_IRQHandler
ISR_HANDLER PUF_IRQHandler
ISR_HANDLER POWERQUAD_IRQHandler
ISR_HANDLER CASPER_IRQHandler
ISR_HANDLER PMC_PMIC_IRQHandler
ISR_HANDLER HASHCRYPT_IRQHandler
ISR_HANDLER FLEXCOMM8_IRQHandler
ISR_HANDLER FLEXCOMM9_IRQHandler
ISR_HANDLER FLEXCOMM10_IRQHandler
ISR_HANDLER FLEXCOMM11_IRQHandler
ISR_HANDLER FLEXCOMM12_IRQHandler
ISR_HANDLER FLEXCOMM13_IRQHandler
ISR_HANDLER FLEXCOMM16_IRQHandler
ISR_HANDLER I3C1_IRQHandler
ISR_HANDLER FLEXIO_IRQHandler
ISR_HANDLER LCDIF_IRQHandler
ISR_HANDLER GPU_IRQHandler
ISR_HANDLER MIPI_IRQHandler
ISR_RESERVED
ISR_HANDLER SDMA_IRQHandler
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
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

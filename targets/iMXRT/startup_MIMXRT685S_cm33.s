/*****************************************************************************
 * Copyright (c) 2018 Rowley Associates Limited.                             *
 * Modified work Copyright (C) 2020 Markus Klein                             *
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
ISR_HANDLER WDT0_IRQHandler             // 16: Windowed watchdog timer 0 (CM33 watchdog)
ISR_HANDLER DMA0_IRQHandler             // 17: DMA controller 0 (secure or CM33 DMA)
ISR_HANDLER GPIO_INTA_IRQHandler        // 18: GPIO interrupt A
ISR_HANDLER GPIO_INTB_IRQHandler        // 19: GPIO interrupt B
ISR_HANDLER PIN_INT0_IRQHandler         // 20: Pin interrupt 0 or pattern match engine slice 0 int
ISR_HANDLER PIN_INT1_IRQHandler         // 21: Pin interrupt 1 or pattern match engine slice 1 int
ISR_HANDLER PIN_INT2_IRQHandler         // 22: Pin interrupt 2 or pattern match engine slice 2 int
ISR_HANDLER PIN_INT3_IRQHandler         // 23: Pin interrupt 3 or pattern match engine slice 3 int
ISR_HANDLER UTICK0_IRQHandler           // 24: Micro-tick Timer
ISR_HANDLER MRT0_IRQHandler             // 25: Multi-Rate Timer
ISR_HANDLER CTIMER0_IRQHandler          // 26: Standard counter/timer CTIMER0
ISR_HANDLER CTIMER1_IRQHandler          // 27: Standard counter/timer CTIMER1
ISR_HANDLER SCT0_IRQHandler             // 28: SCTimer/PWM
ISR_HANDLER CTIMER3_IRQHandler          // 29: Standard counter/timer CTIMER3
ISR_HANDLER FLEXCOMM0_IRQHandler        // 30: Flexcomm Interface 0 (USART, SPI, I2C, I2S)
ISR_HANDLER FLEXCOMM1_IRQHandler        // 31: Flexcomm Interface 1 (USART, SPI, I2C, I2S)
ISR_HANDLER FLEXCOMM2_IRQHandler        // 32: Flexcomm Interface 2 (USART, SPI, I2C, I2S)
ISR_HANDLER FLEXCOMM3_IRQHandler        // 33: Flexcomm Interface 3 (USART, SPI, I2C, I2S)
ISR_HANDLER FLEXCOMM4_IRQHandler        // 34: Flexcomm Interface 4 (USART, SPI, I2C, I2S)
ISR_HANDLER FLEXCOMM5_IRQHandler        // 35: Flexcomm Interface 5 (USART, SPI, I2C, I2S)
ISR_HANDLER FLEXCOMM14_IRQHandler       // 36: Flexcomm Interface 14 (SPI only)
ISR_HANDLER FLEXCOMM15_IRQHandler       // 37: Flexcomm Interface 15 (I2C only)
ISR_HANDLER ADC0_IRQHandler             // 38: ADC0
ISR_HANDLER Reserved39_IRQHandler       // 39: Reserved interrupt
ISR_HANDLER ACMP_IRQHandler             // 40: Analog comparator
ISR_HANDLER DMIC0_IRQHandler            // 41: Digital microphone and DMIC subsystem
ISR_HANDLER Reserved42_IRQHandler       // 42: Reserved interrupt
ISR_HANDLER HYPERVISOR_IRQHandler       // 43: Hypervisor
ISR_HANDLER SECUREVIOLATION_IRQHandler  // 44: Secure violation
ISR_HANDLER HWVAD0_IRQHandler           // 45: Hardware Voice Activity Detector
ISR_HANDLER Reserved46_IRQHandler       // 46: Reserved interrupt
ISR_HANDLER RNG_IRQHandler              // 47: Random number Generator
ISR_HANDLER RTC_IRQHandler              // 48: RTC alarm and wake-up
ISR_HANDLER DSPWAKE_IRQHandler          // 49: Wake-up from DSP
ISR_HANDLER MU_A_IRQHandler             // 50: Messaging Unit port A for CM33
ISR_HANDLER PIN_INT4_IRQHandler         // 51: Pin interrupt 4 or pattern match engine slice 4 int
ISR_HANDLER PIN_INT5_IRQHandler         // 52: Pin interrupt 5 or pattern match engine slice 5 int
ISR_HANDLER PIN_INT6_IRQHandler         // 53: Pin interrupt 6 or pattern match engine slice 6 int
ISR_HANDLER PIN_INT7_IRQHandler         // 54: Pin interrupt 7 or pattern match engine slice 7 int
ISR_HANDLER CTIMER2_IRQHandler          // 55: Standard counter/timer CTIMER2
ISR_HANDLER CTIMER4_IRQHandler          // 56: Standard counter/timer CTIMER4
ISR_HANDLER OS_EVENT_IRQHandler         // 57: OS event timer
ISR_HANDLER FLEXSPI_IRQHandler          // 58: FLEXSPI interface
ISR_HANDLER FLEXCOMM6_IRQHandler        // 59: Flexcomm Interface 6 (USART, SPI, I2C, I2S)
ISR_HANDLER FLEXCOMM7_IRQHandler        // 60: Flexcomm Interface 7 (USART, SPI, I2C, I2S)
ISR_HANDLER USDHC0_IRQHandler           // 61: USDHC0 (Enhanced SDHC) interrupt request
ISR_HANDLER USDHC1_IRQHandler           // 62: USDHC1 (Enhanced SDHC) interrupt request
ISR_HANDLER SGPIO_INTA_IRQHandler       // 63: Secure GPIO interrupt A
ISR_HANDLER SGPIO_INTB_IRQHandler       // 64: Secure GPIO interrupt B
ISR_HANDLER I3C0_IRQHandler             // 65: I3C interface 0
ISR_HANDLER USB_IRQHandler              // 66: High-speed USB device/host
ISR_HANDLER USB_WAKEUP_IRQHandler       // 67: USB Activity Wake-up Interrupt
ISR_HANDLER WDT1_IRQHandler             // 68: Windowed watchdog timer 1 (HiFi 4 watchdog)
ISR_HANDLER USBPHY_DCD_IRQHandler       // 69: USBPHY DCD
ISR_HANDLER DMA1_IRQHandler             // 70: DMA controller 1 (non-secure or HiFi 4 DMA)
ISR_HANDLER PUF_IRQHandler              // 71: Physical Unclonable Function
ISR_HANDLER POWERQUAD_IRQHandler        // 72: PowerQuad math coprocessor
ISR_HANDLER CASPER_IRQHandler           // 73: Casper cryptographic coprocessor
ISR_HANDLER PMC_PMIC_IRQHandler         // 74: Power management IC
ISR_HANDLER HASHCRYPT_IRQHandler        // 75: Hash-AES unit
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

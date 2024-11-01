/*****************************************************************************
 * Copyright (c) 2018 Rowley Associates Limited.                             *
 * Modified work Copyright (C) 2020-2024 Markus Klein                        *
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
ISR_HANDLER NMI_Handler                        // The NMI handler
ISR_HANDLER HardFault_Handler                  // The hard fault handler
ISR_HANDLER MemManage_Handler                  // The MPU fault handler
ISR_HANDLER BusFault_Handler                   // The bus fault handler
ISR_HANDLER UsageFault_Handler                 // The usage fault handler
ISR_HANDLER SecureFault_Handler                // The secure fault handler
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_HANDLER SVC_Handler
ISR_HANDLER DebugMon_Handler
ISR_RESERVED
ISR_HANDLER PendSV_Handler
ISR_HANDLER SysTick_Handler
// External Interrupts
ISR_HANDLER Reserved16_IRQHandler              // Reserved interrupt 16
ISR_HANDLER Reserved17_IRQHandler              // Reserved interrupt 17
ISR_HANDLER Reserved18_IRQHandler              // Reserved interrupt 18
ISR_HANDLER FRO2_IRQHandler                    // FRO: 300MHz FRO in VDD1 domain Async interrupt
ISR_HANDLER GLIKEY4_IRQHandler                 // GLIKEY: Interrupt
ISR_HANDLER UTICK1_IRQHandler                  // Micro-tick Timer
ISR_HANDLER MRT1_IRQHandler                    // MRT: Ored Interrupt request
ISR_HANDLER CTIMER5_IRQHandler                 // CTIMER: Interrupt request
ISR_HANDLER CTIMER6_IRQHandler                 // CTIMER: Interrupt request
ISR_HANDLER CTIMER7_IRQHandler                 // CTIMER: Interrupt request
ISR_HANDLER LPI2C15_IRQHandler                 // LPI2C: Interrupt request
ISR_HANDLER LP_FLEXCOMM17_IRQHandler           // FLEXCOMM: Interrupt request
ISR_HANDLER LP_FLEXCOMM18_IRQHandler           // FLEXCOMM: Interrupt request
ISR_HANDLER LP_FLEXCOMM19_IRQHandler           // FLEXCOMM: Interrupt request
ISR_HANDLER LP_FLEXCOMM20_IRQHandler           // FLEXCOMM: Interrupt request
ISR_HANDLER ADC_IRQHandler                     // ADC0: Interrupt request
ISR_HANDLER SDADC_IRQHandler                   // SDADC0: Interrupt request
ISR_HANDLER GLIKEY1_IRQHandler                 // GLIKEY: Interrupt
ISR_HANDLER ACMP_IRQHandler                    // ACMP: interrupt request
ISR_HANDLER PDM_EVENT_IRQHandler               // MIC: Interrupt request for read data or Error
ISR_HANDLER HYPERVISOR_IRQHandler              // SECURE: Sense domain Hypervisor interrupt
ISR_HANDLER SECURE_VIOLATION_IRQHandler        // SECURE: Sense domain Secure violation interrupt
ISR_HANDLER PDM_HWVAD_EVENT_IRQHandler         // MIC: Hardware Voice Activity Detector interrupt or error interrupt
ISR_HANDLER RTC1_ALARM_IRQHandler              // RTC: Alarm interrupt
ISR_HANDLER RTC1_IRQHandler                    // RTC: wakeup interrupt to Sense domain
ISR_HANDLER HIFI1_IRQHandler                   // HIFI1: Interrupt request
ISR_HANDLER MU1_B_IRQHandler                   // MU1: MUB, CPU1 to CPU0
ISR_HANDLER MU2_B_IRQHandler                   // MU2: MUB, CPU1  to HiFi4
ISR_HANDLER MU3_A_IRQHandler                   // MU3: MUA, CPU1 to HiFi1
ISR_HANDLER PMC_IRQHandler                     // PMC: Sense domain Interrupt
ISR_HANDLER OS_EVENT_IRQHandler                // OSEVENT: Event timer CPU1 Wakeup/interrupt
ISR_HANDLER USDHC0_IRQHandler                  // USDHC: Interrupt request
ISR_HANDLER USDHC1_IRQHandler                  // USDHC: Interrupt request
ISR_HANDLER I3C2_IRQHandler                    // I3C: Interrupt Request
ISR_HANDLER USB0_IRQHandler                    // USB: HSUSB Interrup request
ISR_HANDLER USB1_IRQHandler                    // USB: eUSB Interrup request
ISR_HANDLER WDT2_IRQHandler                    // WDT: Interrupt request
ISR_HANDLER WDT3_IRQHandler                    // WDT: Interrupt request
ISR_HANDLER USBPHY0_IRQHandler                 // HSUSBPHY: UTM interrupt request
ISR_HANDLER PMIC_IRQN_IRQHandler               // PMIC: External PMIC interrupt
ISR_HANDLER I3C3_IRQHandler                    // I3C: Interrupt Request
ISR_HANDLER FLEXIO_IRQHandler                  // flexio: Interrupt request
ISR_HANDLER LCDIF_IRQHandler                   // dcn: Interrupt request
ISR_HANDLER VGPU_IRQHandler                    // VGPU interrupt from graphics core
ISR_HANDLER MIPI_IRQHandler                    // dsi: Interrupt request
ISR_HANDLER EDMA2_CH0_IRQHandler               // edma2: Channel 0 interrupt
ISR_HANDLER EDMA2_CH1_IRQHandler               // edma2: Channel 1 interrupt
ISR_HANDLER EDMA2_CH2_IRQHandler               // edma2: Channel 2 interrupt
ISR_HANDLER EDMA2_CH3_IRQHandler               // edma2: Channel 3 interrupt
ISR_HANDLER EDMA2_CH4_IRQHandler               // edma2: Channel 4 interrupt
ISR_HANDLER EDMA2_CH5_IRQHandler               // edma2: Channel 5 interrupt
ISR_HANDLER EDMA2_CH6_IRQHandler               // edma2: Channel 6 interrupt
ISR_HANDLER EDMA2_CH7_IRQHandler               // edma2: Channel 7 interrupt
ISR_HANDLER EDMA3_CH0_IRQHandler               // edma3: Channel 0 interrupt
ISR_HANDLER EDMA3_CH1_IRQHandler               // edma3: Channel 1 interrupt
ISR_HANDLER EDMA3_CH2_IRQHandler               // edma3: Channel 2 interrupt
ISR_HANDLER EDMA3_CH3_IRQHandler               // edma3: Channel 3 interrupt
ISR_HANDLER EDMA3_CH4_IRQHandler               // edma3: Channel 4 interrupt
ISR_HANDLER EDMA3_CH5_IRQHandler               // edma3: Channel 5 interrupt
ISR_HANDLER EDMA3_CH6_IRQHandler               // edma3: Channel 6 interrupt
ISR_HANDLER EDMA3_CH7_IRQHandler               // edma3: Channel 7 interrupt
ISR_HANDLER GPIO80_IRQHandler                  // rgpio8: Interupt request, channel 0
ISR_HANDLER GPIO81_IRQHandler                  // rgpio8: Interupt request, channel 1
ISR_HANDLER GPIO90_IRQHandler                  // rgpio9: Interupt request, channel 0
ISR_HANDLER GPIO91_IRQHandler                  // rgpio9: Interupt request, channel 1
ISR_HANDLER GPIO100_IRQHandler                 // rgpio10: Interupt request, channel 0
ISR_HANDLER GPIO101_IRQHandler                 // rgpio10: Interupt request, channel 1
ISR_HANDLER Reserved83_IRQHandler              // Reserved interrupt 83
ISR_HANDLER Reserved84_IRQHandler              // Reserved interrupt 84
ISR_HANDLER PIN_INT0_IRQHandler                // pint1: Interupt request 0
ISR_HANDLER PIN_INT1_IRQHandler                // pint1: Interupt request 1
ISR_HANDLER PIN_INT2_IRQHandler                // pint1: Interupt request 2
ISR_HANDLER PIN_INT3_IRQHandler                // pint1: Interupt request 3
ISR_HANDLER SAI3_IRQHandler                    // sai3: TX/RX interrupt
ISR_HANDLER XSPI2_IRQHandler                   // xspi2: Ored interrupt
ISR_HANDLER MMU2_IRQHandler                    // mmu2: Interrupt request
ISR_HANDLER GDET2_IRQHandler                   // gdet2_wrapper: Interrupt request
ISR_HANDLER GDET3_IRQHandler                   // gdet3_wrapper: Interrupt request
ISR_HANDLER CDOG3_IRQHandler                   // cdog3: Interrupt request
ISR_HANDLER CDOG4_IRQHandler                   // cdog4: Interrupt request
ISR_HANDLER Reserved96_IRQHandler              // Reserved interrupt 96
ISR_HANDLER CPU1_IRQHandler                    // CTI interrupt request
ISR_HANDLER JPEGDEC_IRQHandler                 // JPEGDEC Ored context 0-3 interrupt request
ISR_HANDLER PNGDEC_IRQHandler                  // PNGDEC interrupt request
ISR_HANDLER Reserved100_IRQHandler             // Reserved interrupt 100
ISR_HANDLER TEMPDET0_IRQHandler                // TEMPDET0: Temperature Detect Interrupt request 0
ISR_HANDLER TEMPDET1_IRQHandler                // TEMPDET1: Temperature Detect Interrupt request 1
ISR_HANDLER EZHV_IRQHandler                    // ezhv: Interrupt request
ISR_HANDLER SLEEPCON1_IRQHandler               // SLEEPCON_SENSE CPU1 wakeup event
ISR_HANDLER PVTS1_CPU1_IRQHandler              // PVTS1 CPU1 interrupt
ISR_HANDLER Reserved106_IRQHandler             // Reserved interrupt 106
ISR_HANDLER PVTS1_HIFI1_IRQHandler             // PVTS1 HIFI1 interrupt
ISR_HANDLER Reserved108_IRQHandler             // Reserved interrupt 108

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
//	mov sp, r0
	msr msp, r0
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
   Compare Chapter "41.5.1 FlexSPI boot" in the reference manual and also "41.5.1.5.2 FlexSPI NOR flash config parameters"*/
.section .boot_offset
  .fill 0x400, 1, 0


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
ISR_HANDLER UTICK0_IRQHandler                  // Micro-tick Timer
ISR_HANDLER MRT0_IRQHandler                    // MRT: Ored Interrupt request
ISR_HANDLER CTIMER0_IRQHandler                 // CTIMER: Interrupt request
ISR_HANDLER CTIMER1_IRQHandler                 // CTIMER: Interrupt request
ISR_HANDLER SCT0_IRQHandler                    // SCT: Interrupt request
ISR_HANDLER CTIMER3_IRQHandler                 // CTIMER: Interrupt request
ISR_HANDLER LP_FLEXCOMM0_IRQHandler            // FLEXCOMM: Interrupt request
ISR_HANDLER LP_FLEXCOMM1_IRQHandler            // FLEXCOMM: Interrupt request
ISR_HANDLER LP_FLEXCOMM2_IRQHandler            // FLEXCOMM: Interrupt request
ISR_HANDLER LP_FLEXCOMM3_IRQHandler            // FLEXCOMM: Interrupt request
ISR_HANDLER LP_FLEXCOMM4_IRQHandler            // FLEXCOMM: Interrupt request
ISR_HANDLER LP_FLEXCOMM5_IRQHandler            // FLEXCOMM: Interrupt request
ISR_HANDLER LPSPI14_IRQHandler                 // LPSPI: Interrupt request
ISR_HANDLER LPI2C15_IRQHandler                 // LPI2C: Interrupt request
ISR_HANDLER ADC_IRQHandler                     // ADC0: Interrupt request
ISR_HANDLER SDADC_IRQHandler                   // SDADC0: Interrupt request
ISR_HANDLER ACMP_IRQHandler                    // ACMP: interrupt request
ISR_HANDLER PDM_EVENT_IRQHandler               // MIC: Interrupt request for read data or Error
ISR_HANDLER PDM_HWVAD_EVENT_IRQHandler         // MIC: Hardware Voice Activity Detector interrupt or error interrupt
ISR_HANDLER HYPERVISOR_IRQHandler              // SECURE: Compute domain P-Bus Hypervisor interrupt
ISR_HANDLER SECURE_VIOLATION_IRQHandler        // SECURE: Compute domain P-Bus Secure violation interrupt
ISR_HANDLER SENSE_HYPERVISOR_IRQHandler        // Sense domain Hypervisor interrupt
ISR_HANDLER SENSE_SECURE_VIOLATION_IRQHandler  // Sense domain Secure violation interrupt
ISR_HANDLER MEDIA_HYPERVISOR_IRQHandler        // Media domain Hypervisor interrupt
ISR_HANDLER MEDIA_SECURE_VIOLATION_IRQHandler  // Media domain Secure violation interrupt
ISR_HANDLER RTC0_ALARM_IRQHandler              // RTC: Alarm interrupt
ISR_HANDLER RTC0_IRQHandler                    // RTC: wakeup interrupt to Compute domain
ISR_HANDLER Reserved44_IRQHandler              // Reserved interrupt
ISR_HANDLER MU0_A_IRQHandler                   // MU0: MUA, CPU0 to HiFi1
ISR_HANDLER MU1_A_IRQHandler                   // MU1: MUA, CPU0 to CPU1
ISR_HANDLER Reserved47_IRQHandler              // Reserved interrupt
ISR_HANDLER CTIMER2_IRQHandler                 // CTIMER: Interrupt request
ISR_HANDLER CTIMER4_IRQHandler                 // CTIMER: Interrupt request
ISR_HANDLER OS_EVENT_IRQHandler                // OSEVENT: Event timer CPU0 Wakeup/interrupt
ISR_HANDLER LP_FLEXCOMM6_IRQHandler            // FLEXCOMM: Interrupt request
ISR_HANDLER LP_FLEXCOMM7_IRQHandler            // FLEXCOMM: Interrupt request
ISR_HANDLER USDHC0_IRQHandler                  // USDHC: Interrupt request
ISR_HANDLER USDHC1_IRQHandler                  // USDHC: Interrupt request
ISR_HANDLER I3C0_IRQHandler                    // I3C: Interrupt Request
ISR_HANDLER USB0_IRQHandler                    // USB: HSUSB Interrup request
ISR_HANDLER USB1_IRQHandler                    // USB: eUSB Interrup request
ISR_HANDLER WDT0_IRQHandler                    // WDT: Interrupt request
ISR_HANDLER WDT1_IRQHandler                    // WDT: Interrupt request
ISR_HANDLER USBPHY0_IRQHandler                 // HSUSBPHY: UTM interrupt request
ISR_HANDLER PUF_IRQHandler                     // PUF: Interrupt Request
ISR_HANDLER PMIC_IRQN_IRQHandler               // PMIC: External PMIC interrupt
ISR_HANDLER LP_FLEXCOMM8_IRQHandler            // FLEXCOMM: Interrupt request
ISR_HANDLER LP_FLEXCOMM9_IRQHandler            // FLEXCOMM: Interrupt request
ISR_HANDLER LP_FLEXCOMM10_IRQHandler           // FLEXCOMM: Interrupt request
ISR_HANDLER LP_FLEXCOMM11_IRQHandler           // FLEXCOMM: Interrupt request
ISR_HANDLER LP_FLEXCOMM12_IRQHandler           // FLEXCOMM: Interrupt request
ISR_HANDLER LP_FLEXCOMM13_IRQHandler           // FLEXCOMM: Interrupt request
ISR_HANDLER LPSPI16_IRQHandler                 // LPSPI: Interrupt request
ISR_HANDLER I3C1_IRQHandler                    // I3C: Interrupt Request
ISR_HANDLER FLEXIO_IRQHandler                  // FLEXIO: Interrupt request
ISR_HANDLER Reserved72_IRQHandler              // Reserved interrupt
ISR_HANDLER Reserved73_IRQHandler              // Reserved interrupt
ISR_HANDLER Reserved74_IRQHandler              // Reserved interrupt
ISR_HANDLER EDMA0_CH0_IRQHandler               // EDMA: Channel 0 interrupt
ISR_HANDLER EDMA0_CH1_IRQHandler               // EDMA: Channel 1 interrupt
ISR_HANDLER EDMA0_CH2_IRQHandler               // EDMA: Channel 2 interrupt
ISR_HANDLER EDMA0_CH3_IRQHandler               // EDMA: Channel 3 interrupt
ISR_HANDLER EDMA0_CH4_IRQHandler               // EDMA: Channel 4 interrupt
ISR_HANDLER EDMA0_CH5_IRQHandler               // EDMA: Channel 5 interrupt
ISR_HANDLER EDMA0_CH6_IRQHandler               // EDMA: Channel 6 interrupt
ISR_HANDLER EDMA0_CH7_IRQHandler               // EDMA: Channel 7 interrupt
ISR_HANDLER EDMA0_CH8_IRQHandler               // EDMA: Channel 8 interrupt
ISR_HANDLER EDMA0_CH9_IRQHandler               // EDMA: Channel 9 interrupt
ISR_HANDLER EDMA0_CH10_IRQHandler              // EDMA: Channel 10 interrupt
ISR_HANDLER EDMA0_CH11_IRQHandler              // EDMA: Channel 11 interrupt
ISR_HANDLER EDMA0_CH12_IRQHandler              // EDMA: Channel 12 interrupt
ISR_HANDLER EDMA0_CH13_IRQHandler              // EDMA: Channel 13 interrupt
ISR_HANDLER EDMA0_CH14_IRQHandler              // EDMA: Channel 14 interrupt
ISR_HANDLER EDMA0_CH15_IRQHandler              // EDMA: Channel 15 interrupt
ISR_HANDLER EDMA1_CH0_IRQHandler               // EDMA: Channel 0 interrupt
ISR_HANDLER EDMA1_CH1_IRQHandler               // EDMA: Channel 1 interrupt
ISR_HANDLER EDMA1_CH2_IRQHandler               // EDMA: Channel 2 interrupt
ISR_HANDLER EDMA1_CH3_IRQHandler               // EDMA: Channel 3 interrupt
ISR_HANDLER EDMA1_CH4_IRQHandler               // EDMA: Channel 4 interrupt
ISR_HANDLER EDMA1_CH5_IRQHandler               // EDMA: Channel 5 interrupt
ISR_HANDLER EDMA1_CH6_IRQHandler               // EDMA: Channel 6 interrupt
ISR_HANDLER EDMA1_CH7_IRQHandler               // EDMA: Channel 7 interrupt
ISR_HANDLER EDMA1_CH8_IRQHandler               // EDMA: Channel 8 interrupt
ISR_HANDLER EDMA1_CH9_IRQHandler               // EDMA: Channel 9 interrupt
ISR_HANDLER EDMA1_CH10_IRQHandler              // EDMA: Channel 10 interrupt
ISR_HANDLER EDMA1_CH11_IRQHandler              // EDMA: Channel 11 interrupt
ISR_HANDLER EDMA1_CH12_IRQHandler              // EDMA: Channel 12 interrupt
ISR_HANDLER EDMA1_CH13_IRQHandler              // EDMA: Channel 13 interrupt
ISR_HANDLER EDMA1_CH14_IRQHandler              // EDMA: Channel 14 interrupt
ISR_HANDLER EDMA1_CH15_IRQHandler              // EDMA: Channel 15 interrupt
ISR_HANDLER GPIO00_IRQHandler                  // GPIO: Interupt request, channel 0
ISR_HANDLER GPIO01_IRQHandler                  // GPIO: Interupt request, channel 1
ISR_HANDLER GPIO10_IRQHandler                  // GPIO: Interupt request, channel 0
ISR_HANDLER GPIO11_IRQHandler                  // GPIO: Interupt request, channel 1
ISR_HANDLER GPIO20_IRQHandler                  // GPIO: Interupt request, channel 0
ISR_HANDLER GPIO21_IRQHandler                  // GPIO: Interupt request, channel 1
ISR_HANDLER GPIO30_IRQHandler                  // GPIO: Interupt request, channel 0
ISR_HANDLER GPIO31_IRQHandler                  // GPIO: Interupt request, channel 1
ISR_HANDLER GPIO40_IRQHandler                  // GPIO: Interupt request, channel 0
ISR_HANDLER GPIO41_IRQHandler                  // GPIO: Interupt request, channel 1
ISR_HANDLER GPIO50_IRQHandler                  // GPIO: Interupt request, channel 0
ISR_HANDLER GPIO51_IRQHandler                  // GPIO: Interupt request, channel 1
ISR_HANDLER GPIO60_IRQHandler                  // GPIO: Interupt request, channel 0
ISR_HANDLER GPIO61_IRQHandler                  // GPIO: Interupt request, channel 1
ISR_HANDLER GPIO70_IRQHandler                  // GPIO: Interupt request, channel 0
ISR_HANDLER GPIO71_IRQHandler                  // GPIO: Interupt request, channel 1
ISR_HANDLER PIN_INT0_IRQHandler                // PINT: Interupt request 0
ISR_HANDLER PIN_INT1_IRQHandler                // PINT: Interupt request 1
ISR_HANDLER PIN_INT2_IRQHandler                // PINT: Interupt request 2
ISR_HANDLER PIN_INT3_IRQHandler                // PINT: Interupt request 3
ISR_HANDLER PIN_INT4_IRQHandler                // PINT: Interupt request 4
ISR_HANDLER PIN_INT5_IRQHandler                // PINT: Interupt request 5
ISR_HANDLER PIN_INT6_IRQHandler                // PINT: Interupt request 6
ISR_HANDLER PIN_INT7_IRQHandler                // PINT: Interupt request 7
ISR_HANDLER SAI0_IRQHandler                    // SAI: TX/RX interrupt
ISR_HANDLER SAI1_IRQHandler                    // SAI: TX/RX interrupt
ISR_HANDLER SAI2_IRQHandler                    // SAI: TX/RX interrupt
ISR_HANDLER XSPI0_IRQHandler                   // xSPI: Ored interrupt
ISR_HANDLER XSPI1_IRQHandler                   // xSPI: Ored interrupt
ISR_HANDLER XSPI2_IRQHandler                   // xSPI: Ored interrupt
ISR_HANDLER MMU0_IRQHandler                    // MMU: Interrupt request
ISR_HANDLER MMU1_IRQHandler                    // MMU: Interrupt request
ISR_HANDLER MMU2_IRQHandler                    // MMU: Interrupt request
ISR_HANDLER Freqme_IRQHandler                  // FREQME: Interrupt request
ISR_HANDLER GDET0_IRQHandler                   // GDET: Interrupt request
ISR_HANDLER GDET1_IRQHandler                   // GDET: Interrupt request
ISR_HANDLER GDET2_IRQHandler                   // GDET: Interrupt request
ISR_HANDLER GDET3_IRQHandler                   // GDET: Interrupt request
ISR_HANDLER CDOG0_IRQHandler                   // CDOG: Interrupt request
ISR_HANDLER CDOG1_IRQHandler                   // CDOG: Interrupt request
ISR_HANDLER CDOG2_IRQHandler                   // CDOG: Interrupt request
ISR_HANDLER ITRC0_IRQHandler                   // ITRC: Interrupt, cleared by software
ISR_HANDLER ELS_IRQHandler                     // ELS Interrupt request
ISR_HANDLER PKC_IRQHandler                     // PKC: Public Key Cryptography Interrutp
ISR_HANDLER OSCCA_IRQHandler                   // OSCCA: OSCCA Interrupt request
ISR_HANDLER SLEEPCON0_IRQHandler               // WAKEUP: CPU0 wakeup event
ISR_HANDLER CPU0_IRQHandler                    // CTI0: interrupt request
ISR_HANDLER PMC_IRQHandler                     // PMC: Compute domain Interrupt
ISR_HANDLER Reserved155_IRQHandler             // Reserved interrupt
ISR_HANDLER Reserved156_IRQHandler             // Reserved interrupt
ISR_HANDLER NPU_IRQHandler                     // NPU: Interrupt request
ISR_HANDLER TRNG_IRQHandler                    // TRNG: interrupt
ISR_HANDLER TEMPDET0_IRQHandler                // TEMPDET0: Interrupt request 0
ISR_HANDLER TEMPDET1_IRQHandler                // TEMPDET1: Interrupt request 1
ISR_HANDLER EZHV_IRQHandler                    // EZH-V: Interrupt request
ISR_HANDLER GLIKEY0_IRQHandler                 // GLIKEY: Interrupt
ISR_HANDLER GLIKEY2_IRQHandler                 // GLIKEY: Interrupt
ISR_HANDLER GLIKEY3_IRQHandler                 // GLIKEY: Interrupt
ISR_HANDLER GLIKEY5_IRQHandler                 // GLIKEY: Interrupt
ISR_HANDLER PVTS0_CPU0_IRQHandler              // PVTS0 CPU0 interrupt
ISR_HANDLER Reserved167_IRQHandler             // Reserved interrupt 167
ISR_HANDLER PVTS0_HIFI4_IRQHandler             // PVTS0 HIFI4 interrupt
ISR_HANDLER Reserved169_IRQHandler             // Reserved interrupt 169
ISR_HANDLER FRO0_IRQHandler                    // FRO: 300MHz FRO in VDD2 domain Async interrupt
ISR_HANDLER FRO1_IRQHandler                    // FRO: 192MHz FRO in VDD2 domain Async interrupt
ISR_HANDLER FRO2_IRQHandler                    // FRO: 300MHz FRO in VDD1 domain Async interrupt
ISR_HANDLER OCOTP_IRQHandler                   // OCOTP warning and integrity error interrupt

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


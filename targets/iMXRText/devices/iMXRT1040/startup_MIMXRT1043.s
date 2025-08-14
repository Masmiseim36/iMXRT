/*****************************************************************************
 * Copyright (c) 2018 Rowley Associates Limited.                             *
 * Modified work Copyright (C) 2021-2024 Markus Klein                        *
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
ISR_HANDLER2 DMA0_DMA16                           // DMA channel 0/16 transfer complete                               DMA0_DMA16_IRQHandler
ISR_HANDLER2 DMA1_DMA17                           // DMA channel 1/17 transfer complete                               DMA1_DMA17_IRQHandler
ISR_HANDLER2 DMA2_DMA18                           // DMA channel 2/18 transfer complete                               DMA2_DMA18_IRQHandler
ISR_HANDLER2 DMA3_DMA19                           // DMA channel 3/19 transfer complete                               DMA3_DMA19_IRQHandler
ISR_HANDLER2 DMA4_DMA20                           // DMA channel 4/20 transfer complete                               DMA4_DMA20_IRQHandler
ISR_HANDLER2 DMA5_DMA21                           // DMA channel 5/21 transfer complete                               DMA5_DMA21_IRQHandler
ISR_HANDLER2 DMA6_DMA22                           // DMA channel 6/22 transfer complete                               DMA6_DMA22_IRQHandler
ISR_HANDLER2 DMA7_DMA23                           // DMA channel 7/23 transfer complete                               DMA7_DMA23_IRQHandler
ISR_HANDLER2 DMA8_DMA24                           // DMA channel 8/24 transfer complete                               DMA8_DMA24_IRQHandler
ISR_HANDLER2 DMA9_DMA25                           // DMA channel 9/25 transfer complete                               DMA9_DMA25_IRQHandler
ISR_HANDLER2 DMA10_DMA26                          // DMA channel 10/26 transfer complete                              DMA10_DMA26_IRQHandler
ISR_HANDLER2 DMA11_DMA27                          // DMA channel 11/27 transfer complete                              DMA11_DMA27_IRQHandler
ISR_HANDLER2 DMA12_DMA28                          // DMA channel 12/28 transfer complete                              DMA12_DMA28_IRQHandler
ISR_HANDLER2 DMA13_DMA29                          // DMA channel 13/29 transfer complete                              DMA13_DMA29_IRQHandler
ISR_HANDLER2 DMA14_DMA30                          // DMA channel 14/30 transfer complete                              DMA14_DMA30_IRQHandler
ISR_HANDLER2 DMA15_DMA31                          // DMA channel 15/31 transfer complete                              DMA15_DMA31_IRQHandler
ISR_HANDLER2 DMA_ERROR                            // DMA error interrupt channels 0-15 / 16-31                        DMA_ERROR_IRQHandler
ISR_HANDLER2 CTI0_ERROR                           // CTI0_Error                                                       CTI0_ERROR_IRQHandler
ISR_HANDLER2 CTI1_ERROR                           // CTI1_Error                                                       CTI1_ERROR_IRQHandler
ISR_HANDLER2 CORE                                 // CorePlatform exception IRQ                                       CORE_IRQHandler
ISR_HANDLER2 LPUART1                              // LPUART1 TX interrupt and RX interrupt                            LPUART1_IRQHandler
ISR_HANDLER2 LPUART2                              // LPUART2 TX interrupt and RX interrupt                            LPUART2_IRQHandler
ISR_HANDLER2 LPUART3                              // LPUART3 TX interrupt and RX interrupt                            LPUART3_IRQHandler
ISR_HANDLER2 LPUART4                              // LPUART4 TX interrupt and RX interrupt                            LPUART4_IRQHandler
ISR_HANDLER2 LPUART5                              // LPUART5 TX interrupt and RX interrupt                            LPUART5_IRQHandler
ISR_HANDLER2 LPUART6                              // LPUART6 TX interrupt and RX interrupt                            LPUART6_IRQHandler
ISR_HANDLER2 LPUART7                              // LPUART7 TX interrupt and RX interrupt                            LPUART7_IRQHandler
ISR_HANDLER2 LPUART8                              // LPUART8 TX interrupt and RX interrupt                            LPUART8_IRQHandler
ISR_HANDLER2 LPI2C1                               // LPI2C1 interrupt                                                 LPI2C1_IRQHandler
ISR_HANDLER2 LPI2C2                               // LPI2C2 interrupt                                                 LPI2C2_IRQHandler
ISR_HANDLER2 LPI2C3                               // LPI2C3 interrupt                                                 LPI2C3_IRQHandler
ISR_HANDLER2 LPI2C4                               // LPI2C4 interrupt                                                 LPI2C4_IRQHandler
ISR_HANDLER2 LPSPI1                               // LPSPI1 single interrupt vector for all sources                   LPSPI1_IRQHandler
ISR_HANDLER2 LPSPI2                               // LPSPI2 single interrupt vector for all sources                   LPSPI2_IRQHandler
ISR_RESERVED                                      // Reserved interrupt                                               Reserved50_IRQHandler
ISR_HANDLER2 LPSPI3                               // LPSPI3 single interrupt vector for all sources                   LPSPI3_IRQHandler
ISR_HANDLER2 CAN1                                 // CAN1 interrupt                                                   CAN1_IRQHandler
ISR_HANDLER2 CAN2                                 // CAN2 interrupt                                                   CAN2_IRQHandler
ISR_HANDLER2 FLEXRAM                              // FlexRAM address out of range Or access hit IRQ                   FLEXRAM_IRQHandler
ISR_RESERVED                                      // Reserved interrupt                                               Reserved55_IRQHandler
ISR_RESERVED                                      // Reserved interrupt                                               Reserved56_IRQHandler
ISR_HANDLER2 GPR_IRQ                              // GPR interrupt                                                    GPR_IRQ_IRQHandler
ISR_HANDLER2 LCDIF                                // LCDIF interrupt                                                  LCDIF_IRQHandler
ISR_RESERVED                                      // Reserved interrupt                                               Reserved59_IRQHandler
ISR_HANDLER2 PXP                                  // PXP interrupt                                                    PXP_IRQHandler
ISR_HANDLER2 WDOG2                                // WDOG2 interrupt                                                  WDOG2_IRQHandler
ISR_HANDLER2 SNVS_HP_WRAPPER                      // SNVS Functional Interrupt                                        SNVS_HP_WRAPPER_IRQHandler
ISR_HANDLER2 SNVS_HP_WRAPPER_TZ                   // SNVS Security Interrupt                                          SNVS_HP_WRAPPER_TZ_IRQHandler
ISR_HANDLER2 SNVS_LP_WRAPPER                      // ON-OFF button press shorter than 5 secs (pulse event)            SNVS_LP_WRAPPER_IRQHandler
ISR_HANDLER2 CSU                                  // CSU interrupt                                                    CSU_IRQHandler
ISR_HANDLER2 DCP                                  // DCP_IRQ interrupt                                                DCP_IRQHandler
ISR_HANDLER2 DCP_VMI                              // DCP_VMI_IRQ interrupt                                            DCP_VMI_IRQHandler
ISR_RESERVED                                      // Reserved interrupt                                               Reserved68_IRQHandler
ISR_HANDLER2 TRNG                                 // TRNG interrupt                                                   TRNG_IRQHandler
ISR_RESERVED                                      // Reserved interrupt                                               Reserved70_IRQHandler
ISR_HANDLER2 BEE                                  // BEE interrupt                                                    BEE_IRQHandler
ISR_HANDLER2 SAI1                                 // SAI1 interrupt                                                   SAI1_IRQHandler
ISR_HANDLER2 SAI2                                 // SAI1 interrupt                                                   SAI2_IRQHandler
ISR_HANDLER2 SAI3_RX                              // SAI3 interrupt                                                   SAI3_RX_IRQHandler
ISR_HANDLER2 SAI3_TX                              // SAI3 interrupt                                                   SAI3_TX_IRQHandler
ISR_HANDLER2 SPDIF                                // SPDIF interrupt                                                  SPDIF_IRQHandler
ISR_HANDLER2 PMU_EVENT                            // Brown-out event interrupt                                        PMU_EVENT_IRQHandler
ISR_RESERVED                                      // Reserved interrupt                                               Reserved78_IRQHandler
ISR_HANDLER2 TEMP_LOW_HIGH                        // TempSensor low/high interrupt                                    TEMP_LOW_HIGH_IRQHandler
ISR_HANDLER2 TEMP_PANIC                           // TempSensor panic interrupt                                       TEMP_PANIC_IRQHandler
ISR_HANDLER2 USB_PHY                              // USBPHY (UTMI0), Interrupt                                        USB_PHY_IRQHandler
ISR_RESERVED                                      // Reserved interrupt                                               Reserved82_IRQHandler
ISR_HANDLER2 ADC1                                 // ADC1 interrupt                                                   ADC1_IRQHandler
ISR_HANDLER2 ADC2                                 // ADC2 interrupt                                                   ADC2_IRQHandler
ISR_HANDLER2 DCDC                                 // DCDC interrupt                                                   DCDC_IRQHandler
ISR_RESERVED                                      // Reserved interrupt                                               Reserved86_IRQHandler
ISR_RESERVED                                      // Reserved interrupt                                               Reserved87_IRQHandler
ISR_HANDLER2 GPIO1_INT0                           // Active HIGH Interrupt from INT0 from GPIO                        GPIO1_INT0_IRQHandler
ISR_HANDLER2 GPIO1_INT1                           // Active HIGH Interrupt from INT1 from GPIO                        GPIO1_INT1_IRQHandler
ISR_HANDLER2 GPIO1_INT2                           // Active HIGH Interrupt from INT2 from GPIO                        GPIO1_INT2_IRQHandler
ISR_HANDLER2 GPIO1_INT3                           // Active HIGH Interrupt from INT3 from GPIO                        GPIO1_INT3_IRQHandler
ISR_HANDLER2 GPIO1_INT4                           // Active HIGH Interrupt from INT4 from GPIO                        GPIO1_INT4_IRQHandler
ISR_HANDLER2 GPIO1_INT5                           // Active HIGH Interrupt from INT5 from GPIO                        GPIO1_INT5_IRQHandler
ISR_HANDLER2 GPIO1_INT6                           // Active HIGH Interrupt from INT6 from GPIO                        GPIO1_INT6_IRQHandler
ISR_HANDLER2 GPIO1_INT7                           // Active HIGH Interrupt from INT7 from GPIO                        GPIO1_INT7_IRQHandler
ISR_HANDLER2 GPIO1_Combined_0_15                  // Combined interrupt indication for GPIO1 signal 0 throughout 15   GPIO1_Combined_0_15_IRQHandler
ISR_HANDLER2 GPIO1_Combined_16_31                 // Combined interrupt indication for GPIO1 signal 16 throughout 31  GPIO1_Combined_16_31_IRQHandler
ISR_HANDLER2 GPIO2_Combined_0_15                  // Combined interrupt indication for GPIO2 signal 0 throughout 15   GPIO2_Combined_0_15_IRQHandler
ISR_HANDLER2 GPIO2_Combined_16_31                 // Combined interrupt indication for GPIO2 signal 16 throughout 31  GPIO2_Combined_16_31_IRQHandler
ISR_HANDLER2 GPIO3_Combined_0_15                  // Combined interrupt indication for GPIO3 signal 0 throughout 15   GPIO3_Combined_0_15_IRQHandler
ISR_HANDLER2 GPIO3_Combined_16_31                 // Combined interrupt indication for GPIO3 signal 16 throughout 31  GPIO3_Combined_16_31_IRQHandler
ISR_HANDLER2 GPIO4_Combined_0_15                  // Combined interrupt indication for GPIO4 signal 0 throughout 15   GPIO4_Combined_0_15_IRQHandler
ISR_HANDLER2 GPIO4_Combined_16_31                 // Combined interrupt indication for GPIO4 signal 16 throughout 31  GPIO4_Combined_16_31_IRQHandler
ISR_HANDLER2 GPIO5_Combined_0_15                  // Combined interrupt indication for GPIO5 signal 0 throughout 15   GPIO5_Combined_0_15_IRQHandler
ISR_HANDLER2 GPIO5_Combined_16_31                 // Combined interrupt indication for GPIO5 signal 16 throughout 31  GPIO5_Combined_16_31_IRQHandler
ISR_HANDLER2 FLEXIO1                              // FLEXIO1 interrupt                                                FLEXIO1_IRQHandler
ISR_HANDLER2 FLEXIO2                              // FLEXIO2 interrupt                                                FLEXIO2_IRQHandler
ISR_HANDLER2 WDOG1                                // WDOG1 interrupt                                                  WDOG1_IRQHandler
ISR_HANDLER2 RTWDOG                               // RTWDOG interrupt                                                 RTWDOG_IRQHandler
ISR_HANDLER2 EWM                                  // EWM interrupt                                                    EWM_IRQHandler
ISR_HANDLER2 CCM_1                                // CCM IRQ1 interrupt                                               CCM_1_IRQHandler
ISR_HANDLER2 CCM_2                                // CCM IRQ2 interrupt                                               CCM_2_IRQHandler
ISR_HANDLER2 GPC                                  // GPC interrupt                                                    GPC_IRQHandler
ISR_HANDLER2 SRC                                  // SRC interrupt                                                    SRC_IRQHandler
ISR_RESERVED                                      // Reserved interrupt                                               Reserved115_IRQHandler
ISR_HANDLER2 GPT1                                 // GPT1 interrupt                                                   GPT1_IRQHandler
ISR_HANDLER2 GPT2                                 // GPT2 interrupt                                                   GPT2_IRQHandler
ISR_HANDLER2 PWM1_0                               // PWM1 capture 0, compare 0, or reload 0 interrupt                 PWM1_0_IRQHandler
ISR_HANDLER2 PWM1_1                               // PWM1 capture 1, compare 1, or reload 0 interrupt                 PWM1_1_IRQHandler
ISR_HANDLER2 PWM1_2                               // PWM1 capture 2, compare 2, or reload 0 interrupt                 PWM1_2_IRQHandler
ISR_HANDLER2 PWM1_3                               // PWM1 capture 3, compare 3, or reload 0 interrupt                 PWM1_3_IRQHandler
ISR_HANDLER2 PWM1_FAULT                           // PWM1 fault or reload error interrupt                             PWM1_FAULT_IRQHandler
ISR_HANDLER2 FLEXSPI2                             // FlexSPI2 interrupt                                               FLEXSPI2_IRQHandler
ISR_HANDLER2 FLEXSPI                              // FlexSPI interrupt                                                FLEXSPI_IRQHandler
ISR_HANDLER2 SEMC                                 // SEMC interrupt                                                   SEMC_IRQHandler
ISR_HANDLER2 USDHC1                               // USDHC1 interrupt                                                 USDHC1_IRQHandler
ISR_HANDLER2 USDHC2                               // USDHC2 interrupt                                                 USDHC2_IRQHandler
ISR_RESERVED                                      // Reserved interrupt                                               Reserved112_IRQHandler
ISR_HANDLER2 USB_OTG                              // USBO2 USB OTG                                                    USB_OTG_IRQHandler
ISR_HANDLER2 ENET                                 // ENET interrupt                                                   ENET_IRQHandler
ISR_HANDLER2 ENET_1588_Timer                      // ENET_1588_Timer interrupt                                        ENET_1588_Timer_IRQHandler
ISR_HANDLER2 XBAR1_IRQ_0_1                        // XBARA1 output signal 0, 1 interrupt                              XBAR1_IRQ_0_1_IRQHandler
ISR_HANDLER2 XBAR1_IRQ_2_3                        // XBARA1 output signal 2, 3 interrupt                              XBAR1_IRQ_2_3_IRQHandler
ISR_HANDLER2 ADC_ETC_IRQ0                         // ADCETC IRQ0 interrupt                                            ADC_ETC_IRQ0_IRQHandler
ISR_HANDLER2 ADC_ETC_IRQ1                         // ADCETC IRQ1 interrupt                                            ADC_ETC_IRQ1_IRQHandler
ISR_HANDLER2 ADC_ETC_IRQ2                         // ADCETC IRQ2 interrupt                                            ADC_ETC_IRQ2_IRQHandler
ISR_HANDLER2 ADC_ETC_ERROR_IRQ                    // ADCETC Error IRQ interrupt                                       ADC_ETC_ERROR_IRQ_IRQHandler
ISR_HANDLER2 PIT                                  // PIT interrupt                                                    PIT_IRQHandler
ISR_HANDLER2 ACMP1                                // ACMP interrupt                                                   ACMP1_IRQHandler
ISR_HANDLER2 ACMP2                                // ACMP interrupt                                                   ACMP2_IRQHandler
ISR_HANDLER2 ACMP3                                // ACMP interrupt                                                   ACMP3_IRQHandler
ISR_HANDLER2 ACMP4                                // ACMP interrupt                                                   ACMP4_IRQHandler
ISR_RESERVED                                      // Reserved interrupt                                               Reserved143_IRQHandler
ISR_RESERVED                                      // Reserved interrupt                                               Reserved144_IRQHandler
ISR_HANDLER2 ENC1                                 // ENC1 interrupt                                                   ENC1_IRQHandler
ISR_HANDLER2 ENC2                                 // ENC2 interrupt                                                   ENC2_IRQHandler
ISR_HANDLER2 ENC3                                 // ENC3 interrupt                                                   ENC3_IRQHandler
ISR_HANDLER2 ENC4                                 // ENC4 interrupt                                                   ENC4_IRQHandler
ISR_HANDLER2 TMR1                                 // TMR1 interrupt                                                   TMR1_IRQHandler
ISR_HANDLER2 TMR2                                 // TMR2 interrupt                                                   TMR2_IRQHandler
ISR_HANDLER2 TMR3                                 // TMR3 interrupt                                                   TMR3_IRQHandler
ISR_HANDLER2 TMR4                                 // TMR4 interrupt                                                   TMR4_IRQHandler
ISR_HANDLER2 PWM2_0                               // PWM2 capture 0, compare 0, or reload 0 interrupt                 PWM2_0_IRQHandler
ISR_HANDLER2 PWM2_1                               // PWM2 capture 1, compare 1, or reload 0 interrupt                 PWM2_1_IRQHandler
ISR_HANDLER2 PWM2_2                               // PWM2 capture 2, compare 2, or reload 0 interrupt                 PWM2_2_IRQHandler
ISR_HANDLER2 PWM2_3                               // PWM2 capture 3, compare 3, or reload 0 interrupt                 PWM2_3_IRQHandler
ISR_HANDLER2 PWM2_FAULT                           // PWM2 fault or reload error interrupt                             PWM2_FAULT_IRQHandler
ISR_HANDLER2 PWM3_0                               // PWM3 capture 0, compare 0, or reload 0 interrupt                 PWM3_0_IRQHandler
ISR_HANDLER2 PWM3_1                               // PWM3 capture 1, compare 1, or reload 0 interrupt                 PWM3_1_IRQHandler
ISR_HANDLER2 PWM3_2                               // PWM3 capture 2, compare 2, or reload 0 interrupt                 PWM3_2_IRQHandler
ISR_HANDLER2 PWM3_3                               // PWM3 capture 3, compare 3, or reload 0 interrupt                 PWM3_3_IRQHandler
ISR_HANDLER2 PWM3_FAULT                           // PWM3 fault or reload error interrupt                             PWM3_FAULT_IRQHandler
ISR_HANDLER2 PWM4_0                               // PWM4 capture 0, compare 0, or reload 0 interrupt                 PWM4_0_IRQHandler
ISR_HANDLER2 PWM4_1                               // PWM4 capture 1, compare 1, or reload 0 interrupt                 PWM4_1_IRQHandler
ISR_HANDLER2 PWM4_2                               // PWM4 capture 2, compare 2, or reload 0 interrupt                 PWM4_2_IRQHandler
ISR_HANDLER2 PWM4_3                               // PWM4 capture 3, compare 3, or reload 0 interrupt                 PWM4_3_IRQHandler
ISR_HANDLER2 PWM4_FAULT                           // PWM4 fault or reload error interrupt                             PWM4_FAULT_IRQHandler
ISR_RESERVED                                      // Reserved interrupt                                               Reserved168_IRQHandler
ISR_RESERVED                                      // Reserved interrupt                                               Reserved169_IRQHandler
ISR_HANDLER2 CAN3                                 // CAN3 interrupt                                                   CAN3_IRQHandler
ISR_RESERVED                                      // Reserved interrupt                                               Reserved171_IRQHandler
ISR_HANDLER2 FLEXIO3                              // FLEXIO3 interrupt                                                FLEXIO3_IRQHandler
ISR_HANDLER2 GPIO6_7_8_9                          // GPIO6, GPIO7, GPIO8, GPIO9 interrupt                             GPIO6_7_8_9_IRQHandler
ISR_RESERVED                                      // 174                                                              DefaultISR
ISR_RESERVED                                      // 175                                                              DefaultISR
ISR_RESERVED                                      // 176                                                              DefaultISR
ISR_RESERVED                                      // 177                                                              DefaultISR
ISR_RESERVED                                      // 178                                                              DefaultISR
ISR_RESERVED                                      // 179                                                              DefaultISR
ISR_RESERVED                                      // 180                                                              DefaultISR
ISR_RESERVED                                      // 181                                                              DefaultISR
ISR_RESERVED                                      // 182                                                              DefaultISR
ISR_RESERVED                                      // 183                                                              DefaultISR
ISR_RESERVED                                      // 184                                                              DefaultISR
ISR_RESERVED                                      // 185                                                              DefaultISR
ISR_RESERVED                                      // 186                                                              DefaultISR
ISR_RESERVED                                      // 187                                                              DefaultISR
ISR_RESERVED                                      // 188                                                              DefaultISR
ISR_RESERVED                                      // 189                                                              DefaultISR
ISR_RESERVED                                      // 190                                                              DefaultISR
ISR_RESERVED                                      // 191                                                              DefaultISR
ISR_RESERVED                                      // 192                                                              DefaultISR
ISR_RESERVED                                      // 193                                                              DefaultISR
ISR_RESERVED                                      // 194                                                              DefaultISR
ISR_RESERVED                                      // 195                                                              DefaultISR
ISR_RESERVED                                      // 196                                                              DefaultISR
ISR_RESERVED                                      // 197                                                              DefaultISR
ISR_RESERVED                                      // 198                                                              DefaultISR
ISR_RESERVED                                      // 199                                                              DefaultISR
ISR_RESERVED                                      // 200                                                              DefaultISR
ISR_RESERVED                                      // 201                                                              DefaultISR
ISR_RESERVED                                      // 202                                                              DefaultISR
ISR_RESERVED                                      // 203                                                              DefaultISR
ISR_RESERVED                                      // 204                                                              DefaultISR
ISR_RESERVED                                      // 205                                                              DefaultISR
ISR_RESERVED                                      // 206                                                              DefaultISR
ISR_RESERVED                                      // 207                                                              DefaultISR
ISR_RESERVED                                      // 208                                                              DefaultISR
ISR_RESERVED                                      // 209                                                              DefaultISR
ISR_RESERVED                                      // 210                                                              DefaultISR
ISR_RESERVED                                      // 211                                                              DefaultISR
ISR_RESERVED                                      // 212                                                              DefaultISR
ISR_RESERVED                                      // 213                                                              DefaultISR
ISR_RESERVED                                      // 214                                                              DefaultISR
ISR_RESERVED                                      // 215                                                              DefaultISR
ISR_RESERVED                                      // 216                                                              DefaultISR
ISR_RESERVED                                      // 217                                                              DefaultISR
ISR_RESERVED                                      // 218                                                              DefaultISR
ISR_RESERVED                                      // 219                                                              DefaultISR
ISR_RESERVED                                      // 220                                                              DefaultISR
ISR_RESERVED                                      // 221                                                              DefaultISR
ISR_RESERVED                                      // 222                                                              DefaultISR
ISR_RESERVED                                      // 223                                                              DefaultISR
ISR_RESERVED                                      // 224                                                              DefaultISR
ISR_RESERVED                                      // 225                                                              DefaultISR
ISR_RESERVED                                      // 226                                                              DefaultISR
ISR_RESERVED                                      // 227                                                              DefaultISR
ISR_RESERVED                                      // 228                                                              DefaultISR
ISR_RESERVED                                      // 229                                                              DefaultISR
ISR_RESERVED                                      // 230                                                              DefaultISR
ISR_RESERVED                                      // 231                                                              DefaultISR
ISR_RESERVED                                      // 232                                                              DefaultISR
ISR_RESERVED                                      // 233                                                              DefaultISR
ISR_RESERVED                                      // 234                                                              DefaultISR
ISR_RESERVED                                      // 235                                                              DefaultISR
ISR_RESERVED                                      // 236                                                              DefaultISR
ISR_RESERVED                                      // 237                                                              DefaultISR
ISR_RESERVED                                      // 238                                                              DefaultISR
ISR_RESERVED                                      // 239                                                              DefaultISR
ISR_RESERVED                                      // 240                                                              DefaultISR
ISR_RESERVED                                      // 241                                                              DefaultISR
ISR_RESERVED                                      // 242                                                              DefaultISR
ISR_RESERVED                                      // 243                                                              DefaultISR
ISR_RESERVED                                      // 244                                                              DefaultISR
ISR_RESERVED                                      // 245                                                              DefaultISR
ISR_RESERVED                                      // 246                                                              DefaultISR
ISR_RESERVED                                      // 247                                                              DefaultISR
ISR_RESERVED                                      // 248                                                              DefaultISR
ISR_RESERVED                                      // 249                                                              DefaultISR
ISR_RESERVED                                      // 250                                                              DefaultISR
ISR_RESERVED                                      // 251                                                              DefaultISR
ISR_RESERVED                                      // 252                                                              DefaultISR
ISR_RESERVED                                      // 253                                                              DefaultISR
ISR_RESERVED                                      // 254                                                              DefaultISR
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
   Compare Chapter "9.6.1.2 FlexSPI Serial NOR Flash Boot Operation" in the reference manual */
//.section .boot_offset
//  .fill 0, 1, 0

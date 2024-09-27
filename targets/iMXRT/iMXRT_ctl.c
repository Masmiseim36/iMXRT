/*****************************************************************************
 * Copyright (c) 2006, 2009, 2010, 2014 Rowley Associates Limited.           *
 *                                                                           *
 * This file may be distributed under the terms of the License Agreement     *
 * provided with this software.                                              *
 *                                                                           *
 * THIS FILE IS PROVIDED AS IS WITH NO WARRANTY OF ANY KIND, INCLUDING THE   *
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. *
 *
  This file implements the CTL system timer using the Cortex-M
  SysTick timer. The timer is configured to interrupt at a 10 millisecond rate and increment the CTL system timer by 10 to give a 
  millisecond timer. The implementation uses the CMSIS <b>SystemFrequency</b> global variable to determine the CPU clock
  frequency. The CTL samples that are provided in this package have board specific files that implement this variable.</p>

  <p>The CTL interrupt support functions <b>ctl_global_interrupts_set</b>, <b>ctl_set_priority</b>, <b>ctl_unmask_isr</b> 
  and <b>ctl_mask_isr</b> are implemented in this file. The Cortex-M3 and Cortex-M4 implementations uses the lowest half of 
  the available NVIC priorities (top bit set in the priority) for CTL interrupts and disables global interrupts by raising t
  he NVIC basepriority above the highest CTL priority. This enables you to use the upper half of the NVIC priorities for interrupts that do not
  use CTL and should not be disabled by a CTL interrupt. The functions <b>ctl_lowest_isr_priority</b>, <b>ctl_highest_isr_priority</b>,
  and <b>ctl_adjust_isr_priority</b> are provided to assist with setting isr priorities.</p>

 *****************************************************************************/

#include <ctl_api.h>
#include "fsl_device_registers.h"

extern uint32_t SystemCoreClock;

#define ONE_MS (SystemCoreClock/1000)
#define TEN_MS (ONE_MS*10)

static CTL_ISR_FN_t userTimerISR;

void
SysTick_Handler()
{ 
  ctl_enter_isr();
  if (SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)
    {
      ctl_time_increment = (SysTick->LOAD+1)/ONE_MS;     
      userTimerISR();
    }
  ctl_exit_isr();
}

int 
ctl_set_isr(unsigned int irq, 
            unsigned int priority,                 
            CTL_ISR_FN_t isr, 
            CTL_ISR_FN_t *oldisr)
{
  if (oldisr)
    *oldisr = *((CTL_ISR_FN_t*)(SCB->VTOR)+16+irq);
  *((CTL_ISR_FN_t*)(SCB->VTOR)+16+irq) = isr;
  ctl_set_priority(irq, priority);
  return 1;
}

void
ctl_set_priority(int irq, int priority)
{
  NVIC_SetPriority(irq, (1 << (__NVIC_PRIO_BITS - 1)) + priority);
}

int
ctl_unmask_isr(unsigned int irq)
{
  NVIC_EnableIRQ(irq);
  return 1;
}

int
ctl_mask_isr(unsigned int irq)
{
  NVIC_DisableIRQ(irq);
  return 1;
}

int ctl_lowest_isr_priority(void)
{
  return ((1 << (__NVIC_PRIO_BITS - 1)) - 1);
}

int ctl_highest_isr_priority(void)
{
  return 0;
}

int ctl_adjust_isr_priority(int priority, int n)
{
  return priority - n;
}

void ctl_start_timer(CTL_ISR_FN_t timerFn)
{  
  userTimerISR = timerFn;    
  SysTick->LOAD = TEN_MS-1;
  SysTick->VAL = 0;
  SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
  // Set PendSV priority (PendSV must have lowest priority)
  ctl_set_priority(PendSV_IRQn, ctl_lowest_isr_priority());
  // Set SysTick priority
  ctl_set_priority(SysTick_IRQn, ctl_highest_isr_priority());
}

unsigned long
ctl_get_ticks_per_second(void)
{
  return 1000;
}

void ctl_sleep(unsigned delay)
{
  CTL_TIME_t sleep_time;
  unsigned sleep_priority;
  if (delay > 10)
    {
      ctl_global_interrupts_disable();
      sleep_time = ctl_current_time;      
      SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
      if ((ONE_MS * delay) > 0x00FFFFFF || (ONE_MS * delay) < ONE_MS)
        SysTick->LOAD = 0x00FFFFFF;
      else
        SysTick->LOAD = (ONE_MS * delay)-1;
      SysTick->VAL = 0;
      SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
      ctl_interrupt_count = 1;
      sleep_priority = ctl_task_set_priority(ctl_task_executing, 255);
      ctl_interrupt_count = 0;
      ctl_reschedule_on_last_isr_exit = 0;
      ctl_global_interrupts_enable();
    }
  __asm("wfi");
  if (delay > 10)
    {
      ctl_global_interrupts_disable();
      if (ctl_current_time == sleep_time) // something else woke us up and didn't update the time
        {
          if (SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)
            ctl_time_increment = (SysTick->LOAD+1)/ONE_MS;
          else
            ctl_time_increment = (SysTick->LOAD+1-SysTick->VAL)/ONE_MS;
          ctl_increment_tick_from_isr();
          ctl_reschedule_on_last_isr_exit = 0;
        }
      SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;      
      SysTick->LOAD = TEN_MS-1;
      SysTick->VAL = 0;
      SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
      ctl_global_interrupts_enable();
      ctl_task_set_priority(ctl_task_executing, sleep_priority);
    }
}

void ctl_woken()
{  
  if (SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)
    ctl_time_increment = (SysTick->LOAD+1)/ONE_MS;
  else
    ctl_time_increment = (SysTick->LOAD+1-SysTick->VAL)/ONE_MS;  
  ctl_increment_tick_from_isr();
}

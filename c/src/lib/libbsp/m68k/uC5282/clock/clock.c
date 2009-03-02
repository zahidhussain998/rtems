/*
 * Use the last periodic interval timer (PIT3) as the system clock.
 *
 *  Author: W. Eric Norum <norume@aps.anl.gov>
 *
 *  COPYRIGHT (c) 2005.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <rtems.h>
#include <bsp.h>
#include <mcf5282/mcf5282.h>

/*
 * Use INTC0 base
 */
#define CLOCK_VECTOR (64+58)

/*
 * CPU load counters
 * Place in static RAM so updates don't hit the SDRAM
 */
extern int __SRAMBASE[];
#define IDLE_COUNTER      __SRAMBASE[0]
#define FILTERED_IDLE     __SRAMBASE[1]
#define MAX_IDLE_COUNT    __SRAMBASE[2]
#define USEC_PER_TICK     __SRAMBASE[3]
#define FILTER_SHIFT    6

uint32_t bsp_clock_nanoseconds_since_last_tick(void)
{
  uint32_t    tdiff;

  /* Details to consider here:
   *
   *  - PIT is 16-bit. To properly handle differences of two
   *    16-bit numbers (which requires 17-bits) we want to
   *    do the arithmetic in a wider data type. PIT reading
   *    is a unsigned 16-bit count.
   *
   *  => must make sure PIT is a *unsigned* 16-bit type. Otherwise
   *     values get sign-extended when converted to wider type
   *     (regardless of the signedness of wider type):
   *
   *       (unsigned)(signed short)0xffff -> 0xffffffff.
   *
   *     and thus 
   *
   *       (uint32_t)(int16_t)65535 - (uin32_t)(int16_t)1 
   *
   *     would yield 0xfffffffe, not 65534!
   *   
   *
   *  - PIT counts backwards from PMR -> zero, hence
   *
   *      now - tick_base = (PMR - now) - (PMR - tick_base) = tick_base - now;
   * 
   *    result may be negative (if rolled-over).
   *
   *  - PIF flag, counter and PCNTR_AT_TICK must all
   *    be read atomically - otherwise an interrupt may
   *    have altered their values while we're looking.
   *
   *    NOTE: score framework calling this routine disables
   *          interrupts during execution of this callout.
   *
   *  - Last but not least, rollover might have happened
   *    just between reading counter and PIF flag; hence
   *    we have to re-read the counter if PIF is set.
   *
   */

  /* obtain current value */
  tdiff     = (uint16_t)MCF5282_PIT3_PCNTR;

  if (MCF5282_PIT3_PCSR & MCF5282_PIT_PCSR_PIF) {
    /* rollover may just have happened;
     * must reload PCNTR.
     */
    tdiff = + (uint32_t)(uint16_t)MCF5282_PIT3_PMR
            + (uint32_t)(uint16_t)PCNTR_AT_TICK
            - (uint32_t)(uint16_t)MCF5282_PIT3_PCNTR;
  } else {
    tdiff = + (uint32_t)(uint16_t)PCNTR_AT_TICK - tdiff;
  }

  return tdiff * 1000;
}

#define Clock_driver_nanoseconds_since_last_tick \
    bsp_clock_nanoseconds_since_last_tick

/*
 * Periodic interval timer interrupt handler
 */
#define Clock_driver_support_at_tick()                                       \
    do {                                                                     \
        int idle = IDLE_COUNTER;                                             \
        IDLE_COUNTER = 0;                                                    \
        if (idle > MAX_IDLE_COUNT)                                           \
            MAX_IDLE_COUNT = idle;                                           \
        FILTERED_IDLE = idle + FILTERED_IDLE - (FILTERED_IDLE>>FILTER_SHIFT);\
        MCF5282_PIT3_PCSR |= MCF5282_PIT_PCSR_PIF;                           \
    } while (0)

/*
 * Attach clock interrupt handler
 */
#define Clock_driver_support_install_isr( _new, _old )              \
    do {                                                            \
        _old = (rtems_isr_entry)set_vector(_new, CLOCK_VECTOR, 1);  \
    } while(0)

/*
 * Turn off the clock
 */
#define Clock_driver_support_shutdown_hardware()   \
    do {                                           \
        MCF5282_PIT3_PCSR &= ~MCF5282_PIT_PCSR_EN; \
    } while(0)

/*
 * Set up the clock hardware
 *
 * Prescale so that it counts in microseconds
 * System clock frequency better be 2**n (1<=n<=16) MHz!
 */
#define Clock_driver_support_initialize_hardware()                       \
    do {                                                                 \
        int level;                                                       \
        int preScaleCode = -2;                                           \
        int preScaleDivisor = bsp_get_CPU_clock_speed() / 1000000;       \
        while (preScaleDivisor) {                                        \
            preScaleDivisor >>= 1;                                       \
            preScaleCode++;                                              \
        }                                                                \
        IDLE_COUNTER = 0;                                                \
        FILTERED_IDLE = 0;                                               \
        MAX_IDLE_COUNT = 0;                                              \
        bsp_allocate_interrupt(PIT3_IRQ_LEVEL, PIT3_IRQ_PRIORITY);       \
        MCF5282_INTC0_ICR58 = MCF5282_INTC_ICR_IL(PIT3_IRQ_LEVEL) |      \
                              MCF5282_INTC_ICR_IP(PIT3_IRQ_PRIORITY);    \
        rtems_interrupt_disable( level );                                \
        MCF5282_INTC0_IMRH &= ~MCF5282_INTC_IMRH_INT58;                  \
        MCF5282_PIT3_PCSR &= ~MCF5282_PIT_PCSR_EN;                       \
        rtems_interrupt_enable( level );                                 \
        MCF5282_PIT3_PCSR = MCF5282_PIT_PCSR_PRE(preScaleCode) |         \
                            MCF5282_PIT_PCSR_OVW |                       \
                            MCF5282_PIT_PCSR_PIE |                       \
                            MCF5282_PIT_PCSR_RLD;                        \
        USEC_PER_TICK = rtems_configuration_get_microseconds_per_tick(); \
        MCF5282_PIT3_PMR = USEC_PER_TICK - 1;                            \
        MCF5282_PIT3_PCSR = MCF5282_PIT_PCSR_PRE(preScaleCode) |         \
                            MCF5282_PIT_PCSR_PIE |                       \
                            MCF5282_PIT_PCSR_RLD |                       \
                            MCF5282_PIT_PCSR_EN;                         \
    } while (0)

/*
 * Provide our own version of the idle task
 */
void *_BSP_Thread_Idle_body( uintptr_t ignored )
{
    for(;;)
        asm volatile ("addq.l #1,__SRAMBASE"); /* Atomic increment */
}

int rtems_bsp_cpu_load_percentage(void)
{
    return MAX_IDLE_COUNT ?
           (100 - ((100 * (FILTERED_IDLE >> FILTER_SHIFT)) / MAX_IDLE_COUNT)) :
           0;
}

#include "../../../shared/clockdrv_shell.h"

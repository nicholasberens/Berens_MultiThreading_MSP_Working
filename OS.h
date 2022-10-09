#ifndef OS_H_
#define OS_H_

#include "stdio.h"
#include "stdlib.h"
#include "stdint.h"
#include "ti/devices/msp432p4xx/inc/msp.h"
#include "main.h"

typedef void(*thread_t)(void);
typedef unsigned int lock_t;

// fill these depending on your clock
#define TIME_1MS    3000
#define TIME_2MS    6000  						// This is what you should pass as the Timeslice value


						// Function protypes and variable declarations

// ===== OS_Init =====
// Initialize operating system, disable interrupts until OS_Launch
// Initialize OS controlled I/O: SysTick, 3 MHz crystal
extern void OS_Init(void);

// ===== Sets default values in STACK ======
// Inputs: i is the threadnumber
void SetInitialStack(int i);

// ====== OS_AddThread ======
// Add three foreground threads to the scheduler in a Round-Robin fashion
// Inputs: three pointers to a void/void foreground tasks
// Outputs: 1 if successful, 0 if this thread can not be added
int OS_AddThreads(void(*Thread0)(void), void(*Thread1)(void), void(*Thread2)(void));

// ===== OS_Launch ======
// Start the scheduler, Enable interrupts
// Inputs: number of 333.33ms clock cycles that corresponds to 2 ms
//         (maximum of 24 bits)
void OS_Launch(uint32_t theTimeSlice);

// ====== This function (written in assembly) switches to handler mode. (privileged access) =======
extern void yield(void) __attribute__((naked));

extern void SysTick_Handler();

// ===== Setup lock for future use =====
void Lock_Init(lock_t *lock);

// ===== Lock a resource =====
unsigned int Lock_Acquire(lock_t *lock);

// ===== Release specified lock =====
void Lock_Release(lock_t *lock);

// ====== StartCritical ======
// Make a copy of previous I bit, disable interrupts
// Outputs: previous I bit
int32_t StartCritical(void);

// ====== EndCritical ======
// Using the copy of previous I bit, restore I bit to previous value
// Inputs:  previous I bit
void EndCritical(int32_t primask);

#endif

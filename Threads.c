// ===== Include appropriate header files =====
#include "ti/devices/msp432p4xx/inc/msp.h"
#include "OS.h"
#include "Threads.h"
#include "main.h"
#include "UART.h"

int32_t count = 0;
lock_t threadlock;

// These are the user-space threads. Note that they are completely oblivious
// to the technical concerns of the scheduler. The only interface to the
// scheduler is the single function yield() 

// ===== Thread 0 =====
// ===== Responsible for toggling on-board GREEN RGB LED infinitely ====
// ===== Use BIT-BANDING approach to reference the LED =====
// ===== Control taken away pre-emtively by the KERNEL =====
void Thread0(void)
{
  while(1)
  {
	  if(Lock_Acquire(&threadlock)){
	      count++;
	      yield();
	      __delay_cycles(500000);
	      P2->OUT ^= BIT1;
	      Lock_Release(&threadlock);
	      count--;
	  }
	  yield();
  }
  
}


// ===== Thread 1 =====
// ===== Responsible for printing message to the terminal window =====
// ===== Locks need to be implemented for this thread =====
// ===== Control needs to be given up voluntarily by this thread using yield() =====
void Thread1(void)
{
  while(1)
  {
   if(Lock_Acquire(&threadlock))		// Try to acquire lock and if successful
   {
       count++;
    printf("\n\rEntered");
    yield();					// Give up control voluntarily (context switch "interrupt")
    __delay_cycles(500000);
    P1->OUT ^= BIT0;
    printf(" Thread 1 -- pass %d", count);
    Lock_Release(&threadlock);					// Release lock
    count--;
   }
   yield();                		// Give up control voluntarily (context switch "interrupt")
  }
}


// ===== Thread 2 =====
// ===== Responsible for printing message to the terminal window =====
// ===== Locks need to be implemented for this thread =====
// ===== Control needs to be given up voluntarily by this thread using yield() =====
void Thread2(void)
{
  while(1)
  {
   if(Lock_Acquire(&threadlock))		// Try to acquire lock and if successful
   {
       count++;
    printf("\n\rEntered");
    yield();					// Give up control voluntarily (context switch "interrupt")
    __delay_cycles(500000);
    P2->OUT ^= BIT2;
    printf(" Thread 2");
    Lock_Release(&threadlock);			// Release lock
    count--;
   }
   yield();                		// Give up control voluntarily (context switch "interrupt")
  }
}


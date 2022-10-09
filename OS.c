// ===== Include appropriate header files =====
#include "Os.h"
// ===== Function prototypes of functions written in OSasm.asm =====
extern void StartOS();
extern void OS_DisableInterrupts();


#define NUMTHREADS  3             // Maximum number of threads
#define STACKSIZE   200

// ===== Structure to store state of thread =====
struct tcb
{
  int32_t *sp;                      // Pointer to stack (valid for threads not running)
  struct tcb *next;                 // Linked-list pointer
};
typedef struct tcb tcbType;

tcbType tcbs[NUMTHREADS];			// Thread table
tcbType *RunPt;						// Pointer to pointto the next thread
int32_t Stacks[NUMTHREADS][STACKSIZE];	// 2-dimensional arry implemented as STACK


// ===== OS_Init =====
// Initialize operating system, disable interrupts until OS_Launch
// Initialize OS controlled I/O: SysTick, 3 MHz crystal
void OS_Init(void)
{
  OS_DisableInterrupts();
								// Disable SysTick during setup (register approach)
								// Any write to current clears it (register approach)
  SCB -> SHP[1] = (SCB -> SHP[1] & 0x00FFFFFF)|0xE0000000;          // Set priority 7 so that it allows threads to run
}


// ===== Sets default values in STACK ======
// Inputs: i is the threadnumber
void SetInitialStack(int i)
{
  tcbs[i].sp = &Stacks[i][STACKSIZE-16]; // thread stack pointer
  Stacks[i][STACKSIZE-1] = 			 0x01000000;   // XPSR (store appropriate initial value) 	-- Saved by Exception
  Stacks[i][STACKSIZE-3] = 			 0;   // R14 (store appropriate initial value)
  Stacks[i][STACKSIZE-4] = 			 0;   // R12 (store appropriate initial value)
  Stacks[i][STACKSIZE-5] = 			 0;   // R3 (store appropriate initial value)
  Stacks[i][STACKSIZE-6] = 			 0;   // R2 (store appropriate initial value)
  Stacks[i][STACKSIZE-7] = 			 0;   // R1 (store appropriate initial value)
  Stacks[i][STACKSIZE-8] = 			 0;   // R0 (store appropriate initial value)	-- Saved by Exception
  Stacks[i][STACKSIZE-9] = 			 0;   // R11 (store appropriate initial value)	-- Saved by you
  Stacks[i][STACKSIZE-10] = 		 0;   // R10 (store appropriate initial value)
  Stacks[i][STACKSIZE-11] = 		 0;   // R9 (store appropriate initial value)
  Stacks[i][STACKSIZE-12] = 		 0;   // R8 (store appropriate initial value)
  Stacks[i][STACKSIZE-13] = 		 0;   // R7 (store appropriate initial value)
  Stacks[i][STACKSIZE-14] = 		 0;   // R6 (store appropriate initial value)
  Stacks[i][STACKSIZE-15] = 		 0;   // R5 (store appropriate initial value)
  Stacks[i][STACKSIZE-16] = 		 0;   // R4 (store appropriate initial value)	-- Saved by you
}


// ====== OS_AddThread ======
// Add three foreground threads to the scheduler in a Round-Robin fashion
// Inputs: three pointers to a void/void foreground tasks
// Outputs: 1 if successful, 0 if this thread can not be added
int OS_AddThreads(void(*Thread0)(void), void(*Thread1)(void), void(*Thread2)(void))
{
    int32_t status;

    status = StartCritical();                           // Function call to start the critical section

    tcbs[0].next = &tcbs[1];                            // next pointer of Thread 0 points to Thread 1 structure
    tcbs[1].next = &tcbs[2];                            // next pointer of Thread 1 points to Thread 2 structure
    tcbs[2].next = &tcbs[0];                            // next pointer of Thread 2 points to Thread 0 structure
    // For Thread 0:
    SetInitialStack(0);                                 // 1: Set the default values in stack
    Stacks[0][STACKSIZE - 2] = (int32_t)(*Thread0);     // 2: Make ReturnAddress stored on stack to point
    // For Thread 1:
    SetInitialStack(1);                                 // 1: Set the default values in stack
    Stacks[1][STACKSIZE - 2] = (int32_t)(*Thread1);     // 2: Make ReturnAddress stored on stack to point
    // For Thread 2:
    SetInitialStack(2);                                 // 1: Set the default values in stack
    Stacks[2][STACKSIZE - 2] = (int32_t)(*Thread2);     // 2: Make ReturnAddress stored on stack to point


    RunPt = &tcbs[0];       // Make RunPt point to Thread 0 so it will run first
    EndCritical(status);    // Function call to end the critical section

  return 1;               // successful
}

// ===== OS_Launch ======
// Start the scheduler, Enable interrupts
// Inputs: number of 333.33ms clock cycles that corresponds to 2 ms
//         (maximum of 24 bits)
void OS_Launch(uint32_t theTimeSlice)
{
  SysTick->LOAD = theTimeSlice;								 // Load the reload value for Systick Timer
  SysTick->CTRL |= 0x01;								 // Enable the Timer, Use core clock and arm interrupt
  StartOS();                     // start on the first task
}


// ====== StartCritical ======
// Make a copy of previous I bit, disable interrupts
// Outputs: previous I bit
extern int32_t StartCritical(void) __attribute__((naked));
int32_t StartCritical(void)
{
    asm volatile(" MSR    PRIMASK, R0");							// Save old status
	asm volatile(" CPSID  I          ");							// Disable interrupt mechanism in assembly
    asm volatile(" BX     LR         ");							// Return to the calling function
    return 0;
}


// ====== EndCritical ======
// Using the copy of previous I bit, restore I bit to previous value
// Inputs:  previous I bit
extern void EndCritical(int32_t primask) __attribute__((naked));
void EndCritical(int32_t primask)
{
	asm volatile(" MSR    PRIMASK, R0");							// Enable interrupt mechanism in assembly
	asm volatile(" BX     LR         ");							// Return to the calling function
}


// ====== This function (written in assembly) switches to handler mode. (privileged access) =======
void yield(void)
{
    asm volatile(" SVC #00");                            // Gain privileged access

    return;
}


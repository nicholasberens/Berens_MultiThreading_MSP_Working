/* --COPYRIGHT--,BSD
 * Copyright (c) 2017, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
/******************************************************************************
 * MSP432 Empty Project
 *
 * Description: An empty project that uses DriverLib
 *
 *                MSP432P401
 *             ------------------
 *         /|\|                  |
 *          | |                  |
 *          --|RST               |
 *            |                  |
 *            |                  |
 *            |                  |
 *            |                  |
 *            |                  |
 * Author: 
*******************************************************************************/
/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "ti/devices/msp432p4xx/inc/msp.h"

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>

// ===== Include appropriate header files =====
#include "UART.h"
#include "Os.h"
#include "Threads.h"
#include "stdio.h"
#include "stdlib.h"
#include "main.h"
#include "assert.h"

// ===== This is the kernel that will setup everything and run the scheduler ======


#define TIMESLICE               // Thread context switch time in system time units

extern lock_t threadlock;            // Variable Lock

// Code below is the interface to the C standard I/O library
// All the I/O are directed to the console which is UART0
struct __FILE { int handle; };
FILE __stdin = {0};
FILE __stdout = {1};
FILE __stderr = {2};

// ====== Called by C library console / file output =======
int fputc(int c, FILE *f)
{
    int temp;

    temp = UART_Send((char)c);       // Write character to console

  return temp;
}

// ====== For using printf() with format specifiers ======
int fputs(const char *_ptr, register FILE *_fp)
{
    unsigned int len;

    len = UART_MSend(_ptr);     // Send a string

    return len;                 // Return the length of the string
}


// ====== GPIO_Init() =======
void GPIO_Init(void)
{
                // Initialize GPIO periperals needed in this project
    P2->SEL0 &= ~(BIT0 | BIT1 | BIT2);                              //P2.0-2.2 Set as GPIO
    P2->SEL1 &= ~(BIT0 | BIT1 | BIT2);                              //P2.0-2.2 Set as GPIO
    P2->DIR |= (BIT0 | BIT1 | BIT2);                                //P2.0-2.2 Enabled as Outputs for LEDs
    P2->OUT &= ~(BIT0 | BIT1 | BIT2);                               //P2.0-2.2 Initial color set to Black/OFF

    P1->SEL0 &= ~(BIT0);                              //P1.0 Set as GPIO
    P1->SEL1 &= ~(BIT0);                              //P1.0 Set as GPIO
    P1->DIR |= (BIT0);                                //P1.0 Enabled as Outputs for LEDs
    P1->OUT &= ~(BIT0);                               //P1.0 Initial color set to Black/OFF
}


// ======== Function to initialize LOCK (you need to modify) =============
//void Lock_Init(unsigned *lock)
//{
//    ;
//}

//// ======= Function to acquire the lock to be written in LockAcquire.asm ======
//unsigned Lock_Acquire(unsigned *lock)
//{
//  return 1; // always succeeds
//}
//
//// ======== Function to release LOCK (you need to modify) ===========
//void Lock_Release(unsigned *lock)
//{
//    ;
//}


// ======= MAIN =========
int main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;                            // stop watchdog timer
    Lock_Init(&threadlock);                            // Initialize the global thread lock
    OS_Init();                            // Initialize OS
//    UART0_init();                             // Initialize UART0 peripheral
    GPIO_Init();                            // Initialize GPIO peripheral

    int threadSum = 0;
    //CHANGE PER APPLICATION: Add all threads here
    threadSum = OS_AddThreads(Thread0, Thread1, Thread2);
    OS_Launch(TIME_2MS);                            // Launch OS

  return 0;            // This never executes

}

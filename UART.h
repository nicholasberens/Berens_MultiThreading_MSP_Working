#ifndef UART_H_
#define UART_H_

extern void UART0_init(void);
extern int UART_Send(char a);
extern int UART_MSend(char *a);
extern char UART_Receive(void);						// Function prototypes

#endif 

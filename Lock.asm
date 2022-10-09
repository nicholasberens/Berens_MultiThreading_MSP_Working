        .thumb
        .text
        .align 2

        .global threadLock
        .global Lock_Acquire
        .global Lock_Release
        .global Lock_Init

Lock_Acquire:	.asmfunc

		MOV				R1, #1
		LDREX			R2, [R0]
		CMP				R2, R1
		ITT				NE

		STREXNE			R2, R1, [R0]
		CMPNE			R2, #1
		BEQ				lf
		MOV				R0, #1
		BX				LR

lf:
		CLREX
		MOV				R0, #0
		BX				LR

	.endasmfunc

Lock_Init:		.asmfunc

		MOV				R1, #0x0
		STR				R1, [R0]
		BX				LR

		.endasmfunc

Lock_Release:	.asmfunc

		MOV				R1, #0
		STR				R1, [R0]
		BX				LR

	.endasmfunc

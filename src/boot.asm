bits 32
align 4
section .multiboot
	dd 0x1BADB002
	dd 0x07
	dd -(0x1BADB002 + 0x07)
	dd 0
	dd 0
	dd 0
	dd 0
	dd 0
	dd 0
	dd 480
	dd 640
	dd 32

section .stack
stack_bottom:
times 64000 db 0  ; 64kb
stack_top:

section .text

align 16

global start
extern main
extern sse_init
extern fpu_init
start:
	cli
	mov esp, stack_top
	pusha
	call fpu_init
	popa
	mov esp, stack_top ; fixed :3
	push ebx
	push eax
	call main
	cli
	hlt

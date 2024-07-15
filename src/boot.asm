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
times 8000000 db 0  ; 8MB
stack_top:

section .text

align 16

global start
extern main
start:
	cli
	mov esp, stack_top
	push ebx
	push eax
	call main
	cli
	hlt

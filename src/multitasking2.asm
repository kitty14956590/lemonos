bits 32
align 16

[GLOBAL geteip]

geteip:
	pop eax
	jmp eax

global asm_task_switch
asm_task_switch:
	mov ecx, [esp+4]
	mov ebp, [esp+8]
	mov esp, [esp+12]
	mov eax, 0
	sti
	jmp ecx
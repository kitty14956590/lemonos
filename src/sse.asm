global sse_init

sse_init:
	mov eax, 0x1
	cpuid
	test edx, 1 << 25
	jz .exit
	mov eax, cr0
	and ax, 0xFFFB
	or ax, 0x2
	mov cr0, eax
	mov eax, cr4
	or ax, 3 << 9
	mov cr4, eax
.exit:
	ret
void __attribute__((optimize("O0"))) halt() {
	for (;;) {
		asm volatile ("cli\nhlt\n");
	}
}

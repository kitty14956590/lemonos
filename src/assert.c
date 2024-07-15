#include <assert.h>
#include <util.h>

void assert(int expr) {
	if (!expr) {
		halt();
	}
}

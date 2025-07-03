#include <stdio.h>
#include "unwind.h"

int h() {
	unwind();
	return 0;
}

int g() {
	int y = 6;
	h();
	return y;
}

int f() {
	int x = 4;
	return g() + x;
}

int main(void) {
	int xy = f();
	return 0;
}

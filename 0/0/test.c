#include <stdio.h>
#include "vec3.h"

void vec3_print(struct vec3 v) {
	printf("x: %d, y: %d, z: %d\n", v.x, v.y, v.z);
}

int main(void) {
	struct vec3 a = {1, 2, 3};
	struct vec3 b = {4, 5, 6};
	struct vec3 c = {4, 5, 6};

	printf("%d\n", vec3_eq(b, c));
	printf("%d\n", vec3_eq(b, a));

	printf("a + b == "); // expected: 5, 7, 9
	vec3_print(vec3_add(a, b));
	
	printf("a dot b == "); // expected: 32
	printf("%d\n", vec3_dot(a, b));
	
	printf("a * 5 == "); // expected: 5, 10, 15
	vec3_print(vec3_scalar_mul(5, a));
	
	printf("a cross b == "); // -3, 6, 3
	vec3_print(vec3_cross(a, b));
	
	return 0;
}

#include "vec3.h"

int vec3_eq(struct vec3 a, struct vec3 b) {
  return (a.x == b.x && a.y == b.y && a.z == b.z);
}

struct vec3 vec3_add(struct vec3 a, struct vec3 b) {
  return (struct vec3){a.x + b.x, a.y + b.y, a.z + b.z};
}

int vec3_dot(struct vec3 a, struct vec3 b) {
  return (a.x * b.x + a.y * b.y + a.z * b.z);
}

struct vec3 vec3_scalar_mul(int n, struct vec3 v) {
  return (struct vec3){v.x * n, v.y * n, v.z * n};
}

struct vec3 vec3_cross(struct vec3 a, struct vec3 b) {
  struct vec3 c;
  c.x = a.y * b.z - a.z * b.y;
  c.y = a.z * b.x - a.x * b.z;
  c.z = a.x * b.y - a.y * b.x;

  return c;
}

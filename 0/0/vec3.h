struct vec3 {
  int x;
  int y;
  int z;
};

int vec3_eq(struct vec3 a, struct vec3 b);

struct vec3 vec3_add(struct vec3 a, struct vec3 b);

int vec3_dot(struct vec3 a, struct vec3 b);

struct vec3 vec3_scalar_mul(int n, struct vec3 v);

struct vec3 vec3_cross(struct vec3 a, struct vec3 b);

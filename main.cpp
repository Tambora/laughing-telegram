#include <iostream>
#include <bits/stdc++.h>

#include "sphere.h"
#include "hitable_list.h"
#include "float.h"
#include "camera.h"

void fileIO() {
	freopen("input.txt", "r", stdin);
	freopen("output.ppm", "w", stdout);
}


uint32_t xor128(void) {
	static uint32_t x = 123456789;
	static uint32_t y = 362436069;
	static uint32_t z = 521288629;
	static uint32_t w = 88675123;
	uint32_t t;
	t = x ^ (x << 11);
	x = y;
	y = z;
	z = w;
	return w = w ^ (w >> 19) ^ (t ^ (t >> 8));
}

//TODO: add static or const INTMAX === 2147483647
float random0To1() { return (float)(xor128() >> 1) / INT_MAX; }

float InvSqrt(float x) {
	float xhalf = 0.5f * x;
	int i = *(int *)&x;           // store floating-point bits in integer
	i = 0x5f3759df - (i >> 1);    // initial guess for Newton's method
	x = *(float *)&i;             // convert new bits into float
	x = x * (1.5f - xhalf * x * x); // One round of Newton's method
	return x;
}

float Sqrt(float x) { return 1/InvSqrt(x); }


vec3 random_in_unit_sphere() {
	vec3 p;
	do {
		p = 2.0 * vec3(random0To1(), random0To1(), random0To1()) - vec3(1, 1, 1);
	} while (p.squared_Length() >= 1.0);
	return p;
}


vec3 color(const ray &r, hitable *world) {
	hit_record rec;
	if (world->hit(r, 0.001, FLT_MAX, rec)) {
		vec3 target = rec.p + rec.normal + random_in_unit_sphere();
		return 0.5 * color(ray(rec.p, target - rec.p), world);
	} else {
		vec3 unit_direction = unit_vector(r.direction());
		float t = 0.5 * (unit_direction.y() + 1.0);
		return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
	}
}

int main(int argc, char const *argv[]) {
	fileIO();
	int nx = 200;
	int ny = 100;
	int ns = 100;
	std::cout << "P3\n" << nx << " " << ny << "\n255\n";
	hitable *list[2];
	list[0] = new sphere(vec3(0, 0, -1), 0.5);
	list[1] = new sphere(vec3(0, -100.5, -1), 100);
	hitable *world = new hitable_list(list, 2);
	camera cam;
	for (int j = ny - 1; j >= 0; j--) {
		for (int i = 0; i < nx; i++) {
			vec3 col(0, 0, 0);

			for (int s = 0; s < ns; s++) {
				float u = float(i + random0To1()) / float(nx);
				float v = float(j + random0To1()) / float(ny);
				ray r = cam.get_ray(u, v);
				vec3 p = r.point_at_parameter(2.0);
				col += color(r, world);
			}

			col /= float(ns);
			col = vec3(Sqrt(col[0]), Sqrt(col[1]), Sqrt(col[2]));
			int ir = int(255.99 * col[0]);
			int ig = int(255.99 * col[1]);
			int ib = int(255.99 * col[2]);

			std::cout << ir << " " << ig << " " << ib << "\n";
		}
	}
	return 0;
}

// x*x + 0.2*(x*x*x - x*x)
// 0.2*x*x + 0.8*x*x*x

#include <iostream>
#include <bits/stdc++.h>

#include "sphere.h"
#include "hitable_list.h"
#include "float.h"
#include "camera.h"
#include "material.h"

void fileIO() {
	freopen("input.txt", "r", stdin);
	freopen("output.ppm", "w", stdout);
}

float InvSqrt(float x) {
	float xhalf = 0.5f * x;
	int i = *(int *)&x;				// store floating-point bits in integer
	i = 0x5f3759df - (i >> 1);		// initial guess for Newton's method
	x = *(float *)&i;				// convert new bits into float
	x = x * (1.5f - xhalf * x * x); // One round of Newton's method
	return x;
}

float Sqrt(float x) {
	return 1 / InvSqrt(x);
}


vec3 color(const ray &r, hitable *world, int depth) {
	hit_record rec;
	if (world->hit(r, 0.001, FLT_MAX, rec)) {
		ray scattered;
		vec3 attenuation;
		if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
			return attenuation * color(scattered, world, depth + 1);
		} else {
			return vec3(0.2, 0.2, 1);
		}
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
	list[0] = new sphere(vec3(0, 0, -1), 0.5, new lambert(vec3(0.8, 0.3, 0.3)));
	list[1] = new sphere(vec3(0, -100.5, -1), 100, new lambert(vec3(0.8, 0.8, 0.0)));
	hitable *world = new hitable_list(list, 2);
	camera cam;
	for (int j = ny - 1; j >= 0; j--) {
		for (int i = 0; i < nx; i++) {
			vec3 col(0, 0, 0);

			for (int s = 0; s < ns; s++) {
				float u = float(i + random()) / float(nx);
				float v = float(j + random()) / float(ny);
				ray r = cam.get_ray(u, v);
				vec3 p = r.point_at_parameter(2.0);
				col += color(r, world, 0);
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

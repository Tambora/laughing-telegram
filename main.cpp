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

hitable *random_scene() {
	int n = 500;
	hitable **list = new hitable*[n+1];
	list[0] = new sphere(vec3(0,-1000,0), 1000, new lambert(vec3(0.5, 0.5, 0.5)));
	int i = 1;
	for(int a = -11; a < 11; a++) {
		for(int b = -11; b < 11; b++) {
			float choose_mat = random();
			vec3 center(a+0.9 * random(), 0.2, b+0.9*random());
			if ((center-vec3(4,0.2,0)).length() > 0.9) {
				if(choose_mat < 0.8) {
					list[i++] = new sphere(center, 0.2, new lambert(vec3(random()*random(), random()*random(), random()*random())));
				} else if (choose_mat < 0.95) {
					list[i++] = new sphere(center, 0.2, new metal(vec3(0.5*(1 + random()), 0.5*(1 + random()), 0.5*(1 + random())),  0.5*random()));
				} else {
					list[i++] = new sphere(center, 0.2, new dielectric(1.5));
				}
			}
		}
	}
	list[i++] = new sphere(vec3(0,1,0), 1.0, new dielectric(1.5));
	list[i++] = new sphere(vec3(-4, 1, 0), 1.0, new lambert(vec3(0.4, 0.2, 0.1)));
	list[i++] = new sphere(vec3(4, 1, 0), 1.0, new metal(vec3(0.7, 0.6, 0.5), 0.0));

	return new hitable_list(list, i);
}

int main(int argc, char const *argv[]) {
	fileIO();
	int nx = 128;
	int ny = 72;
	int ns = 50;
	std::cout << "P3\n" << nx << " " << ny << "\n255\n";
	// hitable *list[5];
	// list[0] = new sphere(vec3(0, 0, -1), 0.5, new lambert(vec3(0.8, 0.3, 0.3)));
	// list[1] = new sphere(vec3(0, -100.5, -1), 100, new lambert(vec3(0.8, 0.8, 0.0)));
	// list[2] = new sphere(vec3(1, -0, -1), 0.5, new metal(vec3(0.8, 0.6, 0.2),0.0));
	// list[3] = new sphere(vec3(-1, 0, -1), 0.5, new dielectric(1.5));
	// list[4] = new sphere(vec3(-1, 0, -1), -0.475, new dielectric(1.5));
	hitable *world;// = new hitable_list(list, 5);
	world = random_scene();
    vec3 lookfrom(13,2,3);
    vec3 lookat(0,0,0);
	float dist_to_focus = 10;
	float aperture = 0.1;
	camera cam(lookfrom, lookat, vec3(0,1,0), 20, float(nx)/float(ny), aperture, dist_to_focus);
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
			col = vec3(almostSqrt(col[0]), almostSqrt(col[1]), almostSqrt(col[2]));
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

#include <iostream>
#include <stdio.h>
#include <string.h>
#include "sphere.h"
#include "hitable_list.h"
#include "float.h"
#include "bvh.h"
#include "camera.h"
#include "material.h"
#include "bitmap.h"



vec3 color(const ray &r, hitable *world, int depth) {
	hit_record rec;
	if (world->hit(r, 0.001, FLT_MAX, rec)) {
		ray scattered;
		vec3 attenuation;  
		if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
			return attenuation * color(scattered, world, depth + 1);
		} else {
			return vec3(0,0,0);
		}
	} else {
		vec3 unit_direction = unit_vector(r.direction());
		float t = 0.5 * (unit_direction.y() + 1.0);
        return (1.0-t)*vec3(1.0, 1.0, 1.0) + t*vec3(0.5, 0.7, 1.0);
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

	return new bvh_node(list,i);
}

int main(int argc, char const *argv[]) {

    int w = 320; // width
    int h = 180; // height
	int ns = 50; // samples
    int red[w][h]; 
    int green[w][h];
    int blue[w][h];

    unsigned char img[h][w][bytesPerPixel];

	hitable *world;
	world = random_scene();
    vec3 lookfrom(13,2,3);
    vec3 lookat(0,0,0);
	float dist_to_focus = 10;
	float aperture = 0.1;
	camera cam(lookfrom, lookat, vec3(0,1,0), 20, float(w)/float(h), aperture, dist_to_focus);
	for (int j = h - 1; j >= 0; j--) {
		for (int i = 0; i < w; i++) {
			vec3 col(0, 0, 0);

			for (int s = 0; s < ns; s++) {
				float u = float(i + random()) / float(w);
				float v = float(j + random()) / float(h);
				ray r = cam.get_ray(u, v);
				vec3 p = r.point_at_parameter(2.0);
				col += color(r, world, 0);
			}

			col /= float(ns);
			col = vec3(almostSqrt(col[0]), almostSqrt(col[1]), almostSqrt(col[2]));
			img[j][i][2] = (unsigned char)(int)(255 * col[0]);
			img[j][i][1] = (unsigned char)(int)(255 * col[1]);
			img[j][i][0] = (unsigned char)(int)(255 * col[2]);
		}
	}

	generateBitmapImage((unsigned char *)img, h, w, bytesPerPixel*w, "img.bmp");
	return 0;
}

// x*x + 0.2*(x*x*x - x*x)
// 0.2*x*x + 0.8*x*x*x

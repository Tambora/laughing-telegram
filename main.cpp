#include <iostream>
#include <stdio.h>
#include <string.h>
#include "sphere.h"
#include "hitable_list.h"
#include "bvh.h"
#include "camera.h"
#include "material.h"
#include "bitmap.h"
#include "rect.h"
#include "box.h"
#include "constant_medium.h"
#include "pdf.h"
#include "float.h"

inline vec3 de_nan(const vec3 &c) {
	vec3 temp = c;
	if (!(temp[0] == temp[0])) temp[0] = 0;
	if (!(temp[1] == temp[1])) temp[1] = 0;
	if (!(temp[2] == temp[2])) temp[2] = 0;
	return temp;
}

vec3 color(const ray &r, hitable *world, hitable *light_shape, int depth) {
	hit_record hrec;
	if (world->hit(r, 0.001, FLT_MAX, hrec)) {
		scatter_record srec;
		vec3 emitted = hrec.mat_ptr->emitted(r, hrec, hrec.u, hrec.v, hrec.p);
		if (depth < 50 && hrec.mat_ptr->scatter(r, hrec, srec)) {
			if (srec.is_specular) {
				return srec.attenuation * color(srec.specular_ray, world, light_shape, depth + 1);
			} else {
				hitable_pdf plight(light_shape, hrec.p);
				mixture_pdf p(&plight, srec.pdf_ptr);
				ray scattered = ray(hrec.p, p.generate());
				float pdf_val = p.value(scattered.direction());
				delete srec.pdf_ptr; // FIXME: better mem care
				return emitted + srec.attenuation * hrec.mat_ptr->scattering_pdf(r, hrec, scattered) * color(scattered, world, light_shape, depth + 1) / pdf_val;
			}
		} else
			return emitted;
	} else
		return vec3(0, 0, 0); // black sky
}

void cornell_box(hitable **scene, camera **cam, float aspect) {
	int i = 0;
	hitable **list = new hitable*[8];
	material *red  	= new lambert (new solid_texture(vec3(0.65, 0.05, 0.05)));
	material *white	= new lambert (new solid_texture(vec3(0.73, 0.73, 0.73)));
	material *green	= new lambert (new solid_texture(vec3(0.12, 0.45, 0.15)));
	material *light	= new diffuse_light (new solid_texture(vec3(15, 15, 15)));
	material *aluminium = new metal(vec3(0.8,0.85,0.88),0.0);
	material *glass = new dielectric(1.5);

	list[i++] = new flip_normals(new xz_rect(213, 343, 227, 332, 554, light));

	list[i++] = new flip_normals(new yz_rect(0, 555, 0, 555, 555, green));
	list[i++] = new yz_rect(0, 555, 0, 555, 0, red);
	list[i++] = new flip_normals(new xz_rect(0, 555, 0, 555, 555, white));
	list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
	list[i++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, white));

	list[i++] = new sphere(vec3(190, 90, 190),90 , glass);
//	list[i++] = new translate(new rotate_y(new box(vec3(0, 0, 0), vec3(165, 165, 165), white), -18), vec3(130, 0, 65));
	list[i++] = new translate(new rotate_y(new box(vec3(0, 0, 0), vec3(165, 330, 165), white),  15), vec3(265, 0, 295));

	*scene = new hitable_list(list, i);
	// TODO: move the light sample list here
	vec3 lookfrom(278, 278, -800);
	vec3 lookat(278, 278, 0);
	float dist_to_focus = 10.0;
	float aperture = 0.0;
	float vfov = 40;
	*cam = new camera(lookfrom, lookat, vec3(0, 1, 0), vfov, aspect, aperture, dist_to_focus);
}

// void trace() {
// 	do {
// 		int y = nextJob++
// 	}
// }


int main(int argc, char const *argv[]) {

	int w = 720; // width
	int h = 720; // height
	int ns = 5; // samples

	float aspect = float(h) / float(w);

	hitable *world;
	camera *cam;

	//random_scene();
	cornell_box(&world, &cam, aspect);
	hitable *light_shape = new xz_rect(213, 343, 227, 332, 554, 0);
	hitable *glass_sphere = new sphere(vec3(190,90,190), 90, 0);
	hitable *a[2];
	a[0] = light_shape;
	a[1] = glass_sphere;
	hitable_list hlist(a,2);

	unsigned char img[h][w][bytesPerPixel];

	float oneOverW = 1.0f/w;
	float oneOverH = 1.0f/h;

	for (int j = h - 1; j >= 0; j--) {
		for (int i = 0; i < w; i++) {
			vec3 col(0, 0, 0);

			for (int s = 0; s < ns; s++) {
				float u = float(i + random0to1()) * oneOverW;
				float v = float(j + random0to1()) * oneOverH;
				ray r = cam->get_ray(u, v);
				vec3 p = r.point_at_parameter(2.0);
				col += de_nan(color(r, world, &hlist, 0));
			}

			col /= float(ns);
			col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
			col[0] = col[0] > 1 ? 1 : col[0];
			col[1] = col[1] > 1 ? 1 : col[1];
			col[2] = col[2] > 1 ? 1 : col[2];
			img[j][i][2] = (unsigned char)(int)(col[0] * 255);
			img[j][i][1] = (unsigned char)(int)(col[1] * 255);
			img[j][i][0] = (unsigned char)(int)(col[2] * 255);
		}
	}

	generateBitmapImage((unsigned char *)img, h, w, bytesPerPixel * w, "img.bmp");
	free(img);
	return 0;
}

// x*x + 0.2*(x*x*x - x*x)
// 0.2*x*x + 0.8*x*x*x

//int intensity (double x)
// {
//     if ( x > 0.0 )
//         return (x >= 1.0) ? 255 : std::min(int(256.0 * x), 255)
//     else
//         return 0;
// }
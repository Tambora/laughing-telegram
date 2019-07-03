#include <iostream>
#include <stdio.h>
#include <string.h>
#include "sphere.h"
#include "hitable_list.h"
#include "float.h"
#include "camera.h"
#include "material.h"

const int bytesPerPixel = 3; /// red, green, blue
const int fileHeaderSize = 14;
const int infoHeaderSize = 40;

void generateBitmapImage(unsigned char *image, int height, int width, int pitch, const char* imageFileName);
unsigned char* createBitmapFileHeader(int height, int width, int pitch, int paddingSize);
unsigned char* createBitmapInfoHeader(int height, int width);


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

	return new hitable_list(list, i);
}

int main(int argc, char const *argv[]) {
	int nx = 320;
	int ny = 180;
	int ns = 10;


    int w = nx; /* Put here what ever width you want */
    int h = ny; /* Put here what ever height you want */
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
			img[j][i][2] = (unsigned char)(int)(255 * col[0]);
			img[j][i][1] = (unsigned char)(int)(255 * col[1]);
			img[j][i][0] = (unsigned char)(int)(255 * col[2]);


		}
	}

	generateBitmapImage((unsigned char *)img, h, w, bytesPerPixel*w, "img.bmp");

	return 0;
}

void generateBitmapImage(unsigned char *image, int height, int width, int pitch, const char* imageFileName) {

    unsigned char padding[3] = { 0, 0, 0 };
    int paddingSize = (4 - (/*width*bytesPerPixel*/ pitch) % 4) % 4;

    unsigned char* fileHeader = createBitmapFileHeader(height, width, pitch, paddingSize);
    unsigned char* infoHeader = createBitmapInfoHeader(height, width);
    FILE* imageFile = fopen(imageFileName, "wb");

    fwrite(fileHeader, 1, fileHeaderSize, imageFile);
    fwrite(infoHeader, 1, infoHeaderSize, imageFile);

    int i;
    for (i = 0; i < height; i++) {
        fwrite(image + (i*pitch /*width*bytesPerPixel*/), bytesPerPixel, width, imageFile);
        fwrite(padding, 1, paddingSize, imageFile);
    }

    fclose(imageFile);
}

unsigned char* createBitmapFileHeader(int height, int width, int pitch, int paddingSize) {
    int fileSize = fileHeaderSize + infoHeaderSize + (/*bytesPerPixel*width*/pitch + paddingSize) * height;

    static unsigned char fileHeader[] = {
        0,0, /// signature
        0,0,0,0, /// image file size in bytes
        0,0,0,0, /// reserved
        0,0,0,0, /// start of pixel array
    };

    fileHeader[0] = (unsigned char)('B');
    fileHeader[1] = (unsigned char)('M');
    fileHeader[2] = (unsigned char)(fileSize);
    fileHeader[3] = (unsigned char)(fileSize >> 8);
    fileHeader[4] = (unsigned char)(fileSize >> 16);
    fileHeader[5] = (unsigned char)(fileSize >> 24);
    fileHeader[10] = (unsigned char)(fileHeaderSize + infoHeaderSize);

    return fileHeader;
}

unsigned char* createBitmapInfoHeader(int height, int width) {
    static unsigned char infoHeader[] = {
        0,0,0,0, /// header size
        0,0,0,0, /// image width
        0,0,0,0, /// image height
        0,0, /// number of color planes
        0,0, /// bits per pixel
        0,0,0,0, /// compression
        0,0,0,0, /// image size
        0,0,0,0, /// horizontal resolution
        0,0,0,0, /// vertical resolution
        0,0,0,0, /// colors in color table
        0,0,0,0, /// important color count
    };

    infoHeader[0] = (unsigned char)(infoHeaderSize);
    infoHeader[4] = (unsigned char)(width);
    infoHeader[5] = (unsigned char)(width >> 8);
    infoHeader[6] = (unsigned char)(width >> 16);
    infoHeader[7] = (unsigned char)(width >> 24);
    infoHeader[8] = (unsigned char)(height);
    infoHeader[9] = (unsigned char)(height >> 8);
    infoHeader[10] = (unsigned char)(height >> 16);
    infoHeader[11] = (unsigned char)(height >> 24);
    infoHeader[12] = (unsigned char)(1);
    infoHeader[14] = (unsigned char)(bytesPerPixel * 8);

    return infoHeader;
}
	//std::cout << "P3\n" << nx << " " << ny << "\n255\n";
	//std::cout << ir << " " << ig << " " << ib << "\n";

// x*x + 0.2*(x*x*x - x*x)
// 0.2*x*x + 0.8*x*x*x

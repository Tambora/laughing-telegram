#ifndef TEXTUREH
#define TEXTUREH

#include "perlin.h"

class texture {
  public:
	virtual vec3 value(float u, float v, const vec3 &p) const = 0;
};

class solid_texture : public texture {
  public:
	solid_texture() {}
	solid_texture(vec3 c) : color(c) {}
	virtual vec3 value(float u, float v, const vec3 &p) const {
		return color;
	}
	vec3 color;
};

class checkered_texture : public texture {
  public:
	checkered_texture() {}
	checkered_texture(texture *t0, texture *t1) : even(t0), odd(t1) {}
	virtual vec3 value(float u, float v, const vec3 &p) const {
		float sines = sin(10 * p.x()) * sin(10 * p.y()) * sin(10 * p.z());
		if (sines < 0)
			return odd->value(u, v, p);
		else
			return even->value(u, v, p);
	}
	texture *odd;
	texture *even;
};

class noise_texture : public texture {
  public:
	noise_texture() {}
	noise_texture(float s) : scale(s) {}
	virtual vec3 value(float u, float v, const vec3 &p) const { //
//            return vec3(1,1,1)*0.5*(1 + noise.noise(scale * p));
//            return vec3(1,1,1)*noise.turb(scale * p);
              return vec3(1,1,1)*0.5*(1 + sin(scale*p.z() + 8*noise.turb(scale*p))) ;
	}
	perlin noise; //o//
	float scale;
};

#endif
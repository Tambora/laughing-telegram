#ifndef MATERIAL_H
#define MATERIAL_H

struct hit_record;

#include "ray.h"
#include "hitable.h"

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

float random() {
	return ((float) xor128()) / UINT_MAX;
}

vec3 random_in_unit_sphere() {
	vec3 p;
	do {
		p = 2.0 * vec3(random(), random(), random()) - vec3(1, 1, 1);
	} while (p.squared_Length() >= 1.0);
	return p;
}

vec3 reflect(const vec3 &v, const vec3 &n) {
	return v - 2 * dot(v, n) * n;
}

class material  {
  public:
	virtual bool scatter(const ray &r_in, const hit_record &rec, vec3 &attenuation, ray &scattered) const = 0;
};

class lambert : public material {
  public:
	lambert(const vec3 &a) : albedo(a) {}
	virtual bool scatter(const ray &r_in, const hit_record &rec, vec3 &attenuation, ray &scattered) const  {
		vec3 target = rec.p + rec.normal + random_in_unit_sphere();
		scattered = ray(rec.p, target - rec.p);
		attenuation = albedo;
		return true;
	}

	vec3 albedo;
};

class metal : public material {
  public:
	metal(const vec3 &a, float f) : albedo(a) {
		if (f < 1) fuzz = f;
		else fuzz = 1;
	}
	virtual bool scatter(const ray &r_in, const hit_record &rec, vec3 &attenuation, ray &scattered) const  {
		vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
		scattered = ray(rec.p, reflected + fuzz * random_in_unit_sphere());
		attenuation = albedo;
		return (dot(scattered.direction(), rec.normal) > 0);
	}
	vec3 albedo;
	float fuzz;
};

#endif

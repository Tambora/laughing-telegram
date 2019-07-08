#ifndef MATERIAL_H
#define MATERIAL_H

#include "ray.h"
#include "hitable.h"
#include "texture.h"


struct hit_record;

inline float schlick(float c, float ref_idx) {
	float r0 = (1-ref_idx) / (1+ref_idx);
	r0 *= r0;
	c = 1-c;
	float cc = c*c;
	return r0 + (1-r0) * c * cc * cc;
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

bool refract(const vec3 &v, const vec3 &n, float ni_over_nt, vec3 &refracted) {
	vec3 uv = unit_vector(v);
	float dt = dot(uv, n);
	float discriminant = 1.0 - ni_over_nt * ni_over_nt * (1-dt*dt);

	if(discriminant > 0) {
		refracted = ni_over_nt*(uv - n*dt) - n*almostSqrt(discriminant);
		return true;
	} else 
		return false;
}

class material  {
  public:
	virtual bool scatter(const ray &r_in, const hit_record &rec, vec3 &attenuation, ray &scattered) const = 0;
};

class lambert : public material {
  public:
	lambert(texture *a) : albedo(a) {}

	virtual bool scatter(const ray &r_in, const hit_record &rec, vec3 &attenuation, ray &scattered) const  {
		vec3 target = rec.p + rec.normal + random_in_unit_sphere();
		scattered = ray(rec.p, target - rec.p);
		attenuation = albedo -> value(0,0,rec.p);
		return true;
	}
	texture *albedo;
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

class dielectric : public material {
  public:
	dielectric(float ri) : ref_idx(ri) {}

	virtual bool scatter(const ray &r_in, const hit_record &rec, vec3 &attenuation, ray &scattered) const  {
		vec3 outward_normal;
		vec3 reflected = reflect(r_in.direction(), rec.normal);
		float ni_over_nt;
		attenuation = vec3(1.0, 1.0, 1.0);
		vec3 refracted;
		float reflect_prob;
		float cos;

		if(dot(r_in.direction(), rec.normal) > 0) { 
			outward_normal = -rec.normal;
			ni_over_nt = ref_idx;
			cos = dot(r_in.direction(), rec.normal) / r_in.direction().length(); // len = 1???
			cos = almostSqrt(1-ref_idx*ref_idx * (1-cos*cos));
		} else {
			outward_normal = rec.normal;
			ni_over_nt = 1.0 / ref_idx;
			cos = -dot(r_in.direction(), rec.normal) / r_in.direction().length();
		}

		if(refract(r_in.direction(), outward_normal, ni_over_nt, refracted)) {
			reflect_prob = schlick(cos, ref_idx);
		} else {
			reflect_prob = 1.0;
		}

		if(random() < reflect_prob) {
			scattered = ray(rec.p, reflected);
		} else {
			scattered = ray(rec.p, refracted);
		}

		return true;
	}

	float ref_idx;
};

#endif
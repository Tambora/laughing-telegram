#ifndef AABBH
#define AABBH

#include "ray.h"

inline float ffmin(float a, float b) {
	return a < b ? a : b;
}
inline float ffmax(float a, float b) {
	return a > b ? a : b;
}

class aabb {
  public:
	aabb() {};
	aabb(const vec3 &a, const vec3 &b) {
		_min = a;
		_max = b;
	}
	bool hit(const ray &r, float tmin, float tmax) const;
	vec3 min() const {
		return _min;
	}
	vec3 max() const {
		return _max;
	}
	vec3 _min;
	vec3 _max;
};

// inline bool aabb::hit(const ray &r, float tmin, float tmax) const { // replace loops with vec3
// 	vec3 invD = 1.0f / r.direction(); //rcp()
// 	vec3 t0 = (min() - r.origin()) * invD;
// 	vec3 t1 = (max() - r.origin()) * invD;

// 	vec3 tmin = min(t0, t1);
// 	vec3 tmax = max(t0, t1);
// 	if (invD < 0.0f)
// 		std::swap(t0,t1);
// 	tmin = t0 > tmin ? t0 : tmin;
// 	tmax = t1 < tmax ? t1 : tmax;
// 	if (tmax <= tmin) {
// 		return false;
// 	}
// 	return true;
// }

// bool slabs(vec3 p0, vec3 p1, vec3 rayOrigin, vec3 invRaydir) {

//   vec3 t0 = (p0 - rayOrigin) * invRaydir;
//   vec3 t1 = (p1 - rayOrigin) * invRaydir;
//   vec3 tmin = min(t0,t1), tmax = max(t0,t1);

//   return max_component(tmin) <= min_component(tmax);
// }


inline bool aabb::hit(const ray &r, float tmin, float tmax) const { // replace loops with vec3
																	// TODO: tmin and tmax?
	float t0, t1, invD;
	for (int i = 0; i < 3; i++) {
	invD = 1.0f / r.direction()[i]; //rcp()
		if (invD >= 0.0f) {
			t0 = (min()[i] - r.origin()[i]) * invD;
			t1 = (max()[i] - r.origin()[i]) * invD;
		} else {
			t1 = (min()[i] - r.origin()[i]) * invD;
			t0 = (max()[i] - r.origin()[i]) * invD;
		}
		tmin = t0 > tmin ? t0 : tmin;
		tmax = t1 < tmax ? t1 : tmax;
		if (tmax <= tmin) {
			return false;
		}
	}
	return true;
}


aabb surrounding_box(aabb box0, aabb box1) { // TODO: check if fmin/fmax -> ffmin/ffmax makes a difference
	vec3 small( fmin(box0.min().x(), box1.min().x()),
	            fmin(box0.min().y(), box1.min().y()),
	            fmin(box0.min().z(), box1.min().z()));
	vec3 big  ( fmax(box0.max().x(), box1.max().x()),
	            fmax(box0.max().y(), box1.max().y()),
	            fmax(box0.max().z(), box1.max().z()));
	return aabb(small, big);
}


#endif // AABBH



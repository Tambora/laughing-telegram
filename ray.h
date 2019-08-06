#ifndef RAYH
#define RAYH
#include "vec3.h"

class ray {
  public:
	ray() {}
	ray(const vec3 &a, const vec3 &b) {
		A = a;
		B = b;
		invD = vec3(1/b.x(), 1/b.y(), 1/b.z());
		sign[0] = (invD.x() <= 0);
		sign[1] = (invD.y() <= 0);
		sign[2] = (invD.z() <= 0);
	}
	vec3 origin() const {
		return A;
	}
	vec3 direction() const {
		return B;
	}
	vec3 point_at_parameter (float t) const {
		return A + t * B;
	}

	vec3 A;
	vec3 B;
	vec3 invD;
	bool sign[3];
};

#endif
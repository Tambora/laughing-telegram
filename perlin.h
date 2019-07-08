#ifndef PERLINH
#define PERLINH

inline float perlin_interp(vec3 c[2][2][2], float u, float v, float w) {
	float uu = u * u * (3 - 2 * u);
	float vv = v * v * (3 - 2 * v);
	float ww = w * w * (3 - 2 * w);
	float accum = 0;
	for (int i = 0; i < 2; i++){
		for (int j = 0; j < 2; j++){
			for (int k = 0; k < 2; k++) {
				vec3 weight_v(u - i, v - j, w - k);
				accum += (i * uu + (1 - i) * (1 - uu)) *
				         (j * vv + (1 - j) * (1 - vv)) *
				         (k * ww + (1 - k) * (1 - ww)) * dot(c[i][j][k], weight_v);
			}
		}
	}
	return (accum + 1.0f)*0.5f;
}

class perlin {
  public:
	float noise (const vec3 &p) const {
		int i = floor(p.x());
		int j = floor(p.y());
		int k = floor(p.z());
		float u = p.x() - i;
		float v = p.y() - j;
		float w = p.z() - k;
		vec3 c[2][2][2];
		for (int di = 0; di < 2; di++){
			for (int dj = 0; dj < 2; dj++){
				for (int dk = 0; dk < 2; dk++){
					c[di][dj][dk] = ranvec[perm_x[(i + di) & 255] ^ perm_y[(j + dj) & 255] ^ perm_z[(k + dk) & 255]];
				}
			}
		}
		return perlin_interp(c, u, v, w);
	}
	static vec3 *ranvec;
	static int *perm_x;
	static int *perm_y;
	static int *perm_z;
};

static vec3 *perlin_generate() {
	vec3 *p = new vec3[256];
	for (int i = 0; i < 256; i++)
		p[i] = unit_vector(vec3(2*random()-1, 2*random()-1, 2*random()-1));
	return p;
}

void permute(int *p, int n) {
	for (int i = n - 1; i > 0; i--) {
		int target = int(random() * (i + 1));
		int tmp = p[i];
		p[i] = p[target];
		p[target] = tmp;
	}
	return;
}

static int *perlin_generate_perm() {
	int *p = new int[256];
	for (int i = 0; i < 256; i++)
		p[i] = i;
	permute(p, 256);
	return p;
}
vec3 *perlin::ranvec = perlin_generate();
int *perlin::perm_x = perlin_generate_perm();
int *perlin::perm_y = perlin_generate_perm();
int *perlin::perm_z = perlin_generate_perm();

#endif
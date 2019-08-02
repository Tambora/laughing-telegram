#ifndef BVHH
#define BVHH

#include "hitable.h"

class bvh_node : public hitable {
  public:
	bvh_node() {}
	bvh_node(hitable **l, int n);
	virtual bool hit (const ray &r, float tmin, float tmax, hit_record &rec) const;
	virtual bool bounding_box (aabb &box) const;
	hitable *left;
	hitable *right;
	aabb box;
};


bool bvh_node::hit(const ray& r, float tmin, float tmax, hit_record& rec) const {
	if (box.hit(r, tmin, tmax)) {
		if (left->hit(r, tmin, tmax, rec)) {
			right->hit(r, tmin, rec.t, rec);
			return true;
		} else {
			return right->hit(r, tmin, tmax, rec);
		}
	} 
	return false;
}

bool bvh_node::bounding_box(aabb &b) const {
	b = box;
	return true;
}

int box_x_compare (const void *a, const void *b) {
	aabb box_left, box_right;
	hitable *ah = *(hitable **)a;
	hitable *bh = *(hitable **)b;
	if(!ah->bounding_box(box_left) || !bh->bounding_box(box_right))
		std::cerr << "No bounding box in bvh_node constructor.\n";
	return (box_left.min().x() - box_right.min().x() < 0.0) ?  -1 : 1;
}

int box_y_compare (const void *a, const void *b) {
	aabb box_left, box_right;
	hitable *ah = *(hitable **)a;
	hitable *bh = *(hitable **)b;
	if(!ah->bounding_box(box_left) || !bh->bounding_box(box_right))
		std::cerr << "No bounding box in bvh_node constructor.\n";
	return (box_left.min().y() - box_right.min().y() < 0.0) ?  -1 : 1;
}
int box_z_compare (const void *a, const void *b) {
	aabb box_left, box_right;
	hitable *ah = *(hitable **)a;
	hitable *bh = *(hitable **)b;
	if(!ah->bounding_box(box_left) || !bh->bounding_box(box_right))
		std::cerr << "No bounding box in bvh_node constructor.\n";
	return (box_left.min().z() - box_right.min().z() < 0.0) ?  -1 : 1;
}


bvh_node::bvh_node(hitable **l, int n) { // TODO: "this method will be optimized later"
	int axis = int(3 * random0to1());
	switch (axis) {
		case 0:
			qsort(l, n, sizeof(hitable *), box_x_compare);
			break;
		case 1:
			qsort(l, n, sizeof(hitable *), box_y_compare);
			break;
		case 2:
			qsort(l, n, sizeof(hitable *), box_z_compare);
			break;
	}
	switch (n) {
		case 1:
			left = right = l[0];
			break;
		case 2:
			left = l[0];
			right = l[1];
			break;
		default:
			left = new bvh_node(l, n / 2);
			right = new bvh_node(l + n / 2, n - n / 2);
			break;
	}
	aabb box_left, box_right;
	if (!left->bounding_box(box_left) || !right->bounding_box(box_right))
		std::cerr << "No bounding box in bvh_node constructor.\n";
	box = surrounding_box(box_left, box_right);
}

#endif // BVHH

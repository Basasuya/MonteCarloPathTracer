#ifndef __AABB_H
#define __AABB_H
#include "Util.h"
#include "Ray.h"

struct AABB {
	vec3f low, high;
	AABB(){
		for(int i = 0; i < 3; ++i) {
			low[i] = INT_MAX;
			high[i] = INT_MIN;
		}
	};
    AABB(vec3f _low, vec3f _high) {
		for(int i = 0; i < 3; ++i) {
			low[i] = _low[i];
			high[i] = _high[i];
		}
	}
	inline bool intersect(Ray& ray) {
		float tmin = -1, tmax = -1;
		for(int i = 0; i < 3; ++i) {
			if(ray.direction[i] < EPS) return false;
			float t1 = (low[i] - ray.pos[i]) / ray.direction[i];
			float t2 = (high[i] - ray.pos[i]) / ray.direction[i];
			if(i == 0) {
				tmin = min(t1, t2); tmax = max(t1, t2);
			} else {
				tmin = max(tmin, min(t1, t2));
				tmax = min(tmax, max(t1, t2));
			}
		}
		return (tmin < ray.tmax && tmax > ray.tmin && tmin <= tmax && tmax >= 0);
	}
};

AABB merge(AABB box1, AABB box2) {
	vec3f pt1, pt2;
	for(int i = 0; i < 3; ++i) {
		pt1[i] = min(box1.low[i], box2.low[i]);
	}
	for(int i = 0; i < 3; ++i) {
		pt2[i] = max(box1.high[i], box2.high[i]);
	}
	return AABB(pt1, pt2);
}


AABB merge(vec3f &box1, vec3f &box2) {
	vec3f pt1, pt2;
	for(int i = 0; i < 3; ++i) {
		pt1[i] = min(box1[i], box2[i]);
	}
	for(int i = 0; i < 3; ++i) {
		pt2[i] = max(box1[i], box2[i]);
	}
	return AABB(pt1, pt2);
}


AABB merge(AABB box1, vec3f &box2) {
	vec3f pt1, pt2;
	for(int i = 0; i < 3; ++i) {
		pt1[i] = min(box1.low[i], box2[i]);
	}
	for(int i = 0; i < 3; ++i) {
		pt2[i] = max(box1.high[i], box2[i]);
	}
	return AABB(pt1, pt2);
}


#endif
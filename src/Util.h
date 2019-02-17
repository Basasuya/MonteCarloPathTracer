#ifndef __UTIL_H
#define __UTIL_H

#include <cmath>
#include <vector>
#include <tuple>
#include <iostream>
#include <cstring>
#include "Point.h"
using namespace std;
const float EPS = 1e-5;

bool isEqualf(float a, float b) {
	if (fabs(a - b) < EPS) {
		return true;
	}
	return false;
}


struct Material {
    string name;
    vec3f ke;
    vec3f tf;
    vec3f ka;
    vec3f kd;
    vec3f ks;
    float ns;
    float illum;
    float ni;

	Material() {
		ns = 60;
		ni = 0;
		illum = 0;
		for(int i = 0; i < 3; ++i) {
			ke[i] = tf[i] = ka[i] = kd[i] = ks[i] = 0;
		}
	}
};

struct AABB {
	vec3f low, high;
	AABB(){};
    AABB(float* _low, float* _high) {
		for(int i = 0; i < 3; ++i) {
			low[i] = _low[i];
			high[i] = _high[i];
		}
	}
};

AABB merge(const AABB &box1, const AABB &box2) {
	float pt1, pt2;
	for(int i = 0; i < 3; ++i) {
		pt1[i] = min(box1.low[i], box2.low[i]);
	}
	for(int i = 0; i < 3; ++i) {
		pt2[i] = max(box1.high[i], box2.high[i]);
	}
	return AABB(pt1, pt2);
}


#endif
#ifndef __UTIL_H
#define __UTIL_H

#include <cmath>
#include <vector>
#include <tuple>
#include <iostream>
#include <cstring>
#include "Point.h"
#include "Ray.h"
using namespace std;


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
	float tr;

	Material() {
		ns = 60;
		ni = 0;
		illum = 0;
		tr = 0;
		for(int i = 0; i < 3; ++i) {
			ke[i] = tf[i] = ka[i] = kd[i] = ks[i] = 0;
		}
	}
	void print() {
		printf("ns: %.3f ni: %.3f illum: %.3f tr: %.3f\n", ns, ni, illum, tr);
		printf("ke: "); ke.print();
		printf("tf: "); tf.print();
		printf("ka: "); ka.print();
		printf("kd: "); kd.print();
		printf("ks: "); ks.print();
	}
};

struct AABB {
	vec3f low, high;
	AABB(){};
    AABB(vec3f _low, vec3f _high) {
		for(int i = 0; i < 3; ++i) {
			low[i] = _low[i];
			high[i] = _high[i];
		}
	}
	bool intersect(Ray& ray) {
		float tmin = -1, tmax = -1;
		for(int i = 0; i < 3; ++i) {
			float t1 = (low[i] - ray.pos[i]) / ray.direction[i];
			float t2 = (high[i] - ray.pos[i]) / ray.direction[i];
			if(i == 0) {
				tmin = min(t1, t2); tmax = max(t1, t2);
			} else {
				tmin = max(tmin, min(t1, t2));
				tmax = min(tmax, max(t1, t2));
			}
		}
		return (tmin < ray.tmax && tmax > ray.tmax && tmin <= tmax && tmin >= 0);
	}
};

AABB merge(AABB &box1, AABB &box2) {
	vec3f pt1, pt2;
	for(int i = 0; i < 3; ++i) {
		pt1[i] = min(box1.low[i], box2.low[i]);
	}
	for(int i = 0; i < 3; ++i) {
		pt2[i] = max(box1.high[i], box2.high[i]);
	}
	return AABB(pt1, pt2);
}

struct Intersection {
	vec3f point;
	vec3f normal;
	Material material;
};




struct Light {
	vec3f p0, p1, p2;
	vec3f ke;
	
};



#endif
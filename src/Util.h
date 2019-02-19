#ifndef __UTIL_H
#define __UTIL_H

#include <cmath>
#include <vector>
#include <tuple>
#include <iostream>
#include <cstring>
#include "Point.h"
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


struct Triangle {
	vec3f center;
    Material material;
 	int vertex[3];
	int vertexNormal[3];
};


struct Light{
	vec3f emisiion;
	float r;
	vec3f center;
	vec3f normal;
	int type; // 0 square 1 sphare
};

#endif
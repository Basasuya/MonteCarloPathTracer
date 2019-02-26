#ifndef __UTIL_H
#define __UTIL_H

#include <cmath>
#include <vector>
#include <tuple>
#include <iostream>
#include <cstring>
#include <cfloat>
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
	vec3f normal;
	Mat4 mat;
	vec3f origin, edge1, edge2;
 	int vertex[3];
	int vertexNormal[3];
};


struct Light{
	vec3f emisiion;
	float r;
	vec3f center;
	vec3f normal;
	// int type; // 0 square 1 sphare
};

float Rand() {
	return (float)rand() / RAND_MAX;
}



class Ray {
public:    
    vec3f pos;
    int hitLight;
    vec3f direction;
    float tmin,tmax;
    bool hit;
    vec3f hitPoint;
    vec3f hitNormal;
    Triangle hitTriangle;
    

public:
    Ray(vec3f _pos = vec3f(0.0, 0.0, 0.0), vec3f _direction = vec3f(0.0, 0.0, 0.0)) {
        pos = _pos;
        direction = normalize(_direction);
        tmin = EPS;
        hitLight = -1;
        tmax = FLT_MAX;
        hit = false;
    }
    vec3f getPoint(float t) { 
        return pos + direction*t;
    }

    bool isWithinBounds(float tVal) { 
        return tVal < tmax && tVal > tmin; 
    }
};

#endif
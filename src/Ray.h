#ifndef __RAY_H
#define __RAY_H

#include <iostream>
#include <cmath>
#include <cfloat>
#include "Util.h"
#include "Point.h"
using namespace std;

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
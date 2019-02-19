#ifndef __RAY_H
#define __RAY_H

#include <iostream>
#include <cmath>
#include "Point.h"
using namespace std;


class Ray {
public:
    enum SOURCE{ NONE, DIRECT, DIFFUSE_REFLECT, SPECULAR_REFLECT, TRANSMISSON };
    vec3f pos;
    vec3f direction;
    SOURCE source;
    float tmin,tmax;
    float IOR;

public:
    Ray(vec3f _pos = vec3f(0.0, 0.0, 0.0), vec3f _direction = vec3f(0.0, 0.0, 0.0), SOURCE _source = SOURCE::NONE, float _ior = 1.0) {
        pos = _pos;
        direction = normalize(_direction);
        source = _source;
        IOR = _ior;
        tmin = EPS;
        tmax = 1e9;
    }
    vec3f getPoint(float t) { 
        return pos + direction*t;
    }

    bool isWithinBounds(float tVal) { 
        return tVal <= tmax && tVal >= tmin; 
    }

    vec3f reflect(vec3f& normal) { 
        return direction - 2.0f * dot(normal, direction) * normal; 
    }

    bool refract(const vec3f& normal, float nit, vec3f& refract_direction) {
        float ndoti = dot(normal, direction), 
            k = 1.0f - nit * nit * (1.0f - ndoti * ndoti);
        if (k >= 0.0f) {
            refract_direction = nit * direction - normal * (nit * ndoti + sqrt(k));
            return true;
        } else return false;
    }




};

#endif
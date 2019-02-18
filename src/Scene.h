#ifndef __SCENE_H
#define __SCENE_H

#include <iostream>
#include <cstring>
#include <cmath>
#include "Point.h"
using namespace std;
const float PI = 3.14159265358979f;
class Scene {
public:

    int width, height;
    float fovy, fovx;
    vec3f pos, lookat, up;
    vec3f directionX, directionY, directionZ;
    vec3f viewX, viewY, viewZ;
    
    int pixelSample, monteCarloSample;
    int maxRecursizeDepth;
    bool useDirectLight;
    vec3f environment;

    float* colors;
    


public:
    void init() {
        directionY = normalize(up);
        directionZ = normalize(lookat - pos);
        directionX = normalize(cross(directionZ, directionY));
        fovx = fovy / height * width;
        viewX = directionX * 2 * tan(fovx * PI / 360);
        viewY = directionY * 2 * tan(fovy * PI / 360);
        viewZ = directionZ;
        colors = new float[3 * width * height];
    }







};

#endif
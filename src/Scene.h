#ifndef __SCENE_H
#define __SCENE_H

#include <iostream>
#include <cstring>
#include <cmath>
#include "Point.h"
#include "Ray.h"
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
    int lightSample;

    float* colors;
    float* sumColors;
       
private:
    Ray getRay(float x, float y) {
        vec3f direction = viewZ + (x - 0.5f) * viewX + (y - 0.5f) * viewY;
        return Ray(pos, direction);
    }

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
        sumColors = new float[3 * width * height];
        environment = vec3f(0.2, 0.2, 0.2);
    }


    Ray* getRays(float x, float y) {
        float dx = 1.0f / width, dy = 1.0f / height;
        Ray* rays = new Ray[pixelSample];

        for (int i = 0; i < pixelSample; ++i) {
            float randomX = (float)rand() / RAND_MAX;
            float randomY = (float)rand() / RAND_MAX;
            rays[i] = getRay((x + randomX)*dx, (y + randomY)*dy);
        }
        
        
        return rays;
    }




};

#endif
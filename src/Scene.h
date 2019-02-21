#ifndef __SCENE_H
#define __SCENE_H

#include <iostream>
#include <cstring>
#include <cmath>
#include "Util.h"
#include "Model.h"
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
    
    vec3f ambient;
    int monteCarloMaxSample;
    int maxRecursizeDepth;
    int iterations;

    float* colors;
    float* sumColors;
    vector<Light> lights;

    Model *model;



public:
    Scene(string path) {
        struct dirent *ptr;
        DIR *dir;
        vector<string> files;
        dir = opendir(path.c_str());
        while(1) {
            ptr = readdir(dir);
            if(ptr == NULL) break;
            if(ptr->d_name[0] == '.')
                continue;
            files.push_back(ptr->d_name);     
        }
        for(int j = 0, len = files.size(); j < len; ++j) {
            if(files[j].find("parameter") != string::npos) {
                readScene(path + "/" + files[j]);
            }
        } 
    }


    float* render() {
        clock_t t1 = clock();
        iterations ++;
        if (iterations > monteCarloMaxSample) 
            return colors;
        printf("the %d iteration ", iterations);
        for(int i = 0; i < width; ++i) {
            // printf("%d \n", i);
            for(int j = 0; j < height; ++j) {
                vec3f color;
                for (int sy=0; sy<2; sy++) {    // 2x2 subpixel rows 
                    for (int sx=0; sx<2; sx++){  
                        float r1=2*Rand(),  dx=r1<1 ? sqrt(r1)-1: 1-sqrt(2-r1);
                        float r2=2*Rand(), dy=r2<1 ? sqrt(r2)-1: 1-sqrt(2-r2);
                        vec3f d = viewX*( ( (sx+.5 + dx)/2 + i)/width - .5) +
                                viewY*( ( (sy+.5 + dy)/2 + j)/height - .5) + viewZ;  
                        Ray ray = Ray(pos, d); 

                        // another method to generate Ray
                        // float r1 = (Rand() + i) / width - 0.5; 
                        // float r2 = (Rand() + j) / height - 0.5; 
                        // vec3f d = viewX * r1 + viewY * r2 + viewZ;
                        // Ray ray = Ray(pos, d);
                        vec3f tmpcolor = trace(ray, 0);
                        for(int l = 0; l < 3; ++l) {
                            tmpcolor[l] = max(0.0f, min(1.0f, tmpcolor[l]));
                        }
                        color += tmpcolor * 0.25;
                    }
                }
                int index = j * width * 3 + i * 3;
                for(int k = 0; k < 3; ++k) {
                    sumColors[k + index] += color[k];
                }
                for(int k = 0; k < 3; ++k) {
                    colors[k + index] = sumColors[k + index] / iterations;                
                }
            }
        }
        printf("execucate Time: %.3f s\n",(clock() - t1) * 1.0 / CLOCKS_PER_SEC);
        return colors;
    }

    void init() {
        iterations = 0;
        directionY = normalize(up);
        directionZ = normalize(lookat - pos);
        directionX = normalize(cross(directionZ, directionY));
        fovx = fovy / height * width;
        viewX = directionX * 2 * tan(fovx * PI / 360);
        viewY = directionY * 2 * tan(fovy * PI / 360);
        viewZ = directionZ;
        colors = new float[3 * width * height];
        sumColors = new float[3 * width * height];
        ambient = vec3f(0.2, 0.2, 0.2);
    }

private:
    void readScene(string path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            printf("the parameter path is unavailable\n");
            exit(0);
        }    
        // int width, height, fovy;
        file >> width >> height;
        // vec3f pos, lookat, up;
        file >> pos[0] >> pos[1] >> pos[2];
        file >> lookat[0] >> lookat[1] >> lookat[2];
        file >> up[0] >> up[1] >> up[2];
        file >> fovy;

        // file >> pixelSample;
        file >> monteCarloMaxSample;
        file >> maxRecursizeDepth;
         
        int q;
        file >> q;
        for(int i = 0; i < q; ++i) {
            Light tt;
            file >> tt.type >> tt.r;
            if(tt.type == 0) {
                file >> tt.normal[0] >> tt.normal[1] >> tt.normal[2];
            }
            file >> tt.center[0] >> tt.center[1] >> tt.center[2];
            file >> tt.emisiion[0] >> tt.emisiion[1] >> tt.emisiion[2];
            lights.push_back(tt);
        }
        for(int i = 0; i < lights.size(); ++i) {
            printf("%.3f %.3f %.3f\n", lights[i].emisiion[0], lights[i].emisiion[1], lights[i].emisiion[2]);
        }
        // printf("%d\n", maxRecursizeDepth);
        file.close();
        init();
    }

    vec3f trace(Ray ray, int depth) {
        // model->kdtree.burceIntersect(model->kdtree.root, ray, 0);
        model->kdtree.intersect(model->kdtree.root, ray, 0);
        intersectLight(ray);
        // if(ray.hit) printf("yingyingying\n");
        if(!ray.hit || depth > 5) {
            return BLACK;
        } else if(ray.hitLight != -1){
            return lights[ray.hitLight].emisiion;
        } else {
            Material mt = ray.hitTriangle.material;
            // return mt.kd;
            float sumKs = mt.ks.sum(); float sumKd = mt.kd.sum() + sumKs;
            vec3f reflectDirection = normalize(ray.direction - 2 * ray.hitNormal * dot(ray.hitNormal, ray.direction)); // normal may has several bug
            vec3f orientedNormal = dot(ray.hitNormal, ray.direction) < 0 ? ray.hitNormal : - ray.hitNormal;
            float russianRoulette = Rand();

            if(russianRoulette < sumKs / sumKd) { // do SPECULAR;
                Ray newRay = Ray(ray.hitPoint, reflectDirection);
                return mt.ks * trace(newRay, depth + 1);
            } else { // do diffuse;
                vec3f e;
                for(int i = 0, len = lights.size(); i < len; ++i) {
                    vec3f sw=lights[i].center-ray.hitPoint;
                    vec3f su= normalize(cross( (fabs(sw.x)>.1?vec3f(0,1,0):vec3f(1,0,0)) , sw)), sv=cross(sw, su);
                    float cos_a_max = sqrt(1-lights[i].r * lights[i].r / dot((ray.hitPoint-lights[i].center), ray.hitPoint-lights[i].center));
                    float eps1 = Rand(), eps2 = Rand();
                    float cos_a = 1-eps1+eps1*cos_a_max;
                    float sin_a = sqrt(1-cos_a*cos_a);
                    float phi = 2*M_PI*eps2;
                    vec3f l = normalize(su*cos(phi)*sin_a + sv*sin(phi)*sin_a + sw*cos_a);
                    Ray newRay = Ray(ray.hitPoint, l);
                    sphereIntersect(lights[i], newRay, i);
                    // printf("%d\n", newRay.hit);
                    newRay.hit = false;
                    model->kdtree.notIntersect(model->kdtree.root, newRay, 0);
                    // printf("%d\n", newRay.hit);
                    if (newRay.hit == false){  // shadow ray
                        double omega = 2*M_PI*(1-cos_a_max);
                        e = e + max( dot(ray.hitNormal, l), 0.0f) * lights[i].emisiion * dot(l, orientedNormal) * omega * M_1_PI;  // 1/pi for brdf
                        // e.print();
                    }
                }
                float r1 = 2*M_PI * Rand();
                float r2 = Rand();
                float r2s = sqrt(r2); 
                vec3f w = orientedNormal;
                vec3f u = normalize(cross((fabs(w.x)>.1?vec3f(0,1,0):vec3f(1,0,0)), w));
                vec3f v = cross(w, u); 
                vec3f diffuseReflectDirection = normalize((u*cos(r1)*r2s + v*sin(r1)*r2s + w*sqrt(1-r2))); 
                Ray newRay = Ray(ray.hitPoint, diffuseReflectDirection);
                // Ray newRay = Ray(ray.hitPoint, reflectDirection);
                vec3f result = trace(newRay, depth + 1);
                if(result == BLACK) return e + mt.kd;
                // printf("%.3f %.3f %.3f ", dot(ray.hitNormal, diffuseReflectDirection), dot(ray.hitNormal, ray.direction), mt.kd.sum()); e.print();
                return e + mt.kd * max(0.0f, dot(ray.hitNormal, diffuseReflectDirection)) * result;
            }
        }
    }

    void sphereIntersect(Light& l, Ray& r, int id) {
        vec3f op = l.center - r.pos; // Solve t^2*d.d + 2*t*(o-p).d + (o-p).(o-p)-R^2 = 0
        float t, eps=1e-4;
        float b = dot(op, r.direction);
        float det = b * b - dot(op, op) + l.r * l.r;
        if (det<0) return ;
        else det=sqrt(det);
        float hitTime = (t=b-det)>eps ? t : ((t=b+det)>eps ? t : 0);
        if(r.isWithinBounds(hitTime)) {
            r.hit = true;
            r.hitLight = id;
            r.tmax = t;
        }
    }

    void intersectLight(Ray& ray) {
        for(int i = 0, len = lights.size(); i < len; ++i) {
            sphereIntersect(lights[i], ray, i);
        }
    }


};

#endif
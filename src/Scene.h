#ifndef __SCENE_H
#define __SCENE_H

#include <iostream>
#include <cstring>
#include <cmath>
#include "Util.h"
#include "Model.h"
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
    int pixelSample, monteCarloMaxSample;
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
        iterations ++;
        if (iterations > monteCarloMaxSample) 
            return colors;
        printf("the %d iteration\n", iterations);
        for(int i = 0; i < width; ++i) {
            for(int j = 0; j < height; ++j) {
                vec3f color;
                Ray* rays = getRays(i, j);
                for(int k = 0; k < pixelSample; ++k) {
                    vec3f tmpcolor = trace(rays[k], 0);
                    for(int l = 0; l < 3; ++l) {
                        tmpcolor[l] = max(0.0f, min(1.0f, tmpcolor[l]));
                    }
                    color += tmpcolor;
                }
                color /= pixelSample;
                delete[] rays;
                int index = j * width * 3 + i * 3;
                for(int k = 0; k < 3; ++k) {
                    sumColors[k + index] += color[k];
                }
                for(int k = 0; k < 3; ++k) {
                    colors[k + index] = sumColors[k + index] / iterations;
                }
            }
        }
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

        file >> pixelSample;
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
        // for(int i = 0; i < lights.size(); ++i) {
        //     printf("%.3f %.3f %.3f\n", lights[i].emisiion[0], lights[i].emisiion[1], lights[i].emisiion[2]);
        // }
        // printf("%d\n", maxRecursizeDepth);
        file.close();
        init();
    }

    Ray getRay(float x, float y) {
        vec3f direction = viewZ + (x - 0.5f) * viewX + (y - 0.5f) * viewY;
        return Ray(pos, direction);
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


    vec3f trace(Ray& ray, int depth) {
        model->kdtree.intersect(model->kdtree.root, ray, 0);
        
        if(!ray.hit) return BLACK;
        else {
            if(depth > maxRecursizeDepth) return ray.hitTriangle.material.ke;
            vec3f indirectLight, directLight;
            Ray newRay = monteCarloSample(ray);

            if(newRay.source != Ray::SOURCE::NONE) {
                indirectLight = trace(newRay, depth + 1);
                switch (newRay.source) {
                    case Ray::SOURCE::DIFFUSE_REFLECT:
                        indirectLight = ray.hitTriangle.material.kd * indirectLight/**dot(newRay.direction, intersection.normal)*/;
                        break;
                    case Ray::SOURCE::SPECULAR_REFLECT:
                        indirectLight = ray.hitTriangle.material.ks * indirectLight/**pow(dot(view_direction, reflect_direction), material.shiness)*/;
                        break;
                    default :
                        indirectLight = indirectLight;
                        break;
                }
            }
            directLight = directIllumination(ray);

            return ray.hitTriangle.material.ke + directLight + indirectLight + ray.hitTriangle.material.ka * ambient;
        }
    }

    Ray monteCarloSample(Ray& ray) {
        Material& material = ray.hitTriangle.material;

        vec3f direction;
        float num[2];
        num[0] = dot(material.kd, vec3f(1, 1, 1));
        num[1] = dot(material.ks, vec3f(1, 1, 1)) + num[0];

        if (material.illum > 0 && material.ni != 1.0) {
            float ni, nt;
            float cosTheta = dot(ray.direction, ray.hitNormal);
            vec3f normal = cosTheta <= 0.0 ? ray.hitNormal : -ray.hitNormal;

            if (cosTheta>0.0) {
                ni = material.ni;
                nt = 1.0;
            } else {
                ni = 1.0;
                nt = material.ni;
            }

            float f0 = (ni - nt) / (ni + nt);
            f0 *= f0;
            float schlick = f0 + (1 - f0)* pow(1.0 - abs(cosTheta), 5);
            float fresnelIndex = schlick;
            float transmissonSurvival;

            float pro = (double)rand() / RAND_MAX;
            if (pro > fresnelIndex)  {
                if (ray.refract(normal, ni / nt, direction))
                    return Ray(ray.hitPoint, direction, Ray::SOURCE::TRANSMISSON);
                else {
                    direction = ray.reflect(normal);
                    return Ray(ray.hitPoint, direction, Ray::SOURCE::SPECULAR_REFLECT);
                }
            }
        }

        float pro2 = (double)rand() / RAND_MAX;
        if (pro2 > num[0] / num[1]) {
            vec3f prefectReflectDirection = ray.reflect(ray.hitNormal);
            direction = importanceSample(prefectReflectDirection, material.ns);
            return Ray(ray.hitPoint, direction, Ray::SOURCE::SPECULAR_REFLECT);
        } else {
            direction = importanceSample(ray.hitNormal, -1);
            return Ray(ray.hitPoint, direction, Ray::SOURCE::DIFFUSE_REFLECT);
        }
    }

    vec3f importanceSample(vec3f up, float n) {
        //srand(0);
        double phi, theta;
        double r1 = (double)rand() / RAND_MAX, r2 = (double)rand() / RAND_MAX;

        phi = r1 * 2 * PI;
        theta = n<0 ? asin(sqrt(r2)) : acos(pow(r2, 1 / (n + 1)));
        vec3f sample(sin(theta)*cos(phi), cos(theta), sin(theta)*sin(phi));
        vec3f front, right;
        if (fabs(up.x) > fabs(up.y))
            front = vec3f(up.z, 0, -up.x);
        else
            front = vec3f(0, -up.z, up.y);

        front = normalize(front);
        right = cross(up, front);

        return normalize(sample.x*right + sample.y*up + sample.z*front);
    }
    vec3f directIllumination(Ray& ray)  {
        vec3f rgb;
        for (int i = 0, len = lights.size(); i < len; ++i) {
            // float sx = (float)rand() / RAND_MAX;
            // float sy = (float)rand() / RAND_MAX;

            vec3f lightOrigin = lights[i].center;
            vec3f r = lightOrigin - ray.hitPoint;
            float rr = length(r);
            Ray shadowRay = Ray(ray.hitPoint, r);
            shadowRay.tmax = rr;

            model->kdtree.notIntersect(model->kdtree.root, shadowRay, 0);
            if (shadowRay.hit == false)
            {
                vec3f s = normalize(r);

                float cosThetaIn = max(dot(ray.hitNormal, s), 0.0f);
                float cosThetaOut = max(dot(-s, ray.hitNormal), 0.0f);
                float geoFactor = cosThetaIn*cosThetaOut / (rr*rr);
                // Probability: 1/area.
                vec3f intensity = geoFactor * length(lights[i].normal) * lights[i].emisiion;

                if (ray.hitTriangle.material.kd != BLACK)
                {
                    //calculate the diffuse color
                    float mDots = dot(s, ray.hitNormal);
                    if (mDots > 0.0) rgb += mDots*ray.hitTriangle.material.kd
                        *intensity / PI;
                }

                if (ray.hitTriangle.material.ks != BLACK)
                {
                    //calculate the specular color
                    vec3f v = ray.direction.flip();
                    vec3f h = normalize(s + v);
                    float mDotH = dot(h, ray.hitNormal);
                    if (mDotH > 0.0) rgb += pow(mDotH, ray.hitTriangle.material.ns)* ray.hitTriangle.material.ks
                        *intensity
                        *(ray.hitTriangle.material.ns + 1) / (2 * PI);
                }
            }
        }
        return rgb;
    }

//     float intersect(Ray &r, Light &l) {
//         Vec op = l.center-; // Solve t^2*d.d + 2*t*(o-p).d + (o-p).(o-p)-R^2 = 0 
//         double t, eps=1e-4, b=op.dot(r.d), det=b*b-op.dot(op)+rad*rad; 
//         if (det<0) return 0; else det=sqrt(det); 
//         return (t=b-det)>eps ? t : ((t=b+det)>eps ? t : 0); 
//    } 


};

#endif
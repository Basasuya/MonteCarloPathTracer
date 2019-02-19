#ifndef __MODEL_H
#define __MODEL_H

#include <vector>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <dirent.h>
#include <fstream>
#include <tuple>
#include <cstdio>
#include <map>
#include "Util.h"
#include "Scene.h"
#include "Ray.h"
using namespace std;

class Model;

struct Triangle {
	vec3f center;
    Model *model;
    Material material;
 	int vertex[3];
	int vertexNormal[3];
	void copy(Triangle &a) {
		center = a.center;
		for(int i = 0; i < 3; ++i) {
			vertex[i] = a.vertex[i];
			vertexNormal[i] = a.vertexNormal[i];
		}
	}
	bool Triangle::intersect(Ray& ray, Intersection& intersection) {
        vec3f p0 = model->points[vertex[0]];
        vec3f p1 = model->points[vertex[1]];
        vec3f p2 = model->points[vertex[2]];
        vec3f origin = p0;
        vec3f edge1 = p1 - p0;
        vec3f edge2 = p2 - p0;
        vec3f normal = normalize(cross(edge1, edge2));
        Mat4 tmp = Mat4(p0, p1, p2);
        tmp.inverse();

		if (abs(dot(normal, ray.direction))<EPS) {
			return false;
		}

		vec3f v = origin - ray.pos;

		vec3f tmp1 = cross(edge2, ray.direction);
		float beta = -dot(tmp1, v) / dot(tmp1, edge1);

		vec3f tmp2 = cross(edge1, ray.direction);
		float gamma = -dot(tmp2, v) / dot(tmp2, edge2);

		float t = dot(normal, v) / dot(normal, ray.direction);

		if (beta > 0 && gamma > 0 && beta + gamma < 1 && ray.isWithinBounds(t)) {
			intersection.point = ray.getPoint(t);
            vec3f abg = tmp * intersection.point;
            abg.x = abs(abg.x);
            abg.y = abs(abg.y);
            abg.z = abs(abg.z);

            intersection.normal = normalize(abg.x*(model->normals[vertexNormal[0]]) + abg.y*(model->normals[vertexNormal[1]]) + abg.z*(model->normals[vertexNormal[2]]));
			intersection.material = material;
            ray.tmax = t;
			return true;
		}

		return false;
	}
};

struct Node{
    Triangle split;
    Node *lc, *rc;
    int div;
    AABB abstract;
};

struct cmpx{
    int div;
    cmpx(const int &_div) { div = _div; }
    bool operator() (Triangle &a, Triangle& b) {
        for(int i = 0; i < 3; ++i) {
            if(a.center[ (div+i) % 3] != b.center[ (div+i) % 3]) {
                return a.center[ (div+i) % 3] < b.center[ (div+i) % 3];
            }
        }
        return true;
    }
};

bool cmp(Triangle& a, Triangle& b, int div) {
    cmpx cp = cmpx(div);
    return cp(a, b);
}

class Kdtree{
public:
    Node *root;
    vector<Triangle> *triangle;
public:
    Kdtree() {
        // root = new Node();
    }
    void init() {
        root = build(0, triangle->size(), 0);
    }

    bool intersect(Node *rt, Ray& ray, int div, Intersection& intersection) {
        if(!rt->abstract.intersect(ray)) return false;
        bool flag = true;
        if(rt->lc == NULL) {
            return rt->split.intersect(ray, intersection);
        }
        if(ray.direction[div] >= 0) {
            if(rt->lc != NULL) flag |= intersect(rt->lc, ray, (div + 1) % 3, intersection);
            if(rt->rc != NULL && !flag) flag |= intersect(rt->rc, ray, (div + 1) % 3, intersection);
        } else {
            if(rt->rc != NULL) flag |= intersect(rt->rc, ray, (div + 1) % 3, intersection);
            if(rt->lc != NULL && !flag) flag |= intersect(rt->lc, ray, (div + 1) % 3, intersection);
        }
        return flag;
    }

private:
    Node* build(int l, int r, int div) {
        if(l >= r) return NULL;
        Node *p = new Node();
        p->div = div;
        int mid = (l + r) / 2;
        nth_element(triangle->begin() + l, triangle->begin() + mid, triangle->begin() + r, cmpx(div));
        p->split.copy((*triangle)[mid]);
        p->lc = build(l, mid, (div + 1) % 3);
        p->rc = build(mid + 1, r, (div + 1) % 3);
        if(l + 1== r) {
            p->abstract = AABB((*triangle)[mid].center, (*triangle)[mid].center);
        } else {
            p->abstract = merge(p->lc->abstract, p->rc->abstract);
        }
        return p;
    }




};

class Model {
public:
    Model(string path) {
        iterations = 0;
        materialTable.clear();
        // get the files name below this directary
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
        // find and read the obj and mtl file
        for(int j = 0, len = files.size(); j < len; ++j) {
            if(files[j].find("obj") != string::npos) {
                readObj(path + "/" + files[j]);
            } else if(files[j].find("parameter") != string::npos) {
                readScene(path + "/" + files[j]);
            }
        } 
        kdtree.triangle = &triangles;
        kdtree.init();
    }

    float* render() {
        iterations ++;
        if (iterations > scene.monteCarloSample) 
            return scene.colors;
        printf("the %d iteration\n", iterations);
        for(int i = 0; i < scene.width; ++i) {
            for(int j = 0; j < scene.height; ++j) {
                vec3f color;
                Ray* rays = scene.getRays(i, j);
                for(int k = 0; k < scene.pixelSample; ++k) {
                    vec3f tmpcolor = trace(rays[k], 0);
                    for(int l = 0; l < 3; ++l) {
                        tmpcolor[i] = max(0.0f, min(1.0f, tmpcolor[i]));
                    }
                    color += tmpcolor;
                }
                color /= scene.pixelSample;
                delete[] rays;
                int index = j * scene.width * 3 + i * 3;
                for(int k = 0; k < 3; ++k) {
                    scene.sumColors[k + index] += color[k];
                }
                for(int k = 0; k < 3; ++k) {
                    scene.colors[k + index] = scene.sumColors[k + index] / iterations;
                }
            }
        }
        return scene.colors;
    }

public:
    map<string, Material> materialTable;
    AABB box;
    vector<vec3f> points;
    vector<vec3f> normals;
    vector<Triangle> triangles;
    vector<Light> lights;
    Kdtree kdtree;
    Scene scene;
    int iterations;
    bool useDirectLight = true;

    
private:
    void readObj(string path) {
        // int basacnt = 0;

        std::ifstream file(path);
        if (!file.is_open()) {
            printf("the obj path is unavailable\n");
            exit(0);
        }
        string type; Material material;
        while(file >> type) {
            if(type == "mtllib") {
                string mtlFile;
                file >> mtlFile;

                int pos = path.find_last_of('/');
                string mtl_path = path.substr(0, pos + 1) + mtlFile;
                readMtl(mtl_path);
            } else if(type == "usemtl") {
                string name;
                file >> name;
                auto it = materialTable.find(name);
                if(it == materialTable.end()) {
                    printf("can't find %s material\n", name.c_str());
                } else {
                    material = it->second;
                }
            } else if(type == "v"){
                vec3f v;
                file >> v[0] >> v[1] >> v[2];
                for(int i = 0; i < 3; ++i) {
                    box.low[i] = min(box.low[i], v[i]);
                    box.high[i] = min(box.high[i], v[i]);
                }
                points.push_back(v);
            } else if(type == "vn"){
                vec3f vn;
                file >> vn[0] >> vn[1] >> vn[2];
                normals.push_back(vn);
            } else if(type == "f"){
                char* str;
	            char* sub;
                str = (char*)malloc(200 * sizeof(char));
                file.getline(str, 200);
                int vertex[3]; int vertexNormal[3];
                // printf("%s\n", str);
                sub = strtok(str, " ");
                int index = 0;
                while (sub) {
				    string seq(sub);
                    // printf("%s ", sub);
                    int cnt = 0; int pos1 = -1, pos2 = -1;
                    for(int j = 0, len = seq.length(); j < len; ++j) {
                        if(seq[j] == '/') {
                            cnt ++;
                            if(cnt == 1) {
                                pos1 = j;
                            } else pos2 = j;
                        }
                    }
                    vertex[index] = atoi(seq.substr(0, pos1).c_str()) - 1;
                    vertexNormal[index] = atoi(seq.substr(pos2 + 1, seq.length() - pos2 - 1).c_str()) - 1;
                    index ++;
                    if(index == 3) {
                        
                        // basacnt ++;
                        // if(basacnt < 10) {
                        //     // printf("%s %d %d\n", sub, pos1, pos2);
                        //     for(int i = 0; i < 3; ++i) {
                        //         printf("%d ", vertex[i]);
                        //     } printf("\n");
                        //     for(int i = 0; i < 3; ++i) {
                        //         printf("%d ", vertexNormal[i]);
                        //     } printf("\n");
                        // }
                        Triangle tra;
                        tra.model = this;
                        tra.material = material;
                        // tra.p0 = points[vertex[0]];
                        // tra.p1 = points[vertex[1]];
                        // tra.p2 = points[vertex[2]];
                        for(int i = 0; i < 3; ++i) {
                            tra.vertex[i] = vertex[i];
                            tra.vertexNormal[i] = vertexNormal[i];
                        }
                        tra.center = (points[vertex[0]] + points[vertex[0]] + points[vertex[0]]) / 3;
                        triangles.push_back(tra);

                        if(material.ke != BLACK) {
                            Light li;
                            li.p0 = points[vertex[0]];
                            li.p1 = points[vertex[1]];
                            li.p2 = points[vertex[2]];
                            li.ke = material.ke;
                            lights.push_back(li);
                        }
                        // solve the conditoin f with four part f a b c d
                        vertex[1] = vertex[2]; 
                        vertexNormal[1] = vertexNormal[2];
                        index = 2;
                    }
                    sub = strtok(NULL, " ");
                }
            } else file.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
        }
        file.close();
    }
    void readMtl(string path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            printf("the Mtl path is unavailable\n");
            exit(0);
        }        

        string type; string name; Material material;
        bool flag = false;
        while(file >> type) {
            if(type == "newmtl") {
                if(flag) {
                    materialTable[name] = material;
                    material = Material();
                } else flag = true;
                file >> name;
                material.name = name;
            } else if(type == "Kd") {
                file >> material.kd[0] >> material.kd[1] >> material.kd[2];
            } else if(type == "Ka") {
                file >> material.ka[0] >> material.ka[1] >> material.ka[2];
            } else if (type == "Ks") {
                file >> material.ks[0] >> material.ks[1] >> material.ks[2];
            } else if (type == "Ke") {
                file >> material.ke[0] >> material.ke[1] >> material.ke[2];
            } else if (type == "Ns") {
                file >> material.ns;
            } else if (type == "Tr") {
                file >> material.tr;
            } else if (type == "Ni") {
                file >> material.ni;
            } else if (type == "Tf") {
                file >> material.tf[0] >> material.tf[1] >> material.tf[2];
            } else if (type == "illum") {
                file >> material.illum;
            } else file.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
        }
        if(flag) {
            materialTable[name] = material;
        }
        // for(auto it = materialTable.begin(); it != materialTable.end(); ++it) {
        //     printf("%s ", (it->first).c_str()); (it->second).print(); printf("\n");
        // } printf("\n");
        file.close();
    }
    void readScene(string path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            printf("the parameter path is unavailable\n");
            exit(0);
        }    
        // int width, height, fovy;
        file >> scene.width >> scene.height;
        // vec3f pos, lookat, up;
        file >> scene.pos[0] >> scene.pos[1] >> scene.pos[2];
        file >> scene.lookat[0] >> scene.lookat[1] >> scene.lookat[2];
        file >> scene.up[0] >> scene.up[1] >> scene.up[2];
        file >> scene.fovy;

        file >> scene.pixelSample;
        file >> scene.monteCarloSample;
        file >> scene.maxRecursizeDepth;
        int flag;
        file >> flag;
        scene.useDirectLight = flag;
        file >> scene.environment[0] >> scene.environment[1] >> scene.environment[2];
    
        
        scene.init();

        file.close();    
    }

    vec3f trace(Ray& ray, int depth) {
        Intersection intersection;
        bool isIntersect = kdtree.intersect(kdtree.root, ray, 0, intersection);
        if(!isIntersect) return BLACK;
        else {
            if(depth >= scene.maxRecursizeDepth) return intersection.material.ke;
            vec3f indirectLight, directLight;
            Ray newRay = monteCarloSample(ray, intersection);

            if(newRay.source != Ray::SOURCE::NONE) {
                indirectLight = trace(newRay, depth + 1);
                vec3f view_direction = normalize(scene.pos - intersection.point);
                vec3f reflect_direction = normalize(newRay.direction - intersection.normal * 2 * dot(newRay.direction, intersection.normal));
                

                switch (newRay.source)
                {
                case Ray::SOURCE::DIFFUSE_REFLECT:
                    indirectLight = intersection.material.kd * indirectLight/**dot(newRay.direction, intersection.normal)*/;
                    break;
                case Ray::SOURCE::SPECULAR_REFLECT:
                    indirectLight = intersection.material.ks * indirectLight/**pow(dot(view_direction, reflect_direction), material.shiness)*/;
                    break;
                case Ray::SOURCE::TRANSMISSON:
                    indirectLight = indirectLight;
                    break;
                }
            }
            if (useDirectLight) directLight = directIllumination(intersection, ray);

            return material.emission + directIllumination + indirectIllumination + ref.ka*ambient;
        }
    }

    Ray monteCarloSample(Ray& ray, Intersection& intersection) {
        Material& material = intersection.material;

        vec3f direction;
        float num[2];
        num[0] = dot(material.kd, vec3f(1, 1, 1));
        num[1] = dot(material.ks, vec3f(1, 1, 1)) + num[0];

        if (num[1] <= 0) return Ray(intersection.point, direction);

        if (material.illum > 0 && material.ni != 1.0) {
            float ni, nt;
            float cosTheta = dot(ray.direction, intersection.normal);
            vec3f normal = cosTheta <= 0.0 ? intersection.normal : -intersection.normal;

            if (cosTheta>0.0) {
                ni = material.ni;
                nt = 1.0;
                //ni = 1.8;
            } else {
                ni = 1.0;
                nt = material.ni;
                //nt = 1.8;
            }

            float f0 = (ni - nt) / (ni + nt);
            f0 *= f0;
            float schlick = f0 + (1 - f0)* pow(1.0 - abs(cosTheta), 5);
            //Reflection or refraction?
            float fresnelIndex = schlick;
            float transmissonSurvival;

            /*double Tr = 1 - fresnelIndex;
            double P = 0.25 + 0.5*fresnelIndex;
            double RP = fresnelIndex / P;
            double TP = Tr / (1 - P);*/
            float pro = (double)rand() / RAND_MAX;
            if (pro > fresnelIndex)  {
                if (ray.refract(normal, ni / nt, direction))
                    return Ray(intersection.point, direction, Ray::SOURCE::TRANSMISSON);
                else {
                    direction = ray.reflect(normal);
                    return Ray(intersection.point, direction, Ray::SOURCE::SPECULAR_REFLECT);
                }
            }
        }

        //If reflection : diffuse or specular?
        float pro2 = (double)rand() / RAND_MAX;
        if (pro2 > num[0] / num[1]) {
            vec3f prefectReflectDirection = ray.reflect(intersection.normal);
            direction = importanceSample(prefectReflectDirection, material.ns);
            return Ray(intersection.point, direction, Ray::SOURCE::SPECULAR_REFLECT);
        }
        else
        {
            direction = importanceSample(intersection.normal, -1);
            return Ray(intersection.point, direction, Ray::SOURCE::DIFFUSE_REFLECT);
        }
    }

    vec3f importanceSample(vec3f up, float n) {
        //srand(0);
        double phi, theta;
        double r1 = (double)rand() / RAND_MAX, r2 = (double)rand() / RAND_MAX;

        phi = r1 * 2 * PI;
        theta = n<0 ? asin(sqrt(r2)) : acos(pow(r2, 1 / (n + 1)));
        vec3f sample(sin(theta)*cos(phi), cos(theta), sin(theta)*sin(phi));

        //�õ��Ĳ���sample��Ҫ�뷴��ⷽ����������Ӧ
        //Ҳ����˵��sample.y��Ӧup����ĳ��ȣ���������
        vec3f front, right;//

        if (fabs(up.x) > fabs(up.y))
            front = vec3f(up.z, 0, -up.x);
        else
            front = vec3f(0, -up.z, up.y);

        front = normalize(front);
        right = cross(up, front);

        return normalize(sample.x*right + sample.y*up + sample.z*front);
    }
    vec3f directIllumination(Intersection& intersection, Ray& ray)  {
        vec3f color;
        for (int i = 0, len = lights.size(); i < len; ++i) {
            color += renderLight(i, intersection, ray);
        }
        return color;
    }
    vec3f renderLight(int i, Intersection& intersection, Ray& ray) {
        if (lights[i].ke == BLACK) return BLACK;
        
        vec3f rgb;
        //Reflectance& ref = intersection.pGeometry->getReflectance(intersection.point);
        float rate = 1.0f / scene.lightSample;

        for (int i = 0; i < scene.lightSample; i++) 
        {
            float sx = (float)rand() / RAND_MAX;
            float sy = (float)rand() / RAND_MAX * (1.0f - sx);

            vec3f lightOrigin = origin + dx * sx + dy * sy;
            vec3f&r = lightOrigin - intersection.point;
            float rr = length(r);
            Ray& shadowRay = Ray(intersection.point, r);
            shadowRay.tmax = rr;

            if (!scene->isInShadow(shadowRay))
            {
                Vec3f s = normalize(r);

                float cosThetaIn = max(dot(intersection.normal, s), 0.0f);
                float cosThetaOut = max(dot(-s, normal), 0.0f);
                float geoFactor = cosThetaIn*cosThetaOut / (rr*rr);
                // Probability: 1/area.
                Vec3f intensity = geoFactor * area * emission * rate;

                if (ref.kd != BLACK)
                {
                    //calculate the diffuse color
                    float mDots = dot(s, intersection.normal);
                    if (mDots > 0.0) rgb += mDots*ref.kd
                        *intensity / PI;
                }

                if (ref.ks != BLACK)
                {
                    //calculate the specular color
                    Vec3f v = ray.direction.flip();
                    Vec3f h = normalize(s + v);
                    float mDotH = dot(h, intersection.normal);
                    if (mDotH > 0.0) rgb += pow(mDotH, intersectAttr.shiness)*ref.ks
                        *intensity
                        *(intersectAttr.shiness + 1) / (2 * PI);
                }
            }
        }
        //cout << endl;
        return rgb;
    }
};

#endif
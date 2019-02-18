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
#include "Kdtree.h"
#include "Scene.h"
using namespace std;



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
};

#endif
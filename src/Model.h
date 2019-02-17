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
using namespace std;



class Model {
public:
    Model(string path) {
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
            }
        } 
    }

private:
    map<string, Material> materialTable;
    AABB box;
    vector<vec3f> points;
    vector<vec3f> normals;
    
private:
    void readObj(string path) {
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
                readObj(mtl_path);
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
                vector<int> temp_f; 
                char* str; 
                char* sub;
                file.getline(str, 200);
                temp_f.clear();
                sub = strtok(str, " ");
                sub = strtok(NULL, " ");
                while (sub) {
                    std::string s(sub);
                    int pos = -1;
                    for(int j = 0; j < s.length(); ++j) {
                        if(s[j] == '/') {
                            pos = j;
                            break;
                        }
                    }
                    if (pos>0) temp_f.push_back(atoi(s.substr(0, pos).data()) - 1);
                    else temp_f.push_back(atoi(sub) - 1);
                    sub = strtok(NULL, " ");
                }

            }
        }
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
    }
};

#endif
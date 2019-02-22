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
using namespace std;


// class Triangle;
class Model {
public:
    Model(string path) {
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
        // printf("hhh\n");
        kdtree.triangle = &triangles;
        kdtree.points = &points;
        kdtree.normals = &normals;
        kdtree.init();
    }

   

public:
    map<string, Material> materialTable;
    vector<vec3f> points;
    vector<vec3f> normals;
    vector<Triangle> triangles;
    Kdtree kdtree;
private:
    void readObj(string path);
    void readMtl(string path);


};
void Model::readObj(string path) {
    printf("%s\n", path.c_str());
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
            sub = strtok(str, " ");
            int index = 0;
            while (sub) {
                string seq(sub);
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
                    Triangle tra;
                    tra.material = material;
                    for(int i = 0; i < 3; ++i) {
                        tra.vertex[i] = vertex[i];
                        tra.vertexNormal[i] = vertexNormal[i];
                    }
                    vec3f p0 = points[tra.vertex[0]];
                    vec3f p1 = points[tra.vertex[1]];
                    vec3f p2 = points[tra.vertex[2]];
                    tra.origin = p0;
                    tra.edge1 = p1 - p0;
                    tra.edge2 = p2 - p0;
                    tra.normal = normalize(cross(tra.edge1, tra.edge2));
                    tra.mat = Mat4(p0, p1, p2);
                    tra.mat.inverse();

                    tra.center = (points[vertex[0]] + points[vertex[1]] + points[vertex[2]]) / 3;
                    triangles.push_back(tra);

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
void Model::readMtl(string path) {
    printf("%s\n", path.c_str());
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
    file.close();
} 
#endif
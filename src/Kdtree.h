#ifndef __KDTREE_H
#define __KDTREE_H
#include "Util.h"
#include "AABB.h"
#include <iostream>
using namespace std;



struct Node{
    Triangle split;
    Node *lc, *rc;
    int l, r;
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
    int minLeaf = 5;
    Node *root;
    vector<Triangle> *triangle;
    vector<vec3f> *points;
    vector<vec3f> *normals;
public:
    Kdtree() {
        // root = new Node();
    }
    void init() {
        // printf("%d %d %d\n", triangle->size(), points->size(), normals->size());
        root = build(0, triangle->size(), 0);
    }

    void burceIntersect(Node *rt, Ray& ray, int div) {
        for(int i = 0, len = triangle->size(); i < len; ++i) {
            update(ray, (*triangle)[i]);
        }
    } 
    void intersect(Node *rt, Ray& ray, int div) {
        if(!rt->abstract.intersect(ray)) return;
        // if(rt->r - rt->l < minLeaf) {
        //     for(int i = rt->l; i < rt->r; ++i) {
        //         update(ray, (*triangle)[i]);
        //     }
        //     return;
        // }
        update(ray, rt->split);
        if (ray.direction[div] >= 0) {
            if(rt->lc != NULL) intersect(rt->lc, ray, (div + 1) % 3);
            if(rt->rc != NULL) intersect(rt->rc, ray, (div + 1) % 3);
        } else {
            if(rt->rc != NULL) intersect(rt->rc, ray, (div + 1) % 3);           
            if(rt->lc != NULL) intersect(rt->lc, ray, (div + 1) % 3);       
        }
    }

    void notIntersect(Node* rt, Ray& ray, int div) {
        if(ray.hit) return;
        if(!rt->abstract.intersect(ray)) return;
        // if(rt->r - rt->l < minLeaf) {
        //     for(int i = rt->l; i < rt->r; ++i) {
        //         update(ray, (*triangle)[i]);
        //     }
        //     return;
        // }
        update(ray, rt->split);
        if (ray.direction[div] >= 0) {
            if(rt->lc != NULL) intersect(rt->lc, ray, (div + 1) % 3);
            if(rt->rc != NULL && !ray.hit) intersect(rt->rc, ray, (div + 1) % 3);
        } else {
            if(rt->rc != NULL) intersect(rt->rc, ray, (div + 1) % 3);           
            if(rt->lc != NULL && !ray.hit) intersect(rt->lc, ray, (div + 1) % 3);       
        }
    }

private:
    Node* build(int l, int r, int div) {
        // printf("%d %d\n", l, r);
        if(l >= r) return NULL;
        Node *p = new Node();        
        p->div = div;
        p->l = l; p->r = r;
        // if(r - l < minLeaf) { 
        //     p->abstract = getAABB((*triangle)[l]);
        //     for(int i = l + 1; i < r; ++i) {
        //         p->abstract = merge(p->abstract, getAABB((*triangle)[i]));
        //     }
        //     return p;
        // }
        int mid = (l + r) / 2;
        nth_element(triangle->begin() + l, triangle->begin() + mid, triangle->begin() + r, cmpx(div));
        p->split = (*triangle)[mid];
        p->lc = build(l, mid, (div + 1) % 3);
        p->rc = build(mid + 1, r, (div + 1) % 3);
        p->abstract = getAABB((*triangle)[mid]);
        if(p->lc != NULL) p->abstract = merge(p->lc->abstract, p->abstract);
        if(p->rc != NULL) p->abstract = merge(p->rc->abstract, p->abstract);
        return p;
    }

    

    void update(Ray& ray, Triangle tri) {
        if (abs(dot(tri.normal, ray.direction))<EPS) return;

        vec3f v = tri.origin - ray.pos;

        vec3f tmp1 = cross(tri.edge2, ray.direction);
        float beta = -dot(tmp1, v) / dot(tmp1, tri.edge1);

        vec3f tmp2 = cross(tri.edge1, ray.direction);
        float gamma = -dot(tmp2, v) / dot(tmp2, tri.edge2);

        float t = dot(tri.normal, v) / dot(tri.normal, ray.direction);
        if (beta > 0 && gamma > 0 && beta + gamma < 1 && ray.isWithinBounds(t)) {
            ray.hit = true;
            ray.hitLight = -1;
            ray.hitPoint = ray.getPoint(t);
            vec3f abg = tri.mat * ray.hitPoint;
            abg.x = abs(abg.x);
            abg.y = abs(abg.y);
            abg.z = abs(abg.z);

            ray.hitNormal = normalize(abg.x*((*normals)[tri.vertexNormal[0]]) + abg.y*((*normals)[tri.vertexNormal[1]]) + abg.z*((*normals)[tri.vertexNormal[2]]));
            ray.hitTriangle = tri;
            ray.tmax = t;
        }
    }

    AABB getAABB(Triangle& tri) {
        return merge(merge((*points)[tri.vertex[0]], (*points)[tri.vertex[1]]), (*points)[tri.vertex[2]]);
    }

};

#endif
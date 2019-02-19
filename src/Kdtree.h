#ifndef __KDTREE_H
#define __KDTREE_H
#include "Util.h"
#include "AABB.h"
#include <iostream>
using namespace std;



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

    void intersect(Node *rt, Ray& ray, int div) {
        if(!rt->abstract.intersect(ray)) return;
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
        int mid = (l + r) / 2;
        nth_element(triangle->begin() + l, triangle->begin() + mid, triangle->begin() + r, cmpx(div));
        p->split = (*triangle)[mid];
        p->lc = build(l, mid, (div + 1) % 3);
        p->rc = build(mid + 1, r, (div + 1) % 3);
        p->abstract = AABB((*triangle)[mid].center, (*triangle)[mid].center);
        if(p->lc != NULL) p->abstract = merge(p->lc->abstract, p->abstract);
        if(p->rc != NULL) p->abstract = merge(p->rc->abstract, p->abstract);
        return p;
    }

    void update(Ray& ray, Triangle tri) {
        vec3f p0 = (*points)[tri.vertex[0]];
        vec3f p1 = (*points)[tri.vertex[1]];
        vec3f p2 = (*points)[tri.vertex[2]];
        vec3f origin = p0;
        vec3f edge1 = p1 - p0;
        vec3f edge2 = p2 - p0;
        vec3f normal = normalize(cross(edge1, edge2));
        Mat4 tmp = Mat4(p0, p1, p2);
        tmp.inverse();

        if (abs(dot(normal, ray.direction))<EPS) return;

        vec3f v = origin - ray.pos;

        vec3f tmp1 = cross(edge2, ray.direction);
        float beta = -dot(tmp1, v) / dot(tmp1, edge1);

        vec3f tmp2 = cross(edge1, ray.direction);
        float gamma = -dot(tmp2, v) / dot(tmp2, edge2);

        float t = dot(normal, v) / dot(normal, ray.direction);
        if (beta > 0 && gamma > 0 && beta + gamma < 1 && ray.isWithinBounds(t)) {
            ray.hit = true;
            ray.hitPoint = ray.getPoint(t);
            vec3f abg = tmp * ray.hitPoint;
            abg.x = abs(abg.x);
            abg.y = abs(abg.y);
            abg.z = abs(abg.z);

            ray.hitNormal = normalize(abg.x*((*normals)[tri.vertexNormal[0]]) + abg.y*((*normals)[tri.vertexNormal[1]]) + abg.z*((*normals)[tri.vertexNormal[2]]));
            ray.hitTriangle = tri;
            ray.tmax = t;
        }
    }

};

#endif
#ifndef __KDTREE_H
#define __KDTREE_H

#include <iostream>
#include <cstdio>
#include <cstring>
#include <vector>
#include "Point.h"
#include "Util.h"
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
public:
    Kdtree() {
        // root = new Node();
    }
    void init() {
        root = build(0, triangle->size(), 0);
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
        return p;
    }

};



#endif
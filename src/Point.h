#ifndef __POINT_H
#define __POINT_H

#include <cmath>

const float EPS = 1e-5;


bool isEqual(float a, float b) {
	if (fabs(a - b) < EPS) {
		return true;
	}
	return false;
}

class vec3f {
public:
	union {
		struct{ float x, y, z; };
		struct{ float r, g, b; };
        float n[3];
	};

	vec3f() :x(0.0f), y(0.0f), z(0.0f){};
	vec3f(float _x, float _y, float _z):x(_x), y(_y), z(_z) {}

    inline vec3f operator - () { return vec3f(-x, -y, -z); }
    vec3f flip(){ return vec3f(-x, -y, -z); };
	void print() {
		for(int i = 0; i < 3; ++i) printf("%.3f ", n[i]); printf("\n");
	}
	inline bool operator == (const vec3f &u) { return isEqual(x, u.x) && isEqual(y, u.y) && isEqual(z, u.z); }
	inline bool operator != (const vec3f &u) { return !(isEqual(x, u.x) && isEqual(y, u.y) && isEqual(z, u.z)); }

	inline vec3f& operator = (const vec3f &v) { x = v.x; y = v.y; z = v.z; return *this; }
	inline vec3f& operator += (float num) { x += num; y += num; z += num; return *this; }
	inline vec3f& operator += (const vec3f &v) { x += v.x; y += v.y; z += v.z; return *this; }
	inline vec3f& operator -= (float num) { x -= num; y -= num; z -= num; return *this; }
	inline vec3f& operator -= (const vec3f &v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
	inline vec3f& operator *= (float num) { x *= num; y *= num; z *= num; return *this; }
	inline vec3f& operator *= (const vec3f &v) { x *= v.x; y *= v.y; z *= v.z; return *this; }
	inline vec3f& operator /= (float num) { x /= num; y /= num; z /= num; return *this; }
	inline vec3f& operator /= (const vec3f &v) { x /= v.x; y /= v.y; z /= v.z; return *this; }
	friend inline vec3f operator + (const vec3f &u, float num) { return vec3f(u.x + num, u.y + num, u.z + num); }
    friend inline vec3f operator + (float num, const vec3f &u) { return vec3f(num + u.x, num + u.y, num + u.z); }
	friend inline vec3f operator + (const vec3f &u, const vec3f &v) { return vec3f(u.x + v.x, u.y + v.y, u.z + v.z); }
    friend inline vec3f operator - (const vec3f &u, float num) { return vec3f(u.x - num, u.y - num, u.z - num); }
    friend inline vec3f operator - (float num, const vec3f &u) { return vec3f(num - u.x, num - u.y, num - u.z); }
	friend inline vec3f operator - (const vec3f &u, const vec3f &v) { return vec3f(u.x - v.x, u.y - v.y, u.z - v.z); }
	friend inline vec3f operator * (const vec3f &u, float num) { return vec3f(u.x * num, u.y * num, u.z * num); }
	friend inline vec3f operator * (float num, const vec3f &u) { return vec3f(num * u.x, num * u.y, num * u.z); }
	friend inline vec3f operator * (const vec3f &u, const vec3f &v) { return vec3f(u.x * v.x, u.y * v.y, u.z * v.z); }
	friend inline vec3f operator / (const vec3f &u, float num) { return vec3f(u.x / num, u.y / num, u.z / num); }
	friend inline vec3f operator / (float num, const vec3f &u) { return vec3f(num / u.x, num / u.y, num / u.z); }
	friend inline vec3f operator / (const vec3f &u, const vec3f &v) { return vec3f(u.x / v.x, u.y / v.y, u.z / v.z); }

	inline float& operator[](int i) {
		if(i > 2) {
			printf("the size is bigger than array");
			exit(0);
		}
		return n[i];
	}
};


inline vec3f cross(const vec3f &u, const vec3f &v) {
	return vec3f(u.y * v.z - u.z * v.y, u.z * v.x - u.x * v.z, u.x * v.y - u.y * v.x);
}

inline float dot(const vec3f &u, const vec3f &v) {
	return u.x * v.x + u.y * v.y + u.z * v.z;
}

inline float length(const vec3f &u) {
	return sqrt(u.x * u.x + u.y * u.y + u.z * u.z);
}

inline vec3f normalize(const vec3f &u) {
	return u * (1.0f / sqrt(u.x * u.x + u.y * u.y + u.z * u.z));
}


const vec3f BLACK = vec3f(0.0f, 0.0f, 0.0f);

#endif

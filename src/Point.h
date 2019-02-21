#ifndef __POINT_H
#define __POINT_H

#include <cmath>
using namespace std;
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
    float sum() {
        return n[0] + n[1] + n[2];
    }
    float Max() {
        return max(max(x, y), z);
    }
    float Min() {
        return min(min(x, y), z);
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

class Mat4 {
public:
    float mat[16];
    Mat4() {
        mat[0] = 1.0; mat[4] = 0.0; mat[8] = 0.0; mat[12] = 0.0;
        mat[1] = 0.0; mat[5] = 1.0; mat[9] = 0.0; mat[13] = 0.0;
        mat[2] = 0.0; mat[6] = 0.0; mat[10] = 1.0; mat[14] = 0.0;
        mat[3] = 0.0; mat[7] = 0.0; mat[11] = 0.0; mat[15] = 1.0;
    }
    Mat4(vec3f& v0, vec3f& v1, vec3f& v2) {
        mat[0] = v0.x; mat[4] = v1.x; mat[8] = v2.x; mat[12] = 0.0f;
        mat[1] = v0.y; mat[5] = v1.y; mat[9] = v2.y; mat[13] = 0.0f;
        mat[2] = v0.z; mat[6] = v1.z; mat[10] = v2.z; mat[14] = 0.0f;
        mat[3] = 0.0f; mat[7] = 0.0f; mat[11] = 0.0f; mat[15] = 1.0f;
    }
    ~Mat4() { }
    Mat4(const Mat4 &Mat4) {
        for (int i = 0; i < 16; i++) {
            mat[i] = Mat4.mat[i];
        }
    }
    Mat4 transpose() {
        Mat4 Matrix;
        Matrix[0] = mat[0];
        Matrix[1] = mat[4];
        Matrix[2] = mat[8];
        Matrix[3] = mat[12];
        Matrix[4] = mat[1];
        Matrix[5] = mat[5];
        Matrix[6] = mat[9];
        Matrix[7] = mat[13];
        Matrix[8] = mat[2];
        Matrix[9] = mat[6];
        Matrix[10] = mat[10];
        Matrix[11] = mat[14];
        Matrix[12] = mat[3];
        Matrix[13] = mat[7];
        Matrix[14] = mat[11];
        Matrix[15] = mat[15];
        return Matrix;
    }

    bool inverse() {
        float inv[16], det;
        int i;

        inv[0] = mat[5] * mat[10] * mat[15] -
            mat[5] * mat[11] * mat[14] -
            mat[9] * mat[6] * mat[15] +
            mat[9] * mat[7] * mat[14] +
            mat[13] * mat[6] * mat[11] -
            mat[13] * mat[7] * mat[10];

        inv[4] = -mat[4] * mat[10] * mat[15] +
            mat[4] * mat[11] * mat[14] +
            mat[8] * mat[6] * mat[15] -
            mat[8] * mat[7] * mat[14] -
            mat[12] * mat[6] * mat[11] +
            mat[12] * mat[7] * mat[10];

        inv[8] = mat[4] * mat[9] * mat[15] -
            mat[4] * mat[11] * mat[13] -
            mat[8] * mat[5] * mat[15] +
            mat[8] * mat[7] * mat[13] +
            mat[12] * mat[5] * mat[11] -
            mat[12] * mat[7] * mat[9];

        inv[12] = -mat[4] * mat[9] * mat[14] +
            mat[4] * mat[10] * mat[13] +
            mat[8] * mat[5] * mat[14] -
            mat[8] * mat[6] * mat[13] -
            mat[12] * mat[5] * mat[10] +
            mat[12] * mat[6] * mat[9];

        inv[1] = -mat[1] * mat[10] * mat[15] +
            mat[1] * mat[11] * mat[14] +
            mat[9] * mat[2] * mat[15] -
            mat[9] * mat[3] * mat[14] -
            mat[13] * mat[2] * mat[11] +
            mat[13] * mat[3] * mat[10];

        inv[5] = mat[0] * mat[10] * mat[15] -
            mat[0] * mat[11] * mat[14] -
            mat[8] * mat[2] * mat[15] +
            mat[8] * mat[3] * mat[14] +
            mat[12] * mat[2] * mat[11] -
            mat[12] * mat[3] * mat[10];

        inv[9] = -mat[0] * mat[9] * mat[15] +
            mat[0] * mat[11] * mat[13] +
            mat[8] * mat[1] * mat[15] -
            mat[8] * mat[3] * mat[13] -
            mat[12] * mat[1] * mat[11] +
            mat[12] * mat[3] * mat[9];

        inv[13] = mat[0] * mat[9] * mat[14] -
            mat[0] * mat[10] * mat[13] -
            mat[8] * mat[1] * mat[14] +
            mat[8] * mat[2] * mat[13] +
            mat[12] * mat[1] * mat[10] -
            mat[12] * mat[2] * mat[9];

        inv[2] = mat[1] * mat[6] * mat[15] -
            mat[1] * mat[7] * mat[14] -
            mat[5] * mat[2] * mat[15] +
            mat[5] * mat[3] * mat[14] +
            mat[13] * mat[2] * mat[7] -
            mat[13] * mat[3] * mat[6];

        inv[6] = -mat[0] * mat[6] * mat[15] +
            mat[0] * mat[7] * mat[14] +
            mat[4] * mat[2] * mat[15] -
            mat[4] * mat[3] * mat[14] -
            mat[12] * mat[2] * mat[7] +
            mat[12] * mat[3] * mat[6];

        inv[10] = mat[0] * mat[5] * mat[15] -
            mat[0] * mat[7] * mat[13] -
            mat[4] * mat[1] * mat[15] +
            mat[4] * mat[3] * mat[13] +
            mat[12] * mat[1] * mat[7] -
            mat[12] * mat[3] * mat[5];

        inv[14] = -mat[0] * mat[5] * mat[14] +
            mat[0] * mat[6] * mat[13] +
            mat[4] * mat[1] * mat[14] -
            mat[4] * mat[2] * mat[13] -
            mat[12] * mat[1] * mat[6] +
            mat[12] * mat[2] * mat[5];

        inv[3] = -mat[1] * mat[6] * mat[11] +
            mat[1] * mat[7] * mat[10] +
            mat[5] * mat[2] * mat[11] -
            mat[5] * mat[3] * mat[10] -
            mat[9] * mat[2] * mat[7] +
            mat[9] * mat[3] * mat[6];

        inv[7] = mat[0] * mat[6] * mat[11] -
            mat[0] * mat[7] * mat[10] -
            mat[4] * mat[2] * mat[11] +
            mat[4] * mat[3] * mat[10] +
            mat[8] * mat[2] * mat[7] -
            mat[8] * mat[3] * mat[6];

        inv[11] = -mat[0] * mat[5] * mat[11] +
            mat[0] * mat[7] * mat[9] +
            mat[4] * mat[1] * mat[11] -
            mat[4] * mat[3] * mat[9] -
            mat[8] * mat[1] * mat[7] +
            mat[8] * mat[3] * mat[5];

        inv[15] = mat[0] * mat[5] * mat[10] -
            mat[0] * mat[6] * mat[9] -
            mat[4] * mat[1] * mat[10] +
            mat[4] * mat[2] * mat[9] +
            mat[8] * mat[1] * mat[6] -
            mat[8] * mat[2] * mat[5];

        det = mat[0] * inv[0] + mat[1] * inv[4] + mat[2] * inv[8] + mat[3] * inv[12];

        if (det == 0)
            return false;

        det = 1.0f / det;

        for (i = 0; i < 16; i++)
            mat[i] = inv[i] * det;

        return true;
    }

    Mat4& operator = (const Mat4 &Mat4) {
        for (int i = 0; i < 16; i++) {
            mat[i] = Mat4.mat[i];
        }
        return *this;
    }

    inline float& operator [] (int Index) {
        return mat[Index];
    }
    inline float* operator & () {
        return (float*)this;
    }
    friend inline Mat4 operator * (const Mat4 &Matrix1, const Mat4 &Matrix2) {
        Mat4 Matrix3;

        Matrix3.mat[0] = Matrix1.mat[0] * Matrix2.mat[0] + Matrix1.mat[4] * Matrix2.mat[1] + Matrix1.mat[8] * Matrix2.mat[2] + Matrix1.mat[12] * Matrix2.mat[3];
        Matrix3.mat[1] = Matrix1.mat[1] * Matrix2.mat[0] + Matrix1.mat[5] * Matrix2.mat[1] + Matrix1.mat[9] * Matrix2.mat[2] + Matrix1.mat[13] * Matrix2.mat[3];
        Matrix3.mat[2] = Matrix1.mat[2] * Matrix2.mat[0] + Matrix1.mat[6] * Matrix2.mat[1] + Matrix1.mat[10] * Matrix2.mat[2] + Matrix1.mat[14] * Matrix2.mat[3];
        Matrix3.mat[3] = Matrix1.mat[3] * Matrix2.mat[0] + Matrix1.mat[7] * Matrix2.mat[1] + Matrix1.mat[11] * Matrix2.mat[2] + Matrix1.mat[15] * Matrix2.mat[3];

        Matrix3.mat[4] = Matrix1.mat[0] * Matrix2.mat[4] + Matrix1.mat[4] * Matrix2.mat[5] + Matrix1.mat[8] * Matrix2.mat[6] + Matrix1.mat[12] * Matrix2.mat[7];
        Matrix3.mat[5] = Matrix1.mat[1] * Matrix2.mat[4] + Matrix1.mat[5] * Matrix2.mat[5] + Matrix1.mat[9] * Matrix2.mat[6] + Matrix1.mat[13] * Matrix2.mat[7];
        Matrix3.mat[6] = Matrix1.mat[2] * Matrix2.mat[4] + Matrix1.mat[6] * Matrix2.mat[5] + Matrix1.mat[10] * Matrix2.mat[6] + Matrix1.mat[14] * Matrix2.mat[7];
        Matrix3.mat[7] = Matrix1.mat[3] * Matrix2.mat[4] + Matrix1.mat[7] * Matrix2.mat[5] + Matrix1.mat[11] * Matrix2.mat[6] + Matrix1.mat[15] * Matrix2.mat[7];

        Matrix3.mat[8] = Matrix1.mat[0] * Matrix2.mat[8] + Matrix1.mat[4] * Matrix2.mat[9] + Matrix1.mat[8] * Matrix2.mat[10] + Matrix1.mat[12] * Matrix2.mat[11];
        Matrix3.mat[9] = Matrix1.mat[1] * Matrix2.mat[8] + Matrix1.mat[5] * Matrix2.mat[9] + Matrix1.mat[9] * Matrix2.mat[10] + Matrix1.mat[13] * Matrix2.mat[11];
        Matrix3.mat[10] = Matrix1.mat[2] * Matrix2.mat[8] + Matrix1.mat[6] * Matrix2.mat[9] + Matrix1.mat[10] * Matrix2.mat[10] + Matrix1.mat[14] * Matrix2.mat[11];
        Matrix3.mat[11] = Matrix1.mat[3] * Matrix2.mat[8] + Matrix1.mat[7] * Matrix2.mat[9] + Matrix1.mat[11] * Matrix2.mat[10] + Matrix1.mat[15] * Matrix2.mat[11];

        Matrix3.mat[12] = Matrix1.mat[0] * Matrix2.mat[12] + Matrix1.mat[4] * Matrix2.mat[13] + Matrix1.mat[8] * Matrix2.mat[14] + Matrix1.mat[12] * Matrix2.mat[15];
        Matrix3.mat[13] = Matrix1.mat[1] * Matrix2.mat[12] + Matrix1.mat[5] * Matrix2.mat[13] + Matrix1.mat[9] * Matrix2.mat[14] + Matrix1.mat[13] * Matrix2.mat[15];
        Matrix3.mat[14] = Matrix1.mat[2] * Matrix2.mat[12] + Matrix1.mat[6] * Matrix2.mat[13] + Matrix1.mat[10] * Matrix2.mat[14] + Matrix1.mat[14] * Matrix2.mat[15];
        Matrix3.mat[15] = Matrix1.mat[3] * Matrix2.mat[12] + Matrix1.mat[7] * Matrix2.mat[13] + Matrix1.mat[11] * Matrix2.mat[14] + Matrix1.mat[15] * Matrix2.mat[15];

        return Matrix3;
    }
    
    friend inline vec3f operator * (Mat4 &mat4, vec3f &Vector)
    {
        vec3f v;
        v.x = mat4.mat[0] * Vector.x + mat4.mat[4] * Vector.y + mat4.mat[8] * Vector.z;
        v.y = mat4.mat[1] * Vector.x + mat4.mat[5] * Vector.y + mat4.mat[9] * Vector.z;
        v.z = mat4.mat[2] * Vector.x + mat4.mat[6] * Vector.y + mat4.mat[10] * Vector.z;

        return v;

    }
};

#endif

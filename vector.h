#pragma once
#include <d3d9.h>
#include "../D3DX\d3dx9.h"

#define M_PI 3.14159265358979323846264338327950288419716939937510

class Vector2 {
public:
	float x, y;

	Vector2();
	Vector2(float x, float y);
	Vector2(float* vector);

	bool IsNaN();

	float* ToArray();

	float Distance(Vector2 vec);

	Vector2 operator=(Vector2 value);
	Vector2 operator=(float* value);
	Vector2 operator=(float value);

	Vector2 operator+(Vector2 value);
	Vector2 operator+(float* value);
	Vector2 operator+(float value);

	Vector2 operator-(Vector2 value);
	Vector2 operator-(float* value);
	Vector2 operator-(float value);

	Vector2 operator*(Vector2 value);
	Vector2 operator*(float* value);
	Vector2 operator*(float value);

	Vector2 operator/(Vector2 value);
	Vector2 operator/(float* value);
	Vector2 operator/(float value);

	bool operator==(Vector2 value);
	bool operator==(float* value);
	bool operator==(float value);

	bool operator!=(Vector2 value);
	bool operator!=(float* value);
	bool operator!=(float value);
};

inline float sqrtf_(float number)
{
    float temp, mysqrt;
    mysqrt = number / 2;
    temp = 0;
    while (mysqrt != temp) {
        temp = mysqrt;
        mysqrt = (number / temp + temp) / 2;
    }
    return mysqrt;
}

class Vector3
{
public:
    Vector3() : x(0.f), y(0.f), z(0.f)
    {

    }

    Vector3(double _x, double _y, double _z) : x(_x), y(_y), z(_z)
    {

    }
    ~Vector3()
    {

    }

    double x;
    double y;
    double z;

    inline double Dot(Vector3 v)
    {
        return x * v.x + y * v.y + z * v.z;
    }

    inline double Distance(Vector3 v)
    {
        return double(sqrtf_(powf(v.x - x, 2.0) + powf(v.y - y, 2.0) + powf(v.z - z, 2.0)));
    }

    Vector3 operator+(Vector3 v)
    {
        return Vector3(x + v.x, y + v.y, z + v.z);
    }

    Vector3 operator-(Vector3 v)
    {
        return Vector3(x - v.x, y - v.y, z - v.z);
    }

    Vector3 operator*(double number) const {
        return Vector3(x * number, y * number, z * number);
    }
};

struct FQuat
{
    double x;
    double y;
    double z;
    double w;
};

double Dude_Distance(Vector3 u, Vector3 v)
{
    double x = u.x - v.x;
    double y = u.y - v.y;
    double z = u.z - v.z;

    return sqrtf_((x * x) + (y * y) + (z * z)) * 0.0101f;
}

struct FTransform
{
    FQuat rot;
    Vector3 translation;
    char pad[4];
    Vector3 scale;
    char pad1[4];

    D3DMATRIX ToMatrixWithScale()
    {
        D3DMATRIX m;
        m._41 = translation.x;
        m._42 = translation.y;
        m._43 = translation.z;

        float x2 = rot.x + rot.x;
        float y2 = rot.y + rot.y;
        float z2 = rot.z + rot.z;

        float xx2 = rot.x * x2;
        float yy2 = rot.y * y2;
        float zz2 = rot.z * z2;
        m._11 = (1.0f - (yy2 + zz2)) * scale.x;
        m._22 = (1.0f - (xx2 + zz2)) * scale.y;
        m._33 = (1.0f - (xx2 + yy2)) * scale.z;

        float yz2 = rot.y * z2;
        float wx2 = rot.w * x2;
        m._32 = (yz2 - wx2) * scale.z;
        m._23 = (yz2 + wx2) * scale.y;

        float xy2 = rot.x * y2;
        float wz2 = rot.w * z2;
        m._21 = (xy2 - wz2) * scale.y;
        m._12 = (xy2 + wz2) * scale.x;

        float xz2 = rot.x * z2;
        float wy2 = rot.w * y2;
        m._31 = (xz2 + wy2) * scale.z;
        m._13 = (xz2 - wy2) * scale.x;

        m._14 = 0.0f;
        m._24 = 0.0f;
        m._34 = 0.0f;
        m._44 = 1.0f;

        return m;
    }
};
D3DMATRIX MatrixMultiplication(D3DMATRIX pM1, D3DMATRIX pM2)
{
    D3DMATRIX pOut;
    pOut._11 = pM1._11 * pM2._11 + pM1._12 * pM2._21 + pM1._13 * pM2._31 + pM1._14 * pM2._41;
    pOut._12 = pM1._11 * pM2._12 + pM1._12 * pM2._22 + pM1._13 * pM2._32 + pM1._14 * pM2._42;
    pOut._13 = pM1._11 * pM2._13 + pM1._12 * pM2._23 + pM1._13 * pM2._33 + pM1._14 * pM2._43;
    pOut._14 = pM1._11 * pM2._14 + pM1._12 * pM2._24 + pM1._13 * pM2._34 + pM1._14 * pM2._44;
    pOut._21 = pM1._21 * pM2._11 + pM1._22 * pM2._21 + pM1._23 * pM2._31 + pM1._24 * pM2._41;
    pOut._22 = pM1._21 * pM2._12 + pM1._22 * pM2._22 + pM1._23 * pM2._32 + pM1._24 * pM2._42;
    pOut._23 = pM1._21 * pM2._13 + pM1._22 * pM2._23 + pM1._23 * pM2._33 + pM1._24 * pM2._43;
    pOut._24 = pM1._21 * pM2._14 + pM1._22 * pM2._24 + pM1._23 * pM2._34 + pM1._24 * pM2._44;
    pOut._31 = pM1._31 * pM2._11 + pM1._32 * pM2._21 + pM1._33 * pM2._31 + pM1._34 * pM2._41;
    pOut._32 = pM1._31 * pM2._12 + pM1._32 * pM2._22 + pM1._33 * pM2._32 + pM1._34 * pM2._42;
    pOut._33 = pM1._31 * pM2._13 + pM1._32 * pM2._23 + pM1._33 * pM2._33 + pM1._34 * pM2._43;
    pOut._34 = pM1._31 * pM2._14 + pM1._32 * pM2._24 + pM1._33 * pM2._34 + pM1._34 * pM2._44;
    pOut._41 = pM1._41 * pM2._11 + pM1._42 * pM2._21 + pM1._43 * pM2._31 + pM1._44 * pM2._41;
    pOut._42 = pM1._41 * pM2._12 + pM1._42 * pM2._22 + pM1._43 * pM2._32 + pM1._44 * pM2._42;
    pOut._43 = pM1._41 * pM2._13 + pM1._42 * pM2._23 + pM1._43 * pM2._33 + pM1._44 * pM2._43;
    pOut._44 = pM1._41 * pM2._14 + pM1._42 * pM2._24 + pM1._43 * pM2._34 + pM1._44 * pM2._44;

    return pOut;
}
struct FVector {
    float X, Y, Z;

    inline bool IsZero()
    {
        if (X == 0 && Y == 0 && Z == 0)
            return true;

        return false;
    }
};





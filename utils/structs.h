#pragma once
#include <iostream>

struct Color
{
    float R, G, B;
};

struct Vector3
{
    float x, y, z;

    Vector3(float x, float y, float z)
        : x(x), y(y), z(z) {}
    Vector3() {}

    Vector3 Add(const Vector3& other) const
    {
        return Vector3(this->x + other.x, this->y + other.y, this->z + other.z);
    }
    Vector3 operator+(const Vector3& other) const
    {
        return Add(other);
    }
    void operator+=(const Vector3& other)
    {
        this->x += other.x;
        this->y += other.y;
        this->z += other.z;
    }

    Vector3 scalarMultiply(const float& other) const
    {
        return Vector3(this->x * other, this->y * other, this->z * other);
    }
    Vector3 operator*(const float& other) const
    {
        return scalarMultiply(other);
    }
    Vector3 operator-(const Vector3& other) const
    {
        return Vector3(this->x - other.x, this->y - other.y, this->z - other.z);
    }
};

std::ostream& operator<<(std::ostream& stream, const Vector3 &other)
{
    stream << '(' << other.x << ", " << other.y << ", " << other.z << ')';
    return stream;
}
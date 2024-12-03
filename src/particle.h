#ifndef PARTICLE_H
#define PARTICLE_H

#include <math.h>

namespace Particle{
    typedef struct Vector3{
        float x;
        float y;
        float z;

        Vector3 operator+(const Vector3& other) const{
            return Vector3{x + other.x, y + other.y, z + other.z};
        }
        void operator+=(const Vector3& other) {
            x += other.x;
            y += other.y;
            z += other.z;
        }
        Vector3 operator-(const Vector3& other) const{ // Gets a vector pointing from other to this.
            return Vector3{x- other.x, y - other.y, z - other.z};
        }
        void operator-=(const Vector3& other) {
            x -= other.x;
            y -= other.y;
            z -= other.z;
        }
        Vector3 operator*(const float other) const{
            return Vector3{x * other, y * other, z * other};
        }
        void operator*=(const float other) {
            x *= other;
            y *= other;
            z *= other;
        }
        Vector3 operator/(const float other) const{
            return Vector3{x/other, y/other, z/other};
        }
        bool operator==(const Vector3& other) const{
            return x == other.x && y == other.y && z == other.z;
        }

        Vector3 normalize(){
            float magnitude = std::sqrt(x*x + y*y + z*z);
            return Vector3{x/magnitude, y/magnitude, z/magnitude};
        }

    }Vector3;

    typedef struct Particle{
        Vector3 position;
        Vector3 velocity;
        Vector3 force;
        float mass;
        float radius;
        float targetRadius;
        bool isBeingDestroyed;
    } Particle;

    inline float distance_squared(const Vector3& v1, const Vector3& v2){
        float dX = v1.x - v2.x;
        float dY = v1.y - v2.y;
        float dZ = v1.z - v2.z;
        return dX*dX + dY*dY + dZ*dZ;
    }

}

#endif
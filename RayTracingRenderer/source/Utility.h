#pragma once

#include "Constants.h"

#include <random>
#include <cstdlib>

inline double DegreeToRadians(double degreses)
{
	return degreses * pi / 180.0;
}

//inline double random_double() {
//    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
//    static std::mt19937 generator;
//    return distribution(generator);
//}

inline double RandomDouble() 
{
    // Random double in [0,1).
    return rand() / (RAND_MAX + 1.0);
}

inline double RandomDouble(double min, double max) 
{
    // Random double in [min,max).
    return min + (max - min) * RandomDouble();
}

inline Vector3 RandomVector() 
{
    return Vector3(RandomDouble(), RandomDouble(), RandomDouble());
}

inline Vector3 RandomVector(double min, double max) 
{
    return Vector3(RandomDouble(min, max), RandomDouble(min, max), RandomDouble(min, max));
}

Vector3 RandomVectorInUnitSphere() 
{
    while (true) 
    {
        auto v = RandomVector(-1, 1);
        if (v.LengthSquared() >= 1)
        {
            continue;
        }
        return v;
    }
}

Vector3 RandomUnitVector()
{
    return Normalize(RandomVectorInUnitSphere());
}

Vector3 RandomVectorInHemisphere(const Vector3& normal)
{
    auto vector = RandomVectorInUnitSphere();
    if (Dot(vector, normal) > 0.0)
    {
        return vector;
    }
    else
    {
        return -vector;
    }
}

double Reflectance(double cosine, double refractionIndex)
{
    // Schlick's approximation for reflectance.
    auto r0 = (1 - refractionIndex) / (1 + refractionIndex);
    r0 = r0 * r0;
    return r0 + (1 - r0) * std::pow((1 - cosine), 5);
}

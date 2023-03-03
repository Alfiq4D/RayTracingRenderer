#pragma once

#include "Vector3.h"

class Ray
{
public:
	Ray() {};
	Ray(const Point3& origin, const Vector3& direction) :
		origin(origin),
		direction(Normalize(direction))
	{}

	Point3 Origin() const 
	{
		return origin; 
	}

	Vector3 Direction() const
	{
		return direction;
	}

	Point3 At(double t) const
	{
		return origin + t * direction;
	}

private:
	Point3 origin;
	Vector3 direction;
};
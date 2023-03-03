#pragma once

#include "Ray.h"

struct HitRecord
{
	Point3 point;
	Vector3 normal;
	double t;
	bool frontFace;

	inline void SetFaceNormal(const Ray& ray, const Vector3& outwardNormal)
	{
		frontFace = Dot(ray.Direction(), outwardNormal) < 0;
		normal = frontFace ? outwardNormal : -outwardNormal;
	}
};

class Hittable
{
public:
	virtual bool Hit(const Ray& ray, double tMin, double tMax, HitRecord& record) const = 0;
};

#pragma once

#include "Ray.h"

#include <memory>

namespace rtr
{
	class Material;

	struct HitRecord
	{
		Point3 point;
		Vector3 normal;
		double t;
		bool frontFace;
		std::shared_ptr<Material> hittedMaterial;

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
}

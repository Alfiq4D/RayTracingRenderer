#pragma once

#include "HittableObject.h"
#include "Vector3.h"

namespace rtr
{
	class Sphere : public Hittable
	{
	public:
		Sphere() : center(Point3(0.0, 0.0, 0.0)), radius(0.0) {}
		Sphere(Point3 center, double radius, std::shared_ptr<Material> material) : center(center), radius(radius), material(material) {};

		virtual bool Hit(const Ray& ray, double tMin, double tMax, HitRecord& record) const override;

		Point3 center;
		double radius;
		std::shared_ptr<Material> material;
	};

	bool Sphere::Hit(const Ray& ray, double tMin, double tMax, HitRecord& record) const
	{
		Vector3 originCenterVector = ray.Origin() - center;
		auto a = ray.Direction().LengthSquared();
		auto halfB = Dot(originCenterVector, ray.Direction());
		auto c = originCenterVector.LengthSquared() - radius * radius;

		auto discriminant = halfB * halfB - a * c;
		if (discriminant < 0)
		{
			return false;
		}
		auto sqrtDiscriminant = sqrt(discriminant);

		auto root = (-halfB - sqrtDiscriminant) / a;
		if (root < tMin || root > tMax)
		{
			root = (-halfB + sqrtDiscriminant) / a;
			if (root < tMin || root > tMax)
			{
				return false;
			}
		}

		record.t = root;
		record.point = ray.At(root);
		Vector3 outwardNormal = (record.point - center) / radius;
		record.SetFaceNormal(ray, outwardNormal);
		record.hittedMaterial = material;

		return true;
	}
}

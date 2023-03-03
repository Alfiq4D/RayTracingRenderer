#pragma once

#include "HittableObject.h"

#include <memory>
#include <vector>

class Scene
{
public:
	Scene() {}

	void Clear()
	{
		objects.clear();
	}

	void Add(std::shared_ptr<Hittable> object)
	{
		objects.push_back(object);
	}

	bool Hit(const Ray& ray, double tMin, double tMax, HitRecord& hitRecord) const;

private:
	std::vector<std::shared_ptr<Hittable>> objects;
};

bool Scene::Hit(const Ray& ray, double tMin, double tMax, HitRecord& hitRecord) const
{
	HitRecord tmpHit;
	bool hit = false;
	double tClosest = tMax;

	for (const auto& object : objects)
	{
		if (object->Hit(ray, tMin, tClosest, tmpHit))
		{
			hit = true;
			tClosest = tmpHit.t;
			hitRecord = tmpHit;
		}
	}

	return hit;
}

#pragma once

#include "Color.h"
#include "HittableObject.h"
#include "Utility.h"

class Material
{
public:
	virtual bool Scatter(const Ray& inputRay, const HitRecord& hitRecord, Color& attenuation, Ray& scatteredRay) const = 0;
};

class LambertianMaterial : public Material
{
public:
	LambertianMaterial(const Color& color) : albedo(color) {}

	virtual bool Scatter(const Ray& inputRay, const HitRecord& hitRecord, Color& attenuation, Ray& scatteredRay) const override
	{
		// Pseudo Lambertial reflection - harder shadows
		// auto scatterDirection = hitRecord.normal + RandomVectorInUnitSphere();
		// Hemisphere model - even softer shadows
		// auto scatterDirection = RandomVectorInHemisphere(hitRecord.normal);
		// True Lambertian reflection
		auto scatterDirection = hitRecord.normal + RandomUnitVector();

		if (scatterDirection.IsNearZero())
		{
			scatterDirection = hitRecord.normal;
		}

		scatteredRay = Ray(hitRecord.point, scatterDirection);
		attenuation = albedo;
		return true;

		// OR scatter with probability p and attenuate with albedo/p.
	}

	Color albedo;
};

class MetalMaterial : public Material
{
public:
	MetalMaterial(const Color& color, double fuziness) : albedo(color), fuziness(std::clamp(fuziness, 0.0, 1.0)) {}

	virtual bool Scatter(const Ray& inputRay, const HitRecord& hitRecord, Color& attenuation, Ray& scatteredRay) const override
	{
		Vector3 reflectedVector = Reflect(inputRay.Direction(), hitRecord.normal);
		scatteredRay = Ray(hitRecord.point, reflectedVector + fuziness * RandomVectorInUnitSphere());
		attenuation = albedo;

		return (Dot(scatteredRay.Direction(), hitRecord.normal) > 0);
	}

	Color albedo;
	double fuziness;
};

class DielectricMaterial : public Material
{
public:
	DielectricMaterial(double indexOfRefraction) : ir(indexOfRefraction) {}

	virtual bool Scatter(const Ray& inputRay, const HitRecord& hitRecord, Color& attenuation, Ray& scatteredRay) const override
	{
		attenuation = Color(1.0, 1.0, 1.0);
		double refractonRatio = hitRecord.frontFace ? (1.0 / ir) : ir;

		// Why fmin here?
		double cosTheta = std::fmin(Dot(-inputRay.Direction(), hitRecord.normal), 1.0);
		double sinTheta = std::sqrt(1.0 - cosTheta * cosTheta);
		bool cannotRefeact = refractonRatio * sinTheta > 1.0;
		Vector3 direction;

		if (cannotRefeact || Reflectance(cosTheta, refractonRatio) > RandomDouble())
		{
			direction = Reflect(inputRay.Direction(), hitRecord.normal);
		}
		else
		{
			direction = Refract(inputRay.Direction(), hitRecord.normal, refractonRatio);
		}

		scatteredRay = Ray(hitRecord.point, direction);
		return true;
	}

	double ir; // Index of Refraction.
};

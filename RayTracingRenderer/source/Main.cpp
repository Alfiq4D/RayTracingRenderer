#include "Color.h"
#include "Ray.h"
#include "Vector3.h"
#include "Sphere.h"
#include "Scene.h"
#include "Constants.h"
#include "Utility.h"
#include "Camera.h"

#include <iostream>

Color RayColor(const Ray& r, const Scene& scene, int depth)
{
	HitRecord hitRecord;

	// If the ray bounce limit is exceeded, no more light is gathered.
	if (depth <= 0)
	{
		return Color(0.0, 0.0, 0.0);
	}

	if (scene.Hit(r, 0.001, infinity, hitRecord))
	{
		// Pseudo Lambertial reflection - harder shadows
		// Point3 target = hitRecord.point + hitRecord.normal + RandomVectorInUnitSphere();
		// Hemisphere model - even softer shadows
		// Point3 target = hitRecord.point + RandomVectorInHemisphere(hitRecord.normal);
		// True Lambertian reflection
		Point3 target = hitRecord.point + hitRecord.normal + RandomVectorInUnitSphere();
		return 0.5 * RayColor(Ray(hitRecord.point, target - hitRecord.point), scene, depth - 1);
		// Visualize normals on the scene.
		// 0.5 * Color(hitRecord.normal.X() + 1, hitRecord.normal.Y() + 1, hitRecord.normal.Z() + 1);
	}
	Vector3 unitDirection = r.Direction();
	auto t = 0.5 * (unitDirection.Y() + 1.0);

	return (1.0 - t) * Color(1.0, 1.0, 1.0) + t * Color(0.5, 0.7, 1.0);
}

int main()
{
	// Image parameters.
	const double aspectRatio = 16.0 / 9.0;
	const int imageWidth = 400;
	const int imageHeight = static_cast<int>(imageWidth / aspectRatio);
	const int samplesPerPixel = 100;
	const int maxDepth = 50;

	// Camera.
	auto viewportHeight = 2.0;
	auto viewportWidth = aspectRatio * viewportHeight;
	auto focalLength = 1.0;
	Camera camera(viewportWidth, viewportHeight, focalLength);
	// Scene.

	Scene scene;
	scene.Add(std::make_shared<Sphere>(Point3(0.0, 0.0, -1.0), 0.5));
	scene.Add(std::make_shared<Sphere>(Point3(0.0, -50.5, -1.0), 50.0));

	// Render image.
	std::cout << "P3\n" << imageWidth << ' ' << imageHeight << "\n255\n";

	for (int j = imageHeight - 1; j >= 0; --j)
	{
		std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
		for (int i = 0; i < imageWidth; i++)
		{
			Color pixelColor(0.0, 0.0, 0.0);
			for (int sample = 0; sample < samplesPerPixel; sample++)
			{
				auto u = (i + RandomDouble()) / (imageWidth - 1);
				auto v = (j + RandomDouble()) / (imageHeight - 1);
				Ray ray = camera.GetRay(u, v);
				pixelColor += RayColor(ray, scene, maxDepth);
			}
			pixelColor.Normalize(samplesPerPixel);
			std::cout << pixelColor;
		}
	}

	std::cerr << "\nDone\n";
}
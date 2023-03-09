#include "Color.h"
#include "Renderer.h"
#include "Vector3.h"
#include "Sphere.h"
#include "Scene.h"
#include "Constants.h"
#include "Utility.h"
#include "Camera.h"
#include "Material.h"
#include "Image.h"

#include <iostream>


namespace rtr
{
	Scene GeneratePreviewScene()
	{
		// Materials.
		auto groundMaterial = std::make_shared<LambertianMaterial>(Color(0.8, 0.8, 0.8));
		auto centerMaterial = std::make_shared<LambertianMaterial>(Color(0.7, 0.3, 0.3));
		auto leftMaterial = std::make_shared<DielectricMaterial>(1.5);
		auto rightMaterial = std::make_shared<MetalMaterial>(Color(0.8, 0.6, 0.2), 0.2);

		// Scene.
		Scene scene;
		scene.Add(std::make_shared<Sphere>(Point3(0.0, -50.5, -1.0), 50.0, groundMaterial));
		scene.Add(std::make_shared<Sphere>(Point3(0.0, 0.0, -1.0), 0.5, centerMaterial));
		scene.Add(std::make_shared<Sphere>(Point3(-1.0, 0.0, -1.0), 0.5, leftMaterial));
		scene.Add(std::make_shared<Sphere>(Point3(-1.0, 0.0, -1.0), -0.4, leftMaterial));
		scene.Add(std::make_shared<Sphere>(Point3(1.0, 0.0, -1.0), 0.5, rightMaterial));

		return scene;
	}

	Scene GenerateRandomScene()
	{
		Scene scene;

		auto groundMaterial = std::make_shared<LambertianMaterial>(Color(0.8, 0.8, 0.8));
		scene.Add(std::make_shared<Sphere>(Point3(0, -1000, 0), 1000, groundMaterial));

		for (int a = -11; a < 11; a++) {
			for (int b = -11; b < 11; b++) {
				auto materialSelection = util::RandomDouble();
				Point3 center(a + 0.9 * util::RandomDouble(), 0.2, b + 0.9 * util::RandomDouble());

				if ((center - Point3(4, 0.2, 0)).Lenght() > 0.9) {
					std::shared_ptr<Material> sphereMaterial;

					if (materialSelection < 0.8) {
						// Diffuse.
						auto albedo = util::RandomColor() * util::RandomColor();
						sphereMaterial = std::make_shared<LambertianMaterial>(albedo);
						scene.Add(std::make_shared<Sphere>(center, 0.2, sphereMaterial));
					}
					else if (materialSelection < 0.95) {
						// Metal.
						auto albedo = util::RandomColor(0.5, 1);
						auto fuzz = util::RandomDouble(0, 0.5);
						sphereMaterial = std::make_shared<MetalMaterial>(albedo, fuzz);
						scene.Add(std::make_shared<Sphere>(center, 0.2, sphereMaterial));
					}
					else {
						// Glass.
						sphereMaterial = std::make_shared<DielectricMaterial>(1.5);
						scene.Add(std::make_shared<Sphere>(center, 0.2, sphereMaterial));
					}
				}
			}
		}

		auto material1 = std::make_shared<DielectricMaterial>(1.5);
		scene.Add(std::make_shared<Sphere>(Point3(0, 1, 0), 1.0, material1));

		auto material2 = std::make_shared<LambertianMaterial>(Color(0.4, 0.2, 0.1));
		scene.Add(std::make_shared<Sphere>(Point3(-4, 1, 0), 1.0, material2));

		auto material3 = std::make_shared<MetalMaterial>(Color(0.7, 0.6, 0.5), 0.0);
		scene.Add(std::make_shared<Sphere>(Point3(4, 1, 0), 1.0, material3));

		return scene;
	}
}

int main()
{
	const std::string fileName = "C:/Users/Kamil/source/repos/RayTracingRenderer/x64/Release/image.jpg";

	// Image parameters.
	const double aspectRatio = 16.0 / 9.0;
	const int imageWidth = 400;
	const int imageHeight = static_cast<int>(imageWidth / aspectRatio);
	const int samplesPerPixel = 30;
	const int maxDepth = 50;

	// Camera.
	rtr::Point3 cameraPosition(13, 2, 3);
	rtr::Point3 cameraLookAt(0, 0, 0);
	rtr::Point3 cameraFocusPoint(0, 0, 0);
	rtr::Vector3 viewUp(0, 1, 0);
	double cameraFov = 20.0;
	double cameraAperture = 0.1;
	double cameraDistToFocus = 10;

	rtr::Camera camera(cameraPosition, cameraLookAt, viewUp, cameraFov, aspectRatio, cameraAperture, cameraDistToFocus);

	auto scene = rtr::GenerateRandomScene();

	std::vector<float> imageBuffer;
	imageBuffer.resize(static_cast<size_t>(imageWidth) * imageHeight * 3, 0.0f);
	std::vector<float> imageOutBuffer;
	imageOutBuffer.resize(static_cast<size_t>(imageWidth) * imageHeight * 3, 0.0f);

	rtr::Renderer renderer(imageWidth, imageHeight);
	renderer.RenderImage(camera, scene, samplesPerPixel, maxDepth, imageBuffer);
	renderer.DenoiseImage(imageBuffer, imageOutBuffer);
	rtr::SaveImage(fileName, imageOutBuffer, imageWidth, imageHeight);

	std::cout << "Done\n";
}

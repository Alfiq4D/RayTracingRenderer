#pragma once

#include "Color.h"
#include "Ray.h"
#include "Camera.h"
#include "HittableObject.h"
#include "Scene.h"
#include "Material.h"

#include <iostream>
#include <vector>
#include <chrono>
#include <ppl.h>
#include <OpenImageDenoise/oidn.hpp>

namespace rtr
{
	class Renderer
	{
	public:

		Renderer(int renderWidth, int renderHeight) : imageWidth(renderWidth), imageHeight(renderHeight) {}

		void RenderImage(const Camera& camera, const Scene& scene, int samplesPerPixel, int maxDepth, std::vector<float>& imageOutBuffer)
		{	
			concurrency::critical_section criticalSection;
			int renderedLinesCounter = 0;
			const auto startTime = std::chrono::high_resolution_clock::now();

			concurrency::parallel_for(int(0), imageHeight, [&](int k)
				{
					int j = imageHeight - 1 - k;
					for (int i = 0; i < imageWidth; i++)
					{
						Color pixelColor(0.0, 0.0, 0.0);
						for (int sample = 0; sample < samplesPerPixel; sample++)
						{
							auto u = (i + util::RandomDouble()) / (imageWidth - 1);
							auto v = (j + util::RandomDouble()) / (imageHeight - 1);
							rtr::Ray ray = camera.GetRay(u, v);
							pixelColor += RayColor(ray, scene, maxDepth);
						}
						pixelColor.Normalize(samplesPerPixel);
						pixelColor.CorrectGamma();
						imageOutBuffer[(imageHeight - 1 - j) * imageWidth * 3 + i * 3] = static_cast<float>(pixelColor.R());
						imageOutBuffer[(imageHeight - 1 - j) * imageWidth * 3 + i * 3 + 1] = static_cast<float>(pixelColor.G());
						imageOutBuffer[(imageHeight - 1 - j) * imageWidth * 3 + i * 3 + 2] = static_cast<float>(pixelColor.B());
					}

					criticalSection.lock();
					renderedLinesCounter++;
					if (renderedLinesCounter % 50 == 0)
					{
						std::cout << "Rendered lines: " << renderedLinesCounter << "/" << imageHeight << '\n';
					}
					criticalSection.unlock();
				});

			const auto endTime = std::chrono::high_resolution_clock::now();
			std::cout << "Image render time:: " << std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(endTime - startTime).count() << " ms" << '\n';
		}

		void RenderAlbedo(const Camera& camera, const Scene& scene, int samplesPerPixel, std::vector<float>& imageOutBuffer)
		{
			const auto startTime = std::chrono::high_resolution_clock::now();
			concurrency::parallel_for(int(0), imageHeight, [&](int k)
				{
					int j = imageHeight - 1 - k;
					for (int i = 0; i < imageWidth; i++)
					{
						Color pixelColor(0.0, 0.0, 0.0);
						for (int sample = 0; sample < samplesPerPixel; sample++)
						{
							auto u = (i + util::RandomDouble()) / (imageWidth - 1);
							auto v = (j + util::RandomDouble()) / (imageHeight - 1);
							rtr::Ray ray = camera.GetRay(u, v);
							pixelColor += RayAlbedo(ray, scene);
						}
						pixelColor.Normalize(samplesPerPixel);
						pixelColor.CorrectGamma();
						imageOutBuffer[(imageHeight - 1 - j) * imageWidth * 3 + i * 3] = static_cast<float>(pixelColor.R());
						imageOutBuffer[(imageHeight - 1 - j) * imageWidth * 3 + i * 3 + 1] = static_cast<float>(pixelColor.G());
						imageOutBuffer[(imageHeight - 1 - j) * imageWidth * 3 + i * 3 + 2] = static_cast<float>(pixelColor.B());
					}
				});

			const auto endTime = std::chrono::high_resolution_clock::now();
			std::cout << "Albedo render time:: " << std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(endTime - startTime).count() << " ms" << '\n';
		}

		void RenderNormal(const Camera& camera, const Scene& scene, int samplesPerPixel, std::vector<float>& imageOutBuffer)
		{
			const auto startTime = std::chrono::high_resolution_clock::now();
			concurrency::parallel_for(int(0), imageHeight, [&](int k)
				{
					int j = imageHeight - 1 - k;
					for (int i = 0; i < imageWidth; i++)
					{
						Color pixelColor(0.0, 0.0, 0.0);
						for (int sample = 0; sample < samplesPerPixel; sample++)
						{
							auto u = (i + util::RandomDouble()) / (imageWidth - 1);
							auto v = (j + util::RandomDouble()) / (imageHeight - 1);
							rtr::Ray ray = camera.GetRay(u, v);
							pixelColor += RayNormal(ray, scene);
						}
						pixelColor = pixelColor / samplesPerPixel;
						imageOutBuffer[(imageHeight - 1 - j) * imageWidth * 3 + i * 3] = static_cast<float>(pixelColor.R());
						imageOutBuffer[(imageHeight - 1 - j) * imageWidth * 3 + i * 3 + 1] = static_cast<float>(pixelColor.G());
						imageOutBuffer[(imageHeight - 1 - j) * imageWidth * 3 + i * 3 + 2] = static_cast<float>(pixelColor.B());
					}
				});

			const auto endTime = std::chrono::high_resolution_clock::now();
			std::cout << "Normal render time:: " << std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(endTime - startTime).count() << " ms" << '\n';
		}

		void DenoiseImage(const std::vector<float>& imageBuffer, const std::vector<float>& albedoBuffer, const std::vector<float>& normalBuffer, std::vector<float>& imageOutBuffer)
		{
			oidn::DeviceRef device = oidn::newDevice();
			device.commit();

			// Create a filter for denoising a color image using optional auxiliary images.
			oidn::FilterRef filter = device.newFilter("RT"); // generic ray tracing filter
			filter.setImage("color", (void*)&imageBuffer[0], oidn::Format::Float3, imageWidth, imageHeight);
			filter.setImage("albedo", (void*)&albedoBuffer[0], oidn::Format::Float3, imageWidth, imageHeight); // auxiliary
			filter.setImage("normal", (void*)&normalBuffer[0], oidn::Format::Float3, imageWidth, imageHeight); // auxiliary
			filter.setImage("output", (void*)&imageOutBuffer[0], oidn::Format::Float3, imageWidth, imageHeight); // denoised
			filter.set("hdr", false); // image is HDR
			filter.commit();

			// Filter the image.
			filter.execute();

			// Check for errors.
			const char* errorMessage;
			if (device.getError(errorMessage) != oidn::Error::None)
			{
				std::cout << "Error: " << errorMessage << std::endl;
			}
		}

	private:
		Color RayColor(const Ray& r, const Scene& scene, int depth)
		{
			HitRecord hitRecord;

			// If the ray bounce limit is exceeded, no more light is gathered.
			if (depth <= 0)
			{
				return Color(0.0, 0.0, 0.0);
			}

			if (scene.Hit(r, 0.001, consts::infinity, hitRecord))
			{
				Ray scatteredRay;
				Color attenuation;
				if (hitRecord.hittedMaterial->Scatter(r, hitRecord, attenuation, scatteredRay))
				{
					return attenuation * RayColor(scatteredRay, scene, depth - 1);
				}
				return Color(0.0, 0.0, 0.0);
			}
			Vector3 unitDirection = r.Direction();
			auto t = 0.5 * (unitDirection.Y() + 1.0);

			return (1.0 - t) * Color(1.0, 1.0, 1.0) + t * Color(0.5, 0.7, 1.0);
		}

		Color RayAlbedo(const Ray& r, const Scene& scene)
		{
			HitRecord hitRecord;
			if (scene.Hit(r, 0.001, consts::infinity, hitRecord))
			{
				Ray scatteredRay;
				Color attenuation;
				hitRecord.hittedMaterial->Scatter(r, hitRecord, attenuation, scatteredRay);
				return attenuation;
			}
			Vector3 unitDirection = r.Direction();
			auto t = 0.5 * (unitDirection.Y() + 1.0);

			return (1.0 - t) * Color(1.0, 1.0, 1.0) + t * Color(0.5, 0.7, 1.0);
		}

		Color RayNormal(const Ray& r, const Scene& scene)
		{
			HitRecord hitRecord;
			if (scene.Hit(r, 0.001, consts::infinity, hitRecord))
			{
				// Visualize normals on the scene.
				// 0.5 * Color(hitRecord.normal.X() + 1, hitRecord.normal.Y() + 1, hitRecord.normal.Z() + 1);
				return Color(hitRecord.normal.X(), hitRecord.normal.Y(), hitRecord.normal.Z());
			}
			Vector3 unitDirection = r.Direction();
			auto t = 0.5 * (unitDirection.Y() + 1.0);

			return -Color(r.Direction().X(), r.Direction().Y(), r.Direction().Z());
		}

	private:
		int imageWidth;
		int imageHeight;
	};
}

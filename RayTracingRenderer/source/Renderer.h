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
			std::cout << "Render time:: " << std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(endTime - startTime).count() << " ms" << '\n';
		}

		void DenoiseImage(const std::vector<float>& imageBuffer, std::vector<float>& imageOutBuffer)
		{
			oidn::DeviceRef device = oidn::newDevice();
			device.commit();

			// Create a filter for denoising a beauty (color) image using optional auxiliary images too
			oidn::FilterRef filter = device.newFilter("RT"); // generic ray tracing filter
			filter.setImage("color", (void*)&imageBuffer[0], oidn::Format::Float3, imageWidth, imageHeight);
			//filter.setImage("albedo", albedoPtr, oidn::Format::Float3, width, height); // auxiliary
			//filter.setImage("normal", normalPtr, oidn::Format::Float3, width, height); // auxiliary
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


			//// Create a filter for denoising a beauty (color) image using prefiltered auxiliary images too
			//oidn::FilterRef filter = device.newFilter("RT"); // generic ray tracing filter
			//filter.setImage("color", colorPtr, oidn::Format::Float3, width, height); // beauty
			//filter.setImage("albedo", albedoPtr, oidn::Format::Float3, width, height); // auxiliary
			//filter.setImage("normal", normalPtr, oidn::Format::Float3, width, height); // auxiliary
			//filter.setImage("output", outputPtr, oidn::Format::Float3, width, height); // denoised beauty
			//filter.set("hdr", true); // beauty image is HDR
			//filter.set("cleanAux", true); // auxiliary images will be prefiltered
			//filter.commit();

			//// Create a separate filter for denoising an auxiliary albedo image (in-place)
			//oidn::FilterRef albedoFilter = device.newFilter("RT"); // same filter type as for beauty
			//albedoFilter.setImage("albedo", albedoPtr, oidn::Format::Float3, width, height);
			//albedoFilter.setImage("output", albedoPtr, oidn::Format::Float3, width, height);
			//albedoFilter.commit();

			//// Create a separate filter for denoising an auxiliary normal image (in-place)
			//oidn::FilterRef normalFilter = device.newFilter("RT"); // same filter type as for beauty
			//normalFilter.setImage("normal", normalPtr, oidn::Format::Float3, width, height);
			//normalFilter.setImage("output", normalPtr, oidn::Format::Float3, width, height);
			//normalFilter.commit();

			//// Prefilter the auxiliary images
			//albedoFilter.execute();
			//normalFilter.execute();

			//// Filter the beauty image
			//filter.execute();
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
				// Visualize normals on the scene.
				// 0.5 * Color(hitRecord.normal.X() + 1, hitRecord.normal.Y() + 1, hitRecord.normal.Z() + 1);
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

	private:
		int imageWidth;
		int imageHeight;
	};
}

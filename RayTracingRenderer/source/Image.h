#pragma once

#include "Constants.h"
#include "Utility.h"
#include "Color.h"

#include <vector>
#include <string>
#include <fstream>

#define __STDC_LIB_EXT1__
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

namespace rtr
{
	std::vector<unsigned char> ConvertFloatBufferToBytes(const std::vector<float>& imageBuffer, int imageWidth, int imageHeight)
	{
		std::vector<unsigned char> charBuffer;
		charBuffer.resize(static_cast<size_t>(imageWidth) * imageHeight * consts::channels, 0);

		for (size_t i = 0; i < imageBuffer.size(); i++)
		{
			charBuffer[i] = DoubleToByteColor(imageBuffer[i]);
		}

		return charBuffer;
	}

	void SaveImage(const std::string fileName, const std::vector<float>& imageBuffer, int imageWidth, int imageHeight)
	{
		auto extension = fileName.substr(fileName.find_last_of(".") + 1);
		if (extension == "ppm")
		{
			std::ofstream file(fileName);
			file << "P3\n" << imageWidth << ' ' << imageHeight << "\n255\n";
			for (int i = 0; i < imageWidth * imageHeight * consts::channels; i += consts::channels)
			{
				auto pixelColor = Color(imageBuffer[i], imageBuffer[i + 1], imageBuffer[i + 2]);
				file << pixelColor;
			}
		}
		else if (extension == "png")
		{
			auto byteBuffer = ConvertFloatBufferToBytes(imageBuffer, imageWidth, imageHeight);
			stbi_write_png(fileName.c_str(), imageWidth, imageHeight, consts::channels, (void*)&byteBuffer[0], 0);
		}
		else if (extension == "bmp")
		{
			auto byteBuffer = ConvertFloatBufferToBytes(imageBuffer, imageWidth, imageHeight);
			stbi_write_bmp(fileName.c_str(), imageWidth, imageHeight, consts::channels, (void*)&byteBuffer[0]);
		}
		else if (extension == "jpg")
		{
			auto byteBuffer = ConvertFloatBufferToBytes(imageBuffer, imageWidth, imageHeight);
			stbi_write_jpg(fileName.c_str(), imageWidth, imageHeight, consts::channels, (void*)&byteBuffer[0], 100);
		}
		else
		{
			std::cout << "Not supported file extension.\n";
		}
	}
}
#pragma once

#include "Vector3.h"

class Camera
{
public:
	Camera(Point3 position, Point3 lookAt, Vector3 viewUpVector, double vFov, double aspectRatio) : origin(position)
	{
        auto theta = DegreeToRadians(vFov);
        auto height = std::tan(theta / 2.0);
        auto viewportHeight = 2.0 * height;
        auto viewportWidth = aspectRatio * viewportHeight;

        // Focal lenght == 1.
        auto cameraZ = Normalize(origin - lookAt);
        auto cameraX = Normalize(Cross(viewUpVector, cameraZ));
        auto cameraY = Cross(cameraZ, cameraX);

        horizontalVector = viewportWidth * cameraX;
        verticalVector = viewportHeight * cameraY;
        lowerLeftOffset = -horizontalVector / 2 - verticalVector / 2 - cameraZ;
    }

    Ray GetRay(double u, double v) const {
        return Ray(origin, lowerLeftOffset + u * horizontalVector + v * verticalVector);
    }

private:
    Point3 origin;
    Point3 lowerLeftOffset;
    Vector3 horizontalVector;
    Vector3 verticalVector;
};
#pragma once

#include "Vector3.h"

class Camera
{
public:
	Camera(double viewportWidth, double viewportHeight, double focalLength)
	{
        origin = Point3(0, 0, 0);
        horizontal = Vector3(viewportWidth, 0.0, 0.0);
        vertical = Vector3(0.0, viewportHeight, 0.0);
        // TODO: base direction
        lowerLeftCorner = origin - horizontal / 2 - vertical / 2 - Vector3(0, 0, focalLength);
    }

    Ray GetRay(double u, double v) const {
        return Ray(origin, lowerLeftCorner + u * horizontal + v * vertical - origin);
    }

private:
    Point3 origin;
    Point3 lowerLeftCorner;
    Vector3 horizontal;
    Vector3 vertical;
};
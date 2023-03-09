#pragma once

#include "Vector3.h"
#include "Utility.h"

namespace rtr
{
    class Camera
    {
    public:
        Camera(Point3 position, Point3 lookAt, Vector3 viewUpVector, double vFov, double aspectRatio, double aperture, double focusDistance) : origin(position)
        {
            auto theta = util::DegreeToRadians(vFov);
            auto height = std::tan(theta / 2.0);
            auto viewportHeight = 2.0 * height;
            auto viewportWidth = aspectRatio * viewportHeight;

            // Focal lenght == 1.
            cameraZ = Normalize(origin - lookAt);
            cameraX = Normalize(Cross(viewUpVector, cameraZ));
            cameraY = Cross(cameraZ, cameraX);

            horizontalVector = focusDistance * viewportWidth * cameraX;
            verticalVector = focusDistance * viewportHeight * cameraY;
            lowerLeftOffset = -horizontalVector / 2 - verticalVector / 2 - focusDistance * cameraZ;

            lensRadius = aperture / 2;
        }

        Ray GetRay(double u, double v) const
        {
            Vector3 offsetDisk = lensRadius * util::RandomVectorInUnitDisk();
            Vector3 offsetVector = cameraX * offsetDisk.X() + cameraY * offsetDisk.Y();

            return Ray(origin + offsetVector, lowerLeftOffset + u * horizontalVector + v * verticalVector - offsetVector);
        }

        void SetAperture(double aperture)
        {
            lensRadius = aperture / 2;
        }

    private:
        Point3 origin;
        Point3 lowerLeftOffset;
        Vector3 horizontalVector;
        Vector3 verticalVector;
        Vector3 cameraX;
        Vector3 cameraY;
        Vector3 cameraZ;
        double lensRadius;
    };
}

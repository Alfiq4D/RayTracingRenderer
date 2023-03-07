#pragma once

#include "Constants.h"

#include <cmath>
#include <iostream>

namespace rtr
{
	class Vector3
	{
	public:
		Vector3() : x(0.0), y(0.0), z(0.0) {}

		Vector3(double x, double y, double z) : x(x), y(y), z(z) {}

		double X() const
		{
			return x;
		}

		double Y() const
		{
			return y;
		}

		double Z() const
		{
			return z;
		}

		double& X()
		{
			return x;
		}

		double& Y()
		{
			return y;
		}

		double& Z()
		{
			return z;
		}

		Vector3 operator-() const
		{
			return Vector3(-x, -y, -z);
		}

		Vector3& operator+=(const Vector3& vector)
		{
			x += vector.x;
			y += vector.y;
			z += vector.z;
			return *this;
		}

		Vector3& operator*=(const double d)
		{
			x *= d;
			y *= d;
			z *= d;
			return *this;
		}

		Vector3& operator/=(const double d)
		{
			return *this *= 1 / d;
		}

		double Lenght() const
		{
			return std::sqrt(LengthSquared());
		}

		double LengthSquared() const
		{
			return x * x + y * y + z * z;
		}

		bool IsNearZero()
		{
			return (std::fabs(x) < consts::eps) && (std::fabs(y) < consts::eps) && (std::fabs(z) < consts::eps);
		}

	private:
		double x;
		double y;
		double z;
	};

	using Point3 = Vector3;

	inline std::ostream& operator<<(std::ostream& out, const Vector3& v)
	{
		return out << v.X() << ' ' << v.Y() << ' ' << v.Z();
	}

	inline Vector3 operator+(const Vector3& v, const Vector3& u)
	{
		return Vector3(u.X() + v.X(), u.Y() + v.Y(), u.Z() + v.Z());
	}

	inline Vector3 operator-(const Vector3& u, const Vector3& v)
	{
		return Vector3(u.X() - v.X(), u.Y() - v.Y(), u.Z() - v.Z());
	}

	inline Vector3 operator*(const Vector3& u, const Vector3& v)
	{
		return Vector3(u.X() * v.X(), u.Y() * v.Y(), u.Z() * v.Z());
	}

	inline Vector3 operator*(double d, const Vector3& v)
	{
		return Vector3(d * v.X(), d * v.Y(), d * v.Z());
	}

	inline Vector3 operator*(const Vector3& v, double d)
	{
		return d * v;
	}

	inline Vector3 operator/(const Vector3& v, double d)
	{
		return (1 / d) * v;
	}

	inline double Dot(const Vector3& u, const Vector3& v)
	{
		return u.X() * v.X() + u.Y() * v.Y() + u.Z() * v.Z();
	}

	inline Vector3 Cross(const Vector3& u, const Vector3& v)
	{
		return Vector3(u.Y() * v.Z() - u.Z() * v.Y(),
			u.Z() * v.X() - u.X() * v.Z(),
			u.X() * v.Y() - u.Y() * v.X());
	}

	inline Vector3 Normalize(const Vector3& v)
	{
		return v / v.Lenght();
	}

	Vector3 Reflect(const Vector3& v, const Vector3& n)
	{
		return v - 2 * Dot(v, n) * n;
	}

	Vector3 Refract(const Vector3& v, const Vector3& n, double refractionRatio)
	{
		auto cosTheta = std::fmin(Dot(-v, n), 1.0);
		auto refractedPerp = refractionRatio * (v + cosTheta * n);
		auto refractedParallel = -std::sqrt(std::fabs(1.0 - refractedPerp.LengthSquared())) * n;
		return refractedPerp + refractedParallel;
	}
}

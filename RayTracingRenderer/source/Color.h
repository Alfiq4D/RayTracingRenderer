#pragma once

#include <algorithm>
#include <iostream>

namespace rtr
{
	class Color
	{
	public:
		Color() : r(0.0), g(0.0), b(0.0) {}
		Color(double r, double g, double b) : r(r), g(g), b(b) {}

		double R() const
		{
			return r;
		}

		double G() const
		{
			return g;
		}

		double B() const
		{
			return b;
		}

		double& R()
		{
			return r;
		}

		double& G()
		{
			return g;
		}

		double& B()
		{
			return b;
		}

		Color operator-() const
		{
			return Color(-r, -g, -b);
		}

		Color& operator+=(const Color& color)
		{
			r += color.r;
			g += color.g;
			b += color.b;
			return *this;
		}

		Color& operator*=(const double d)
		{
			r *= d;
			g *= d;
			b *= d;
			return *this;
		}

		Color& operator/=(const double d)
		{
			return *this *= 1 / d;
		}

		void Normalize(int samplesCount)
		{
			double scale = 1.0 / samplesCount;
			r = std::clamp(r * scale, 0.0, 1.0);
			g = std::clamp(g * scale, 0.0, 1.0);
			b = std::clamp(b * scale, 0.0, 1.0);
		}

		void CorrectGamma()
		{
			// TODO: Better gamma correction.
			r = sqrt(r);
			g = sqrt(g);
			b = sqrt(b);
		}

	private:
		double r;
		double g;
		double b;
	};

	inline Color operator+(const Color& c1, const Color& c2)
	{
		return Color(c2.R() + c1.R(), c2.G() + c1.G(), c2.B() + c1.B());
	}

	inline Color operator-(const Color& c1, const Color& c2)
	{
		return Color(c1.R() - c2.R(), c1.G() - c2.G(), c1.B() - c2.B());
	}

	inline Color operator*(const Color& c1, const Color& c2)
	{
		return Color(c1.R() * c2.R(), c1.G() * c2.G(), c1.B() * c2.B());
	}

	inline Color operator*(double d, const Color& c)
	{
		return Color(d * c.R(), d * c.G(), d * c.B());
	}

	inline Color operator*(const Color& c, double d)
	{
		return d * c;
	}

	inline Color operator/(const Color& c, double d)
	{
		return (1 / d) * c;
	}

	std::ostream& operator<<(std::ostream& out, Color& color)
	{
		out << static_cast<int>(255.999 * color.R()) << ' '
			<< static_cast<int>(255.999 * color.G()) << ' '
			<< static_cast<int>(255.999 * color.B()) << '\n';
		return out;
	}
}

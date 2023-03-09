#pragma once

#include <limits>

namespace rtr::consts
{
	const double infinity = std::numeric_limits<double>::infinity();
	const double pi = 3.1415926535897932385;
	const double eps = 1e-8;
	const int channels = 3;
	const double doubleToByteRatio = 255.999;
}

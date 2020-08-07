#pragma once

#include "Constants.h"
#include <cstddef>
#include <vector>

namespace CommandSerializer
{
	using Buffer = std::vector<unsigned char>;

	Buffer EscapeSpecials(const Buffer& src);
	unsigned char sumChecksum(const Buffer& src);
}

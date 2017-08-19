#pragma once

#include <vector>

namespace IMAGE {
	struct rgba
	{
		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t a;
	};
	typedef std::vector<uint8_t> buffer;
	typedef std::vector<rgba> pixels;

	namespace BLP {
		bool Write(const pixels& input, buffer& output, int Width, int Height, int Quality);
		bool Read(const buffer& input, pixels& output, int* Width = NULL, int* Height = NULL);
	}
	namespace BMP {
		bool Write(const pixels& input, buffer& output, int Width, int Height, int Quality);
		bool Read(const buffer& input, pixels& output, int* Width = NULL, int* Height = NULL);
	}
	namespace JPEG {
		bool Write(const pixels& input, buffer& output, int Width, int Height, int Quality);
		bool Read(const buffer& input, pixels& output, unsigned int Width, unsigned int Height);
	}
}

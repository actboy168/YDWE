#pragma once

#include <stdint.h>

namespace hash
{
	class sha1
	{
	public:
		sha1();
		void update(const unsigned char* buf, size_t len);
		void final(unsigned char* digest);

	private:
		uint32_t state[5];
		uint32_t count[2];
		unsigned char buffer[64];
	};
}

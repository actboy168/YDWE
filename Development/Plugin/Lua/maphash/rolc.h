#pragma once

#include <stdint.h>

namespace hash
{
	class rolc {
	public:
		rolc()
			: init(false)
			, h(0)
		{ }

		static uint32_t rol3(uint32_t x)
		{
			return (x << 3) | (x >> 29);
		}

		void update(const unsigned char* buf, size_t len)
		{
			uint32_t h = 0;
			size_t i = 0;
			for (; i + 3 < len; i += 4) {
				h = rol3(h ^ ((uint32_t)buf[i] | ((uint32_t)buf[i + 1] << 8) | ((uint32_t)buf[i + 2] << 16) | ((uint32_t)buf[i + 3] << 24)));
			}
			for (; i < len; ++i) {
				h = rol3(h ^ (uint32_t)buf[i]);
			}
			update(h);
		}

		void update(uint32_t v)
		{
			if (!init) {
				init = true;
				h = v;
			}
			else {
				h = rol3(h ^ v);
			}
		}

		uint32_t final()
		{
			return init ? h : 0;
		}

	private:
		bool     init;
		uint32_t h;
	};
}

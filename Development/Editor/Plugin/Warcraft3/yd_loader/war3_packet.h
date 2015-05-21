#pragma once

#include <cstdint>

namespace auto_enter {

	struct war3_packet
	{
		uint8_t  magic;
		uint8_t  type;
		uint16_t size;

		bool vaild(int len)
		{
			return (this
				&& (len > 0)
				&& ((size_t)len >= sizeof(war3_packet))
				&& (this->magic == 0xF7)
				&& (this->size == (size_t)len)
				);
		}
	};

	enum WAR3_CMD_CODE_UDP
	{
		UDP_GAME_SEARCH = 0x2F,
		UDP_GAME_INFO = 0x30,
		UDP_GAME_CREATE = 0x31,
		UDP_GAME_UPDATE = 0x32,
		UDP_GAME_CANCEL = 0x33,
	};



	template <class Output, class Input>
	Output packet_decode(const Input& input)
	{
		std::deque<uint8_t> decoded;
		uint8_t mask = 0;

		for (size_t pos = 0; pos < input.size(); ++pos)
		{
			uint8_t b = (uint8_t)input[pos];

			if (b == 0) break;

			if (pos % 8 == 0)
			{
				mask = b;
			}
			else
			{
				if ((mask & (0x1 << (pos % 8))) == 0)
					decoded.push_back((uint8_t)(b - 1));
				else
					decoded.push_back(b);
			}
		}

		return std::move(Output(decoded.begin(), decoded.end()));
	}
}

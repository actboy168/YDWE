#pragma once

#include <string>
#include <cctype>
#include <sstream>
#include "Buffer.hpp"

namespace SLK
{
	namespace
	{
		std::string StringTrim(const std::string& str)
		{
			int first = 0;
			int last  = int(str.size()) - 1;

			while (first <= last && std::isspace(str[first])) ++first;
			while (last >= first && std::isspace(str[last]))  --last;

			return std::move(std::string(str, first, last - first + 1));
		}
	}

	class IniReader
	{
	public:
		template <class _Callback>
		static void Read(Buffer& buf, _Callback Fn)
		{
			static const uint32_t eof = -1; 
			std::string szSection;

			while (!buf.IsEnd())
			{
				uint32_t c = buf.Get();
				while (c != eof && std::isspace(c)) c = buf.Get();
				if (c != eof)
				{
					if (c == ';')
					{
						while (c != eof && c != '\n') c = buf.Get();
					}
					else if (c == '[')
					{
						std::string key;
						c = buf.Get();
						while (c != eof && c != ']' && c != '\n') { key += (char) c; c = buf.Get(); }

						szSection = StringTrim(key);
					}
					else
					{
						std::string key;
						while (c != eof && c != '=' && c != '\n') { key += (char) c; c = buf.Get(); }
						std::string value;
						if (c == '=')
						{
							c = buf.Get();
							while (c != eof && c != '\n') { value += (char) c; c = buf.Get(); }
						}

						if (!szSection.empty())
						{
							Fn(szSection, StringTrim(key), StringTrim(value));
						}
					}
				}
			}
		}
	};
}

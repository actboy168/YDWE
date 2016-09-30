#pragma once

#include <stdint.h>
#include <fstream>
#include <string>
#include "File.hpp"

namespace SLK
{
	class Buffer
	{
	private:
		std::string _buf;
		uint32_t    _pos;
	public:
		Buffer(): _pos(0) { }
		Buffer(std::string&& buf) : _buf(std::move(buf)), _pos(0)  { }

		~Buffer() { }

		void FromFile(std::string filename)
		{
			_buf.clear();
			_pos = 0;

			_buf = File(filename).Reads();
		}

		void ToFile(std::string szFileName)
		{
			std::ofstream os(szFileName, std::ios_base::binary);
			std::ostreambuf_iterator<char> ositer(os);  
			std::copy(_buf.begin(), _buf.end(), ositer);
			os.close();
		}

		template<class _struct>
		_struct* Read()
		{
			_struct* pRet = (_struct*)&(_buf[_pos]);
			_pos += sizeof(_struct);
			return pRet;
		}

		uint32_t Get()
		{
			if (IsEnd())
			{
				return (uint32_t)-1;
			}
			else
			{
				return (uint32_t)(* Read<uint8_t>());
			}
		}

		int GetPos()
		{
			return _pos;
		}

		void Seek(int pos)
		{
			this->_pos = pos;
		}

		uint8_t PeekChar()
		{
			return (uint8_t)(_buf[_pos]);
		}

		void Write(char* buf, uint32_t size)
		{
			_buf.append(buf, size);
		}

		bool IsEnd()
		{
			return (_buf.size() <= _pos);
		}

		std::string& GetData()
		{
			return _buf;
		}

		std::string ReadString()
		{
			uint32_t start = _pos;

			while (_buf[_pos++] != '\0') {
				nullptr;
			}

			return std::move(std::string(_buf, start, _pos - start));
		}
	};
}

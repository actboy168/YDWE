#pragma once

#include <fstream>
#include <string>
#include "Exception.hpp"

namespace SLK
{
	class File
	{
	private:
		std::ifstream _file;
	public:
		File();
		File(std::string filename)
		{
			_file.open(filename, std::ios::binary);

			if (!_file)
			{
				throw Exception("Can not open the file %s.", filename.c_str());
			}
		}

		~File() { _file.close();}

		std::string Reads()
		{
			return std::move(std::string((std::istreambuf_iterator<char>(_file)), (std::istreambuf_iterator<char>())));
		}
	};
}

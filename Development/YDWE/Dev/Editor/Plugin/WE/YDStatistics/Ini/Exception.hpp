#pragma once

#include <exception>
#include <string>
#include <vector>

namespace SLK
{
	class Exception : public std::exception 
	{   
	public:
		explicit Exception() : _msg()
		{
		}

		explicit Exception(std::string format, ...)
		{
			if (format.empty())
			{
				_msg.assign("");
				return ;
			}

			va_list argsList = NULL; 

			va_start(argsList, format); 

			size_t formattedStrLen = _vscprintf(format.c_str(), argsList) + 1;

			std::vector<char> buffer(formattedStrLen, '\0');

			int nWritten = _vsnprintf_s(&buffer[0], buffer.size(), formattedStrLen, format.c_str(), argsList);    

			if (nWritten > 0)
			{
				_msg = &buffer[0];
			}

			va_end(argsList); 
		}

		virtual ~Exception() throw () 
		{
		}

		virtual const char* what() const throw () 
		{
			return _msg.c_str();
		}

	private:
		std::string _msg;
	};
}

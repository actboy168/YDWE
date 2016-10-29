#include <base/file/stream.h>
#include <base/exception/exception.h>

namespace base { 
namespace file {

	stream::stream(const char* filename, std::ios_base::openmode mode)
	{
		file_.open(filename, std::ios::binary | mode);
	}

	stream::stream(const wchar_t* filename, std::ios_base::openmode mode)
	{
#if defined(_MSC_VER)
		file_.open(filename, std::ios::binary | mode);
#else
		file_.open(util::w2u(filename), std::ios::binary | mode);
#endif
	}

	stream::~stream()
	{
		file_.close();
	}

	bool stream::is_open() const
	{
		return !!file_;
	}

	write_stream::write_stream(const char* filename)
		: file_(filename, std::ios_base::out)
	{
		if (!file_.is_open())
		{
			throw exception("Can not open the file %s.", filename);
		}
	}

	write_stream::write_stream(const std::string& filename)
		: file_(filename.c_str(), std::ios_base::out)
	{
		if (!file_.is_open())
		{
			throw exception("Can not open the file %s.", filename.c_str());
		}
	}

	write_stream::write_stream(const wchar_t* filename)
		: file_(filename, std::ios_base::out)
	{
		if (!file_.is_open())
		{
			throw exception(L"Can not open the file %s.", filename);
		}
	}

	write_stream::write_stream(const std::wstring& filename)
		: file_(filename.c_str(), std::ios_base::out)
	{
		if (!file_.is_open())
		{
			throw exception(L"Can not open the file %s.", filename.c_str());
		}
	}

	write_stream::write_stream(const fs::path& filename)
		: file_(filename.c_str(), std::ios_base::out)
	{
		if (!file_.is_open())
		{
			throw exception(L"Can not open the file %s.", filename.c_str());
		}
	}

	read_stream::read_stream(const char* filename)
		: file_(filename, std::ios_base::in)
	{
		if (!file_.is_open())
		{
			throw exception("Can not open the file %s.", filename);
		}
	}

	read_stream::read_stream(const std::string& filename)
		: file_(filename.c_str(), std::ios_base::in)
	{
		if (!file_.is_open())
		{
			throw exception("Can not open the file %s.", filename.c_str());
		}
	}

	read_stream::read_stream(const wchar_t* filename)
		: file_(filename, std::ios_base::in)
	{
		if (!file_.is_open())
		{
			throw exception(L"Can not open the file %s.", filename);
		}
	}

	read_stream::read_stream(const std::wstring& filename)
		: file_(filename.c_str(), std::ios_base::in)
	{
		if (!file_.is_open())
		{
			throw exception(L"Can not open the file %s.", filename.c_str());
		}
	}

	read_stream::read_stream(const fs::path& filename)
		: file_(filename.c_str(), std::ios_base::in)
	{
		if (!file_.is_open())
		{
			throw exception(L"Can not open the file %s.", filename.c_str());
		}
	}
}

}

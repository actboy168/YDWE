#include <ydwe/file/steam.h>
#include <ydwe/exception/exception.h>

namespace ydwe { namespace file {

	steam::steam(const char* filename, std::ios_base::openmode mode)
	{
		file_.open(filename, std::ios::binary | mode);
	}

	steam::steam(const wchar_t* filename, std::ios_base::openmode mode)
	{
#if defined(_MSC_VER)
		file_.open(filename, std::ios::binary | mode);
#else
		file_.open(util::w2u(filename), std::ios::binary | mode);
#endif
	}

	steam::~steam()
	{
		file_.close();
	}

	bool steam::is_open() const
	{
		return !!file_;
	}

	write_steam::write_steam(const char* filename)
		: file_(filename, std::ios_base::out)
	{
		if (!file_.is_open())
		{
			throw exception(L"Can not open the file %s.", filename);
		}
	}

	write_steam::write_steam(const std::string& filename)
		: file_(filename.c_str(), std::ios_base::out)
	{
		if (!file_.is_open())
		{
			throw exception(L"Can not open the file %s.", filename.c_str());
		}
	}

	write_steam::write_steam(const wchar_t* filename)
		: file_(filename, std::ios_base::out)
	{
		if (!file_.is_open())
		{
			throw exception(L"Can not open the file %s.", filename);
		}
	}

	write_steam::write_steam(const std::wstring& filename)
		: file_(filename.c_str(), std::ios_base::out)
	{
		if (!file_.is_open())
		{
			throw exception(L"Can not open the file %s.", filename.c_str());
		}
	}

	write_steam::write_steam(const boost::filesystem::path& filename)
		: file_(filename.c_str(), std::ios_base::out)
	{
		if (!file_.is_open())
		{
			throw exception(L"Can not open the file %s.", filename.c_str());
		}
	}

	read_steam::read_steam(const char* filename)
		: file_(filename, std::ios_base::in)
	{
		if (!file_.is_open())
		{
			throw exception(L"Can not open the file %s.", filename);
		}
	}

	read_steam::read_steam(const std::string& filename)
		: file_(filename.c_str(), std::ios_base::in)
	{
		if (!file_.is_open())
		{
			throw exception(L"Can not open the file %s.", filename.c_str());
		}
	}

	read_steam::read_steam(const wchar_t* filename)
		: file_(filename, std::ios_base::in)
	{
		if (!file_.is_open())
		{
			throw exception(L"Can not open the file %s.", filename);
		}
	}

	read_steam::read_steam(const std::wstring& filename)
		: file_(filename.c_str(), std::ios_base::in)
	{
		if (!file_.is_open())
		{
			throw exception(L"Can not open the file %s.", filename.c_str());
		}
	}

	read_steam::read_steam(const boost::filesystem::path& filename)
		: file_(filename.c_str(), std::ios_base::in)
	{
		if (!file_.is_open())
		{
			throw exception(L"Can not open the file %s.", filename.c_str());
		}
	}
}}

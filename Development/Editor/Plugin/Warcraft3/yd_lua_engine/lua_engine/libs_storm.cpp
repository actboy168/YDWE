#include "lua_helper.h"
#include "storm.h"
#include <base/path/service.h>
#include <base/path/helper.h>
#include <base/util/unicode.h>
#include <boost/assign.hpp>

namespace base { namespace warcraft3 { namespace lua_engine { namespace storm {

	int storm_load(lua_State* L)
	{
		size_t path_size = 0;
		const char* path = lua_tolstring(L, 1, &path_size);

		if (!path)
		{
			lua_pushnil(L);
			return 1;
		}

		std::string path_ansi;
		try {
			path_ansi = u2a(std::string_view(path, path_size), conv_method::stop);
		}
		catch (...) {
			lua_pushnil(L);
			return 1;
		}

		const void* buf_data = nullptr;
		size_t      buf_size = 0;
		storm_dll&s = storm_s::instance();
		if (!s.load_file(path_ansi.c_str(), &buf_data, &buf_size))
		{
			lua_pushnil(L);
			return 1;
		}

		lua_pushlstring(L, (const char*)buf_data, buf_size);
		s.unload_file(buf_data);
		return 1;

	}

	bool storm_path_valid(const boost::filesystem::path& root_path, const boost::filesystem::path& file_path)
	{
		static std::set<std::wstring> s_blacklist = boost::assign::list_of
			(L"mix")(L"asi")(L"m3d")(L"flt")(L"flt")
			(L"exe")(L"dll")
			;

		std::wstring ext = file_path.extension().wstring().substr(1, 4);
		std::transform(ext.begin(), ext.end(), ext.begin(), ::towlower);

		if (s_blacklist.find(ext) != s_blacklist.end())
		{
			return false;
		}

		for (boost::filesystem::path p = file_path.parent_path(); !p.empty(); p = p.parent_path())
		{
			if (base::path::equal(p, root_path))
			{
				return true;
			}
		}
		return false;
	}

	int storm_save(lua_State* L)
	{
		const char* path = lua_tostring(L, 1);
		const char* buf_data = lua_tostring(L, 2);
		size_t      buf_size = lua_rawlen(L, 2);

		try {
			if (path && buf_data && buf_size)
			{
				boost::filesystem::path root_path = base::path::get(base::path::DIR_EXE).remove_filename();
				boost::filesystem::path file_path = root_path / base::u2w(path);
				
				if (storm_path_valid(root_path, file_path))
				{
					boost::filesystem::create_directories(file_path.parent_path());
					std::fstream fs(file_path.c_str(), std::ios::binary | std::ios::out);
					if (fs.is_open())
					{
						std::copy(buf_data, buf_data + buf_size, std::ostreambuf_iterator<char>(fs));
						lua_pushboolean(L, 1);
						return 1;
					}
				}
			}
		} catch (...) { }

		lua_pushboolean(L, 0);
		return 1;
	}

	int open(lua_State* L)
	{
		lua_newtable(L);
		{
			lua_pushstring(L, "load");
			lua_pushcclosure(L, storm_load, 0);
			lua_rawset(L, -3);

			lua_pushstring(L, "save");
			lua_pushcclosure(L, storm_save, 0);
			lua_rawset(L, -3);
		}
		return 1;
	}
}}}}

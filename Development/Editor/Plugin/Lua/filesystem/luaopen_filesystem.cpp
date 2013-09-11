
#pragma warning(push, 3)
#include <lua.hpp>
#include <luabind/luabind.hpp>
#include <luabind/object.hpp>
#include <luabind/operator.hpp>
#include <luabind/table_policy.hpp>
#include <luabind/return_reference_to_policy.hpp>
#include <luabind/iterator_policy.hpp>
#include <base/lua/luabind.h>
#pragma warning(pop)
#include <boost/filesystem.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/scoped_array.hpp>
#include <base/path/service.h>
namespace fs = boost::filesystem;

namespace NLuaAPI { namespace NFileSystemAdditional {

	static void LuaCopyFile(const fs::path &fromPath, const fs::path &toPath, bool overwritten)
	{
		fs::copy_file(fromPath, toPath, overwritten ? fs::copy_option::overwrite_if_exists : fs::copy_option::fail_if_exists);
	}

	class directory_iterator
		: public boost::iterator_facade<directory_iterator, fs::path, boost::single_pass_traversal_tag, fs::path const&>
	{
		typedef boost::iterator_facade<directory_iterator, fs::path, boost::single_pass_traversal_tag, fs::path const&> mybase;
	public:
		directory_iterator()
		{ }

		explicit directory_iterator(const fs::path& p)
			: itr_()
		{
			boost::system::error_code ec;
			itr_ = fs::directory_iterator(p, ec);
			if (!!ec)
			{
				itr_ = fs::directory_iterator();
			}
		}

		directory_iterator(const fs::path& p, boost::system::error_code& ec)
			: itr_(p, ec)
		{ }

		~directory_iterator() 
		{ }

		directory_iterator& increment(boost::system::error_code& ec)
		{
			itr_.increment(ec);
			return *this;
		}

	private:
		fs::directory_iterator itr_;

		friend class boost::iterator_core_access;

		mybase::reference dereference() const 
		{
			return itr_->path();
		}

		void increment() 
		{
			boost::system::error_code ec;
			itr_.increment(ec);
			if (!!ec)
			{
				itr_ = fs::directory_iterator();
			}
		}

		bool equal(const directory_iterator& rhs) const
		{
			return itr_ == rhs.itr_;
		}
	};

	class directory_container
	{
	public:
		directory_container(directory_container const& that)
			: p_(that.p_)
		{ }

		directory_container(fs::path const& that)
			: p_(that)
		{ }

		directory_iterator begin()
		{
			return directory_iterator(p_);
		}

		directory_iterator begin() const
		{
			return directory_iterator(p_);
		}

		directory_iterator end()
		{
			return directory_iterator();
		}

		directory_iterator end() const
		{
			return directory_iterator();
		}

	private:
		fs::path p_;
	};

	directory_container LuaDirectoryFactory(fs::path const& p)
	{
		return directory_container(p);
	}
}}


int luaopen_filesystem(lua_State *pState)
{
	using namespace luabind;

	// Bind filesystem
	module(pState, "fs")
	[
		class_<fs::path>("path")
			.def(constructor<>())
			.def(constructor<const std::string &>())
			.def(constructor<const fs::path &>())
			.def("string", (std::string (fs::path::*)() const)&fs::path::string)
			.def("generic_string", (std::string (fs::path::*)() const)&fs::path::generic_string)
			.def("native", &fs::path::native)
			.def("filename", &fs::path::filename)
			.def("root_path", &fs::path::root_path)
			.def("root_name", &fs::path::root_name)
			.def("root_directory", &fs::path::root_directory)
			.def("relative_path", &fs::path::relative_path)
			.def("parent_path", &fs::path::parent_path)
			.def("stem", &fs::path::stem)
			.def("extension", &fs::path::extension)
			.def("clear", &fs::path::clear)
			.def("swap", &fs::path::swap)
			.def("empty", &fs::path::empty)
			.def("is_absolute", &fs::path::is_absolute)
			.def("is_relative", &fs::path::is_relative)
			.def("has_root_path", &fs::path::has_root_path)
			.def("has_root_name", &fs::path::has_root_name)
			.def("has_root_directory", &fs::path::has_root_directory)
			.def("has_relative_path", &fs::path::has_relative_path)
			.def("has_filename", &fs::path::has_filename)
			.def("has_parent_path", &fs::path::has_parent_path)
			.def("has_stem", &fs::path::has_stem)
			.def("has_extension", &fs::path::has_extension)
			.def("make_preferred", &fs::path::make_preferred, return_reference_to(_1))
			.def("remove_filename", &fs::path::remove_filename, return_reference_to(_1))
			.def("replace_extension", &fs::path::replace_extension, return_reference_to(_1))
			.def("list_directory",   &NLuaAPI::NFileSystemAdditional::LuaDirectoryFactory, return_stl_iterator)
			.def(const_self / const_self)
			.def(other<const std::string &>() / const_self)
			.def(const_self / other<const std::string &>())
			.def(const_self == const_self)
			.def(other<const std::string &>() == const_self)
			.def(const_self == other<const std::string &>())
			.def(const_self < const_self)
			.def(other<const std::string &>() < const_self)
			.def(const_self < other<const std::string &>())
			.def(const_self <= const_self)
			.def(other<const std::string &>() <= const_self)
			.def(const_self <= other<const std::string &>())
			.def(tostring(const_self))
		,
		
		def("exists", (bool (*)(const fs::path &))&fs::exists),
		def("is_directory", (bool (*)(const fs::path &))&fs::is_directory),
		def("is_regular_file", (bool (*)(const fs::path &))&fs::is_regular_file),
		def("is_empty", (bool (*)(const fs::path &))&fs::is_empty),
		def("is_other", (bool (*)(const fs::path &))&fs::is_other),
		def("file_size", (boost::uintmax_t (*)(const fs::path &))&fs::file_size),
		def("create_directory", (bool (*)(const fs::path &))&fs::create_directory),
		def("create_directories", (bool (*)(const fs::path &))&fs::create_directories),
		def("remove", (bool (*)(const fs::path &))&fs::remove),
		def("remove_all", (boost::uintmax_t (*)(const fs::path &))&fs::remove_all),
		def("rename", (void (*)(const fs::path &, const fs::path &))&fs::rename),
		def("equivalent", (bool (*)(const fs::path &, const fs::path &))&fs::equivalent),
		def("copy_file", NLuaAPI::NFileSystemAdditional::LuaCopyFile),
		def("set_current_path", (void (*)(const fs::path &))&fs::current_path),
		def("current_path", (fs::path (*)())&fs::current_path),
		def("initial_path", (fs::path (*)())&fs::initial_path),
		def("extension", (std::string (*)(const fs::path &))&fs::extension),
		def("basename", (std::string (*)(const fs::path &))&fs::basename),
		def("system_complete", (fs::path (*)(const fs::path &))&fs::system_complete),
		def("canonical", (fs::path (*)(const fs::path &, const fs::path &))&fs::canonical),
		def("get", (fs::path (*)(uint32_t))&base::path::get)
	];

	lua_getglobal(pState, "fs");
	object constantTable(from_stack(pState, -1));

#define LUA_AREA_CONSTANT(name) constantTable[#name] = base::path:: ## name

	LUA_AREA_CONSTANT(DIR_EXE);
	LUA_AREA_CONSTANT(DIR_MODULE);
	LUA_AREA_CONSTANT(DIR_TEMP);
	LUA_AREA_CONSTANT(DIR_WINDOWS);
	LUA_AREA_CONSTANT(DIR_SYSTEM);
	LUA_AREA_CONSTANT(DIR_PROGRAM_FILES);
	LUA_AREA_CONSTANT(DIR_IE_INTERNET_CACHE);
	LUA_AREA_CONSTANT(DIR_COMMON_START_MENU);
	LUA_AREA_CONSTANT(DIR_START_MENU);
	LUA_AREA_CONSTANT(DIR_APP_DATA);
	LUA_AREA_CONSTANT(DIR_COMMON_APP_DATA);
	LUA_AREA_CONSTANT(DIR_PROFILE);
	LUA_AREA_CONSTANT(DIR_LOCAL_APP_DATA);
	LUA_AREA_CONSTANT(DIR_SOURCE_ROOT); 
	LUA_AREA_CONSTANT(DIR_USER_DESKTOP);
	LUA_AREA_CONSTANT(DIR_COMMON_DESKTOP);
	LUA_AREA_CONSTANT(DIR_USER_QUICK_LAUNCH);
	LUA_AREA_CONSTANT(DIR_DEFAULT_USER_QUICK_LAUNCH);
	LUA_AREA_CONSTANT(DIR_TASKBAR_PINS);

	return 0;
}

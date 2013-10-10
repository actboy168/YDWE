#include "Regedit.h"

namespace stlsoft
{
	namespace winstl_project
	{
		template<   ss_typename_param_k C
			,   ss_typename_param_k T
			,   ss_typename_param_k A
		>
		bool reg_delete_tree(basic_reg_key<C,T,A> const& key, C const* sub_key_name)
		{
			typedef T                                           traits_type;
			typedef ss_typename_type_k traits_type::result_type result_type;

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
			static const char message[] = "could not delete tree";
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */

			result_type res = traits_type::reg_delete_tree(key.get_key_handle(), sub_key_name);

			switch (res)
			{
			case    ERROR_SUCCESS:
				return true;
			default:
#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
				if(ERROR_ACCESS_DENIED == res)
				{
					STLSOFT_THROW_X(access_denied_exception(message, res));
				}
				else
				{
					STLSOFT_THROW_X(registry_exception(message, res));
				}
#else /* ? STLSOFT_CF_EXCEPTION_SUPPORT */
				::SetLastError(res);
				// Fall through
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */
			case    ERROR_FILE_NOT_FOUND:
				return false;
			}
		}
	}
}

FileAssociation::Classes::Classes(std::wstring const& name)
	: name_(name)
{ }

std::wstring const& FileAssociation::Classes::name() const
{
	return name_;
}

bool FileAssociation::Classes::has(winstl::reg_key_w& root, std::wstring const& command) const
{
	try {
		winstl::reg_key_w reg = root.open_sub_key(name_.c_str());
		winstl::reg_key_w sub_reg = reg.open_sub_key(L"shell\\open\\command");
		winstl::reg_value_w value = sub_reg.get_value(L"");

		return command == value.value_sz().c_str();
	} catch (winstl::registry_exception const& ) { }

	return false;
}

bool FileAssociation::Classes::remove(winstl::reg_key_w const& root)
{
	try {
		return winstl::reg_delete_tree(root, name_.c_str());
	} catch (winstl::registry_exception const& ) { }

	return false;
}

bool FileAssociation::Classes::set(winstl::reg_key_w const& root, std::wstring const& description, std::wstring const& icon_path, std::wstring const& command)
{
	try {
		winstl::reg_key_w reg = winstl::reg_key_w::create_key(root, name_.c_str());
		reg.set_value(L"", description.c_str());

		// Default Icon
		winstl::reg_key_w reg_icon_path = winstl::reg_key_w::create_key(reg, L"DefaultIcon");
		reg_icon_path.set_value(L"", icon_path.c_str());

		// Shell\Open\Command
		winstl::reg_key_w reg_command = winstl::reg_key_w::create_key(reg, L"shell\\open\\command");
		reg_command.set_value(L"", command.c_str());

		return true;
	} catch (winstl::registry_exception const& ) { }

	return false;
}

FileAssociation::Ext::Ext(std::wstring const& ext)
	: ext_(ext)
{ }

bool FileAssociation::Ext::has(winstl::reg_key_w& root, Classes const& c) const
{
	try {
		winstl::reg_key_w reg = root.open_sub_key(ext_.c_str());
		winstl::reg_value_w value = reg.get_value(L"");

		return c.name() == value.value_sz().c_str();
	} catch (winstl::registry_exception const& ) { }

	return false;
}

bool FileAssociation::Ext::remove(winstl::reg_key_w const& root)
{
	try {
		return winstl::reg_delete_tree(root, ext_.c_str());
	} catch (winstl::registry_exception const& ) { }

	return false;
}

bool FileAssociation::Ext::set(winstl::reg_key_w const& root, Classes const& c)
{
	try {
		winstl::reg_key_w reg = winstl::reg_key_w::create_key(root, ext_.c_str());
		return reg.set_value(L"", c.name().c_str());
	} catch (winstl::registry_exception const& ) { }

	return false;
}

bool FileAssociation::Ext::has_owl(winstl::reg_key_w& root) const
{
	try {
		return root.open_sub_key(ext_.c_str()).has_value(L"Application");
	} catch (winstl::registry_exception const& ) { }

	return false;
}

bool FileAssociation::Ext::set_owl(winstl::reg_key_w& root)
{
	try {
		return root.open_sub_key(ext_.c_str()).delete_value(L"Application");
	} catch (winstl::registry_exception const& ) { }

	return false;
}

FileAssociation::FileAssociation(fs::path const& ydwe_path)
	: root_(HKEY_CURRENT_USER, L"Software\\Classes")
	, root2_(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts")
	, classes_(L"YDWEMap")
	, ext_w3x_(L".w3x")
	, ext_w3m_(L".w3m")
	, icon_path_(ydwe_path.parent_path() / L"bin" / L"logo.ico")
	, command_(L"\"" + ydwe_path.wstring() + L"\" -loadfile \"%1\"")
{ }

bool FileAssociation::has_w3x()
{
	return classes_.has(root_, command_) 
		&& ext_w3x_.has(root_, classes_)
		&& ext_w3x_.has_owl(root2_);
}

bool FileAssociation::has_w3m()
{
	return classes_.has(root_, command_) 
		&& ext_w3m_.has(root_, classes_)
		&& ext_w3m_.has_owl(root2_);
}

bool FileAssociation::remove_w3x()
{
	return ext_w3x_.remove(root_);
}

bool FileAssociation::remove_w3m()
{
	return ext_w3m_.remove(root_);
}

bool FileAssociation::remove_classes()
{
	return classes_.remove(root_);
}

void FileAssociation::remove()
{
	remove_classes();
	remove_w3x();
	remove_w3m();
}

bool FileAssociation::set_w3x()
{
	return ext_w3x_.set(root_, classes_) && ext_w3x_.set_owl(root2_);
}

bool FileAssociation::set_w3m()
{
	return ext_w3m_.set(root_, classes_) && ext_w3m_.set_owl(root2_);
}

bool FileAssociation::set_classes()
{
	return classes_.set(root_, L"YDWE Map File", icon_path_.wstring(), command_);
}

bool Registry::has(HKEY hkey, std::wstring const& name, std::wstring const& key)
{
	try {
		winstl::reg_key_w   reg(hkey, name.c_str());
		winstl::reg_value_w value = reg.get_value(key.c_str());
		return !!value.value_dword();
	} catch (winstl::registry_exception const& ) { }

	return false;
}

bool Registry::set(HKEY hkey, std::wstring const& name, std::wstring const& key, bool value)
{
	try {
		winstl::reg_key_w reg = winstl::reg_key_w::create_key(hkey, name.c_str());
		return reg.set_value(key.c_str(), value);
	} catch (winstl::registry_exception const& ) { }

	return false;
}

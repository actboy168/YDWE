#include "Regedit.h"

FileAssociation::Classes::Classes(std::wstring const& name)
	: name_(name)
{ }

std::wstring const& FileAssociation::Classes::name() const
{
	return name_;
}

bool FileAssociation::Classes::has(base::registry::write_key_w& root, std::wstring const& command) const
{
	try {
		return command == (root / name_ / L"shell\\open\\command")[L""].get<std::wstring>();
	} catch (base::registry::registry_exception const& ) { }

	return false;
}

bool FileAssociation::Classes::remove(base::registry::write_key_w& root)
{
	try {
		return root.delete_sub_key(name_, true);
	} catch (base::registry::registry_exception const& ) { }

	return false;
}

bool FileAssociation::Classes::set(base::registry::write_key_w const& root, std::wstring const& description, std::wstring const& icon_path, std::wstring const& command)
{
	try {
		base::registry::write_key_w reg(root, name_);
		reg[L""] = description;

		// Default Icon
		(reg / L"DefaultIcon")[L""] = icon_path;

		// Shell\Open\Command
		(reg / L"shell\\open\\command")[L""] = command;

		return true;
	} catch (base::registry::registry_exception const& ) { }

	return false;
}

FileAssociation::Ext::Ext(std::wstring const& ext)
	: ext_(ext)
{ }

bool FileAssociation::Ext::has(base::registry::write_key_w& root, Classes const& c) const
{
	try {
		return c.name() == (root / ext_)[L""].get<std::wstring>();
	} catch (base::registry::registry_exception const& ) { }

	return false;
}

bool FileAssociation::Ext::remove(base::registry::write_key_w& root)
{
	try {
		return root.delete_sub_key(ext_, true);
	} catch (base::registry::registry_exception const& ) { }

	return false;
}

bool FileAssociation::Ext::set(base::registry::write_key_w& root, Classes const& c)
{
	try {
		(root / ext_)[L""] = c.name();
	} catch (base::registry::registry_exception const& ) { }

	return false;
}

bool FileAssociation::Ext::has_owl(base::registry::write_key_w& root) const
{
	try {
		return (root / ext_)[L"Application"].has();
	} catch (base::registry::registry_exception const& ) { }

	return false;
}

bool FileAssociation::Ext::set_owl(base::registry::write_key_w& root)
{
	try {
		return (root / ext_)[L"Application"].del();
	} catch (base::registry::registry_exception const& ) { }

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
		&& (!ext_w3x_.has_owl(root2_));
}

bool FileAssociation::has_w3m()
{
	return classes_.has(root_, command_) 
		&& ext_w3m_.has(root_, classes_)
		&& (!ext_w3m_.has_owl(root2_));
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
		return !!base::registry::read_key_w(hkey, name)[key].get<uint32_t>();
	} catch (base::registry::registry_exception const& ) { }

	return false;
}

bool Registry::set(HKEY hkey, std::wstring const& name, std::wstring const& key, bool value)
{
	try {
		base::registry::write_key_w(hkey, name)[key] = uint32_t(value);
		return true;
	} catch (base::registry::registry_exception const& ) { }

	return false;
}

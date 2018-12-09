#include "Regedit.h"

FileAssociation::Classes::Classes(std::wstring const& name)
	: name_(name)
{ }

std::wstring const& FileAssociation::Classes::name() const
{
	return name_;
}

bool FileAssociation::Classes::has(bee::registry::key_w& root, std::wstring const& command) const
{
	try {
		return command == (root / name_ / L"shell\\open\\command")[L""].get_string();
	} catch (bee::registry::registry_exception const& ) { }

	return false;
}

bool FileAssociation::Classes::remove(bee::registry::key_w& root)
{
	try {
		return root.del(name_, true);
	} catch (bee::registry::registry_exception const& ) { }

	return false;
}

bool FileAssociation::Classes::set(bee::registry::key_w const& root, std::wstring const& description, std::wstring const& icon_path, std::wstring const& command1, std::wstring const& command2)
{
	try {
		bee::registry::key_w reg = root / name_;
		reg[L""] = description;

		(reg / L"DefaultIcon")[L""] = icon_path;
		(reg / L"shell\\open\\command")[L""] = command1;
		(reg / L"shell\\run_war3")[L""] = L"”√ydwe≤‚ ‘µÿÕº";
		(reg / L"shell\\run_war3\\command")[L""] = command2;

		return true;
	} catch (bee::registry::registry_exception const& ) { }

	return false;
}

FileAssociation::Ext::Ext(std::wstring const& ext)
	: ext_(ext)
{ }

bool FileAssociation::Ext::has(bee::registry::key_w& root, Classes const& c) const
{
	try {
		return c.name() == (root / ext_)[L""].get_string();
	} catch (bee::registry::registry_exception const& ) { }

	return false;
}

bool FileAssociation::Ext::remove(bee::registry::key_w& root)
{
	try {
		return root.del(ext_, true);
	} catch (bee::registry::registry_exception const& ) { }

	return false;
}

bool FileAssociation::Ext::set(bee::registry::key_w& root, Classes const& c)
{
	try {
		(root / ext_)[L""] = c.name();
	} catch (bee::registry::registry_exception const& ) { }

	return false;
}

bool FileAssociation::Ext::has_owl(bee::registry::key_w& root) const
{
	try {
		return (root / ext_)[L"Application"].has();
	} catch (bee::registry::registry_exception const& ) { }

	return false;
}

bool FileAssociation::Ext::set_owl(bee::registry::key_w& root)
{
	try {
		return (root / ext_)[L"Application"].del();
	} catch (bee::registry::registry_exception const& ) { }

	return false;
}

FileAssociation::FileAssociation(fs::path const& ydwe_path)
	: root_(HKEY_CURRENT_USER, L"Software", L"Classes")
	, root2_(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer", L"FileExts")
	, classes_(L"YDWEMap")
	, ext_w3x_(L".w3x")
	, ext_w3m_(L".w3m")
	, icon_path_(ydwe_path.parent_path() / L"bin" / L"logo.ico")
	, command1_(L"\"" + ydwe_path.wstring() + L"\" -loadfile \"%1\"")
	, command2_(L"\"" + (ydwe_path.parent_path() / L"ydwe.exe").wstring() + L"\" -war3 -loadfile \"%1\"")
{ }

bool FileAssociation::has_w3x()
{
	return classes_.has(root_, command1_)
		&& ext_w3x_.has(root_, classes_)
		&& (!ext_w3x_.has_owl(root2_));
}

bool FileAssociation::has_w3m()
{
	return classes_.has(root_, command1_)
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
	return classes_.set(root_, L"YDWE Map File", icon_path_.wstring(), command1_, command2_);
}

bool Registry::has(HKEY hkey, std::wstring const& name, std::wstring const& key)
{
	try {
		return !!bee::registry::key_w(hkey, L"", name)[key].get_uint32_t();
	} catch (bee::registry::registry_exception const& ) { }

	return false;
}

bool Registry::set(HKEY hkey, std::wstring const& name, std::wstring const& key, bool value)
{
	try {
		bee::registry::key_w(hkey, L"", name)[key] = uint32_t(value);
		return true;
	} catch (bee::registry::registry_exception const& ) { }

	return false;
}

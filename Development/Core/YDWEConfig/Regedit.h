#pragma once

#include <windows.h>

#include <base/win/registry/key.h>
#include <base/win/registry/value.h>
#include <base/filesystem.h>

class FileAssociation
{
private:
	class Classes
	{
	public:
		Classes(std::wstring const& name);
		std::wstring const& name() const;
		bool has(base::registry::key_w& root, std::wstring const& command) const;
		bool remove(base::registry::key_w& root);
		bool set(base::registry::key_w const& root, std::wstring const& description, std::wstring const& icon_path, std::wstring const& command1, std::wstring const& command2);

	private:
		std::wstring name_;
	};

	class Ext
	{
	public:
		Ext(std::wstring const& ext);
		bool has(base::registry::key_w& root, Classes const& c) const;
		bool remove(base::registry::key_w& root);
		bool set(base::registry::key_w& root, Classes const& c);

		bool has_owl(base::registry::key_w& root) const;
		bool set_owl(base::registry::key_w& root);

	private:
		std::wstring ext_;
	};

public:
	FileAssociation(fs::path const& ydwe_path);
	void remove();
	bool has_w3x();
	bool has_w3m();
	bool set_w3x();
	bool set_w3m();
	bool set_classes();

private:
	bool remove_w3x();
	bool remove_w3m();
	bool remove_classes();

	base::registry::key_w root_;
	base::registry::key_w root2_;
	FileAssociation::Classes classes_;
	FileAssociation::Ext ext_w3x_;
	FileAssociation::Ext ext_w3m_;

	fs::path icon_path_;
	std::wstring command1_;
	std::wstring command2_;
};

class Registry
{
public:
	static bool has(HKEY hkey, std::wstring const& name, std::wstring const& key);
	static bool set(HKEY hkey, std::wstring const& name, std::wstring const& key, bool value);
};

template <class T, class P1>
class SingletonPtr
{
public:
	static bool initialize(P1 p1)
	{
		try {
			s_instance.reset(new T(p1));
		} catch (...) {
			return false;
		}
		return true;
	}

	static T* get()
	{
		return s_instance.get();
	}

private:
	static  std::unique_ptr<T>  s_instance;
};

template <class T, class P1>
std::unique_ptr<T>  SingletonPtr<T, P1>::s_instance;

typedef SingletonPtr<FileAssociation, fs::path const&> FileAssociationS;

#pragma once

#include <Windows.h>
#include <string>
#include <memory>
#include <boost/filesystem.hpp>
namespace fs = ::boost::filesystem;

class FontManager
{
public:
	FontManager(fs::path&& module_path);
	~FontManager();
	void postWindow(HWND hWnd);

private:
	HFONT font_;
	fs::path font_path_;
};

extern std::unique_ptr<FontManager> g_fontptr;

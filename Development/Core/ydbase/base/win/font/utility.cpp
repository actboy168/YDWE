#include <base/win/font/utility.h>
#include <Windows.h>
#include <cassert>

namespace base {  namespace font {

	namespace detail
	{
		class scoped_GetDC
		{
		public:
			explicit scoped_GetDC(HWND hwnd)
				: hwnd_(hwnd)
				, hdc_(::GetDC(hwnd))
			{
				if (hwnd_)
				{
					assert(::IsWindow(hwnd_));
				}
				assert(hdc_);
			}

			~scoped_GetDC()
			{
				if (hdc_)
				{
					::ReleaseDC(hwnd_, hdc_);
				}
			}

			operator HDC()
			{
				return hdc_;
			}

		private:
			HWND hwnd_;
			HDC  hdc_;
		};
	}

	size_t size_to_height(size_t size)
	{
		detail::scoped_GetDC screen_dc(NULL);
		return ::MulDiv(size, ::GetDeviceCaps(screen_dc, LOGPIXELSY), 72);
	}

	size_t height_to_size(size_t height)
	{
		detail::scoped_GetDC screen_dc(NULL);
		return ::MulDiv(height, 72, ::GetDeviceCaps(screen_dc, LOGPIXELSY));
	}

	static int CALLBACK enum_font_families(ENUMLOGFONTEXW* logical_font, NEWTEXTMETRICEXW* /*physical_font*/, DWORD /*font_type*/, LPARAM lparam)
	{
		std::set<std::wstring>* font_names = reinterpret_cast<std::set<std::wstring>*>(lparam);
		if (font_names)
		{
			const LOGFONTW& lf = logical_font->elfLogFont;
			if (lf.lfFaceName[0] && lf.lfFaceName[0] != '@')
			{
				std::wstring face_name(lf.lfFaceName);
				font_names->insert(face_name);
			}
		}
		return 1;
	}

	std::set<std::wstring> get_list()
	{
		std::set<std::wstring> font_names;
		LOGFONTW logfont;
		memset(&logfont, 0, sizeof(logfont));
		logfont.lfCharSet = DEFAULT_CHARSET;
		detail::scoped_GetDC screen_dc(NULL);
		::EnumFontFamiliesExW(screen_dc, &logfont, (FONTENUMPROCW)&enum_font_families, (LPARAM)&font_names, 0);
		return font_names;
	}
}}

#include <base/path/ydwe.h>
#include <base/path/get_path.h>

// http://blogs.msdn.com/oldnewthing/archive/2004/10/25/247180.aspx
extern "C" IMAGE_DOS_HEADER __ImageBase;

namespace base { namespace path {
	fs::path ydwe(bool support_dev)
	{
		fs::path ydwe = module(reinterpret_cast<HMODULE>(&__ImageBase));
		ydwe = ydwe.parent_path().parent_path();
		if (support_dev) {
			fs::path ydwedev = ydwe.parent_path().parent_path().parent_path();
			if (fs::exists(ydwedev / "build.root")) {
				ydwe = ydwedev / L"Component";
			}
		}
		return ydwe;
	}
}}

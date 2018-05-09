#include <base/path/ydwe.h>
#include <base/path/get_path.h>

namespace base { namespace path {
	fs::path ydwe(bool support_dev)
	{
		fs::path ydwe = get(DIR_MODULE);
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

#include <base/path/ydwe.h>
#include <base/path/get_path.h>

namespace base { namespace path {
	fs::path ydwe(bool support_dev)
	{
		fs::path ydwe = get(DIR_MODULE);
		ydwe.remove_filename().remove_filename();
		if (support_dev) {
			fs::path ydwedev = ydwe.parent_path().remove_filename().remove_filename();
			if (fs::exists(ydwedev / "build.root")) {
				ydwe = ydwedev / L"Component";
			}
		}
		return ydwe;
	}
}}

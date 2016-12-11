#pragma once

#include <base/filesystem.h>

namespace Shortcuts
{
	struct Desktop
	{
	public:
		static bool CreateOrUpdate(fs::path const& target_path);
		static bool Remove(fs::path const& target_path);
		static bool Has(fs::path const& target_path);
	};

	struct Taskbar
	{
		static bool CreateOrUpdate(fs::path const& target_path);
		static bool Remove(fs::path const& target_path);
		static bool Has(fs::path const& target_path);
	};
}

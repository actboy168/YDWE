#include "Shortcuts.h"
#include <Shobjidl.h>
#include <memory>
#include <bee/platform/version.h>
#include <base/path/get_path.h>
#include <bee/utility/path_helper.h>
#include <base/com/unique_ptr.h>
#include <bee/error.h>

namespace Shortcuts
{
	namespace detail
	{
#define ENSURE(cond) if (FAILED(cond)) throw bee::make_syserror(#cond " failed.");

		fs::path quick_launch_path()
		{
			wchar_t buffer[MAX_PATH];
			buffer[0] = 0;
			ENSURE(::SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, buffer));
			fs::path result(buffer);
			// http://stackoverflow.com/questions/76080/how-do-you-reliably-get-the-quick-
			// http://www.microsoft.com/technet/scriptcenter/resources/qanda/sept05/hey0901.mspx
			result = result / L"Microsoft" / L"Internet Explorer" / L"Quick Launch";
			return std::move(result);
		}

		fs::path desktop_path()
		{
			wchar_t buffer[MAX_PATH];
			buffer[0] = 0;
			ENSURE(::SHGetFolderPathW(NULL, CSIDL_DESKTOPDIRECTORY, NULL, SHGFP_TYPE_CURRENT, buffer));
			return std::move(fs::path(buffer));
		}

		bool CreateOrUpdateShortcutLink(fs::path const& shortcut_path, fs::path const& target_path)
		{
			bool shortcut_existed = fs::exists(shortcut_path);

			{
				base::com::unique_ptr<IShellLinkW> pShellLink;
				HRESULT hr = pShellLink.CreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER);
				if (FAILED(hr))
					return false;

				pShellLink->SetPath(target_path.c_str());

				base::com::unique_ptr<IPersistFile> pPf;
				hr = pPf.QueryFrom(pShellLink.get());
				if (FAILED(hr))
					return false;

				hr = pPf->Save(shortcut_path.c_str(), TRUE);
				if (FAILED(hr))
					return false;

			}

			if (shortcut_existed) 
			{
				::SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);
			} 
			else 
			{
				::SHChangeNotify(SHCNE_CREATE, SHCNF_PATH, shortcut_path.c_str(), NULL);
			}

			return true;
		}

		bool ResolveShortcut(fs::path const& shortcut_path, fs::path* target_path) 
		{
			HRESULT hr;
			base::com::unique_ptr<IShellLinkW> pShellLink;

			hr = pShellLink.CreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER);
			if (FAILED(hr))
				return false;

			base::com::unique_ptr<IPersistFile> pPf;

			hr = pPf.QueryFrom(pShellLink.get());
			if (FAILED(hr))
				return false;

			hr = pPf->Load(shortcut_path.c_str(), STGM_READ);
			if (FAILED(hr))
				return false;

			wchar_t temp[MAX_PATH];
			if (target_path) 
			{
				hr = pShellLink->Resolve(0, SLR_NO_UI | SLR_NOSEARCH);
				if (FAILED(hr))
					return false;

				hr = pShellLink->GetPath(temp, MAX_PATH, NULL, SLGP_UNCPRIORITY);
				if (FAILED(hr))
					return false;

				*target_path = fs::path(temp);
			}

			return true;
		}

		bool TaskbarPinShortcutLink(fs::path const& shortcut_path) 
		{
			int result = reinterpret_cast<int>(::ShellExecute(NULL, L"taskbarpin", shortcut_path.c_str(), NULL, NULL, 0));
			return result > 32;
		}

		bool TaskbarUnpinShortcutLink(fs::path const& shortcut_path) 
		{
			int result = reinterpret_cast<int>(::ShellExecute(NULL, L"taskbarunpin", shortcut_path.c_str(), NULL, NULL, 0));
			return result > 32;
		}
	}

	bool Desktop::CreateOrUpdate(fs::path const& target_path) 
	{
		try
		{
			fs::path shortcut_path = detail::desktop_path() / target_path.filename().replace_extension(L".lnk");

			if (!detail::CreateOrUpdateShortcutLink(shortcut_path, target_path))
			{
				throw std::domain_error(("Couldn't create or update shortcut at " + shortcut_path.string()).c_str());
			}

			return true;
		}
		catch (...)
		{
		}

		return false;
	}

	bool Desktop::Remove(fs::path const& target_path)
	{
		try
		{
			fs::path desktop_path = detail::desktop_path();
			if (!fs::exists(desktop_path))
			{
				throw std::domain_error("Couldn't find path to desktop.");
			}

			fs::directory_iterator endItr;
			for (fs::directory_iterator itr(desktop_path); itr != endItr; ++itr)
			{
				fs::path const& shortcut_path = itr->path();
				if (!fs::is_directory(shortcut_path) && shortcut_path.extension() == L".lnk")
				{
					fs::path read_target;

					if (!detail::ResolveShortcut(shortcut_path, &read_target))
					{
						continue;
					}

					if (bee::path_helper::equal(read_target, target_path))
					{
						fs::remove(shortcut_path);
						return true;
					}
				}
			}
		}
		catch (...)
		{
		}

		return false;
	}

	bool Desktop::Has(fs::path const& target_path)
	{
		try
		{
			fs::path desktop_path = detail::desktop_path();
			if (!fs::exists(desktop_path))
			{
				throw std::domain_error("Couldn't find path to desktop.");
			}

			fs::directory_iterator endItr;
			for (fs::directory_iterator itr(desktop_path); itr != endItr; ++itr)
			{
				fs::path const& shortcut_path = itr->path();
				if (!fs::is_directory(shortcut_path) && shortcut_path.extension() == L".lnk")
				{
					fs::path read_target;

					if (!detail::ResolveShortcut(shortcut_path, &read_target))
					{
						continue;
					}

					if (bee::path_helper::equal(read_target, target_path))
					{
						return true;
					}
				}
			}
		}
		catch (...)
		{
		}

		return false;
	}

	bool Taskbar::CreateOrUpdate(fs::path const& target_path) 
	{
		try
		{
			if (bee::platform::get_version().ver >= +bee::platform::WinVer::Win10)
			{
			}
			else if (bee::platform::get_version().ver >= +bee::platform::WinVer::Win7)
			{
				fs::path shortcut_path = base::path::temp() / target_path.filename().replace_extension(L".lnk");

				if (!detail::CreateOrUpdateShortcutLink(shortcut_path, target_path))
				{
					throw std::domain_error(("Couldn't create or update shortcut at " + shortcut_path.string()).c_str());
				}

				bool result = detail::TaskbarPinShortcutLink(shortcut_path);
				fs::remove(shortcut_path);
				return result;
			}
			else
			{
				fs::path shortcut_path = detail::quick_launch_path() / target_path.filename().replace_extension(L".lnk");

				return detail::CreateOrUpdateShortcutLink(shortcut_path, target_path);
			}
		}
		catch (...)
		{
		}

		return false;
	}

	bool Taskbar::Remove(fs::path const& target_path)
	{
		try
		{
			fs::path taskbar_path;
			if (bee::platform::get_version().ver >= +bee::platform::WinVer::Win10)
			{
				return false;
			}
			else if (bee::platform::get_version().ver >= +bee::platform::WinVer::Win7)
			{
				taskbar_path = detail::quick_launch_path() / L"User Pinned" / L"TaskBar";
			}
			else
			{
				taskbar_path = detail::quick_launch_path();
			}

			if (!fs::exists(taskbar_path))
			{
				throw std::domain_error("Couldn't find path to taskbar pins.");
			}

			fs::directory_iterator endItr;
			for (fs::directory_iterator itr(taskbar_path); itr != endItr; ++itr)
			{
				fs::path const& shortcut_path = itr->path();
				if (!fs::is_directory(shortcut_path) && shortcut_path.extension() == L".lnk")
				{
					fs::path read_target;

					if (!detail::ResolveShortcut(shortcut_path, &read_target))
					{
						continue;
					}

					if (bee::path_helper::equal(read_target, target_path))
					{
						if (bee::platform::get_version().ver >= +bee::platform::WinVer::Win7)
						{
							return detail::TaskbarUnpinShortcutLink(shortcut_path);
						}
						else
						{
							return fs::remove(shortcut_path);

						}
					}
				}
			}
		}
		catch (...)
		{
		}

		return false;
	}

	bool Taskbar::Has(fs::path const& target_path)
	{
		try
		{
			fs::path taskbar_path;
			if (bee::platform::get_version().ver >= +bee::platform::WinVer::Win7)
			{
				taskbar_path = detail::quick_launch_path() / L"User Pinned" / L"TaskBar";
			}
			else
			{
				taskbar_path = detail::quick_launch_path();
			}

			if (!fs::exists(taskbar_path))
			{
				throw std::domain_error("Couldn't find path to taskbar pins.");
			}

			fs::directory_iterator endItr;
			for (fs::directory_iterator itr(taskbar_path); itr != endItr; ++itr)
			{
				fs::path const& shortcut_path = itr->path();
				if (!fs::is_directory(shortcut_path) && shortcut_path.extension() == L".lnk")
				{
					fs::path read_target;

					if (!detail::ResolveShortcut(shortcut_path, &read_target))
					{
						continue;
					}

					if (bee::path_helper::equal(read_target, target_path))
					{
						return true;
					}
				}
			}
		}
		catch (...)
		{
		}

		return false;
	}
}

#include <string>
#include <boost/thread.hpp>
#include <ydwe/util/unicode.h>
#include "DllMain.h"
#include <SlkLib/IniTable.hpp>
#include <SlkLib/IniReader.hpp>
#include <SlkLib/IniReader.cpp>
#include "storm.h"

namespace NYDWE
{
bool WorldEditStrings(std::string const& value, std::string& result)
{
	try {
		slk::IniTable table;
		slk::IniReader::Read(storm().load("UI\\WorldEditStrings.txt"), table);
		result = table["WorldEditStrings"][value];

		return true;
	}
	catch (...) {
	}

	return false;
}

volatile bool gWindowMonitorShouldExit;

static void WindowMonitorTask()
{
	LOG4CXX_DEBUG(NYDWE::gInjectLogger, "Monitor thread started.");

	try
	{
		std::string result;
		while (!WorldEditStrings("WESTRING_MODIFYTEXTURES", result))
		{
			boost::this_thread::sleep(boost::posix_time::milliseconds(125));
		}
		std::wstring text = ydwe::util::u2w(result);

		while (!gWindowMonitorShouldExit)
		{
			boost::this_thread::sleep(boost::posix_time::milliseconds(125));

			// Check dialog
			HWND dialog = FindWindowW(L"#32770", text.c_str());
			if (dialog && IsDlgButtonChecked(dialog, 0xC /* CheckBox */))
			{
				EnableWindow(GetDlgItem(dialog, 0x10 /* Delete style */), TRUE);
				EnableWindow(GetDlgItem(dialog, 0x11 /* Add style */), TRUE);
				EnableWindow(GetDlgItem(dialog, 0x12 /* Replace style */), TRUE);
			}
		}
	}
	catch (boost::thread_interrupted &) 
	{
		LOG4CXX_DEBUG(NYDWE::gInjectLogger, "Monitor thread interruppted.");
	}

	LOG4CXX_DEBUG(NYDWE::gInjectLogger, "Monitor thread ends.");
}

} // namespace NYDWE

static boost::shared_ptr<boost::thread> pgMonitorThread;

void PluginStart()
{
	LOG4CXX_DEBUG(NYDWE::gInjectLogger, "Starting monitor thread.");
	NYDWE::gWindowMonitorShouldExit = false;

	try
	{
		pgMonitorThread.reset(new boost::thread(NYDWE::WindowMonitorTask));

		LOG4CXX_DEBUG(NYDWE::gInjectLogger, "Finished starting monitor thread.");
	}
	catch (boost::thread_resource_error &) 
	{
		LOG4CXX_ERROR(NYDWE::gInjectLogger, "Starting monitor thread failed.");
	}
}

void PluginStop()
{
	if (!pgMonitorThread)
	{
		LOG4CXX_DEBUG(NYDWE::gInjectLogger, "Monitor thread not started. No need to stop.");
		return;
	}

	LOG4CXX_DEBUG(NYDWE::gInjectLogger, "Ending monitor thread.");
	NYDWE::gWindowMonitorShouldExit = true;

	try
	{
		pgMonitorThread->interrupt();
		pgMonitorThread->timed_join(boost::posix_time::milliseconds(1000));
		pgMonitorThread.reset();

		LOG4CXX_DEBUG(NYDWE::gInjectLogger, "Finished ending monitor thread.");
	}
	catch (...)
	{
		LOG4CXX_ERROR(NYDWE::gInjectLogger, "Ending monitor thread failed.");
	}
}

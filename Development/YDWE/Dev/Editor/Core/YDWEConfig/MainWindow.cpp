#include "MainWindow.h"
#include <slk/reader/IniReader.hpp>
#include <slk/utility/list_of.h>
#include <slk/writer/IniWriter.hpp>
#include <slk/reader/IniReader.cpp>
#include <slk/utility/sequence.cpp>
#include <slk/reader/CommonReader.cpp>
#include "Regedit.h"
#include "Shortcuts.h"
#include "Warcraft3Directory.h"
#include <ydwe/file/steam.h>
#include <ydwe/path/self.h>
#include <ydwe/path/service.h>
#include <ydwe/path/filesystem_helper.h>
#include <ydwe/util/unicode.h>
#include <ydwe/win/version.h>
#include <ydwe/win/file_version.h>

namespace 
{
	class Attribute
	{
	public:
		Attribute(uint8_t count)
			: m_section("Option")
			, m_count(count)
		{ }

		Attribute(std::string const& section)
			: m_section(section)
			, m_count(0)
		{ }

		std::string const& Section() const { return m_section; }
		uint8_t            Count() const { return m_count; }

	private:
		std::string m_section;
		uint8_t     m_count;
	};

	std::map<std::string, Attribute> configAttribute = slk::list_of
		("MapSave",                                 Attribute(4))
		("War3Patch",                               Attribute(3))
		("ScriptInjection",                         Attribute(2))
		("EnableJassHelper",                        Attribute("ScriptCompiler"))
		("EnableJassHelperDebug",                   Attribute("ScriptCompiler"))
		("EnableJassHelperOptimization",            Attribute("ScriptCompiler"))
		("EnableCJass",                             Attribute("ScriptCompiler"))
		("LaunchOpenGL",                            Attribute("MapTest"))
		("LaunchWindowed",                          Attribute("MapTest"))
		("LaunchFullWindowed",                      Attribute("MapTest"))
		("LaunchLockingMouse",                      Attribute("MapTest"))
		("LaunchFixedRatioWindowed",                Attribute("MapTest"))
		("EnableDotNetSupport",                     Attribute("ThirdPartyPlugin"))
		("EnableTesh",                              Attribute("ThirdPartyPlugin"))
		("EnableYDTrigger",                         Attribute("ThirdPartyPlugin"))
		("EnableManualNewId",                       Attribute("FeatureToggle"))
		("EnableTriggerCopyEncodingAutoConversion", Attribute("FeatureToggle"))
		("EnableShowInternalAttributeId",           Attribute("FeatureToggle"));
}

CMainWindow::CMainWindow() 
{
	try
	{
		m_ydwe_path = ydwe::path::get(ydwe::path::DIR_EXE).remove_filename().remove_filename() / L"YDWE.exe";
	}
	catch (...)
	{
	}
}

LPCTSTR CMainWindow::GetWindowClassName() const 
{ 
	return _T("YDWEConfig"); 
}

void CMainWindow::OnFinalMessage(HWND /*hWnd*/) 
{ 
	delete this; 
}

fs::path CMainWindow::GetSkinZip() const 
{
	return std::move(ydwe::path::self().remove_filename() / L"skin.zip"); 
}

void CMainWindow::ContrlSelected(std::string const& name, bool bSelect)
{
	DuiLib::CCheckBoxUI* ctrl = m_controls[name];
	if (ctrl) ctrl->Selected(bSelect);
}

void CMainWindow::ContrlSetEnabled(std::string const& name, bool bEnable)
{
	DuiLib::CCheckBoxUI* ctrl = m_controls[name];
	if (ctrl) ctrl->SetEnabled(bEnable);
}

void CMainWindow::InitWindow()
{
	foreach(auto it, configAttribute)
	{
		std::string const& name = it.first;
		Attribute const& attribute = it.second;
		if (attribute.Section() == "Option")
		{
			for (uint8_t i = 0; i < attribute.Count(); ++i)
			{
				std::string radioname = name + "_" + std::to_string((long long)i);
				m_controls[radioname] = dynamic_cast<DuiLib::CRadioButtonUI*>(m_pm.FindControl(ydwe::util::u2w(radioname).c_str()));
			}
		}
		else
		{
			m_controls[name] = dynamic_cast<DuiLib::CCheckBoxUI*>(m_pm.FindControl(ydwe::util::u2w(name).c_str()));
		}
	}

	m_pEnableJassHelper = m_controls["EnableJassHelper"];
	m_pEnableCJass      = m_controls["EnableCJass"];
	m_pLaunchWindowed   = m_controls["LaunchWindowed"];

	m_pFileAssociation_w3x = dynamic_cast<DuiLib::CCheckBoxUI*>(m_pm.FindControl(_T("FileAssociation_w3x")));
	m_pFileAssociation_w3m = dynamic_cast<DuiLib::CCheckBoxUI*>(m_pm.FindControl(_T("FileAssociation_w3m")));
	m_pShortcuts_desktop   = dynamic_cast<DuiLib::CCheckBoxUI*>(m_pm.FindControl(_T("Shortcuts_desktop")));
	m_pShortcuts_taskbar   = dynamic_cast<DuiLib::CCheckBoxUI*>(m_pm.FindControl(_T("Shortcuts_taskbar")));	
	m_pAllowLocalFiles     = dynamic_cast<DuiLib::CCheckBoxUI*>(m_pm.FindControl(_T("AllowLocalFiles")));
	m_pWar3PatchList       = dynamic_cast<DuiLib::CVerticalLayoutUI*>(m_pm.FindControl(_T("War3PatchList")));
	m_pWar3PluginList      = dynamic_cast<DuiLib::CVerticalLayoutUI*>(m_pm.FindControl(_T("War3PluginList")));
	m_pWarcraft3Directory  = dynamic_cast<DuiLib::CLabelUI*>(m_pm.FindControl(_T("Warcraft3Directory")));	

	m_pm.AddNotifier(this);
}

void CMainWindow::ResetConfig(slk::IniTable& table)
{
	table["MapSave"]["Option"] = "0";
	table["War3Patch"]["Option"] = "0";
	table["MapTest"]["LaunchOpenGL"] = "0";
	table["MapTest"]["LaunchWindowed"] = "1";
	table["MapTest"]["LaunchFullWindowed"] = "0";
	table["MapTest"]["LaunchLockingMouse"] = "0";
	table["MapTest"]["LaunchFixedRatioWindowed"] = "1";
	table["ScriptCompiler"]["EnableJassHelper"] = "1";
	table["ScriptCompiler"]["EnableJassHelperDebug"] = "0";
	table["ScriptCompiler"]["EnableJassHelperOptimization"] = "1";
	table["ScriptCompiler"]["EnableCJass"] = "0";
	table["ScriptInjection"]["Option"] = "0";
	table["ThirdPartyPlugin"]["EnableDotNetSupport"] = "1";
	table["ThirdPartyPlugin"]["EnableTesh"] = "1";
	table["ThirdPartyPlugin"]["EnableYDTrigger"] = "1";
	table["FeatureToggle"]["EnableManualNewId"] = "0";
	table["FeatureToggle"]["EnableTriggerCopyEncodingAutoConversion"] = "1";
	table["FeatureToggle"]["EnableShowInternalAttributeId"] = "0";
}

bool CMainWindow::LoadConfig(slk::IniTable& table)
{
	try
	{
		ResetConfig(table);
		slk::buffer_reader reader(ydwe::file::read_steam(ydwe::path::self().remove_filename() / L"EverConfig.cfg").read<slk::buffer>());
		slk::IniReader::Read(reader, table);
	}
	catch (...)
	{
		return false;
	}

	return true;
}

bool CMainWindow::SaveConfig(slk::IniTable const& table)
{
	try
	{
		ydwe::file::write_steam(ydwe::path::self().remove_filename() / L"EverConfig.cfg").write(slk::IniWriter::Write<slk::buffer>(table));
	}
	catch (...)
	{
		return false;
	}

	return true;
}

void CMainWindow::ConfigToUI(slk::IniTable& table)
{
	foreach(auto it, configAttribute)
	{
		std::string const& name = it.first;
		Attribute const& attribute = it.second;
		if (attribute.Section() == "Option")
		{
			int val = std::stoi(table[name]["Option"]);

			if ((val >= 0) && (unsigned int(val) < attribute.Count()))
			{
				DuiLib::CCheckBoxUI* ctrl = m_controls[name + "_" + std::to_string((long long)val)];
				if (ctrl)
				{
					ctrl->Selected(true);
				}
			}
		}
		else
		{
			DuiLib::CCheckBoxUI* ctrl = m_controls[name];
			if (ctrl)
			{
				ctrl->Selected(table[attribute.Section()][name]  != "0");
			}
		}
	}

	m_pm.SendNotify(m_pEnableCJass, DUI_MSGTYPE_SELECTCHANGED);
	m_pm.SendNotify(m_pEnableJassHelper, DUI_MSGTYPE_SELECTCHANGED);
}

void CMainWindow::UIToConfig(slk::IniTable& table)
{
	foreach(auto it, configAttribute)
	{
		std::string const& name = it.first;
		Attribute const& attribute = it.second;
		if (attribute.Section() == "Option")
		{
			for (uint8_t i = 0; i < attribute.Count(); ++i)
			{
				DuiLib::CRadioButtonUI* ctrl = dynamic_cast<DuiLib::CRadioButtonUI*>(m_controls[name + "_" + std::to_string((long long)i)]);
				if (ctrl && ctrl->IsSelected())
				{
					table[name]["Option"] = std::to_string((long long)i);
					break;
				}
			}
		}
		else
		{
			DuiLib::CCheckBoxUI* ctrl = m_controls[name];
			if (ctrl)
			{
				table[attribute.Section()][name] = ctrl->IsSelected()? "1" : "0";;
			}
		}
	}
}

void CMainWindow::EnableMapSave(bool bEnable)
{
	ContrlSetEnabled("MapSave_0", bEnable);
	ContrlSetEnabled("MapSave_1", bEnable);
	ContrlSetEnabled("MapSave_2", bEnable);
	ContrlSetEnabled("MapSave_3", bEnable);
	if (!bEnable) ContrlSelected("MapSave_0", true);
}

void CMainWindow::EnableScriptInjection(bool bEnable)
{
	ContrlSetEnabled("ScriptInjection_0", bEnable);
	ContrlSetEnabled("ScriptInjection_1", bEnable);
	if (!bEnable) ContrlSelected("ScriptInjection_1", true);
}

void CMainWindow::DisableCJass(bool bEnable)
{
	ContrlSetEnabled("EnableYDTrigger", bEnable);

	EnableMapSave(bEnable);
	EnableScriptInjection(bEnable);
}

void CMainWindow::EnableJassHelper(bool bEnable)
{
	ContrlSetEnabled("EnableJassHelperOptimization", bEnable);
	ContrlSetEnabled("EnableJassHelperDebug", bEnable);
	ContrlSetEnabled("EnableCJass", bEnable);

	DisableCJass(!m_pEnableCJass->IsSelected() && bEnable);
}

void CMainWindow::InitRegistryUI()
{
	if (m_pAllowLocalFiles)
		m_pAllowLocalFiles->Selected(Registry::has(HKEY_CURRENT_USER, L"Software\\Blizzard Entertainment\\Warcraft III", L"Allow Local Files"));
}

void CMainWindow::DoneRegistryUI()
{
	if (m_pAllowLocalFiles)
		Registry::set(HKEY_CURRENT_USER, L"Software\\Blizzard Entertainment\\Warcraft III", L"Allow Local Files", m_pAllowLocalFiles->IsSelected());
}

void CMainWindow::InitOSHelpUI()
{
	if (ydwe::win::version() >= ydwe::win::VERSION_WIN7)
	{
		m_pShortcuts_taskbar->SetText(L"固定到任务栏");
	}
	else
	{
		m_pShortcuts_taskbar->SetText(L"添加到快速启动栏");
	}

	if (!fs::exists(m_ydwe_path))
	{
		if (m_pFileAssociation_w3x)
			m_pFileAssociation_w3x->SetEnabled(false);

		if (m_pFileAssociation_w3m)
			m_pFileAssociation_w3m->SetEnabled(false);

		if (m_pShortcuts_desktop)
			m_pShortcuts_desktop->SetEnabled(false);

		if (m_pShortcuts_taskbar)
			m_pShortcuts_taskbar->SetEnabled(false);
	}
	else
	{
		FileAssociationS::initialize(m_ydwe_path);

		if (m_pFileAssociation_w3x)
			m_pFileAssociation_w3x->Selected(FileAssociationS::get()->has_w3x());

		if (m_pFileAssociation_w3m)
			m_pFileAssociation_w3m->Selected(FileAssociationS::get()->has_w3m());	

		if (m_pShortcuts_desktop)
			m_pShortcuts_desktop->Selected(Shortcuts::Desktop::Has(m_ydwe_path));

		if (m_pShortcuts_taskbar)
			m_pShortcuts_taskbar->Selected(Shortcuts::Taskbar::Has(m_ydwe_path));
	}
}

void CMainWindow::DoneOSHelpUI()
{
	if (fs::exists(m_ydwe_path))
	{
		if (m_pFileAssociation_w3x && m_pFileAssociation_w3m)
		{
			FileAssociationS::get()->remove();

			if (m_pFileAssociation_w3x->IsSelected())
				FileAssociationS::get()->set_w3x();

			if (m_pFileAssociation_w3m->IsSelected())
				FileAssociationS::get()->set_w3m();

			if (m_pFileAssociation_w3x->IsSelected() || m_pFileAssociation_w3m->IsSelected())
				FileAssociationS::get()->set_classes();
		}

		if (m_pShortcuts_desktop)
		{
			if (m_pShortcuts_desktop->IsSelected())
			{
				Shortcuts::Desktop::CreateOrUpdate(m_ydwe_path);
			}
			else
			{
				Shortcuts::Desktop::Remove(m_ydwe_path);
			}
		}

		if (m_pShortcuts_taskbar)
		{
			if (m_pShortcuts_taskbar->IsSelected())
			{
				Shortcuts::Taskbar::CreateOrUpdate(m_ydwe_path);
			}
			else
			{
				Shortcuts::Taskbar::Remove(m_ydwe_path);
			}
		}
	}
}

void CMainWindow::InitPluginUI()
{
	if (!m_pWar3PluginList)
		return;

	try {
		fs::path plugin_path = m_ydwe_path.parent_path() / L"plugin" / L"warcraft3";

		slk::IniTable table;
		try {
			slk::buffer_reader reader(ydwe::file::read_steam(plugin_path / L"config.cfg").read<slk::buffer>());
			slk::IniReader::Read(reader, table);	
		}
		catch(...) {
		}

		fs::directory_iterator end_itr;
		for (fs::directory_iterator itr(plugin_path); itr != end_itr; ++itr)
		{
			try {
				if (!fs::is_directory(*itr))
				{
					if (ydwe::path::equal(itr->path().extension(), L".dll") 
						&& (!ydwe::path::equal(itr->path().filename(), L"yd_loader.dll")))
					{
						ydwe::win::file_version fv(itr->path().c_str());

						DuiLib::CCheckBoxUI* node = new DuiLib::CCheckBoxUI;
						m_pWar3PluginList->Add(node);
						node->SetManager(m_pWar3PluginList->GetManager(), m_pWar3PluginList, false);
						node->SetAttribute(L"class", L"CheckBox");
						node->SetAttribute(L"text", fv[L"FileDescription"]);
						node->SetAttribute(L"tooltip", itr->path().filename().c_str());
						node->Selected("0" != table["Enable"][ydwe::util::w2u(itr->path().filename().wstring())]);
					}
				}
			}
			catch(...) {
			}
		}
	}
	catch(...) {
	}
}

void CMainWindow::DonePluginUI()
{
	if (!m_pWar3PluginList)
		return ;

	try {
		slk::IniTable table;

		for (int i = 0; i < m_pWar3PluginList->GetCount(); ++i) 
		{
			DuiLib::CCheckBoxUI* pCheckBox = dynamic_cast<DuiLib::CCheckBoxUI*>(m_pWar3PluginList->GetItemAt(i));

			if (pCheckBox)
			{
				
				table["Enable"][ydwe::util::w2u(pCheckBox->GetToolTip())] = pCheckBox->IsSelected()? "1": "0";
			}
		}

		fs::path plugin_path = m_ydwe_path.parent_path() / L"plugin" / L"warcraft3";
		ydwe::file::write_steam(plugin_path/ L"config.cfg").write(slk::IniWriter::Write<slk::buffer>(table));
	}
	catch (...) {
	}
}

void CMainWindow::InitPatchUI(slk::IniTable& table)
{
	if (!m_pWar3PatchList)
		return;

	bool empty = true;
	fs::path patch_path = m_ydwe_path.parent_path() / L"share" / L"patch";

	if (fs::exists(patch_path))
	{
		fs::directory_iterator endItr;
		for (fs::directory_iterator itr(patch_path); itr != endItr; ++itr)
		{
			fs::path patch = itr->path();
			if (fs::is_directory(patch))
			{
				fs::path game_dll  = patch / L"Game.dll";
				fs::path patch_mpq = patch / L"Patch.mpq";
				if (fs::exists(game_dll) && fs::exists(patch_mpq))
				{
					patch.normalize();
					ydwe::win::file_version versionInfo(game_dll.c_str());

					DuiLib::CRadioButtonUI* node = new DuiLib::CRadioButtonUI;
					m_pWar3PatchList->Add(node);
					node->SetManager(m_pWar3PatchList->GetManager(), m_pWar3PatchList, false);
					node->SetAttribute(L"class", L"NormalRadioButton");
					node->SetAttribute(L"group", L"War3PatchItem");
					node->SetAttribute(L"text", (std::wstring(L"[") + versionInfo[L"FileVersion"] + L"]" + patch.filename().wstring()).c_str());
					node->SetAttribute(L"tooltip", patch.c_str());
					node->Selected(ydwe::path::equal(patch.filename(), table["War3Patch"]["DirName"]));
					empty = false;
				}
			}
		}

		m_pWar3PatchList->SetEnabled("2" == table["War3Patch"]["Option"]);
	}

	if (empty)
	{
		DuiLib::CLabelUI* node;

		node = new DuiLib::CLabelUI;
		m_pWar3PatchList->Add(node);
		node->SetManager(m_pWar3PatchList->GetManager(), m_pWar3PatchList, false);
		node->SetAttribute(L"font", L"2");
		node->SetAttribute(L"text", L"请先去www.ydwe.net下载补丁包");
	}
}

void CMainWindow::DonePatchUI(slk::IniTable& table)
{
	if (m_pWar3PatchList && m_controls["War3Patch_2"] && m_controls["War3Patch_2"]->IsSelected())
	{
		m_pWar3PatchList->ForeachControl([&table](DuiLib::CControlUI* pControl)->bool
		{
			DuiLib::CRadioButtonUI* pRadioButton = dynamic_cast<DuiLib::CRadioButtonUI*>(pControl);
			if (pRadioButton && pRadioButton->IsSelected())
			{
				table["War3Patch"]["DirName"] = ydwe::util::w2u(fs::path(pRadioButton->GetToolTip()).filename().wstring());
				return false;
			}
			return true;
		});
	}
}

void CMainWindow::UpdateWarcraft3Directory()
{
	if (m_pWarcraft3Directory)
	{
		boost::filesystem::path result;
		if (warcraft3_directory::read(result))
		{
			m_pWarcraft3Directory->SetText(result.c_str());
		}
		else
		{
			m_pWarcraft3Directory->SetText(L"");
		}
	}
}

void CMainWindow::Notify(DuiLib::TNotifyUI& msg)
{
	try
	{
		if (msg.sType == DUI_MSGTYPE_WINDOWINIT)
		{
			slk::IniTable table;
			if (!LoadConfig(table)) { ResetConfig(table); }
			ConfigToUI(table);
			InitRegistryUI();
			InitOSHelpUI();
			InitPatchUI(table);
			InitPluginUI();
			UpdateWarcraft3Directory();
		}
		else if (msg.sType == DUI_MSGTYPE_SELECTCHANGED) 
		{
			if (m_pEnableJassHelper && m_pEnableJassHelper == msg.pSender)
			{
				bool bEnable = m_pEnableJassHelper->IsSelected();
				EnableJassHelper(bEnable);
			}
			else if (m_pEnableCJass && m_pEnableCJass == msg.pSender)
			{
				bool bEnable = !m_pEnableCJass->IsSelected();
				DisableCJass(bEnable);
			}
			else if (m_pLaunchWindowed && m_pLaunchWindowed == msg.pSender)
			{
				bool bEnable = m_pLaunchWindowed->IsSelected();
				ContrlSetEnabled("LaunchFullWindowed",       bEnable);
				ContrlSetEnabled("LaunchLockingMouse",       bEnable);
				ContrlSetEnabled("LaunchFixedRatioWindowed", bEnable);
			}
		}
		else if (msg.sType == DUI_MSGTYPE_CLICK) 
		{
			std::wstring const& name = msg.pSender->GetName();
			if (name == _T("ok")) 
			{
				slk::IniTable table;
				UIToConfig(table);
				DoneRegistryUI();
				DoneOSHelpUI();
				DonePatchUI(table);
				DonePluginUI();
				SaveConfig(table);

				::PostQuitMessage(0L);
			}
			else if (name == _T("cancel"))
			{
				::PostQuitMessage(0L);
			}
			else if (name == _T("reset"))
			{
				slk::IniTable table;
				ResetConfig(table);
				ConfigToUI(table);
			}
			else if (name == _T("choose_war3_dir"))
			{
				if (warcraft3_directory::choose(nullptr))
				{
					UpdateWarcraft3Directory();
				}

			}
			else if (name.size() == 11 && name.substr(0, 10) == _T("War3Patch_"))
			{
				if (m_pWar3PatchList)
				{
					long n = std::stol(name.substr(10));
					m_pWar3PatchList->SetEnabled(n == 2);
				}
			}
			else if (name.size() == 5 && name.substr(0, 4) == _T("tab_"))
			{
				DuiLib::CTabLayoutUI* tabs = dynamic_cast<DuiLib::CTabLayoutUI*>(m_pm.FindControl(_T("tabs")));
				if (tabs)
				{
					tabs->SelectItem(std::stol(name.substr(4)));
				}
			}
		}
	}
	catch (std::exception const& /*e*/)
	{
		assert(false);
	}
}

#include <windows.h>
#include <base/hook/fp_call.h>
#include <base/warcraft3/hashtable.h>
#include <base/warcraft3/jass.h>
#include <base/warcraft3/jass/global_variable.h>
#include <base/warcraft3/jass/hook.h>
#include <boost/preprocessor/repetition.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <map>

namespace commonj
{
	extern const char CameraSetupGetDestPositionLoc[] = "CameraSetupGetDestPositionLoc";
	extern const char GetCameraEyePositionLoc[]       = "GetCameraEyePositionLoc";
	extern const char GetCameraTargetPositionLoc[]    = "GetCameraTargetPositionLoc";
	extern const char GetOrderPointLoc[]              = "GetOrderPointLoc";
	extern const char GetSpellTargetLoc[]             = "GetSpellTargetLoc";
	extern const char GetStartLocationLoc[]           = "GetStartLocationLoc";
	extern const char GetUnitLoc[]                    = "GetUnitLoc";
	extern const char GetUnitRallyPoint[]             = "GetUnitRallyPoint";
	extern const char Location[]                      = "Location";
	extern const char RemoveLocation[]                = "RemoveLocation";
	extern const char AddSpecialEffect[]              = "AddSpecialEffect";
	extern const char AddSpecialEffectLoc[]           = "AddSpecialEffectLoc";
	extern const char AddSpecialEffectTarget[]        = "AddSpecialEffectTarget";
	extern const char AddSpellEffect[]                = "AddSpellEffect";
	extern const char AddSpellEffectLoc[]             = "AddSpellEffectLoc";
	extern const char AddSpellEffectById[]            = "AddSpellEffectById";
	extern const char AddSpellEffectByIdLoc[]         = "AddSpellEffectByIdLoc";
	extern const char AddSpellEffectTarget[]          = "AddSpellEffectTarget";
	extern const char AddSpellEffectTargetById[]      = "AddSpellEffectTargetById";
	extern const char DestroyEffect[]                 = "DestroyEffect";
	extern const char CreateGroup[]                   = "CreateGroup";
	extern const char DestroyGroup[]                  = "DestroyGroup";
	extern const char CreateRegion[]                  = "CreateRegion";
	extern const char RemoveRegion[]                  = "RemoveRegion";
	extern const char Rect[]                          = "Rect";
	extern const char RectFromLoc[]                   = "RectFromLoc";
	extern const char RemoveRect[]                    = "RemoveRect";
	extern const char CreateForce[]                   = "CreateForce";
	extern const char DestroyForce[]                  = "DestroyForce";

	extern const char location[]                      = "location";
	extern const char effect[]                        = "effect";
	extern const char group[]                         = "group";
	extern const char region[]                        = "region";
	extern const char rect[]                          = "rect";
	extern const char force[]                         = "force";
}

namespace monitor
{
	template <const char* type_name>
	class handle_manager 
	{
	public:
		static std::map<uintptr_t, uintptr_t>& instance()
		{
			static std::map<uintptr_t, uintptr_t> s;
			return s;
		}
	};

	template <const char* type_name, size_t param, const char* porc_name>
	class creater;

#define DEFINE_CLASS_CREATER(z, n, unused) \
	template <const char* type_name, const char* porc_name> \
	class creater<type_name, n, porc_name> \
	{ \
	public: \
	static void initialize() \
	{ \
	base::warcraft3::jass::async_hook(porc_name, &real_proc, (uintptr_t)fake_proc); \
	} \
	private: \
	static uintptr_t real_proc; \
	static uintptr_t __cdecl fake_proc(BOOST_PP_ENUM_PARAMS(n, uint32_t p)) \
	{ \
			uintptr_t retval = base::c_call<uintptr_t>(real_proc BOOST_PP_COMMA_IF(n) BOOST_PP_ENUM_PARAMS(n, p)); \
			uintptr_t vm = base::warcraft3::get_current_jass_virtual_machine(); \
			handle_manager<type_name>::instance()[retval] = *(uintptr_t*)(vm + 0x20); \
			return retval; \
		} \
	}; \
	template <const char* type_name, const char* porc_name> uintptr_t creater<type_name, n, porc_name>::real_proc = 0;

	BOOST_PP_REPEAT(5, DEFINE_CLASS_CREATER, ~)

	template <const char* type_name, const char* porc_name>
	struct destroyer
	{
	public:
		static void initialize()
		{
			base::warcraft3::jass::async_hook(porc_name, &real_proc, (uintptr_t)fake_proc);
		}

	private:
		static uintptr_t real_proc;
		static void __cdecl fake_proc(uintptr_t h)
		{
			handle_manager<type_name>::instance().erase(h);
			base::c_call<void>(real_proc, h);
		}
	};

	template <const char* type_name, const char* porc_name>
	uintptr_t destroyer<type_name, porc_name>::real_proc = 0;
}

#define LEAK_MONITOR      "yd_leak_monitor::"
#define LEAK_MONITOR_SIZE (sizeof(LEAK_MONITOR)-1)

static uintptr_t RealGetLocalizedHotkey = 0;
uint32_t __cdecl FakeGetLocalizedHotkey(uint32_t s)
{
	const char* str = base::warcraft3::jass::from_string(s);
	if (str)
	{
		if (0 == strncmp(LEAK_MONITOR, str, LEAK_MONITOR_SIZE))
		{
			if (strcmp(str + LEAK_MONITOR_SIZE, commonj::location) == 0)
			{
				return monitor::handle_manager<commonj::location>::instance().size();
			}
			else if (strcmp(str + LEAK_MONITOR_SIZE, commonj::effect) == 0)
			{
				return monitor::handle_manager<commonj::effect>::instance().size();
			}
			else if (strcmp(str + LEAK_MONITOR_SIZE, commonj::group) == 0)
			{
				return monitor::handle_manager<commonj::group>::instance().size();
			}
			else if (strcmp(str + LEAK_MONITOR_SIZE, commonj::region) == 0)
			{
				return monitor::handle_manager<commonj::region>::instance().size();
			}
			else if (strcmp(str + LEAK_MONITOR_SIZE, commonj::rect) == 0)
			{
				return monitor::handle_manager<commonj::rect>::instance().size();
			}
			else if (strcmp(str + LEAK_MONITOR_SIZE, commonj::force) == 0)
			{
				return monitor::handle_manager<commonj::force>::instance().size();
			}
		}
	}

	return base::c_call<uint32_t>(RealGetLocalizedHotkey, s);
}

void Initialize()
{
	monitor::creater  <commonj::location, 1, commonj::CameraSetupGetDestPositionLoc>::initialize();
	monitor::creater  <commonj::location, 1, commonj::GetCameraEyePositionLoc>::initialize();
	monitor::creater  <commonj::location, 1, commonj::GetCameraTargetPositionLoc>::initialize();
	monitor::creater  <commonj::location, 0, commonj::GetOrderPointLoc>::initialize();
	monitor::creater  <commonj::location, 0, commonj::GetSpellTargetLoc>::initialize();
	monitor::creater  <commonj::location, 1, commonj::GetStartLocationLoc>::initialize();
	monitor::creater  <commonj::location, 1, commonj::GetUnitLoc>::initialize();
	monitor::creater  <commonj::location, 1, commonj::GetUnitRallyPoint>::initialize();
	monitor::creater  <commonj::location, 2, commonj::Location>::initialize();
	monitor::destroyer<commonj::location, commonj::RemoveLocation>::initialize();

	monitor::creater  <commonj::effect,   3, commonj::AddSpecialEffect>::initialize();
	monitor::creater  <commonj::effect,   2, commonj::AddSpecialEffectLoc>::initialize();
	monitor::creater  <commonj::effect,   3, commonj::AddSpecialEffectTarget>::initialize();
	monitor::creater  <commonj::effect,   4, commonj::AddSpellEffect>::initialize();
	monitor::creater  <commonj::effect,   3, commonj::AddSpellEffectLoc>::initialize();
	monitor::creater  <commonj::effect,   4, commonj::AddSpellEffectById>::initialize();
	monitor::creater  <commonj::effect,   3, commonj::AddSpellEffectByIdLoc>::initialize();
	monitor::creater  <commonj::effect,   4, commonj::AddSpellEffectTarget>::initialize();
	monitor::creater  <commonj::effect,   4, commonj::AddSpellEffectTargetById>::initialize();
	monitor::destroyer<commonj::effect, commonj::DestroyEffect>::initialize();

	monitor::creater  <commonj::group,    0, commonj::CreateGroup>::initialize();
	monitor::destroyer<commonj::group, commonj::DestroyGroup>::initialize();

	monitor::creater  <commonj::region,   0, commonj::CreateRegion>::initialize();
	monitor::destroyer<commonj::region, commonj::RemoveRegion>::initialize();

	monitor::creater  <commonj::rect,     4, commonj::Rect>::initialize();
	monitor::creater  <commonj::rect,     2, commonj::RectFromLoc>::initialize();
	monitor::destroyer<commonj::rect, commonj::RemoveRect>::initialize();

	monitor::creater  <commonj::force,    0, commonj::CreateForce>::initialize();
	monitor::destroyer<commonj::force, commonj::DestroyForce>::initialize();

	base::warcraft3::jass::async_hook("GetLocalizedHotkey", &RealGetLocalizedHotkey, (uintptr_t)FakeGetLocalizedHotkey);
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID /*pReserved*/)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(module);
	}

	return TRUE;
}

const char *PluginName()
{
	return "yd_leak_monitor";
}

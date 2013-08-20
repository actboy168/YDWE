#include <windows.h>
#include <aero/function/fp_call.hpp>
#include <ydwe/warcraft3/native_function.h>
#include <ydwe/warcraft3/jass.h>
#include <boost/preprocessor/repetition.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <set>

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
	class handle_manager : public std::set<uintptr_t>
	{
	public:
		static bool insert(uintptr_t h)
		{
			return mgr_.insert(h).second;
		}

		static void erase(uintptr_t h)
		{
			mgr_.erase(h);
		}

		static size_t size()
		{
			return mgr_.size();
		}

	private:
		static std::set<uintptr_t> mgr_;
	};
	template <const char* type_name> std::set<uintptr_t> handle_manager<type_name>::mgr_;

	template <const char* type_name, size_t param, const char* porc_name>
	class creater;

#define DEFINE_CLASS_CREATER(z, n, unused) \
	template <const char* type_name, const char* porc_name> \
	class creater<type_name, n, porc_name> \
	{ \
	public: \
		static void initialize() \
		{ \
			ydwe::warcraft3::native_function::async_hook(porc_name, &real_proc, (uintptr_t)fake_proc); \
		} \
	private: \
		static uintptr_t real_proc; \
		static uintptr_t __cdecl fake_proc(BOOST_PP_ENUM_PARAMS(n, uint32_t p)) \
		{ \
			uintptr_t retval = aero::c_call<uintptr_t>(real_proc BOOST_PP_COMMA_IF(n) BOOST_PP_ENUM_PARAMS(n, p)); \
			handle_manager<type_name>::insert(retval); \
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
			ydwe::warcraft3::native_function::async_hook(porc_name, &real_proc, (uintptr_t)fake_proc);
		}

	private:
		static uintptr_t real_proc;
		static void __cdecl fake_proc(uintptr_t h)
		{
			handle_manager<type_name>::erase(h);
			aero::c_call<void>(real_proc, h);
		}
	};

	template <const char* type_name, const char* porc_name>
	uintptr_t destroyer<type_name, porc_name>::real_proc = 0;
}

static uintptr_t RealGetLocalizedHotkey = 0;
uint32_t __cdecl FakeGetLocalizedHotkey(uint32_t s)
{
	const char* str = ydwe::warcraft3::jass::from_string(s);
	if (str && (0 == strncmp("yd_leak_monitor::", str, sizeof("yd_leak_monitor::")-1)))
	{
		if (strcmp(str + (sizeof("yd_leak_monitor::")-1), commonj::location) == 0)
		{
			return monitor::handle_manager<commonj::location>::size();
		}
		else if (strcmp(str + (sizeof("yd_leak_monitor::")-1), commonj::effect) == 0)
		{
			return monitor::handle_manager<commonj::effect>::size();
		}
		else if (strcmp(str + (sizeof("yd_leak_monitor::")-1), commonj::group) == 0)
		{
			return monitor::handle_manager<commonj::group>::size();
		}
		else if (strcmp(str + (sizeof("yd_leak_monitor::")-1), commonj::region) == 0)
		{
			return monitor::handle_manager<commonj::region>::size();
		}
		else if (strcmp(str + (sizeof("yd_leak_monitor::")-1), commonj::rect) == 0)
		{
			return monitor::handle_manager<commonj::rect>::size();
		}
		else if (strcmp(str + (sizeof("yd_leak_monitor::")-1), commonj::force) == 0)
		{
			return monitor::handle_manager<commonj::force>::size();
		}
	}

	return aero::c_call<uint32_t>(RealGetLocalizedHotkey, s);
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

	monitor::creater  <commonj::region,    0, commonj::CreateRegion>::initialize();
	monitor::destroyer<commonj::region, commonj::RemoveRegion>::initialize();

	monitor::creater  <commonj::rect,    4, commonj::Rect>::initialize();
	monitor::creater  <commonj::rect,    2, commonj::RectFromLoc>::initialize();
	monitor::destroyer<commonj::rect, commonj::RemoveRect>::initialize();

	monitor::creater  <commonj::force,    0, commonj::CreateForce>::initialize();
	monitor::destroyer<commonj::force, commonj::DestroyForce>::initialize();

	ydwe::warcraft3::native_function::async_hook("GetLocalizedHotkey", &RealGetLocalizedHotkey, (uintptr_t)FakeGetLocalizedHotkey);
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

#include <windows.h>
#include <base/hook/fp_call.h>
#include <base/path/self.h>
#include <base/warcraft3/hashtable.h>
#include <base/warcraft3/jass.h>
#include <base/warcraft3/jass/global_variable.h>
#include <base/warcraft3/jass/hook.h>
#include <base/warcraft3/jass/opcode.h>
#include <base/warcraft3/war3_searcher.h>
#include <base/util/format.h>
#include <base/util/unicode.h>
#include <map>
#include <fstream>

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

#if 0
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
			handle_manager<type_name>::instance()[retval] = base::warcraft3::get_current_jass_pos();  \
			return retval; \
		} \
	}; \
	template <const char* type_name, const char* porc_name> uintptr_t creater<type_name, n, porc_name>::real_proc = 0;

	BOOST_PP_REPEAT(5, DEFINE_CLASS_CREATER, ~)
#endif
	template <const char* type_name, const char* porc_name, size_t param>
	struct creater_;

	template <const char* type_name, const char* porc_name>
	struct creater_ <type_name, porc_name, 0>
	{
		static uintptr_t real;
		static uintptr_t __cdecl fake()
		{
			uintptr_t retval = base::c_call<uintptr_t>(real);
			handle_manager<type_name>::instance()[retval] = base::warcraft3::get_current_jass_pos();
			return retval;
		}
	};
	template <const char* type_name, const char* porc_name>
	struct creater_ <type_name, porc_name, 1> {
		static uintptr_t real;
		static uintptr_t __cdecl fake(intptr_t a1)
		{
			uintptr_t retval = base::c_call<uintptr_t>(real, a1);
			handle_manager<type_name>::instance()[retval] = base::warcraft3::get_current_jass_pos();
			return retval;
		}
	};
	template <const char* type_name, const char* porc_name>
	struct creater_ <type_name, porc_name, 2> {
		static uintptr_t real;
		static uintptr_t __cdecl fake(intptr_t a1, intptr_t a2)
		{
			uintptr_t retval = base::c_call<uintptr_t>(real, a1, a2);
			handle_manager<type_name>::instance()[retval] = base::warcraft3::get_current_jass_pos();
			return retval;
		}
	};
	template <const char* type_name, const char* porc_name>
	struct creater_ <type_name, porc_name, 3> {
		static uintptr_t real;
		static uintptr_t __cdecl fake(intptr_t a1, intptr_t a2, intptr_t a3)
		{
			uintptr_t retval = base::c_call<uintptr_t>(real, a1, a2, a3);
			handle_manager<type_name>::instance()[retval] = base::warcraft3::get_current_jass_pos();
			return retval;
		}
	};
	template <const char* type_name, const char* porc_name>
	struct creater_ <type_name, porc_name, 4> {
		static uintptr_t real;
		static uintptr_t __cdecl fake(intptr_t a1, intptr_t a2, intptr_t a3, intptr_t a4)
		{
			uintptr_t retval = base::c_call<uintptr_t>(real, a1, a2, a3, a4);
			handle_manager<type_name>::instance()[retval] = base::warcraft3::get_current_jass_pos();
			return retval;
		}
	};
	template <const char* type_name, const char* porc_name> uintptr_t creater_<type_name, porc_name, 0>::real = 0;
	template <const char* type_name, const char* porc_name> uintptr_t creater_<type_name, porc_name, 1>::real = 0;
	template <const char* type_name, const char* porc_name> uintptr_t creater_<type_name, porc_name, 2>::real = 0;
	template <const char* type_name, const char* porc_name> uintptr_t creater_<type_name, porc_name, 3>::real = 0;
	template <const char* type_name, const char* porc_name> uintptr_t creater_<type_name, porc_name, 4>::real = 0;

	template <const char* type_name, size_t param, const char* porc_name>
	void creater() {
		base::warcraft3::jass::async_hook(porc_name, &creater_<type_name, porc_name, param>::real, (uintptr_t)creater_<type_name, porc_name, param>::fake);
	}

	template <const char* type_name>
	struct destroyer_ {
		static uintptr_t real;
		static void __cdecl fake(uintptr_t h) {
			handle_manager<type_name>::instance().erase(h);
			base::c_call<void>(real, h);
		}
	};
	template <const char* type_name> uintptr_t destroyer_<type_name>::real = 0;

	template <const char* type_name, const char* porc_name>
	void destroyer() {
		base::warcraft3::jass::async_hook(porc_name, &destroyer_<type_name>::real, (uintptr_t)destroyer_<type_name>::fake);
	}
}

struct handle_info_t
{
	uint32_t handle;
	uint32_t object;
	uint32_t reference;
	uint32_t pos;
	std::vector<std::string> gv_reference;

	handle_info_t()
		: handle(0)
		, object(0)
		, reference(0)
		, pos(0)
		, gv_reference()
	{ }
};

class handle_table_t
	: public std::map<uint32_t, handle_info_t>
{
public:
	void add_handle(uint32_t handle, uint32_t object, uint32_t reference)
	{
		handle_info_t hi;
		hi.handle = handle;
		hi.object = object;
		hi.reference = reference;
		insert(std::make_pair(handle, hi));
	}

	void add_gv_reference(uint32_t handle, const std::string& name)
	{
		auto it = find(handle);
		if (it == end())
		{
			return;
		}

		it->second.gv_reference.push_back(name);
	}

	void update_pos(std::map<uintptr_t, uintptr_t> m)
	{
		for (auto it = m.begin(); it != m.end(); ++it)
		{
			auto h = find(it->first);
			if (h != end()) 
			{
				h->second.pos = it->second;
			}
		}
	}
};

void create_report(std::fstream& fs)
{
	handle_table_t ht;

	using namespace base::warcraft3;

	hashtable::reverse_table* table = &((*get_jass_vm()->handle_table)->table);
	for (auto it = table->begin(); it != table->end(); ++it)
	{
		ht.add_handle(it->index_, (uint32_t)table->at(3 * (it->index_ - 0x100000) + 1), (uint32_t)table->at(3 * (it->index_ - 0x100000)));
	}

	hashtable::variable_table* vt = get_jass_vm()->global_table;
	for (auto it = vt->begin(); it != vt->end(); ++it)
	{
		jass::global_variable gv(&*it);

		if (jass::OPCODE_VARIABLE_HANDLE == gv.type())
		{
			ht.add_gv_reference((uint32_t)gv, gv.name());
		}
		else if (jass::OPCODE_VARIABLE_HANDLE_ARRAY == gv.type())
		{
			for (uint32_t i = 0; i < gv.array_size(); ++i)
			{
				ht.add_gv_reference(gv[i], base::format("%s[%d]", gv.name(), i));
			}
		}
	}

	ht.update_pos(monitor::handle_manager<commonj::location>::instance());
	ht.update_pos(monitor::handle_manager<commonj::effect>::instance());
	ht.update_pos(monitor::handle_manager<commonj::group>::instance());
	ht.update_pos(monitor::handle_manager<commonj::region>::instance());
	ht.update_pos(monitor::handle_manager<commonj::rect>::instance());
	ht.update_pos(monitor::handle_manager<commonj::force>::instance());


	fs << "---------------------------------------" << std::endl;
	fs << "           YDWE Leak Monitor           " << std::endl;
	fs << "---------------------------------------" << std::endl;
	fs << "Total:" << ht.size() << std::endl;
	fs << "---------------------------------------" << std::endl;

	for (auto it = ht.begin(); it != ht.end(); ++it)
	{
		handle_info_t& h = it->second;

		fs << base::format("%08X %08X", h.handle, h.reference) << std::endl;
		if (h.object)
		{
			uint32_t type = get_object_type(h.object);
			fs << base::format("  type: %c%c%c%c", ((const char*)&type)[3], ((const char*)&type)[2], ((const char*)&type)[1], ((const char*)&type)[0]) << std::endl;
		}
		if (h.pos)
		{
			jass::opcode *current_op = (jass::opcode *)h.pos;
			jass::opcode *op;
			for (op = current_op; op->opcode_type != jass::OPTYPE_FUNCTION; --op)
			{ }

			fs << base::format("  create: %s, %d", jass::from_stringid(op->arg), current_op - op) << std::endl;
		}
		if (!h.gv_reference.empty())
		{
			fs << base::format("  global:") << std::endl;
			for (auto gv = h.gv_reference.begin(); gv != h.gv_reference.end(); ++gv)
			{
				fs << base::format("    | %s", gv->c_str()) << std::endl;
			}
		}
	}
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
			if (strcmp(str + LEAK_MONITOR_SIZE, "create_report") == 0)
			{
				std::fstream fs;
				try {
					fs::path report = base::path::self().remove_filename().remove_filename().remove_filename() / L"logs" / L"leak_moniter_report.txt";

					std::fstream fs(report.c_str(), std::ios::out);
					if (fs.is_open())
					{
						create_report(fs);

						{
							using namespace base::warcraft3;
							uint32_t x = 0;
							uint32_t y = 0;
							std::string msg = base::w2u(base::format(L"*泄漏检测报告*已经保存到：%s", report.wstring()));
							jass::call("DisplayTextToPlayer", jass::call("GetLocalPlayer"), &x, &y, (jass::jstring_t)jass::to_string(msg.c_str()));
						}
						return 1;
					}
				} catch (...) { }
				return 0;
			}
			else if (strcmp(str + LEAK_MONITOR_SIZE, commonj::location) == 0)
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
	monitor::creater  <commonj::location, 1, commonj::CameraSetupGetDestPositionLoc>();
	monitor::creater  <commonj::location, 1, commonj::GetCameraEyePositionLoc>();
	monitor::creater  <commonj::location, 1, commonj::GetCameraTargetPositionLoc>();
	monitor::creater  <commonj::location, 0, commonj::GetOrderPointLoc>();
	monitor::creater  <commonj::location, 0, commonj::GetSpellTargetLoc>();
	monitor::creater  <commonj::location, 1, commonj::GetStartLocationLoc>();
	monitor::creater  <commonj::location, 1, commonj::GetUnitLoc>();
	monitor::creater  <commonj::location, 1, commonj::GetUnitRallyPoint>();
	monitor::creater  <commonj::location, 2, commonj::Location>();
	monitor::destroyer<commonj::location, commonj::RemoveLocation>();

	monitor::creater  <commonj::effect,   3, commonj::AddSpecialEffect>();
	monitor::creater  <commonj::effect,   2, commonj::AddSpecialEffectLoc>();
	monitor::creater  <commonj::effect,   3, commonj::AddSpecialEffectTarget>();
	monitor::creater  <commonj::effect,   4, commonj::AddSpellEffect>();
	monitor::creater  <commonj::effect,   3, commonj::AddSpellEffectLoc>();
	monitor::creater  <commonj::effect,   4, commonj::AddSpellEffectById>();
	monitor::creater  <commonj::effect,   3, commonj::AddSpellEffectByIdLoc>();
	monitor::creater  <commonj::effect,   4, commonj::AddSpellEffectTarget>();
	monitor::creater  <commonj::effect,   4, commonj::AddSpellEffectTargetById>();
	monitor::destroyer<commonj::effect, commonj::DestroyEffect>();

	monitor::creater  <commonj::group,    0, commonj::CreateGroup>();
	monitor::destroyer<commonj::group, commonj::DestroyGroup>();

	monitor::creater  <commonj::region,   0, commonj::CreateRegion>();
	monitor::destroyer<commonj::region, commonj::RemoveRegion>();

	monitor::creater  <commonj::rect,     4, commonj::Rect>();
	monitor::creater  <commonj::rect,     2, commonj::RectFromLoc>();
	monitor::destroyer<commonj::rect, commonj::RemoveRect>();

	monitor::creater  <commonj::force,    0, commonj::CreateForce>();
	monitor::destroyer<commonj::force, commonj::DestroyForce>();

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

#include <windows.h>


namespace base { namespace warcraft3 { namespace japi {
	void InitializeUnitState();
	void InitializeAbilityState();
	void InitializeEventDamageData();
	void InitializeDisplayChat();

	void initialize()
	{
		InitializeUnitState();
		InitializeAbilityState();
		InitializeEventDamageData();
		InitializeDisplayChat();
	}
}}}

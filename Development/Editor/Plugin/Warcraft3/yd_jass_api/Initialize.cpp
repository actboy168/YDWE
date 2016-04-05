#include <windows.h>


namespace base { namespace warcraft3 { namespace japi {
	void InitializeUnitState();
	void InitializeAbilityState();
	void InitializeItemState();
	void InitializeEventDamageData();
	void InitializeDisplayChat();
	void InitializeDisableButtonBlp();

	void initialize()
	{
		InitializeUnitState();
		InitializeAbilityState();
		InitializeItemState();
		InitializeEventDamageData();
		InitializeDisplayChat();
		InitializeDisableButtonBlp();
	}
}}}

#include <windows.h>


namespace warcraft3::japi {
	void InitializeUnitState();
	void InitializeAbilityState();
	void InitializeItemState();
	void InitializeEventDamageData();
	void InitializeDisplayChat();
	void InitializeDisableButtonBlp();
	void InitializeEffect();

	void initialize()
	{
		InitializeUnitState();
		InitializeAbilityState();
		InitializeItemState();
		InitializeEventDamageData();
		InitializeDisplayChat();
		InitializeDisableButtonBlp();
		InitializeEffect();
	}
}

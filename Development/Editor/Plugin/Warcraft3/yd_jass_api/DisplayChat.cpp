#include <base/warcraft3/jass.h>
#include <base/warcraft3/jass/hook.h>
#include <base/warcraft3/war3_searcher.h>
#include <base/hook/fp_call.h>

namespace base { namespace warcraft3 { namespace japi {


uintptr_t searchInGameChatWhat()
{
	//=========================================
	// (1)
	//
	//  mov     ecx, "InGameChatWhat" 
	//  call    sub_6F3304C0
	//  ds:flt_6F946378
	//  mov     eax, [esi+98h]
	//  movzx   edx, byte ptr [esi+15h]
	//  push    ecx             ; txtTime
	//  lea     ecx, [esp+114h+text]
	//  fstp    [esp+114h+var_114]
	//  push    eax             ; ctype
	//  push    ecx             ; text
	//  push    edx             ; PlayerID
	//  mov     ecx, ebx        ; this
	//  call    InGameChatWhat                  <----
	//=========================================
	uintptr_t ptr = get_war3_searcher().search_string("InGameChatWhat");
	ptr += 0x04;
	ptr = next_opcode(ptr, 0xE8, 5);
	ptr += 0x05;
	ptr = next_opcode(ptr, 0xE8, 5);
	return convert_function(ptr);
}

void __cdecl EXDisplayChat(uint32_t player_handle, uint32_t chat_recipient, uint32_t message)
{
	static uintptr_t InGameChatWhat = searchInGameChatWhat();

	uint32_t CGameUI = get_war3_searcher().get_gameui(0, 0);
	if (CGameUI != 0)
	{
		this_call<void>(InGameChatWhat, CGameUI, jass::call("GetPlayerId", player_handle), jass::from_trigstring(jass::from_string(message)), chat_recipient, (float)10.0f);
	}
}

void InitializeDisplayChat()
{
	jass::japi_add((uintptr_t)EXDisplayChat, "EXDisplayChat", "(Hplayer;IS)V");
}
}}}

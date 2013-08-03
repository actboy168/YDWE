#pragma once

#include <boost/cstdint.hpp>

boost::uint16_t weSetWindowCaptionPattern[] =
{
	0x56FF,
	0x8BFF, 0xF1FF,
	0x8BFF, 0xCAFF,
	0xE8FF, 0xC600, 0x6200, 0xFF00, 0xFFFF,
	0xA1FF, 0x8C00, 0x3C00, 0x8000, 0x00FF,
	0x50FF,
	0x56FF,
	0xFFFF, 0x15FF, 0xD800, 0x9600, 0x7500, 0x0000,
	0x5EFF,
	0xC3FF
};

/*
.text:004D1DB0 55                                   push    ebp
.text:004D1DB1 8B EC                                mov     ebp, esp
.text:004D1DB3 51                                   push    ecx
.text:004D1DB4 8B C2                                mov     eax, edx
.text:004D1DB6 8B 55 08                             mov     edx, [ebp+flag]
.text:004D1DB9 52                                   push    edx
.text:004D1DBA 8D 55 FC                             lea     edx, [ebp+var_4]
.text:004D1DBD 52                                   push    edx
.text:004D1DBE 8B D1                                mov     edx, ecx
.text:004D1DC0 8B 0D 04 00 82 00                    mov     ecx, dword_820004
.text:004D1DC6 50                                   push    eax
.text:004D1DC7 C7 45 FC 00 00 00 00                 mov     [ebp+var_4], 0
.text:004D1DCE E8 7D 91 F3 FF                       call    sub_40AF50
.text:004D1DD3 8B 45 FC                             mov     eax, [ebp+var_4]
.text:004D1DD6 8B E5                                mov     esp, ebp
.text:004D1DD8 5D                                   pop     ebp
.text:004D1DD9 C2 04 00                             retn    4
.text:004D1DD9                      sub_4D1DB0      endp
.text:004D1DDC 90 90 90 90                          align 10h
.text:004D1DE0 55                                   push    ebp
.text:004D1DE1 8B EC                                mov     ebp, esp
.text:004D1DE3 83 EC 08                             sub     esp, 8
.text:004D1DE6 53                                   push    ebx
.text:004D1DE7 8B 5D 08                             mov     ebx, [ebp+childKey]
.text:004D1DEA 56                                   push    esi
.text:004D1DEB 57                                   push    edi
.text:004D1DEC 8B 7D 0C                             mov     edi, [ebp+flag]
 */
/// Pattern for finding WeGetSystemParameter
boost::uint16_t weGetSystemParameterPattern[] =
{
	0xFF55,
	0xFF8B, 0xFFEC,
	0xFF51,
	0xFF8B, 0xFFC2,
	0xFF8B, 0xFF55, 0xFF08,
	0xFF52,
	0xFF8D, 0xFF55, 0xFFFC,
	0xFF52,
	0xFF8B, 0xFFD1,
	0xFF8B, 0xFF0D, 0x0004, 0x0000, 0x0082, 0x0000,
	0xFF50,
	0xFFC7, 0xFF45, 0xFFFC, 0xFF00, 0xFF00, 0xFF00, 0xFF00,
	0xFFE8, 0x007D, 0x0091, 0x00F3, 0x00FF,
	0xFF8B, 0xFF45, 0xFFFC,
	0xFF8B, 0xFFE5,
	0xFF5D,
	0xFFC2, 0xFF04, 0xFF00,
	0xFF90, 0xFF90, 0xFF90, 0xFF90,
	0xFF55,
	0xFF8B, 0xFFEC,
	0xFF83, 0xFFEC, 0xFF08,
	0xFF53
};

/*
.text:004E1EF0                      sub_4E1EF0      proc near
.text:004E1EF0 83 F9 FF                             cmp     ecx, -1
.text:004E1EF3 75 07                                jnz     short loc_4E1EFC
.text:004E1EF5 E8 C6 EB FE FF                       call    sub_4D0AC0
.text:004E1EFA 8B C8                                mov     ecx, eax
.text:004E1EFC
.text:004E1EFC                      loc_4E1EFC:
.text:004E1EFC 83 F9 01                             cmp     ecx, 1
.text:004E1EFF B8 C0 00 00 00                       mov     eax, 192
.text:004E1F04 7C 05                                jl      short loc_4E1F0B
.text:004E1F06 B8 00 01 00 00                       mov     eax, 256
.text:004E1F0B
.text:004E1F0B                      loc_4E1F0B:
.text:004E1F0B 0F AF C0                             imul    eax, eax
.text:004E1F0E C3                                   retn
.text:004E1F0E                      sub_4E1EF0      endp
*/
boost::uint16_t weVerifyMapCellsLimitPattern[] = 
{
	0xFF83, 0xFFF9, 0xFFFF, 
	0xFF75, 0xFF07, 
	0xFFE8, 0x00C6, 0x00EB, 0x00FE, 0x00FF, 
	0xFF8B, 0xFFC8, 
	0xFF83, 0xFFF9, 0xFF01, 
	0xFFB8, 0xFFC0, 0xFF00, 0xFF00, 0xFF00, 
	0xFF7C, 0xFF05, 
	0xFFB8, 0xFF00, 0xFF01, 0xFF00, 0xFF00, 
	0xFF0F, 0xFFAF, 0xFFC0, 
	0xFFC3
};

/*
.text:005A4B40                ; BOOL __fastcall trigger_name_validate(const char *trigger_name, BOOL allow_spaces)
.text:005A4B40                trigger_name_validate proc near         ; CODE XREF: sub_51EA10+18p
.text:005A4B40                                                        ; sub_51F970+14p ...
.text:005A4B40
.text:005A4B40                var_4           = dword ptr -4
.text:005A4B40
.text:005A4B40 55                             push    ebp
.text:005A4B41 8B EC                          mov     ebp, esp
.text:005A4B43 51                             push    ecx
.text:005A4B44 53                             push    ebx
.text:005A4B45 56                             push    esi
.text:005A4B46 57                             push    edi
.text:005A4B47 8B F9                          mov     edi, ecx
.text:005A4B49 8B F2                          mov     esi, edx
.text:005A4B4B 57                             push    edi
.text:005A4B4C 89 75 FC                       mov     [ebp+var_4], esi
.text:005A4B4F E8 94 04 12 00                 call    Storm_506
.text:005A4B54 8B D8                          mov     ebx, eax
.text:005A4B56 85 DB                          test    ebx, ebx
.text:005A4B58 75 07                          jnz     short loc_5A4B61
.text:005A4B5A 5F                             pop     edi
.text:005A4B5B 5E                             pop     esi
.text:005A4B5C 5B                             pop     ebx
.text:005A4B5D 8B E5                          mov     esp, ebp
.text:005A4B5F 5D                             pop     ebp
.text:005A4B60 C3                             retn
.text:005A4B61                ; ---------------------------------------------------------------------------
.text:005A4B61
.text:005A4B61                loc_5A4B61:                             ; CODE XREF: trigger_name_validate+18j
.text:005A4B61 8A 44 3B FF                    mov     al, [ebx+edi-1]
.text:005A4B65 3C 5F                          cmp     al, 5Fh
.text:005A4B67 75 09                          jnz     short loc_5A4B72
.text:005A4B69 5F                             pop     edi
.text:005A4B6A 5E                             pop     esi
.text:005A4B6B 33 C0                          xor     eax, eax
.text:005A4B6D 5B                             pop     ebx
.text:005A4B6E 8B E5                          mov     esp, ebp
.text:005A4B70 5D                             pop     ebp
.text:005A4B71 C3                             retn
.text:005A4B72                ; ---------------------------------------------------------------------------
.text:005A4B72
.text:005A4B72                loc_5A4B72:                             ; CODE XREF: trigger_name_validate+27j
.text:005A4B72 85 F6                          test    esi, esi
.text:005A4B74 74 0D                          jz      short loc_5A4B83
.text:005A4B76 3C 20                          cmp     al, 20h
.text:005A4B78 75 09                          jnz     short loc_5A4B83
.text:005A4B7A 5F                             pop     edi
.text:005A4B7B 5E                             pop     esi
.text:005A4B7C 33 C0                          xor     eax, eax
.text:005A4B7E 5B                             pop     ebx
.text:005A4B7F 8B E5                          mov     esp, ebp
.text:005A4B81 5D                             pop     ebp
.text:005A4B82 C3                             retn
.text:005A4B83                ; ---------------------------------------------------------------------------
.text:005A4B83
.text:005A4B83                loc_5A4B83:                             ; CODE XREF: trigger_name_validate+34j
.text:005A4B83                                                        ; trigger_name_validate+38j
.text:005A4B83 8A 07                          mov     al, [edi]
.text:005A4B85 3C 30                          cmp     al, 30h
.text:005A4B87 7C 0D                          jl      short loc_5A4B96
.text:005A4B89 3C 39                          cmp     al, 39h
.text:005A4B8B 7F 09                          jg      short loc_5A4B96
.text:005A4B8D
.text:005A4B8D                loc_5A4B8D:                             ; CODE XREF: trigger_name_validate+6Dj
.text:005A4B8D 5F                             pop     edi
.text:005A4B8E 5E                             pop     esi
.text:005A4B8F 33 C0                          xor     eax, eax
.text:005A4B91 5B                             pop     ebx
.text:005A4B92 8B E5                          mov     esp, ebp
.text:005A4B94 5D                             pop     ebp
.text:005A4B95 C3                             retn
.text:005A4B96                ; ---------------------------------------------------------------------------
.text:005A4B96
.text:005A4B96                loc_5A4B96:                             ; CODE XREF: trigger_name_validate+47j
.text:005A4B96                                                        ; trigger_name_validate+4Bj
.text:005A4B96 33 F6                          xor     esi, esi
.text:005A4B98 85 DB                          test    ebx, ebx
.text:005A4B9A 7E 18                          jle     short loc_5A4BB4
.text:005A4B9C 8D 64 24 00                    lea     esp, [esp+0]
.text:005A4BA0
.text:005A4BA0                loc_5A4BA0:                             ; CODE XREF: trigger_name_validate+72j
.text:005A4BA0 8B 55 FC                       mov     edx, [ebp+var_4]
.text:005A4BA3 8A 0C 3E                       mov     cl, [esi+edi]
.text:005A4BA6 E8 55 FF FF FF                 call    trigger_name_char_validate
.text:005A4BAB 85 C0                          test    eax, eax
.text:005A4BAD 74 DE                          jz      short loc_5A4B8D
.text:005A4BAF 46                             inc     esi
.text:005A4BB0 3B F3                          cmp     esi, ebx
.text:005A4BB2 7C EC                          jl      short loc_5A4BA0
.text:005A4BB4
.text:005A4BB4                loc_5A4BB4:                             ; CODE XREF: trigger_name_validate+5Aj
.text:005A4BB4 5F                             pop     edi
.text:005A4BB5 5E                             pop     esi
.text:005A4BB6 B8 01 00 00 00                 mov     eax, 1
.text:005A4BBB 5B                             pop     ebx
.text:005A4BBC 8B E5                          mov     esp, ebp
.text:005A4BBE 5D                             pop     ebp
.text:005A4BBF C3                             retn
.text:005A4BBF                trigger_name_validate endp
*/
/// Pattern for finding WeTriggerNameCheck
boost::uint16_t weTriggerNameCheckPattern[] =
{
	0xFF55,
	0xFF8B, 0xFFEC,
	0xFF51, 0xFF53,	0xFF56, 0xFF57,
	0xFF8B, 0xFFF9,
	0xFF8B, 0xFFF2,
	0xFF57,
	0xFF89, 0xFF75, 0xFFFC,
	0xFFE8, 0x0094, 0x0004, 0x0012, 0x0000,
	0xFF8B, 0xFFD8,
	0xFF85, 0xFFDB,
	0xFF75, 0x0007,
	0xFF5F, 0xFF5E, 0xFF5B,
	0xFF8B, 0xFFE5,
	0xFF5D,
	0xFFC3
};

/*
.text:0042E390                ; BOOL __fastcall char_verify(unsigned __int32 c, unsigned __int32 flag)
.text:0042E390                char_verify     proc near               ; CODE XREF: sub_42E360+19 p
.text:0042E390                                                        ; char_message_verify+36p
.text:0042E390 83 F9 20                       cmp     ecx, 20h
.text:0042E393 73 06                          jnb     short loc_42E39B
.text:0042E395 B8 01 00 00 00                 mov     eax, 1
.text:0042E39A C3                             retn
.text:0042E39B                ; ---------------------------------------------------------------------------
.text:0042E39B
.text:0042E39B                loc_42E39B:                             ; CODE XREF: char_verify+3 j
.text:0042E39B F6 C2 01                       test    dl, 1
.text:0042E39E 74 20                          jz      short loc_42E3C0
.text:0042E3A0 83 F9 61                       cmp     ecx, 61h
.text:0042E3A3 72 0B                          jb      short loc_42E3B0
.text:0042E3A5 83 F9 7A                       cmp     ecx, 7Ah
.text:0042E3A8 77 06                          ja      short loc_42E3B0
.text:0042E3AA B8 01 00 00 00                 mov     eax, 1
.text:0042E3AF C3                             retn
.text:0042E3B0                ; ---------------------------------------------------------------------------
.text:0042E3B0
.text:0042E3B0                loc_42E3B0:                             ; CODE XREF: char_verify+13 j
.text:0042E3B0                                                        ; char_verify+18 j
.text:0042E3B0 83 F9 41                       cmp     ecx, 41h
.text:0042E3B3 72 0B                          jb      short loc_42E3C0
.text:0042E3B5 83 F9 5A                       cmp     ecx, 5Ah
.text:0042E3B8 77 06                          ja      short loc_42E3C0
.text:0042E3BA B8 01 00 00 00                 mov     eax, 1
.text:0042E3BF C3                             retn
.text:0042E3C0                ; ---------------------------------------------------------------------------
.text:0042E3C0
.text:0042E3C0                loc_42E3C0:                             ; CODE XREF: char_verify+E j
.text:0042E3C0                                                        ; char_verify+23 j ...
.text:0042E3C0 F6 C2 02                       test    dl, 2
.text:0042E3C3 74 10                          jz      short loc_42E3D5
.text:0042E3C5 83 F9 30                       cmp     ecx, 30h
.text:0042E3C8 72 0B                          jb      short loc_42E3D5
.text:0042E3CA 83 F9 39                       cmp     ecx, 39h
.text:0042E3CD 77 06                          ja      short loc_42E3D5
.text:0042E3CF B8 01 00 00 00                 mov     eax, 1
.text:0042E3D4 C3                             retn
.text:0042E3D5                ; ---------------------------------------------------------------------------
.text:0042E3D5
.text:0042E3D5                loc_42E3D5:                             ; CODE XREF: char_verify+33 j
.text:0042E3D5                                                        ; char_verify+38 j ...
.text:0042E3D5 F6 C2 04                       test    dl, 4
.text:0042E3D8 74 10                          jz      short loc_42E3EA
.text:0042E3DA 83 F9 2B                       cmp     ecx, 2Bh
.text:0042E3DD 74 05                          jz      short loc_42E3E4
.text:0042E3DF 83 F9 2D                       cmp     ecx, 2Dh
.text:0042E3E2 75 06                          jnz     short loc_42E3EA
.text:0042E3E4
.text:0042E3E4                loc_42E3E4:                             ; CODE XREF: char_verify+4D j
.text:0042E3E4 B8 01 00 00 00                 mov     eax, 1
.text:0042E3E9 C3                             retn
.text:0042E3EA                ; ---------------------------------------------------------------------------
.text:0042E3EA
.text:0042E3EA                loc_42E3EA:                             ; CODE XREF: char_verify+48 j
.text:0042E3EA                                                        ; char_verify+52 j
.text:0042E3EA F6 C2 08                       test    dl, 8
.text:0042E3ED 74 0B                          jz      short loc_42E3FA
.text:0042E3EF 83 F9 2E                       cmp     ecx, 2Eh
.text:0042E3F2 75 06                          jnz     short loc_42E3FA
.text:0042E3F4 B8 01 00 00 00                 mov     eax, 1
.text:0042E3F9 C3                             retn
.text:0042E3FA                ; ---------------------------------------------------------------------------
.text:0042E3FA
.text:0042E3FA                loc_42E3FA:                             ; CODE XREF: char_verify+5D j
.text:0042E3FA                                                        ; char_verify+62 j
.text:0042E3FA F6 C2 10                       test    dl, 10h
.text:0042E3FD 74 0B                          jz      short loc_42E40A
.text:0042E3FF 83 F9 20                       cmp     ecx, 20h
.text:0042E402 75 06                          jnz     short loc_42E40A
.text:0042E404 B8 01 00 00 00                 mov     eax, 1
.text:0042E409 C3                             retn
.text:0042E40A                ; ---------------------------------------------------------------------------
.text:0042E40A
.text:0042E40A                loc_42E40A:                             ; CODE XREF: char_verify+6D j
.text:0042E40A                                                        ; char_verify+72 j
.text:0042E40A F6 C2 20                       test    dl, 20h
.text:0042E40D 74 0B                          jz      short loc_42E41A
.text:0042E40F 83 F9 5F                       cmp     ecx, 5Fh
.text:0042E412 75 06                          jnz     short loc_42E41A
.text:0042E414 B8 01 00 00 00                 mov     eax, 1
.text:0042E419 C3                             retn
.text:0042E41A                ; ---------------------------------------------------------------------------
.text:0042E41A
.text:0042E41A                loc_42E41A:                             ; CODE XREF: char_verify+7D j
.text:0042E41A                                                        ; char_verify+82 j
.text:0042E41A F6 C2 40                       test    dl, 40h
.text:0042E41D 74 0B                          jz      short loc_42E42A
.text:0042E41F 83 F9 7F                       cmp     ecx, 7Fh
.text:0042E422 73 06                          jnb     short loc_42E42A
.text:0042E424 B8 01 00 00 00                 mov     eax, 1
.text:0042E429 C3                             retn
.text:0042E42A                ; ---------------------------------------------------------------------------
.text:0042E42A
.text:0042E42A                loc_42E42A:                             ; CODE XREF: char_verify+8D j
.text:0042E42A                                                        ; char_verify+92 j
.text:0042E42A 33 C0                          xor     eax, eax
.text:0042E42C C3                             retn
.text:0042E42C                char_verify     endp
 */
/// Pattern for finding WeTriggerNameInputCharCheck
boost::uint16_t weTriggerNameInputCharCheckPattern[] =
{
	0xFF83, 0xFFF9, 0xFF20,
	0xFF73, 0x0006,
	0xFFB8, 0xFF01, 0xFF00, 0xFF00, 0xFF00,
	0xFFC3,
	0xFFF6, 0xFFC2, 0xFF01,
	0xFF74, 0x0020,
	0xFF83, 0xFFF9, 0xFF61,
	0xFF72, 0x000B,
	0xFF83, 0xFFF9, 0xFF7A,
	0xFF77, 0x0006,
	0xFFB8, 0xFF01, 0xFF00, 0xFF00, 0xFF00,
	0xFFC3
};

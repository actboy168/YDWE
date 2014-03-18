#include "PEMemoryFileInfo.h"
#include "MemoryPatternSearch.h"

#include "YDWELogger.h"
#include "YDWEEditorNativeFunction.h"
#include <base/util/format.h>
#include <base/hook/fp_call.h>

namespace NYDWE
{

/*
.text:004EEC00                   ; int __fastcall WeTemplateStringTranslate(const char *templateString, char *buffer, size_t bufferSize, int silent)
.text:004EEC00                   WeTemplateStringTranslate proc near     ; CODE XREF: sub_402540+8Cp
.text:004EEC00                                                           ; sub_402540+A0p ...
.text:004EEC00
.text:004EEC00                   format_buffer   = byte ptr -208h
.text:004EEC00                   temp_buffer     = byte ptr -104h
.text:004EEC00                   bufferSize     = dword ptr  8
.text:004EEC00                   silent          = dword ptr  0Ch
.text:004EEC00
.text:004EEC00 55                                push    ebp
.text:004EEC01 8B EC                             mov     ebp, esp
.text:004EEC03 81 EC 08 02 00 00                 sub     esp, 208h
.text:004EEC09 8B 45 08                          mov     eax, [ebp+bufferSize]
.text:004EEC0C 53                                push    ebx
.text:004EEC0D 56                                push    esi
.text:004EEC0E 57                                push    edi
.text:004EEC0F 6A 00                             push    0               ; a5
.text:004EEC11 8B DA                             mov     ebx, edx
.text:004EEC13 8B F1                             mov     esi, ecx
.text:004EEC15 50                                push    eax             ; bufferSize
.text:004EEC16 53                                push    ebx             ; buffer
.text:004EEC17 8B D6                             mov     edx, esi        ; templateString
.text:004EEC19 B9 50 11 7C 00                    mov     ecx, offset aWorldeditstrin ; "WorldEditStrings"
.text:004EEC1E E8 9D 32 FE FF                    call    sub_4D1EC0
.text:004EEC23 8B F8                             mov     edi, eax
.text:004EEC25 85 FF                             test    edi, edi
.text:004EEC27 0F 85 86 00 00 00                 jnz     loc_4EECB3
.text:004EEC2D 8B 45 0C                          mov     eax, [ebp+silent]
.text:004EEC30 85 C0                             test    eax, eax
.text:004EEC32 75 74                             jnz     short loc_4EECA8
.text:004EEC34 68 FF FF FF 7F                    push    7FFFFFFFh       ; max_size
.text:004EEC39 68 30 11 7C 00                    push    offset aWestring_wa_13 ; "WESTRING_WARNING_MISSINGSTRING"
.text:004EEC3E 56                                push    esi             ; str_a
.text:004EEC3F E8 B6 63 1D 00                    call    storm_string_ncompare
.text:004EEC44 85 C0                             test    eax, eax
.text:004EEC46 75 18                             jnz     short loc_4EEC60
.text:004EEC48 68 FF FF FF 7F                    push    7FFFFFFFh       ; size
.text:004EEC4D 68 FC 10 7C 00                    push    offset aTheMissingStri ; "The 'missing string' warning string is "...
.text:004EEC52 8D 8D FC FE FF FF                 lea     ecx, [ebp+temp_buffer]
.text:004EEC58 51                                push    ecx             ; dest
.text:004EEC59 E8 5A 63 1D 00                    call    storm_string_copy
.text:004EEC5E EB 17                             jmp     short loc_4EEC77
.text:004EEC60                   ; ---------------------------------------------------------------------------
.text:004EEC60
.text:004EEC60                   loc_4EEC60:                             ; CODE XREF: WeTemplateStringTranslate+46j
.text:004EEC60 6A 00                             push    0               ; a4
.text:004EEC62 68 04 01 00 00                    push    104h            ; bufferSize
.text:004EEC67 8D 95 FC FE FF FF                 lea     edx, [ebp+temp_buffer] ; buffer
.text:004EEC6D B9 30 11 7C 00                    mov     ecx, offset aWestring_wa_13 ; "WESTRING_WARNING_MISSINGSTRING"
.text:004EEC72 E8 89 FF FF FF                    call    WeTemplateStringTranslate
.text:004EEC77
.text:004EEC77                   loc_4EEC77:                             ; CODE XREF: WeTemplateStringTranslate+5Ej
.text:004EEC77 56                                push    esi
.text:004EEC78 8D 95 FC FE FF FF                 lea     edx, [ebp+temp_buffer]
.text:004EEC7E 52                                push    edx
.text:004EEC7F 68 CC 97 7A 00                    push    offset aSS_11   ; "%s: %s"
.text:004EEC84 8D 85 F8 FD FF FF                 lea     eax, [ebp+format_buffer]
.text:004EEC8A 68 04 01 00 00                    push    104h            ; bufferSize
.text:004EEC8F 50                                push    eax             ; buffer
.text:004EEC90 E8 1D 63 1D 00                    call    storm_string_printf
.text:004EEC95 83 C4 14                          add     esp, 14h
.text:004EEC98 BA 01 00 00 00                    mov     edx, 1
.text:004EEC9D 8D 8D F8 FD FF FF                 lea     ecx, [ebp+format_buffer]
.text:004EECA3 E8 58 6C FE FF                    call    sub_4D5900
.text:004EECA8
.text:004EECA8                   loc_4EECA8:                             ; CODE XREF: WeTemplateStringTranslate+32j
.text:004EECA8 8B 4D 08                          mov     ecx, [ebp+bufferSize]
.text:004EECAB 51                                push    ecx             ; size
.text:004EECAC 56                                push    esi             ; source
.text:004EECAD 53                                push    ebx             ; dest
.text:004EECAE E8 05 63 1D 00                    call    storm_string_copy
.text:004EECB3
.text:004EECB3                   loc_4EECB3:                             ; CODE XREF: WeTemplateStringTranslate+27j
.text:004EECB3 8B C7                             mov     eax, edi
.text:004EECB5 5F                                pop     edi
.text:004EECB6 5E                                pop     esi
.text:004EECB7 5B                                pop     ebx
.text:004EECB8 8B E5                             mov     esp, ebp
.text:004EECBA 5D                                pop     ebp
.text:004EECBB C2 08 00                          retn    8
.text:004EECBB                   WeTemplateStringTranslate endp
	*/
static uint16_t gWeTemplateStringTranslatePattern[] =
{
	0xFF55, 
	0xFF8B, 0xFFEC,
	0xFF81, 0xFFEC, 0xFF08, 0xFF02, 0xFF00, 0xFF00,
	0xFF8B, 0xFF45, 0xFF08,
	0xFF53,
	0xFF56,
	0xFF57,
	0xFF6A, 0xFF00,
	0xFF8B, 0xFFDA,
	0xFF8B, 0x00F1,
	0xFF50,
	0xFF53,
	0xFF8B, 0xFFD6,
	0xFFB9, 0x0050, 0x0071, 0x007C, 0x0000,
	0xFFE8, 0x009D, 0x0032, 0x00FE, 0x00FF
};

static void *fpgWeTemplateStringTranslate;

BOOL WeTemplateStringTranslate(const char *templateString, char *buffer, size_t bufferSize, int silent)
{
	if (fpgWeTemplateStringTranslate)
		return base::fast_call<BOOL>(fpgWeTemplateStringTranslate, templateString, buffer, bufferSize, silent);
	else
		return FALSE;
}

/*
.text:004D5900                      ; void __fastcall WeMessageShow(const char *message, int flag)
.text:004D5900                      WeMessageShow proc near               ; CODE XREF: sub_4D5AB0+27p
.text:004D5900                                                              ; sub_4DCC40+68p ...
.text:004D5900
.text:004D5900                      dest            = byte ptr -104h
.text:004D5900
.text:004D5900 55                                   push    ebp
.text:004D5901 8B EC                                mov     ebp, esp
.text:004D5903 81 EC 04 01 00 00                    sub     esp, 104h
.text:004D5909 56                                   push    esi
.text:004D590A 57                                   push    edi
.text:004D590B 68 FF FF FF 7F                       push    7FFFFFFFh       ; size
.text:004D5910 51                                   push    ecx             ; source
.text:004D5911 8D 85 FC FE FF FF                    lea     eax, [ebp+dest]
.text:004D5917 50                                   push    eax             ; dest
.text:004D5918 8B F2                                mov     esi, edx
.text:004D591A E8 99 F6 1E 00                       call    storm_string_copy
.text:004D591F BA 04 01 00 00                       mov     edx, 104h
.text:004D5924 8D 8D FC FE FF FF                    lea     ecx, [ebp+dest]
.text:004D592A E8 91 93 01 00                       call    sub_4EECC0
.text:004D592F 85 F6                                test    esi, esi
.text:004D5931 0F 84 54 01 00 00                    jz      loc_4D5A8B
.text:004D5937 83 3D 0C FB 81 00 FF                 cmp     dword_81FB0C, 0FFFFFFFFh
.text:004D593E 74 73                                jz      short loc_4D59B3
.text:004D5940 8D 8D FC FE FF FF                    lea     ecx, [ebp+dest]
.text:004D5946 51                                   push    ecx
.text:004D5947 E8 C0 F6 1E 00                       call    Storm_590
.text:004D594C 8B F8                                mov     edi, eax
.text:004D594E A1 0C FB 81 00                       mov     eax, dword_81FB0C
.text:004D5953 23 C7                                and     eax, edi
.text:004D5955 8D 14 40                             lea     edx, [eax+eax*2]
.text:004D5958 A1 04 FB 81 00                       mov     eax, dword_81FB04
.text:004D595D 33 C9                                xor     ecx, ecx
.text:004D595F 8D 44 90 04                          lea     eax, [eax+edx*4+4]
.text:004D5963 8B 40 04                             mov     eax, [eax+4]
.text:004D5966 85 C0                                test    eax, eax
.text:004D5968 0F 9E C1                             setle   cl
.text:004D596B 49                                   dec     ecx
.text:004D596C 23 C8                                and     ecx, eax
.text:004D596E 8B F1                                mov     esi, ecx
.text:004D5970 85 F6                                test    esi, esi
.text:004D5972 7E 3F                                jle     short loc_4D59B3
.text:004D5974
.text:004D5974                      loc_4D5974:                             ; CODE XREF: WeMessageShow+B1j
.text:004D5974 39 3E                                cmp     [esi], edi
.text:004D5976 75 1D                                jnz     short loc_4D5995
.text:004D5978 8B 46 14                             mov     eax, [esi+14h]
.text:004D597B 68 FF FF FF 7F                       push    7FFFFFFFh
.text:004D5980 8D 95 FC FE FF FF                    lea     edx, [ebp+dest]
.text:004D5986 52                                   push    edx
.text:004D5987 50                                   push    eax
.text:004D5988 E8 79 F6 1E 00                       call    Storm_509
.text:004D598D 85 C0                                test    eax, eax
.text:004D598F 0F 84 07 01 00 00                    jz      loc_4D5A9C
.text:004D5995
.text:004D5995                      loc_4D5995:                             ; CODE XREF: WeMessageShow+76j
.text:004D5995 A1 0C FB 81 00                       mov     eax, dword_81FB0C
.text:004D599A 8B 15 04 FB 81 00                    mov     edx, dword_81FB04
.text:004D59A0 23 C7                                and     eax, edi
.text:004D59A2 8D 0C 40                             lea     ecx, [eax+eax*2]
.text:004D59A5 8D 04 8A                             lea     eax, [edx+ecx*4]
.text:004D59A8 8B 00                                mov     eax, [eax]
.text:004D59AA 03 C6                                add     eax, esi
.text:004D59AC 8B 70 04                             mov     esi, [eax+4]
.text:004D59AF 85 F6                                test    esi, esi
.text:004D59B1 7F C1                                jg      short loc_4D5974
.text:004D59B3
.text:004D59B3                      loc_4D59B3:                             ; CODE XREF: WeMessageShow+3Ej
.text:004D59B3                                                              ; WeMessageShow+72j
.text:004D59B3 53                                   push    ebx
.text:004D59B4 8D 85 FC FE FF FF                    lea     eax, [ebp+dest]
.text:004D59BA 50                                   push    eax
.text:004D59BB E8 4C F6 1E 00                       call    Storm_590
.text:004D59C0 8B D8                                mov     ebx, eax
.text:004D59C2 A1 0C FB 81 00                       mov     eax, dword_81FB0C
.text:004D59C7 83 F8 FF                             cmp     eax, 0FFFFFFFFh
.text:004D59CA 75 36                                jnz     short loc_4D5A02
.text:004D59CC 6A 04                                push    4
.text:004D59CE B9 FC FA 81 00                       mov     ecx, offset dword_81FAFC
.text:004D59D3 C7 05 0C FB 81 00 03+                mov     dword_81FB0C, 3
.text:004D59DD E8 4E 30 00 00                       call    sub_4D8A30
.text:004D59E2 33 FF                                xor     edi, edi
.text:004D59E4 33 F6                                xor     esi, esi
.text:004D59E6
.text:004D59E6                      loc_4D59E6:                             ; CODE XREF: WeMessageShow+100j
.text:004D59E6 8B 0D 04 FB 81 00                    mov     ecx, dword_81FB04
.text:004D59EC 6A 04                                push    4
.text:004D59EE 03 CE                                add     ecx, esi
.text:004D59F0 E8 0B 28 00 00                       call    sub_4D8200
.text:004D59F5 A1 0C FB 81 00                       mov     eax, dword_81FB0C
.text:004D59FA 47                                   inc     edi
.text:004D59FB 83 C6 0C                             add     esi, 0Ch
.text:004D59FE 3B F8                                cmp     edi, eax
.text:004D5A00 76 E4                                jbe     short loc_4D59E6
.text:004D5A02
.text:004D5A02                      loc_4D5A02:                             ; CODE XREF: WeMessageShow+CAj
.text:004D5A02 23 C3                                and     eax, ebx
.text:004D5A04 8B F0                                mov     esi, eax
.text:004D5A06 56                                   push    esi
.text:004D5A07 B9 E8 FA 81 00                       mov     ecx, offset dword_81FAE8
.text:004D5A0C E8 CF 2D 00 00                       call    sub_4D87E0
.text:004D5A11 85 C0                                test    eax, eax
.text:004D5A13 74 08                                jz      short loc_4D5A1D
.text:004D5A15 8B 35 0C FB 81 00                    mov     esi, dword_81FB0C
.text:004D5A1B 23 F3                                and     esi, ebx
.text:004D5A1D
.text:004D5A1D                      loc_4D5A1D:                             ; CODE XREF: WeMessageShow+113j
.text:004D5A1D A1 04 FB 81 00                       mov     eax, dword_81FB04
.text:004D5A22 6A 00                                push    0
.text:004D5A24 8D 14 76                             lea     edx, [esi+esi*2]
.text:004D5A27 8D 0C 90                             lea     ecx, [eax+edx*4]
.text:004D5A2A 8B 15 E8 FA 81 00                    mov     edx, dword_81FAE8
.text:004D5A30 6A 00                                push    0
.text:004D5A32 51                                   push    ecx
.text:004D5A33 B9 E8 FA 81 00                       mov     ecx, offset dword_81FAE8
.text:004D5A38 FF 52 04                             call    dword ptr [edx+4]
.text:004D5A3B 6A 00                                push    0
.text:004D5A3D 8B F0                                mov     esi, eax
.text:004D5A3F 6A 02                                push    2
.text:004D5A41 56                                   push    esi
.text:004D5A42 B9 EC FA 81 00                       mov     ecx, offset dword_81FAEC
.text:004D5A47 E8 B4 2C 00 00                       call    sub_4D8700
.text:004D5A4C 89 1E                                mov     [esi], ebx
.text:004D5A4E 8B 46 14                             mov     eax, [esi+14h]
.text:004D5A51 8D 8D FC FE FF FF                    lea     ecx, [ebp+dest]
.text:004D5A57 3B C1                                cmp     eax, ecx
.text:004D5A59 5B                                   pop     ebx
.text:004D5A5A 74 2F                                jz      short loc_4D5A8B
.text:004D5A5C 85 C0                                test    eax, eax
.text:004D5A5E 74 12                                jz      short loc_4D5A72
.text:004D5A60 6A 00                                push    0
.text:004D5A62 68 38 0A 00 00                       push    0A38h
.text:004D5A67 68 D8 A9 7A 00                       push    offset a______StormHSt ; "..\\..\\..\\Storm\\H\\stpl.h"
.text:004D5A6C 50                                   push    eax
.text:004D5A6D E8 2E F5 1E 00                       call    Storm_403
.text:004D5A72
.text:004D5A72                      loc_4D5A72:                             ; CODE XREF: WeMessageShow+15Ej
.text:004D5A72 68 39 0A 00 00                       push    0A39h
.text:004D5A77 68 D8 A9 7A 00                       push    offset a______StormHSt ; "..\\..\\..\\Storm\\H\\stpl.h"
.text:004D5A7C 8D 95 FC FE FF FF                    lea     edx, [ebp+dest]
.text:004D5A82 52                                   push    edx
.text:004D5A83 E8 A2 F5 1E 00                       call    Storm_507
.text:004D5A88 89 46 14                             mov     [esi+14h], eax
.text:004D5A8B
.text:004D5A8B                      loc_4D5A8B:                             ; CODE XREF: WeMessageShow+31j
.text:004D5A8B                                                              ; WeMessageShow+15Aj
.text:004D5A8B 8D 85 FC FE FF FF                    lea     eax, [ebp+dest]
.text:004D5A91 50                                   push    eax
.text:004D5A92 6A 04                                push    4
.text:004D5A94 E8 47 71 F4 FF                       call    sub_41CBE0
.text:004D5A99 83 C4 08                             add     esp, 8
.text:004D5A9C
.text:004D5A9C                      loc_4D5A9C:                             ; CODE XREF: WeMessageShow+8Fj
.text:004D5A9C 5F                                   pop     edi
.text:004D5A9D 5E                                   pop     esi
.text:004D5A9E 8B E5                                mov     esp, ebp
.text:004D5AA0 5D                                   pop     ebp
.text:004D5AA1 C3                                   retn
.text:004D5AA1                      WeMessageShow endp
*/
static uint16_t gWeMessageShowPattern[] =
{
	0xFF55, 
	0xFF8B, 0xFFEC,
	0xFF81, 0xFFEC, 0xFF04, 0xFF01, 0xFF00, 0xFF00,
	0xFF56,
	0xFF57,
	0xFF68, 0xFFFF, 0xFFFF, 0xFFFF, 0xFF7F,
	0xFF51,
	0xFF8D, 0xFF85, 0xFFFC, 0xFFFE, 0xFFFF, 0xFFFF,
	0xFF50,
	0xFF8B, 0xFFF2,
	0xFFE8, 0x0099, 0x00F6, 0x001E, 0x0000, 
	0xFFBA, 0xFF04, 0xFF01, 0xFF00, 0xFF00
};
static void *fpgWeMessageShow;

void WeMessageShow(const char *message, int flag)
{
	if (fpgWeMessageShow)
		base::fast_call<void>(fpgWeMessageShow, message, flag);
}

void NativeInit()
{
	CPEMemoryFileInfo pm;
	CPEMemoryFileInfo::TSectionInfoResult result = pm.querySection(".text");
	uintptr_t pWeTextSectionBase = result->get<0>();
	uint32_t weTextSectionLength = result->get<1>();

	LOGGING_DEBUG(lg) << "WE native function initialization begins.";

	fpgWeTemplateStringTranslate = MemoryPatternSearch((void *)pWeTextSectionBase, weTextSectionLength, &gWeTemplateStringTranslatePattern[0], sizeof(gWeTemplateStringTranslatePattern));
	LOGGING_TRACE(lg) << base::format("Found WeTranlateTemplateString at 0x%08X.", fpgWeTemplateStringTranslate);

	fpgWeMessageShow = MemoryPatternSearch((void *)pWeTextSectionBase, weTextSectionLength, &gWeMessageShowPattern[0], sizeof(gWeMessageShowPattern));
	LOGGING_TRACE(lg) << base::format("Found WeMessageShow at 0x%08X.", fpgWeMessageShow);

	LOGGING_DEBUG(lg) << "WE native function initialization finished.";
}

} // namespace NYDWE

void YDWEShowMessage(const char *message, int flag)
{
	NYDWE::WeMessageShow(message, flag);
}

intptr_t YDWETranslateTemplateString(const char *templateString, char *buffer, size_t bufferSize, int silent)
{
	return NYDWE::WeTemplateStringTranslate(templateString, buffer, bufferSize, silent);
}

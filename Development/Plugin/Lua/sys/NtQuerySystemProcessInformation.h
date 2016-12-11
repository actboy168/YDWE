#pragma once

#include <Windows.h>
#include <functional>

typedef std::function<void(DWORD, PWSTR, USHORT)> PFN_NTCBPROCESSINFORMATION;
bool NtQuerySystemProcessInformation(PFN_NTCBPROCESSINFORMATION NtCBProcessInformation);

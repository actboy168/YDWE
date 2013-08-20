#pragma once

#include <functional>
#include <string>

void ParseCommandLine(std::function<void(std::wstring const&, std::wstring const&)> Proc);

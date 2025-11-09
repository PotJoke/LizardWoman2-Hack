#pragma once

#include <Windows.h>
#include <Psapi.h>
#include <vector>
#include <string>
#include <sstream>

#pragma comment(lib, "Psapi.lib")

static uintptr_t FindPatternInModule(const char* moduleName, const char* pattern)
{
    HMODULE hModule = GetModuleHandleA(moduleName);
    if (!hModule) return 0;

    MODULEINFO mi = {};
    if (!GetModuleInformation(GetCurrentProcess(), hModule, &mi, sizeof(mi)))
        return 0;

    BYTE* base = reinterpret_cast<BYTE*>(mi.lpBaseOfDll);
    size_t size = static_cast<size_t>(mi.SizeOfImage);

    std::vector<int> bytes;
    std::istringstream iss(pattern);
    std::string token;
    while (iss >> token) {
        if (token == "?" || token == "??") {
            bytes.push_back(-1);
        }
        else {
            int val = 0;
            std::stringstream ss;
            ss << std::hex << token;
            ss >> val;
            bytes.push_back(val & 0xFF);
        }
    }

    size_t plen = bytes.size();
    if (plen == 0 || plen > size) return 0;

    for (size_t i = 0; i <= size - plen; ++i) {
        bool ok = true;
        for (size_t j = 0; j < plen; ++j) {
            if (bytes[j] != -1 && bytes[j] != base[i + j]) {
                ok = false;
                break;
            }
        }
        if (ok) return reinterpret_cast<uintptr_t>(base + i);
    }

    return 0;
}

static uintptr_t GetOffsetFromPattern(const char* moduleName, const char* pattern)
{
    uintptr_t addr = FindPatternInModule(moduleName, pattern);
    if (!addr) return 0;
    HMODULE hModule = GetModuleHandleA(moduleName);
    if (!hModule) return 0;
    return addr - reinterpret_cast<uintptr_t>(hModule);
}
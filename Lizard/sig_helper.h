#pragma once
#include "pch.h"
#include <Windows.h>
#include <Psapi.h>
#include <cstdio>
#include <string>

#pragma comment(lib, "Psapi.lib")

static uintptr_t ResolveAddressValue(uintptr_t providedValue, HMODULE module)
{
    MODULEINFO mi = {};
    if (!GetModuleInformation(GetCurrentProcess(), module, &mi, sizeof(mi)))
        return 0;

    uintptr_t base = reinterpret_cast<uintptr_t>(mi.lpBaseOfDll);
    uintptr_t end = base + mi.SizeOfImage;

    if (providedValue >= base && providedValue < end)
        return providedValue;
    return base + providedValue;
}

static void DumpSignature(uintptr_t absoluteAddr, size_t length = 32)
{
    BYTE* p = reinterpret_cast<BYTE*>(absoluteAddr);
    printf("Dump at %p, len=%zu\n", (void*)absoluteAddr, length);

    std::string hexLine;
    std::string pattern;
    for (size_t i = 0; i < length; ++i) {
        BYTE b = p[i];
        char buf[8];
        sprintf_s(buf, "%02X ", b);
        hexLine += buf;

        if ((b == 0xE8 || b == 0xE9) && i + 4 < length) {
            pattern += "E8 ";
            pattern += "?? ?? ?? ?? ";
            i += 4;
            continue;
        }

        char twohex[8];
        sprintf_s(twohex, "%02X ", b);
        pattern += twohex;
    }

    printf("Hex: %s\n", hexLine.c_str());
    printf("Pattern: %s\n", pattern.c_str());
    printf("Suggested pattern length: %zu bytes (adjust and replace dynamic fields with '??')\n", length);
}

static void DumpSignatureForModule(const char* moduleName, uintptr_t valueOrRVA, size_t length = 32)
{
    HMODULE hMod = GetModuleHandleA(moduleName);
    if (!hMod) {
        printf("Module %s not loaded\n", moduleName);
        return;
    }
    uintptr_t addr = ResolveAddressValue(valueOrRVA, hMod);
    if (!addr) {
        printf("Failed to resolve address\n");
        return;
    }
    DumpSignature(addr, length);
}

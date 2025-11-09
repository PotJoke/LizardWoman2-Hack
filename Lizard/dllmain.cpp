#include "pattern_scan.h"
#include "sig_helper.h"

#include <Windows.h>
#include <MinHook.h>
#include <cstdio>

uintptr_t GameAssembly = (uintptr_t)GetModuleHandle("GameAssembly.dll");

//I will make button to hide this
void CreateConsole() {
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);
}

void init() {
    MH_Initialize();
    CreateConsole();
}

bool(__fastcall* getStoryModeEnabled_o)(DWORD*, DWORD*);
bool __stdcall getStoryModeEnabled(DWORD* __this, DWORD* method) {
    printf("getStoryModeEnabled called\n");
    return false;
}

bool(__fastcall* StoryModeEnabled_o)(DWORD*);
bool __stdcall StoryModeEnabled(DWORD* method) {
    printf("StoryModeEnabled called\n");
    return false;
}

bool(__fastcall* CheckOneOfScenariosUnlocked_o)(DWORD*, DWORD*, DWORD*);
bool(__fastcall* CheckSetOfScenariosUnlocked_o)(DWORD*, DWORD*, DWORD*);
bool __stdcall CheckScenariosUnlocked(DWORD* __this, DWORD* scenes, DWORD* method) {
    printf("CheckScenariosUnlocked called\n");
    return true;
}

bool(__fastcall* isSceneOpened_o)(DWORD*, DWORD*, DWORD*, DWORD*);
bool __stdcall isSceneOpened(DWORD* __this, DWORD* novelId, DWORD* scenario, DWORD* method) {
    printf("isSceneOpened called\n");
    return true;
}

int(__fastcall* getMoneyFromSave_o)(DWORD*, DWORD*);
int __stdcall getMoneyFromSave(DWORD* __this, DWORD* method) {
    printf("getMoney called\n");
    return 999999;
}

//Do not pay attention
void DumpAllSignatures()
{
    printf("=== Dump signatures ===\n");

    DumpSignatureForModule("GameAssembly.dll", 0x3887B0, 64); // GameController$$GetStoryModeEnabled
    DumpSignatureForModule("GameAssembly.dll", 0x4945F0, 64); // CustomStoryFunctions$$StoryModeEnabled
    DumpSignatureForModule("GameAssembly.dll", 0x4645C0, 64); // Controllers.StoryStateUtilities$$CheckOneOfScenariosUnlocked
    DumpSignatureForModule("GameAssembly.dll", 0x387E20, 64); // FStoryModel$$CheckSetOfScenariosUnlocked
    DumpSignatureForModule("GameAssembly.dll", 0x391B20, 64); // Player$$isSceneOpened
    DumpSignatureForModule("GameAssembly.dll", 0x391AF0, 64); // Player$$GetCoinsFromSave

    printf("=== End ===\n");
}

void main() {
    init();
    printf("Greetings from PotJoke\n");

    uintptr_t off;

    // GetStoryMode -> создаём хук сразу, если найден паттерн
    off = GetOffsetFromPattern("GameAssembly.dll",
        "48 83 EC 28 48 83 B9 98 00 00 00 00 74 2D 48 8B 91 98 00 00 00 48 8B 89 90 00 00 00 48 85 C9 74 21 48 8B 52 10 45 33 C0 E8 ?? ?? ?? ?? 48 85 C0 74 10 0F B6 40 30 48 83 C4 28 C3 32 C0 48 83 C4"); // GetStoryMode
    if (off) {
        printf("Found GetStoryMode offset 0x%IX\n", off);
        MH_CreateHook(reinterpret_cast<LPVOID*>(GameAssembly + off),
            &getStoryModeEnabled, (LPVOID*)&getStoryModeEnabled_o);
    }
    else {
        printf("GetStoryMode pattern not found, skipping hook\n");
    }

    // StoryMode
    off = GetOffsetFromPattern("GameAssembly.dll",
        "48 83 EC 28 80 3D 17 8A 2E 02 00 75 13 48 8D 0D 54 AB 16 02 E8 ?? ?? ?? ?? C6 05 02 8A 2E 02 01 48 8B 05 41 AB 16 02 48 8B 88 B8 00 00 00 48 8B 09 48 85 C9 74 0B 33 D2 48 83 C4 28 E9 7F 41 EF"); // StoryMode
    if (off) {
        printf("Found StoryMode offset 0x%IX\n", off);
        MH_CreateHook(reinterpret_cast<LPVOID*>(GameAssembly + off),
            &StoryModeEnabled, (LPVOID*)&StoryModeEnabled_o);
    }
    else {
        printf("StoryMode pattern not found, skipping hook\n");
    }

    // OneScene
    off = GetOffsetFromPattern("GameAssembly.dll",
        "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC 20 80 3D 2B 88 31 02 00 48 8B FA 48 8B F1 75 13 48 8D 0D 73 AB 19 02 E8 ?? ?? ?? ?? C6 05 10 88 31 02 01 33 DB 8B C3 48 85 FF 74 73 66 0F 1F 44 00 00"); // OneScene
    if (off) {
        printf("Found OneScene offset 0x%IX\n", off);
        MH_CreateHook(reinterpret_cast<LPVOID*>(GameAssembly + off),
            &CheckScenariosUnlocked, (LPVOID*)&CheckOneOfScenariosUnlocked_o);
    }
    else {
        printf("OneScene pattern not found, skipping hook\n");
    }

    // SetScene
    off = GetOffsetFromPattern("GameAssembly.dll",
        "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC 20 80 3D E7 47 3F 02 00 48 8B FA 48 8B F1 75 13 48 8D 0D 13 73 27 02 E8 ?? ?? ?? ?? C6 05 CC 47 3F 02 01 33 DB 8B C3 48 85 FF 74 74 66 0F 1F 44 00 00"); // SetScene
    if (off) {
        printf("Found SetScene offset 0x%IX\n", off);
        MH_CreateHook(reinterpret_cast<LPVOID*>(GameAssembly + off),
            &CheckScenariosUnlocked, (LPVOID*)&CheckSetOfScenariosUnlocked_o);
    }
    else {
        printf("SetScene pattern not found, skipping hook\n");
    }

    // SceneOpen
    off = GetOffsetFromPattern("GameAssembly.dll",
        "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC 20 80 3D FC AA 3E 02 00 49 8B F0 48 8B FA 48 8B D9 75 13 48 8D 0D F0 40 28 02 E8 ?? ?? ?? ?? C6 05 DE AA 3E 02 01 33 D2 48 8B CB E8 ?? ?? ?? ?? 84 C0"); // SceneOpen
    if (off) {
        printf("Found SceneOpen offset 0x%IX\n", off);
        MH_CreateHook(reinterpret_cast<LPVOID*>(GameAssembly + off),
            &isSceneOpened, (LPVOID*)&isSceneOpened_o);
    }
    else {
        printf("SceneOpen pattern not found, skipping hook\n");
    }

    // GetMoney
    off = GetOffsetFromPattern("GameAssembly.dll",
        "40 53 48 83 EC 20 80 79 28 00 48 8B D9 75 14 33 D2 E8 ?? ?? ?? ?? 89 43 2C C6 43 28 01 48 83 C4 20 5B C3 8B 41 2C 48 83 C4 20 5B C3 CC CC CC CC 48 89 5C 24 08 48 89 74 24 10 57 48 83 EC 20 80"); // GetMoney
    if (off) {
        printf("Found GetMoney offset 0x%IX\n", off);
        MH_CreateHook(reinterpret_cast<LPVOID*>(GameAssembly + off),
            &getMoneyFromSave, (LPVOID*)&getMoneyFromSave_o);
    }
    else {
        printf("GetMoney pattern not found, skipping hook\n");
    }

    MH_EnableHook(MH_ALL_HOOKS);

    printf("Ready to serve master!\nPlease don't close console\n");
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(0, 0, (PTHREAD_START_ROUTINE)main, hModule, 0, 0);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
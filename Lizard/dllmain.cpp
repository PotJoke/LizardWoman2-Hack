// dllmain.cpp : Определяет точку входа для приложения DLL.
#include "pch.h"

#include <Windows.h>
#include <MinHook.h>
#include <cstdio>

uintptr_t base = (uintptr_t)GetModuleHandle(NULL);
uintptr_t GameAssembly = (uintptr_t)GetModuleHandle("GameAssembly.dll");
uintptr_t GetStoryMode = 0x388E10; //GameController$$GetStoryModeEnabled
uintptr_t StoryMode = 0x386760; //CustomStoryFunctions$$StoryModeEnabled
uintptr_t OneScene = 0x4649B0; //Controllers.StoryStateUtilities$$CheckOneOfScenariosUnlocked
uintptr_t SetScene = 0x388680; //FStoryModel$$CheckSetOfScenariosUnlocked
uintptr_t SceneOpen = 0x392040; //Player$$isSceneOpened

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
    return true;
}

bool(__fastcall* StoryModeEnabled_o)(DWORD*);
bool __stdcall StoryModeEnabled(DWORD* method) {
    printf("StoryModeEnabled called\n");
    return true;
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


void main() {
    init();
    printf("Greetings from PotJoke\n");
    printf("Looks like everything is ok\n");
    printf("Ready to serve master!\n");
    printf("Now every scene is unlocked and StoryMode Enabled\n");
    printf("Please don't close console\n");

    MH_CreateHook(reinterpret_cast<LPVOID*>(GameAssembly + GetStoryMode),
        &getStoryModeEnabled, (LPVOID*)&getStoryModeEnabled_o);

    MH_CreateHook(reinterpret_cast<LPVOID*>(GameAssembly + StoryMode),
        &StoryModeEnabled, (LPVOID*)&StoryModeEnabled_o);

    MH_CreateHook(reinterpret_cast<LPVOID*>(GameAssembly + OneScene),
        &CheckScenariosUnlocked, (LPVOID*)&CheckOneOfScenariosUnlocked_o);
    MH_CreateHook(reinterpret_cast<LPVOID*>(GameAssembly + SetScene),
        &CheckScenariosUnlocked, (LPVOID*)&CheckSetOfScenariosUnlocked_o);

    MH_CreateHook(reinterpret_cast<LPVOID*>(GameAssembly + SceneOpen),
        &isSceneOpened, (LPVOID*)&isSceneOpened_o);

    MH_EnableHook(MH_ALL_HOOKS);
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

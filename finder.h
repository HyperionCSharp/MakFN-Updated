//
// Made by HyperionCSharp
//

#pragma once
#include <string.h>
#include <wininet.h>
#include <windows.h>
#include <iostream>

static const void* game_rbx_jmp;

class Updater {
public:
    bool Init(uintptr_t UObjectArray, uintptr_t GetObjectName, uintptr_t GetNameByIndex, uintptr_t FnFree);
    DWORD FindOffset(const char* Class, const char* var);
    PVOID FindObject(const char* name);

private:
    uintptr_t UObjectArray = NULL;
    uintptr_t GetObjectName = NULL;
    uintptr_t GetNameByIndex = NULL;
    uintptr_t FnFree = NULL;
    DWORD ObjectsCount = 0;

    void cFixName(char* Name);
    void FreeObjName(uintptr_t Address);
    char* fGetObjectName(uintptr_t Object);
    char* fGetNameByIndex(int Index);
};
//
// HyperionCSharp: Created Offset Finder
//

#include "finder.h"

template<class T>
struct TArray {
    friend struct FString;

public:
    inline TArray() {
        Data = nullptr;
        Count = Max = 0;
    }

    inline int Num() const {
        return Count;
    }

    inline T& operator[](int i) {
        return Data[i];
    }

    inline const T& operator[](int i) const {
        return Data[i];
    }

    inline bool IsValidIndex(int i) const {
        return i < Num();
    }

private:
    T* Data;
    int Count;
    int Max;
};

class UObject {
public:
    PVOID vTableObject;
    DWORD ObjectFlags;
    DWORD InternalIndex;
    DWORD* Class;
    BYTE _padding_0[0x8];
    UObject* Outer;
};

struct FString : private TArray<wchar_t>
{
    inline FString() {
    };

    inline bool IsValid() const {
        return Data != nullptr;
    }

    inline const wchar_t* c_str() const {
        return Data;
    }

    std::string ToString() const {
        auto length = std::wcslen(Data);
        std::string str(length, '\0');
        std::use_facet<std::ctype<wchar_t>>(std::locale()).narrow(Data, Data + length, '?', &str[0]);
        return str;
    }
};

struct FName {
    int32_t ComparisonIndex;
    int32_t Number;
};

bool Updater::Init(uintptr_t UObjectArray, uintptr_t GetObjectName, uintptr_t GetNameByIndex, uintptr_t FnFree) {
    if (!UObjectArray || !GetObjectName || !GetNameByIndex || !FnFree) return false;
    game_rbx_jmp = gadget(NULL);

    this->ObjectsCount = *(DWORD*)(UObjectArray + 0x14);
    if (this->ObjectsCount > 0x9000) this->ObjectsCount = 0x9000;

    uintptr_t defer_1 = *(uintptr_t*)UObjectArray;

    // HyperionCSharp: TODO: Add Spoof Call Which Helps To Find Object Arrays

    uintptr_t defer_2 = *(uintptr_t*)defer_1;

    // HyperionCSharp: TODO: Add Spoof Call Which Helps To Find Object Arrays

    this->UObjectArray = defer_2;
    this->GetObjectName = GetObjectName;
    this->GetNameByIndex = GetNameByIndex;
    this->FnFree = FnFree;

    return true;
}

DWORD Updater::FindOffset(const char *Class, const char *var) {
    for (DWORD i = 0x0; i < 0x9000; i++) {
        auto CurrentObject = *(uintptr_t*)(this->UObjectArray + (i * 0x18));
        // HyperionCSharp: TODO: Add Spoof Call Which Helps To Find Object Arrays

        char* CurObjectName = (char*)malloc(340);

        strcpy(CurObjectName, this->fGetObjectName(CurrentObject));

        // HyperionCSharp: TODO: Finish Offsets Finder And Signatures Finder (ALL AUTOMATED PROCESSES WITHIN THE EXE)
    }
}

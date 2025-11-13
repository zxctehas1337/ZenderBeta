#pragma once
#include "../Game/UserCmd.h"
#include "../Core/MemoryMgr.h"
#include <Windows.h>

// CreateMove hook for safe Silent Aim implementation
class CreateMoveHook {
private:
    static void* originalCreateMove;
    static bool isHooked;
    
    // Original view angles storage for Silent Aim
    static Vec3 originalViewAngles;
    static bool silentAimActive;
    static Vec3 silentAimAngles;
    
public:
    // Hook management
    static bool Initialize();
    static void Shutdown();
    static bool IsHooked() { return isHooked; }
    
    // CreateMove hook function
    static bool __fastcall HookedCreateMove(void* thisptr, int edx, float flInputSampleTime, CUserCmd* cmd);
    
    // Silent Aim interface (safe alternative to direct memory writes)
    static void SetSilentAim(const Vec3& angles);
    static void DisableSilentAim();
    static bool IsSilentAimActive() { return silentAimActive; }
    
    // Auto fire interface (safe button manipulation)
    static void SetAutoFire(bool enable);
    static void PressButton(ButtonFlags button);
    static void ReleaseButton(ButtonFlags button);
    
    // Utility functions
    static Vec3 GetOriginalViewAngles() { return originalViewAngles; }
    static CUserCmd* GetCurrentCmd() { return g_pCurrentCmd; }
    
private:
    // Internal processing
    static void ProcessSilentAim(CUserCmd* cmd);
    static void ProcessAutoFire(CUserCmd* cmd);
    static void ProcessAntiAim(CUserCmd* cmd);
    static void ValidateAngles(Vec3& angles);
};

// Global current command pointer
extern CUserCmd* g_pCurrentCmd;
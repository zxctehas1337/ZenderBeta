#include "CreateMoveHook.h"
#include "../Helpers/Logger.h"
#include "../Helpers/Math.h"
#include <detours.h>

// Static member initialization
void* CreateMoveHook::originalCreateMove = nullptr;
bool CreateMoveHook::isHooked = false;
Vec3 CreateMoveHook::originalViewAngles = {0, 0, 0};
bool CreateMoveHook::silentAimActive = false;
Vec3 CreateMoveHook::silentAimAngles = {0, 0, 0};
CUserCmd* g_pCurrentCmd = nullptr;

bool CreateMoveHook::Initialize() {
    if (isHooked) {
        return true;
    }
    
    // Find CreateMove function address
    // This would need to be implemented with signature scanning
    // For now, using placeholder address
    HMODULE clientDll = GetModuleHandleA("client.dll");
    if (!clientDll) {
        Log::Error("Failed to get client.dll handle");
        return false;
    }
    
    // TODO: Implement signature scanning for CreateMove
    // Signature for CS2 2025: "55 8B EC 8B 45 ? 85 C0 75 ?"
    void* createMoveAddr = nullptr; // GetProcAddress or signature scan result
    
    if (!createMoveAddr) {
        Log::Warning("CreateMove address not found - using fallback method");
        return false;
    }
    
    // Hook CreateMove using Microsoft Detours
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach(&originalCreateMove, HookedCreateMove);
    
    if (DetourTransactionCommit() == NO_ERROR) {
        isHooked = true;
        Log::Info("CreateMove hook installed successfully");
        return true;
    } else {
        Log::Error("Failed to install CreateMove hook");
        return false;
    }
}

void CreateMoveHook::Shutdown() {
    if (!isHooked) {
        return;
    }
    
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourDetach(&originalCreateMove, HookedCreateMove);
    
    if (DetourTransactionCommit() == NO_ERROR) {
        isHooked = false;
        silentAimActive = false;
        Log::Info("CreateMove hook removed successfully");
    } else {
        Log::Error("Failed to remove CreateMove hook");
    }
}

bool __fastcall CreateMoveHook::HookedCreateMove(void* thisptr, int edx, float flInputSampleTime, CUserCmd* cmd) {
    // Store current command globally
    g_pCurrentCmd = cmd;
    
    // Store original view angles for restoration
    originalViewAngles = cmd->viewangles;
    
    // Call original CreateMove first
    bool result = reinterpret_cast<bool(__thiscall*)(void*, float, CUserCmd*)>(originalCreateMove)(thisptr, flInputSampleTime, cmd);
    
    // Apply our modifications
    if (cmd && cmd->command_number != 0) {
        ProcessSilentAim(cmd);
        ProcessAutoFire(cmd);
        ProcessAntiAim(cmd);
        
        // Validate final angles
        ValidateAngles(cmd->viewangles);
    }
    
    return result;
}

void CreateMoveHook::SetSilentAim(const Vec3& angles) {
    silentAimAngles = angles;
    silentAimActive = true;
}

void CreateMoveHook::DisableSilentAim() {
    silentAimActive = false;
}

void CreateMoveHook::SetAutoFire(bool enable) {
    if (!g_pCurrentCmd) return;
    
    if (enable) {
        g_pCurrentCmd->buttons |= IN_ATTACK;
    } else {
        g_pCurrentCmd->buttons &= ~IN_ATTACK;
    }
}

void CreateMoveHook::PressButton(ButtonFlags button) {
    if (!g_pCurrentCmd) return;
    g_pCurrentCmd->buttons |= button;
}

void CreateMoveHook::ReleaseButton(ButtonFlags button) {
    if (!g_pCurrentCmd) return;
    g_pCurrentCmd->buttons &= ~button;
}

void CreateMoveHook::ProcessSilentAim(CUserCmd* cmd) {
    if (!silentAimActive || !cmd) {
        return;
    }
    
    // Apply silent aim angles
    cmd->viewangles = silentAimAngles;
    
    // Normalize angles
    Math::NormalizeAngles(cmd->viewangles);
    
    // Clamp to valid ranges
    cmd->viewangles.x = Math::Clamp(cmd->viewangles.x, -89.0f, 89.0f);
    cmd->viewangles.y = Math::Clamp(cmd->viewangles.y, -180.0f, 180.0f);
    cmd->viewangles.z = 0.0f;
}

void CreateMoveHook::ProcessAutoFire(CUserCmd* cmd) {
    if (!cmd) return;
    
    // Auto fire logic would be implemented here
    // This is called from aimbot when target is acquired
}

void CreateMoveHook::ProcessAntiAim(CUserCmd* cmd) {
    if (!cmd) return;
    
    // Anti-aim logic for rage mode would be implemented here
    // For now, just ensure we don't break movement
}

void CreateMoveHook::ValidateAngles(Vec3& angles) {
    // Ensure angles are within valid ranges
    Math::NormalizeAngles(angles);
    
    // Clamp pitch
    if (angles.x > 89.0f) angles.x = 89.0f;
    if (angles.x < -89.0f) angles.x = -89.0f;
    
    // Normalize yaw
    while (angles.y > 180.0f) angles.y -= 360.0f;
    while (angles.y < -180.0f) angles.y += 360.0f;
    
    // Roll should always be 0
    angles.z = 0.0f;
}
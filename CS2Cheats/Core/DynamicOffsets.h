#pragma once
#include <Windows.h>
#include <string>
#include <vector>

// Dynamic offset finder for CS2 2025
class DynamicOffsets {
private:
    struct OffsetSignature {
        std::string name;
        std::string pattern;
        std::string mask;
        int offset;
        DWORD* targetOffset;
    };
    
    static std::vector<OffsetSignature> signatures;
    static bool initialized;
    
public:
    // Initialize dynamic offset system
    static bool Initialize();
    static void Shutdown();
    static bool IsInitialized() { return initialized; }
    
    // Find offsets using signatures
    static bool FindAllOffsets();
    static bool FindOffset(const std::string& name, const std::string& pattern, const std::string& mask, int offset, DWORD* target);
    
    // Signature scanning functions
    static DWORD FindPattern(HMODULE module, const std::string& pattern, const std::string& mask);
    static DWORD FindPatternInSection(HMODULE module, const char* sectionName, const std::string& pattern, const std::string& mask);
    
    // Utility functions
    static bool IsValidAddress(DWORD address);
    static DWORD GetModuleSize(HMODULE module);
    static void* GetModuleSection(HMODULE module, const char* sectionName, DWORD* sectionSize);
    
    // Update offsets from cs2-dumper (2025)
    static void RegisterCS2025Offsets();
    static bool ValidateOffsets();
    
private:
    // Internal scanning functions
    static DWORD ScanPattern(BYTE* data, DWORD size, const std::string& pattern, const std::string& mask);
    static bool ComparePattern(BYTE* data, const std::string& pattern, const std::string& mask);
    static std::vector<BYTE> PatternToBytes(const std::string& pattern);
};

// CS2 2025 offset signatures (based on cs2-dumper July 2025)
namespace CS2Signatures {
    // View angles (critical for aimbot)
    constexpr const char* VIEW_ANGLES = "F3 0F 11 86 ? ? ? ? F3 0F 10 46";
    constexpr const char* VIEW_ANGLES_MASK = "xxxx????xxxx";
    constexpr int VIEW_ANGLES_OFFSET = 4;
    
    // Aim punch (for recoil control)
    constexpr const char* AIM_PUNCH = "F3 0F 11 8E ? ? ? ? F3 0F 10 4E";
    constexpr const char* AIM_PUNCH_MASK = "xxxx????xxxx";
    constexpr int AIM_PUNCH_OFFSET = 4;
    
    // Health
    constexpr const char* HEALTH = "89 86 ? ? ? ? 8B 86 ? ? ? ? 89 86";
    constexpr const char* HEALTH_MASK = "xx????xx????xx";
    constexpr int HEALTH_OFFSET = 2;
    
    // Team number
    constexpr const char* TEAM_NUM = "89 8E ? ? ? ? 8B 8E ? ? ? ? 89 8E";
    constexpr const char* TEAM_NUM_MASK = "xx????xx????xx";
    constexpr int TEAM_NUM_OFFSET = 2;
    
    // Bone data
    constexpr const char* BONE_ARRAY = "48 8B 8E ? ? ? ? 48 85 C9 74 ? 48 8B 01";
    constexpr const char* BONE_ARRAY_MASK = "xxx????xxxx?xxx";
    constexpr int BONE_ARRAY_OFFSET = 3;
    
    // Flash duration
    constexpr const char* FLASH_DURATION = "F3 0F 11 86 ? ? ? ? F3 0F 10 86 ? ? ? ?";
    constexpr const char* FLASH_DURATION_MASK = "xxxx????xxxx????";
    constexpr int FLASH_DURATION_OFFSET = 4;
    
    // Shots fired
    constexpr const char* SHOTS_FIRED = "89 86 ? ? ? ? 8B 86 ? ? ? ? FF C0";
    constexpr const char* SHOTS_FIRED_MASK = "xx????xx????xx";
    constexpr int SHOTS_FIRED_OFFSET = 2;
    
    // CreateMove function
    constexpr const char* CREATE_MOVE = "55 8B EC 8B 45 ? 85 C0 75 ? 8B 45";
    constexpr const char* CREATE_MOVE_MASK = "xxxxx?xxx?xx";
    constexpr int CREATE_MOVE_OFFSET = 0;
    
    // Engine trace interface
    constexpr const char* ENGINE_TRACE = "A1 ? ? ? ? 56 8B F1 85 C0 74 ?";
    constexpr const char* ENGINE_TRACE_MASK = "x????xxxxxx?";
    constexpr int ENGINE_TRACE_OFFSET = 1;
}

// Global dynamic offsets instance
extern DynamicOffsets g_DynamicOffsets;
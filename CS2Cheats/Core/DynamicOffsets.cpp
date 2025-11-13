#include "DynamicOffsets.h"
#include "../Helpers/Logger.h"
#include "../Offsets/Offsets.h"
#include <algorithm>

// Static member initialization
std::vector<DynamicOffsets::OffsetSignature> DynamicOffsets::signatures;
bool DynamicOffsets::initialized = false;
DynamicOffsets g_DynamicOffsets;

bool DynamicOffsets::Initialize() {
    if (initialized) {
        return true;
    }
    
    Log::Info("Initializing dynamic offset system...");
    
    // Register CS2 2025 signatures
    RegisterCS2025Offsets();
    
    // Find all offsets
    if (!FindAllOffsets()) {
        Log::Error("Failed to find some critical offsets");
        return false;
    }
    
    // Validate found offsets
    if (!ValidateOffsets()) {
        Log::Error("Offset validation failed");
        return false;
    }
    
    initialized = true;
    Log::Info("Dynamic offset system initialized successfully");
    return true;
}

void DynamicOffsets::Shutdown() {
    signatures.clear();
    initialized = false;
}

void DynamicOffsets::RegisterCS2025Offsets() {
    signatures.clear();
    
    // Register critical offsets with their signatures
    signatures.push_back({
        "ViewAngles",
        CS2Signatures::VIEW_ANGLES,
        CS2Signatures::VIEW_ANGLES_MASK,
        CS2Signatures::VIEW_ANGLES_OFFSET,
        &Offset.Pawn.angEyeAngles
    });
    
    signatures.push_back({
        "AimPunch",
        CS2Signatures::AIM_PUNCH,
        CS2Signatures::AIM_PUNCH_MASK,
        CS2Signatures::AIM_PUNCH_OFFSET,
        &Offset.Pawn.aimPunchAngle
    });
    
    signatures.push_back({
        "Health",
        CS2Signatures::HEALTH,
        CS2Signatures::HEALTH_MASK,
        CS2Signatures::HEALTH_OFFSET,
        &Offset.Pawn.CurrentHealth
    });
    
    signatures.push_back({
        "TeamNum",
        CS2Signatures::TEAM_NUM,
        CS2Signatures::TEAM_NUM_MASK,
        CS2Signatures::TEAM_NUM_OFFSET,
        &Offset.Pawn.iTeamNum
    });
    
    signatures.push_back({
        "BoneArray",
        CS2Signatures::BONE_ARRAY,
        CS2Signatures::BONE_ARRAY_MASK,
        CS2Signatures::BONE_ARRAY_OFFSET,
        &Offset.Pawn.BoneArray
    });
    
    signatures.push_back({
        "FlashDuration",
        CS2Signatures::FLASH_DURATION,
        CS2Signatures::FLASH_DURATION_MASK,
        CS2Signatures::FLASH_DURATION_OFFSET,
        &Offset.Pawn.flFlashDuration
    });
    
    signatures.push_back({
        "ShotsFired",
        CS2Signatures::SHOTS_FIRED,
        CS2Signatures::SHOTS_FIRED_MASK,
        CS2Signatures::SHOTS_FIRED_OFFSET,
        &Offset.Pawn.iShotsFired
    });
}

bool DynamicOffsets::FindAllOffsets() {
    HMODULE clientDll = GetModuleHandleA("client.dll");
    HMODULE engineDll = GetModuleHandleA("engine2.dll");
    
    if (!clientDll || !engineDll) {
        Log::Error("Failed to get module handles");
        return false;
    }
    
    int foundCount = 0;
    int totalCount = signatures.size();
    
    for (auto& sig : signatures) {
        HMODULE targetModule = clientDll; // Most offsets are in client.dll
        
        // Some offsets might be in engine2.dll
        if (sig.name == "CreateMove" || sig.name == "EngineTrace") {
            targetModule = engineDll;
        }
        
        DWORD address = FindPattern(targetModule, sig.pattern, sig.mask);
        if (address != 0) {
            // Calculate final offset
            DWORD finalOffset = address + sig.offset;
            
            // Read the actual offset value from memory
            DWORD offsetValue = 0;
            if (ReadProcessMemory(GetCurrentProcess(), (LPCVOID)finalOffset, &offsetValue, sizeof(DWORD), nullptr)) {
                *sig.targetOffset = offsetValue;
                foundCount++;
                Log::Info("Found " + sig.name + " offset: 0x" + std::to_string(offsetValue));
            } else {
                Log::Warning("Failed to read " + sig.name + " offset from memory");
            }
        } else {
            Log::Warning("Failed to find " + sig.name + " signature");
        }
    }
    
    Log::Info("Found " + std::to_string(foundCount) + "/" + std::to_string(totalCount) + " offsets");
    return foundCount >= (totalCount * 0.8f); // Allow 20% failure rate
}

bool DynamicOffsets::FindOffset(const std::string& name, const std::string& pattern, const std::string& mask, int offset, DWORD* target) {
    HMODULE clientDll = GetModuleHandleA("client.dll");
    if (!clientDll) {
        return false;
    }
    
    DWORD address = FindPattern(clientDll, pattern, mask);
    if (address == 0) {
        return false;
    }
    
    DWORD finalOffset = address + offset;
    DWORD offsetValue = 0;
    
    if (ReadProcessMemory(GetCurrentProcess(), (LPCVOID)finalOffset, &offsetValue, sizeof(DWORD), nullptr)) {
        *target = offsetValue;
        return true;
    }
    
    return false;
}

DWORD DynamicOffsets::FindPattern(HMODULE module, const std::string& pattern, const std::string& mask) {
    if (!module) {
        return 0;
    }
    
    DWORD moduleSize = GetModuleSize(module);
    if (moduleSize == 0) {
        return 0;
    }
    
    BYTE* moduleBase = reinterpret_cast<BYTE*>(module);
    return ScanPattern(moduleBase, moduleSize, pattern, mask);
}

DWORD DynamicOffsets::FindPatternInSection(HMODULE module, const char* sectionName, const std::string& pattern, const std::string& mask) {
    DWORD sectionSize = 0;
    void* sectionData = GetModuleSection(module, sectionName, &sectionSize);
    
    if (!sectionData || sectionSize == 0) {
        return 0;
    }
    
    return ScanPattern(reinterpret_cast<BYTE*>(sectionData), sectionSize, pattern, mask);
}

bool DynamicOffsets::IsValidAddress(DWORD address) {
    if (address == 0) {
        return false;
    }
    
    // Check if address is within reasonable range
    if (address < 0x1000 || address > 0x7FFFFFFF) {
        return false;
    }
    
    // Try to read from the address to verify it's valid
    BYTE testByte = 0;
    return ReadProcessMemory(GetCurrentProcess(), (LPCVOID)address, &testByte, sizeof(BYTE), nullptr);
}

DWORD DynamicOffsets::GetModuleSize(HMODULE module) {
    if (!module) {
        return 0;
    }
    
    IMAGE_DOS_HEADER* dosHeader = reinterpret_cast<IMAGE_DOS_HEADER*>(module);
    if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
        return 0;
    }
    
    IMAGE_NT_HEADERS* ntHeaders = reinterpret_cast<IMAGE_NT_HEADERS*>(
        reinterpret_cast<BYTE*>(module) + dosHeader->e_lfanew);
    
    if (ntHeaders->Signature != IMAGE_NT_SIGNATURE) {
        return 0;
    }
    
    return ntHeaders->OptionalHeader.SizeOfImage;
}

void* DynamicOffsets::GetModuleSection(HMODULE module, const char* sectionName, DWORD* sectionSize) {
    if (!module || !sectionName || !sectionSize) {
        return nullptr;
    }
    
    IMAGE_DOS_HEADER* dosHeader = reinterpret_cast<IMAGE_DOS_HEADER*>(module);
    IMAGE_NT_HEADERS* ntHeaders = reinterpret_cast<IMAGE_NT_HEADERS*>(
        reinterpret_cast<BYTE*>(module) + dosHeader->e_lfanew);
    
    IMAGE_SECTION_HEADER* sectionHeader = IMAGE_FIRST_SECTION(ntHeaders);
    
    for (int i = 0; i < ntHeaders->FileHeader.NumberOfSections; i++) {
        if (strcmp(reinterpret_cast<char*>(sectionHeader[i].Name), sectionName) == 0) {
            *sectionSize = sectionHeader[i].SizeOfRawData;
            return reinterpret_cast<BYTE*>(module) + sectionHeader[i].VirtualAddress;
        }
    }
    
    return nullptr;
}

bool DynamicOffsets::ValidateOffsets() {
    // Validate critical offsets
    if (!IsValidAddress(Offset.Pawn.angEyeAngles)) {
        Log::Error("Invalid ViewAngles offset");
        return false;
    }
    
    if (!IsValidAddress(Offset.Pawn.CurrentHealth)) {
        Log::Error("Invalid Health offset");
        return false;
    }
    
    if (!IsValidAddress(Offset.Pawn.BoneArray)) {
        Log::Error("Invalid BoneArray offset");
        return false;
    }
    
    Log::Info("All critical offsets validated successfully");
    return true;
}

DWORD DynamicOffsets::ScanPattern(BYTE* data, DWORD size, const std::string& pattern, const std::string& mask) {
    if (!data || size == 0 || pattern.empty() || mask.empty()) {
        return 0;
    }
    
    for (DWORD i = 0; i < size - mask.length(); i++) {
        if (ComparePattern(data + i, pattern, mask)) {
            return reinterpret_cast<DWORD>(data + i);
        }
    }
    
    return 0;
}

bool DynamicOffsets::ComparePattern(BYTE* data, const std::string& pattern, const std::string& mask) {
    if (!data || pattern.empty() || mask.empty() || pattern.length() != mask.length()) {
        return false;
    }
    
    for (size_t i = 0; i < mask.length(); i++) {
        if (mask[i] == 'x' && data[i] != static_cast<BYTE>(pattern[i])) {
            return false;
        }
    }
    
    return true;
}

std::vector<BYTE> DynamicOffsets::PatternToBytes(const std::string& pattern) {
    std::vector<BYTE> bytes;
    
    for (size_t i = 0; i < pattern.length(); i += 3) {
        if (pattern.substr(i, 2) == "??") {
            bytes.push_back(0x00);
        } else {
            bytes.push_back(static_cast<BYTE>(std::stoul(pattern.substr(i, 2), nullptr, 16)));
        }
    }
    
    return bytes;
}
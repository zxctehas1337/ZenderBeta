#include "VisibilityCheck.h"
#include "../Helpers/Logger.h"
#include "../Game/Game.h"
#include "../Core/MemoryMgr.h"

// Static member initialization
void* VisibilityCheck::engineTrace = nullptr;
bool VisibilityCheck::initialized = false;
VisibilityCheck g_VisibilityCheck;

bool VisibilityCheck::Initialize() {
    if (initialized) {
        return true;
    }
    
    // Find IEngineTrace interface
    HMODULE engineDll = GetModuleHandleA("engine2.dll");
    if (!engineDll) {
        Log::Error("Failed to get engine2.dll handle");
        return false;
    }
    
    // TODO: Implement signature scanning for IEngineTrace
    // Signature for CS2 2025: "A1 ? ? ? ? 56 8B F1 85 C0 74 ?"
    engineTrace = nullptr; // GetProcAddress or signature scan result
    
    if (!engineTrace) {
        Log::Warning("IEngineTrace not found - using simplified visibility check");
        initialized = true; // Still allow simplified checks
        return true;
    }
    
    initialized = true;
    Log::Info("Visibility system initialized successfully");
    return true;
}

void VisibilityCheck::Shutdown() {
    engineTrace = nullptr;
    initialized = false;
}

bool VisibilityCheck::IsVisible(const CEntity& from, const CEntity& to, const Vec3& targetPos) {
    if (!initialized) {
        return true; // Fallback to always visible if not initialized
    }
    
    Vec3 eyePos = from.Pawn.CameraPos;
    
    // Basic distance check
    float distance = Math::Distance(eyePos, targetPos);
    if (distance > 8000.0f) { // Max reasonable distance
        return false;
    }
    
    // If we have engine trace, use it
    if (engineTrace) {
        TraceResult result;
        if (TraceLine(eyePos, targetPos, MASK_SHOT, result)) {
            // Check if we hit the target entity or nothing
            return (result.hitEntity == 0 || result.hitEntity == to.Controller.Address || result.fraction > 0.97f);
        }
    }
    
    // Simplified visibility check (fallback)
    // Check for basic line of sight using entity positions
    Vec3 direction = targetPos - eyePos;
    direction.Normalize();
    
    // Sample points along the line to check for obvious obstructions
    int samples = static_cast<int>(distance / 100.0f); // Sample every 100 units
    samples = Math::Clamp(samples, 5, 50); // Limit samples
    
    for (int i = 1; i < samples; i++) {
        Vec3 samplePos = eyePos + direction * (distance * i / samples);
        
        // Check if sample position is inside world geometry (simplified)
        // This would need proper world collision data
        if (samplePos.z < -1000.0f || samplePos.z > 2000.0f) {
            return false; // Likely underground or too high
        }
    }
    
    return true; // Assume visible if no obvious obstructions
}

bool VisibilityCheck::IsVisibleThroughSmoke(const Vec3& start, const Vec3& end) {
    if (!initialized) {
        return true;
    }
    
    // Check for smoke entities between start and end points
    return !CheckSmokeEntities(start, end);
}

bool VisibilityCheck::CanPenetrate(const Vec3& start, const Vec3& end, float minDamage) {
    if (!initialized || !engineTrace) {
        return false; // Conservative approach - no penetration without proper trace
    }
    
    TraceResult result;
    if (!TraceLine(start, end, MASK_SHOT, result)) {
        return true; // No obstruction
    }
    
    if (result.fraction > 0.95f) {
        return true; // Almost clear path
    }
    
    // Calculate potential damage through wall
    float damage = CalculateDamage(start, end, 100.0f);
    return damage >= minDamage;
}

bool VisibilityCheck::TraceLine(const Vec3& start, const Vec3& end, TraceMask mask, TraceResult& result) {
    if (!engineTrace) {
        // Simplified trace without engine interface
        result.startPos = start;
        result.endPos = end;
        result.hitPos = end;
        result.fraction = 1.0f;
        result.didHit = false;
        result.hitEntity = 0;
        return true;
    }
    
    Ray ray;
    SetupRay(ray, start, end);
    return PerformTrace(ray, mask, result);
}

bool VisibilityCheck::TraceRay(const Ray& ray, TraceMask mask, TraceResult& result) {
    if (!engineTrace) {
        return false;
    }
    
    return PerformTrace(ray, mask, result);
}

bool VisibilityCheck::TraceHull(const Vec3& start, const Vec3& end, const Vec3& mins, const Vec3& maxs, TraceMask mask, TraceResult& result) {
    if (!engineTrace) {
        return false;
    }
    
    Ray ray;
    SetupHullRay(ray, start, end, mins, maxs);
    return PerformTrace(ray, mask, result);
}

float VisibilityCheck::CalculateDamage(const Vec3& start, const Vec3& end, float baseDamage) {
    if (!initialized) {
        return baseDamage;
    }
    
    float distance = Math::Distance(start, end);
    float damageFalloff = 1.0f - (distance / 8000.0f); // Damage falls off over distance
    damageFalloff = Math::Clamp(damageFalloff, 0.1f, 1.0f);
    
    // Check for wall penetration
    TraceResult result;
    if (TraceLine(start, end, MASK_SHOT, result) && result.didHit) {
        // Reduce damage based on material and thickness
        float penetrationFactor = 0.5f; // Assume 50% damage reduction through walls
        return baseDamage * damageFalloff * penetrationFactor;
    }
    
    return baseDamage * damageFalloff;
}

bool VisibilityCheck::IsInSmoke(const Vec3& position) {
    // TODO: Implement smoke entity checking
    // This would require scanning for smoke grenade entities and checking their radius
    return false;
}

bool VisibilityCheck::IsInFlash(const CEntity& entity) {
    // Check flash duration from entity data
    return entity.Pawn.FlashDuration > 0.0f;
}

bool VisibilityCheck::CanAutoWall(const Vec3& start, const Vec3& end, float& damage) {
    damage = CalculateDamage(start, end, 100.0f);
    return damage >= 20.0f; // Minimum damage threshold for autowall
}

int VisibilityCheck::GetPenetrationCount(const Vec3& start, const Vec3& end) {
    if (!engineTrace) {
        return 0;
    }
    
    int penetrations = 0;
    Vec3 currentPos = start;
    Vec3 direction = end - start;
    direction.Normalize();
    float totalDistance = Math::Distance(start, end);
    float currentDistance = 0.0f;
    
    while (currentDistance < totalDistance && penetrations < 4) { // Max 4 penetrations
        TraceResult result;
        Vec3 traceEnd = currentPos + direction * (totalDistance - currentDistance);
        
        if (TraceLine(currentPos, traceEnd, MASK_SHOT, result) && result.didHit) {
            penetrations++;
            currentPos = result.hitPos + direction * 1.0f; // Move past the hit point
            currentDistance = Math::Distance(start, currentPos);
        } else {
            break;
        }
    }
    
    return penetrations;
}

float VisibilityCheck::GetWallThickness(const Vec3& start, const Vec3& end) {
    TraceResult result;
    if (TraceLine(start, end, MASK_SHOT, result) && result.didHit) {
        return Math::Distance(start, result.hitPos);
    }
    return 0.0f;
}

void VisibilityCheck::SetupRay(Ray& ray, const Vec3& start, const Vec3& end) {
    ray.start = start;
    ray.end = end;
    ray.mins = Vec3(0, 0, 0);
    ray.maxs = Vec3(0, 0, 0);
    ray.isRay = true;
    ray.isSwept = true;
}

void VisibilityCheck::SetupHullRay(Ray& ray, const Vec3& start, const Vec3& end, const Vec3& mins, const Vec3& maxs) {
    ray.start = start;
    ray.end = end;
    ray.mins = mins;
    ray.maxs = maxs;
    ray.isRay = false;
    ray.isSwept = true;
}

bool VisibilityCheck::PerformTrace(const Ray& ray, TraceMask mask, TraceResult& result) {
    if (!engineTrace) {
        return false;
    }
    
    // TODO: Implement actual engine trace call
    // This would use the IEngineTrace interface to perform the trace
    // For now, return a simplified result
    
    result.startPos = ray.start;
    result.endPos = ray.end;
    result.hitPos = ray.end;
    result.fraction = 1.0f;
    result.didHit = false;
    result.hitEntity = 0;
    
    return true;
}

bool VisibilityCheck::CheckSmokeEntities(const Vec3& start, const Vec3& end) {
    // TODO: Implement smoke entity scanning
    // This would scan for active smoke grenades and check if the line passes through them
    return false;
}

bool VisibilityCheck::CheckFlashEffects(const CEntity& entity) {
    return entity.Pawn.FlashDuration > 0.0f;
}
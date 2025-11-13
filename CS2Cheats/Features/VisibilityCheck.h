#pragma once
#include "../OS-ImGui/OS-ImGui_Struct.h"
#include "../Game/Entity.h"

// Trace masks for CS2 2025
enum TraceMask : uint32_t {
    MASK_ALL = 0xFFFFFFFF,
    MASK_SOLID = 0x200400B,
    MASK_PLAYERSOLID = 0x33000B,
    MASK_SHOT = 0x1000003,           // For bullet traces
    MASK_SHOT_HULL = 0x600400B,     // For hull traces
    MASK_VISIBLE = 0x1000003,       // For visibility checks
    MASK_OPAQUE = 0x1,              // For smoke/grate checks
};

// Trace result structure
struct TraceResult {
    Vec3 startPos;
    Vec3 endPos;
    Vec3 hitPos;
    Vec3 hitNormal;
    float fraction;
    int hitEntity;
    int hitGroup;
    bool didHit;
    bool startSolid;
    bool allSolid;
    int contents;
    int surface;
};

// Ray structure for tracing
struct Ray {
    Vec3 start;
    Vec3 end;
    Vec3 mins;
    Vec3 maxs;
    bool isRay;
    bool isSwept;
};

// Visibility checker class
class VisibilityCheck {
private:
    static void* engineTrace;
    static bool initialized;
    
public:
    // Initialize visibility system
    static bool Initialize();
    static void Shutdown();
    
    // Main visibility check functions
    static bool IsVisible(const CEntity& from, const CEntity& to, const Vec3& targetPos);
    static bool IsVisibleThroughSmoke(const Vec3& start, const Vec3& end);
    static bool CanPenetrate(const Vec3& start, const Vec3& end, float minDamage = 20.0f);
    
    // Advanced trace functions
    static bool TraceLine(const Vec3& start, const Vec3& end, TraceMask mask, TraceResult& result);
    static bool TraceRay(const Ray& ray, TraceMask mask, TraceResult& result);
    static bool TraceHull(const Vec3& start, const Vec3& end, const Vec3& mins, const Vec3& maxs, TraceMask mask, TraceResult& result);
    
    // Utility functions
    static float CalculateDamage(const Vec3& start, const Vec3& end, float baseDamage = 100.0f);
    static bool IsInSmoke(const Vec3& position);
    static bool IsInFlash(const CEntity& entity);
    
    // Autowall functions for rage mode
    static bool CanAutoWall(const Vec3& start, const Vec3& end, float& damage);
    static int GetPenetrationCount(const Vec3& start, const Vec3& end);
    static float GetWallThickness(const Vec3& start, const Vec3& end);
    
private:
    // Internal trace functions
    static void SetupRay(Ray& ray, const Vec3& start, const Vec3& end);
    static void SetupHullRay(Ray& ray, const Vec3& start, const Vec3& end, const Vec3& mins, const Vec3& maxs);
    static bool PerformTrace(const Ray& ray, TraceMask mask, TraceResult& result);
    
    // Smoke and flash detection
    static bool CheckSmokeEntities(const Vec3& start, const Vec3& end);
    static bool CheckFlashEffects(const CEntity& entity);
};

// Global visibility checker instance
extern VisibilityCheck g_VisibilityCheck;
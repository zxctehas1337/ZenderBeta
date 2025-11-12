#pragma once

#include <Windows.h>
#include <vector>
#include <map>
#include "../Game/Entity.h"
#include "../Core/Config.h"
#include "../Core/Cheats.h"

// Forward declarations instead of including implementation files
namespace Render { class HealthBar; }
namespace ESP { }
namespace GUI { }

// Trace masks for CS2
#define MASK_SHOT 0x46004003
#define CONTENTS_GRATE 0x00000001

// Button constants
#define IN_ATTACK (1 << 0)

// CUserCmd structure for Silent Aim
struct CUserCmd {
    Vec3 viewangles;
    int buttons;
    int tick_count;
    // Add other fields as needed
};

namespace Aimbot
{
    // Initialize aimbot systems
    bool Initialize();

    // Bone IDs for targeting
    enum BoneID {
        HEAD = 6,
        NECK = 5,
        CHEST = 4,
        STOMACH = 3,
        PELVIS = 0
    };

    // Hitbox IDs for advanced targeting
    enum HitboxID {
        HITBOX_HEAD = 0,
        HITBOX_NECK = 1,
        HITBOX_PELVIS = 2,
        HITBOX_STOMACH = 3,
        HITBOX_LOWER_CHEST = 4,
        HITBOX_CHEST = 5,
        HITBOX_UPPER_CHEST = 6,
        HITBOX_LEFT_THIGH = 7,
        HITBOX_RIGHT_THIGH = 8,
        HITBOX_LEFT_CALF = 9,
        HITBOX_RIGHT_CALF = 10,
        HITBOX_LEFT_FOOT = 11,
        HITBOX_RIGHT_FOOT = 12,
        HITBOX_LEFT_HAND = 13,
        HITBOX_RIGHT_HAND = 14,
        HITBOX_LEFT_UPPER_ARM = 15,
        HITBOX_LEFT_FOREARM = 16,
        HITBOX_RIGHT_UPPER_ARM = 17,
        HITBOX_RIGHT_FOREARM = 18
    };

    // Trace ray filter for visibility checks
    struct Ray_t {
        Vec3 start;
        Vec3 delta;
        Vec3 startOffset;
        Vec3 extents;
        bool isRay;
        bool isSwept;
        
        Ray_t() : startOffset(0, 0, 0), extents(0, 0, 0), isRay(true), isSwept(false) {}
        
        void Init(Vec3 src, Vec3 end) {
            start = src;
            delta = end - src;
            isSwept = (delta.x != 0 || delta.y != 0 || delta.z != 0);
        }
    };

    struct TraceFilter_t {
        void* pSkip;
        
        TraceFilter_t(void* skip = nullptr) : pSkip(skip) {}
    };

    struct CGameTrace {
        bool hit;
        Vec3 endPos;
        Vec3 planeNormal;
        float fraction;
        int hitbox;
        int hitgroup;
        void* hitEntity;
        bool startSolid;
        bool allSolid;
        
        CGameTrace() : hit(false), endPos(0, 0, 0), planeNormal(0, 0, 1), 
                       fraction(1.0f), hitbox(-1), hitgroup(0), hitEntity(nullptr), 
                       startSolid(false), allSolid(false) {}
    };

    // Get the best target based on FOV and other criteria
    bool GetBestTarget(const CEntity& localPlayer, const std::vector<EntityResult>& entities, 
                      Vec3& targetAngle, Vec3& targetPos, int& targetBone);

    // Get the appropriate bone position based on configuration
    Vec3 GetBonePosition(const CEntity& entity, int& outBone);

    // Get hitbox position for advanced targeting
    Vec3 GetHitboxPosition(const CEntity& entity, int hitbox, bool& isValid);

    // Real TraceLine implementation using engine trace
    bool TraceRay(const Vec3& start, const Vec3& end, CGameTrace& trace, void* skipEntity = nullptr);

    // Check if target is visible using real trace line
    bool IsTargetVisible(const CEntity& localPlayer, const CEntity& target, const Vec3& targetPos);

    // Legacy visibility check for compatibility
    bool IsTargetVisibleLegacy(const CEntity& localPlayer, const CEntity& target, const Vec3& targetPos);

    // Fallback trace implementation
    bool TraceRayFallback(const Vec3& start, const Vec3& end, CGameTrace& trace, void* skipEntity);

    // Apply ping prediction to target position
    Vec3 ApplyPingPrediction(const Vec3& targetPos, const Vec3& targetVelocity, float predictionAmount);

    // Resolver for anti-aim techniques
    Vec3 ResolveTarget(const CEntity& localPlayer, const CEntity& entity, const Vec3& originalPos);

    // Get velocity from entity
    Vec3 GetVelocity(const CEntity& entity);

    // Normalize angle helper
    float NormalizeAngle(float angle);

    // Calculate aim angle to target
    Vec3 CalculateAngle(const Vec3& source, const Vec3& destination);

    // Apply smoothing to aim
    Vec3 ApplySmoothing(const Vec3& currentAngle, const Vec3& targetAngle, float smoothFactor);

    // Apply recoil control
    Vec3 ApplyRecoilControl(const Vec3& aimAngle, const CEntity& localPlayer);

    // Check if aimbot should be active (now only checks if enabled)
    bool IsAimbotKeyActive();

    // Handle auto-fire functionality
    void HandleAutoFire(CUserCmd* cmd, bool targetAcquired, const Vec3& targetPos);

    // CreateMove hook for silent aim and triggerbot
    bool CreateMove(float* inputX, float* inputY, bool* inAttack);

    // Main aimbot function
    void Run(CUserCmd* cmd, const CEntity& localPlayer, const std::vector<EntityResult>& entities);

    // Draw FOV circle
    void DrawFOV(const CEntity& localPlayer);

    // Angle clamping helper
    void ClampAngle(Vec3& angle);

    // RAGE MODE SPECIFIC FUNCTIONS
    // Perfect silent aim with no visual movement
    bool ApplyRageSilentAim(CUserCmd* cmd, const Vec3& targetAngle, const Vec3& localPos, const Vec3& targetPos);
    
    // Instant hit calculation for rage mode
    bool CalculateInstantHit(const Vec3& source, const Vec3& target, Vec3& outAngle, float& outTime);
    
    // Autowall penetration calculation
    bool CanPenetrateWall(const Vec3& source, const Vec3& target, float& damage);
    
    // Multipoint scanning for rage mode
    bool GetMultipointHitbox(const CEntity& entity, int hitbox, Vec3& outPos, float scale = 0.8f);
    
    // Anti-aim integration
    Vec3 ApplyAntiAim(const CEntity& localPlayer, CUserCmd* cmd, bool& shouldSend);
    
    // LBY breaker for anti-resolver
    void BreakLBY(CUserCmd* cmd, float& yaw);
    
    // Resolver bypass techniques
    Vec3 ApplyResolverBypass(const CEntity& entity, const Vec3& originalPos);
    
    // No spread compensation
    Vec3 ApplyNoSpread(const Vec3& aimAngle, CUserCmd* cmd);
    
    // Force headshot with damage calculation
    bool ShouldForceHeadshot(const CEntity& target, float weaponDamage);
    
    // Advanced visibility check for rage mode
    bool IsVisibleForRage(const CEntity& localPlayer, const CEntity& target, const Vec3& pos, bool ignoreWalls = false);
}
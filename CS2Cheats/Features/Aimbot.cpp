#include "Aimbot.h"
#include "../Core/GUI.h"
#include "../Helpers/Logger.h"
#include "../Game/Entity.h"
#include "../Game/Game.h"
#include <cmath>
#include <map>
#include <random>
#include <chrono>

// Trace masks for CS2
#define MASK_SHOT 0x46004003
#define CONTENTS_GRATE 0x00000001

// Math constants - M_PI is already defined in Bone.h

// Random number generator
thread_local std::random_device rd;
thread_local std::mt19937 gen(rd());
thread_local std::uniform_int_distribution<> dis(0, 2);

// Interface for engine trace
class IEngineTrace {
public:
    virtual void* GetFunction(int index) = 0;
    virtual void TraceRay(const Aimbot::Ray_t& ray, unsigned int mask, const Aimbot::TraceFilter_t* filter, Aimbot::CGameTrace* trace) = 0;
};

// Engine trace function pointer typedef  
typedef void(__cdecl* TraceRayFn)(void*, const Aimbot::Ray_t*, unsigned int, const Aimbot::TraceFilter_t*, Aimbot::CGameTrace*);
IEngineTrace* g_pEngineTrace = nullptr;

// VTable hook for TraceRay
TraceRayFn oTraceRay = nullptr;
bool g_bTraceRayHooked = false;

// Hooked TraceRay function
void __cdecl hkTraceRay(void* thisptr, const Aimbot::Ray_t* ray, unsigned int mask, const Aimbot::TraceFilter_t* filter, Aimbot::CGameTrace* trace)
{
    // Call original function
    if (oTraceRay) {
        oTraceRay(thisptr, ray, mask, filter, trace);
    }
}

// Hook VTable function
bool HookVTableFunction(void** vtable, size_t index, void* detour) {
    if (!vtable || !detour) return false;
    
    DWORD oldProtect;
    if (!VirtualProtect(&vtable[index], sizeof(void*), PAGE_EXECUTE_READWRITE, &oldProtect)) {
        return false;
    }
    
    vtable[index] = detour;
    
    VirtualProtect(&vtable[index], sizeof(void*), oldProtect, &oldProtect);
    return true;
}

// Interface manager for getting engine interfaces
class InterfaceManager {
public:
    template<typename T>
    static T* CreateInterface(const char* moduleName, const char* interfaceName) {
        // Get module handle
        HMODULE hModule = GetModuleHandleA(moduleName);
        if (!hModule) {
            hModule = LoadLibraryA(moduleName);
            if (!hModule) return nullptr;
        }

        // Get CreateInterface function
        typedef void*(__cdecl* CreateInterfaceFn)(const char*, int*);
        CreateInterfaceFn CreateInterface = (CreateInterfaceFn)GetProcAddress(hModule, "CreateInterface");
        if (!CreateInterface) return nullptr;

        // Create interface
        return (T*)CreateInterface(interfaceName, nullptr);
    }
};

// Function to initialize trace interface with VTable hook
bool InitializeTrace()
{
    g_pEngineTrace = reinterpret_cast<IEngineTrace*>(InterfaceManager::CreateInterface<int>("engine.dll", "EngineTraceClient004"));
    if (!g_pEngineTrace) {
        return false;
    }
    
    // Hook TraceRay via VTable for better safety
    void** vtable = *(void***)g_pEngineTrace;
    if (vtable) {
        // TraceRay is typically at index 5 in IEngineTrace VTable
        if (HookVTableFunction(vtable, 5, (void*)hkTraceRay)) {
            oTraceRay = (TraceRayFn)vtable[5];
            g_bTraceRayHooked = true;
            return true;
        }
    }
    
    return false;
}

// Global variables for aimbot state
namespace {
    DWORD g_lastFireTime = 0;
    const DWORD FIRE_DELAY_MS = 50; // Delay between auto-fire shots in milliseconds
    
    // CreateMove state
    Vec3 g_silentAngle = {0, 0, 0};
    bool g_shouldAttack = false;
    
    // Tick rate for prediction
    float g_tickRate = 64.0f; // Default tick rate
}

namespace Aimbot
{
    // Initialize aimbot systems
    bool Initialize()
    {
        // Initialize trace interface
        if (!InitializeTrace()) {
            // Fallback to legacy trace if interface fails
            return false;
        }
        return true;
    }

    bool GetBestTarget(const CEntity& localPlayer, const std::vector<EntityResult>& entities, 
                      Vec3& targetAngle, Vec3& targetPos, int& targetBone)
    {
        if (!AimbotCFG::AimbotCFG.AimbotEnabled)
            return false;

        float bestFov = AimbotCFG::AimbotCFG.FOVRadius;
        bool foundTarget = false;
        Vec3 localEyePos = localPlayer.Pawn.CameraPos; // Eye position

        for (const auto& result : entities)
        {
            if (!result.isValid)
                continue;

            const auto& entity = result.entity;

            // Skip if not alive
            if (!entity.IsAlive())
                continue;

            // Skip teammates if team check enabled
            if (AimbotCFG::AimbotCFG.TeamCheck && entity.Controller.TeamID == localPlayer.Controller.TeamID)
                continue;

            // Skip if beyond max distance
            float distance = entity.Pawn.Pos.DistanceTo(localPlayer.Pawn.Pos);
            if (distance > AimbotCFG::AimbotCFG.MaxDistance)
                continue;

            // Get target position based on targeting mode
            Vec3 bonePos;
            int bone;
            
            if (AimbotCFG::AimbotCFG.HitboxBasedTargeting) {
                // Use hitbox-based targeting
                bool hitboxValid = false;
                int hitbox = AimbotCFG::AimbotCFG.HitboxPriority;
                if (hitbox == 3) { // Auto mode
                    // Try hitboxes in priority order
                    int hitboxPriority[] = {HITBOX_HEAD, HITBOX_CHEST, HITBOX_STOMACH, HITBOX_PELVIS};
                    for (int hb : hitboxPriority) {
                        bonePos = GetHitboxPosition(entity, hb, hitboxValid);
                        if (hitboxValid) {
                            hitbox = hb;
                            break;
                        }
                    }
                } else {
                    bonePos = GetHitboxPosition(entity, hitbox, hitboxValid);
                }
                
                if (!hitboxValid) continue;
                bone = hitbox; // Map hitbox to bone for compatibility
            } else {
                // Use traditional bone-based targeting
                bonePos = GetBonePosition(entity, bone);
            }
            
            // Apply resolver if enabled
            Vec3 resolvedPos = ResolveTarget(localPlayer, entity, bonePos);
            
            // Apply ping prediction if enabled
            Vec3 predictedPos = resolvedPos;
            if (AimbotCFG::AimbotCFG.PingPrediction) {
                // Use actual velocity vector instead of just speed
                Vec3 targetVelocity = GetVelocity(entity);
                predictedPos = ApplyPingPrediction(resolvedPos, targetVelocity, AimbotCFG::AimbotCFG.PingPredictionAmount);
            }
            
            // Calculate angle to target
            Vec3 angle = CalculateAngle(localEyePos, predictedPos);
            
            // Calculate FOV
            float fov = std::hypot(angle.x - localPlayer.Pawn.ViewAngle.x, angle.y - localPlayer.Pawn.ViewAngle.y);
            
            // Check if target is within FOV
            if (fov < bestFov)
            {
                // Check visibility if enabled
                if (AimbotCFG::AimbotCFG.VisibleCheck && !IsTargetVisible(localPlayer, entity, predictedPos))
                    continue;

                // If we only want to target head and this isn't a head bone, skip
                if (AimbotCFG::AimbotCFG.TargetHeadOnly && bone != HEAD)
                    continue;

                bestFov = fov;
                targetAngle = angle;
                targetPos = predictedPos;
                targetBone = bone;
                foundTarget = true;
            }
        }

        return foundTarget;
    }

    Vec3 GetVelocity(const CEntity& entity)
    {
        return Vec3(
            entity.Pawn.Velocity.x,
            entity.Pawn.Velocity.y,
            entity.Pawn.Velocity.z
        );
    }

    Vec3 GetBonePosition(const CEntity& entity, int& outBone)
    {
        // Default to the selected bone from config
        switch (AimbotCFG::AimbotCFG.AimBone)
        {
        case 0: // Head
            outBone = HEAD;
            return entity.GetBone().GetBonePosition(HEAD);
        case 1: // Neck
            outBone = NECK;
            return entity.GetBone().GetBonePosition(NECK);
        case 2: // Chest
            outBone = CHEST;
            return entity.GetBone().GetBonePosition(CHEST);
        case 3: // Stomach
            outBone = STOMACH;
            return entity.GetBone().GetBonePosition(STOMACH);
        case 4: // Pelvis
            outBone = PELVIS;
            return entity.GetBone().GetBonePosition(PELVIS);
        case 5: // Auto - try to find the best visible bone with hitbox priority
            {
                // Bone priority list based on damage potential and visibility likelihood
                const int bonePriority[] = { HEAD, NECK, CHEST, STOMACH, PELVIS };
                const float damageMultiplier[] = { 1.0f, 0.9f, 0.8f, 0.7f, 0.6f }; // Head does most damage
                Vec3 bestBonePos;
                float bestScore = -1.0f;
                int bestBone = HEAD;
                CBone boneData = entity.GetBone();
                
                for (int i = 0; i < sizeof(bonePriority) / sizeof(bonePriority[0]); i++)
                {
                    int bone = bonePriority[i];
                    Vec3 bonePos = boneData.GetBonePosition(bone);
                    float distance = bonePos.DistanceTo(entity.Pawn.Pos);
                    
                    // Calculate score based on damage potential and distance (closer is better)
                    float score = damageMultiplier[i] * (1.0f / (1.0f + distance * 0.1f));
                    
                    // Bonus for head if targeting head only
                    if (bone == HEAD && AimbotCFG::AimbotCFG.TargetHeadOnly)
                        score *= 2.0f;
                    
                    if (score > bestScore)
                    {
                        bestScore = score;
                        bestBonePos = bonePos;
                        bestBone = bone;
                    }
                }
                
                outBone = bestBone;
                return bestBonePos;
            }
        default:
            outBone = HEAD;
            return entity.GetBone().GetBonePosition(HEAD);
        }
    }

    bool IsTargetVisible(const CEntity& localPlayer, const CEntity& target, const Vec3& targetPos)
    {
        if (AimbotCFG::AimbotCFG.RealTraceLine) {
            // Fallback to old visibility check
            return IsTargetVisibleLegacy(localPlayer, target, targetPos);
        }
        
        // Use real engine trace line
        Vec3 eyePos = localPlayer.Pawn.CameraPos;
        Aimbot::CGameTrace trace;
        
        if (TraceRay(eyePos, targetPos, trace, (void*)localPlayer.Pawn.Address)) {
            // Check if we hit the target entity
            return trace.hit && trace.hitEntity == (void*)target.Pawn.Address;
        }
        
        return false;
    }
    
    bool IsTargetVisibleLegacy(const CEntity& localPlayer, const CEntity& target, const Vec3& targetPos)
    {
        // Use game's trace system to check if there's a clear line of sight
        Vec3 startPos = localPlayer.Pawn.CameraPos; // Eye position
        Vec3 endPos = targetPos;
        
        // Calculate direction vector
        Vec3 direction;
        direction.x = endPos.x - startPos.x;
        direction.y = endPos.y - startPos.y;
        direction.z = endPos.z - startPos.z;
        
        float distance = sqrt(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);
        
        // Normalize direction
        if (distance > 0)
        {
            direction.x /= distance;
            direction.y /= distance;
            direction.z /= distance;
        }
        
        // Convert view angle to forward vector manually
        float pitch = localPlayer.Pawn.ViewAngle.x * 3.1415926535f / 180.0f;
        float yaw = localPlayer.Pawn.ViewAngle.y * 3.1415926535f / 180.0f;
        
        Vec3 forward;
        forward.x = cos(pitch) * cos(yaw);
        forward.y = cos(pitch) * sin(yaw);
        forward.z = -sin(pitch);
        
        // Calculate dot product manually
        float dotProduct = direction.x * forward.x + direction.y * forward.y + direction.z * forward.z;
        
        // If target is not in front (dot product negative), it's not visible
        if (dotProduct < 0.0f)
            return false;
        
        // In a real implementation, this would use the game's trace functionality
        // to check for obstacles between startPos and endPos
        // For now, we'll use an improved heuristic based on distance and direction
        if (distance > 5000.0f) // Arbitrary max distance threshold
            return false;
        
        // Additional check: if target is behind a wall or obstacle
        // This would be replaced with actual game engine trace calls
        // For simulation, we'll assume visibility decreases with distance
        float visibilityFactor = 1.0f - (distance / 5000.0f);
        return visibilityFactor > 0.3f; // At least 30% visibility required
    }

    bool TraceRay(const Vec3& start, const Vec3& end, Aimbot::CGameTrace& trace, void* skipEntity)
    {
        if (!g_pEngineTrace || !g_bTraceRayHooked) {
            return TraceRayFallback(start, end, trace, skipEntity);
        }

        Aimbot::Ray_t ray;
        ray.Init(start, end);

        Aimbot::TraceFilter_t filter(skipEntity);
        filter.pSkip = skipEntity;

        // Use hooked TraceRay for better safety
        if (oTraceRay) {
            oTraceRay(g_pEngineTrace, &ray, MASK_SHOT | CONTENTS_GRATE, &filter, &trace);
        } else {
            // Fallback to direct call if hook fails
            g_pEngineTrace->TraceRay(ray, MASK_SHOT | CONTENTS_GRATE, &filter, &trace);
        }
        
        return true;
    }
    
    bool TraceRayFallback(const Vec3& start, const Vec3& end, Aimbot::CGameTrace& trace, void* skipEntity)
    {
        // Simple fallback trace implementation
        Vec3 direction = end - start;
        float distance = direction.Length();
        
        if (distance <= 0) {
            trace.hit = false;
            trace.fraction = 1.0f;
            trace.endPos = end;
            return true;
        }
        
        // Normalize direction
        direction = direction / distance;
        
        // Simple distance-based visibility check
        trace.hit = (distance < 5000.0f); // Arbitrary max distance
        trace.fraction = trace.hit ? 1.0f : 0.0f;
        trace.endPos = trace.hit ? end : start;
        trace.hitEntity = nullptr;
        
        return true;
    }
    
    Vec3 ApplyPingPrediction(const Vec3& targetPos, const Vec3& targetVelocity, float predictionAmount)
    {
        if (!AimbotCFG::AimbotCFG.PingPrediction || predictionAmount <= 0.0f) {
            return targetPos;
        }
        
        // Get interpolation and lag compensation values
        static float cl_interp = 0.1f;  // Default interpolation time
        static float lerp = cl_interp;   // Lerp value
        static float sv_unlag = 1.0f;    // Server lag compensation enabled
        
        // Calculate time to target based on distance and projectile speed
        Vec3 localPos = {0, 0, 0}; // Would need actual local player position
        float distance = targetPos.DistanceTo(localPos);
        float projectileSpeed = 1000.0f; // CS2 bullet speed approximation
        float timeToTarget = distance / projectileSpeed;
        
        // Apply proper lag compensation
        float ping = 50.0f; // Would need actual ping from game
        float lerpTime = cl_interp;
        float unlagTime = sv_unlag * ping / 1000.0f;
        
        // Total prediction time with all factors
        float totalPredictionTime = timeToTarget * predictionAmount + lerpTime + unlagTime;
        
        // Predict target position with improved accuracy
        Vec3 predictedPos = targetPos;
        predictedPos.x += targetVelocity.x * totalPredictionTime;
        predictedPos.y += targetVelocity.y * totalPredictionTime;
        predictedPos.z += targetVelocity.z * totalPredictionTime;
        
        // Add gravity compensation for projectiles
        float gravity = 800.0f; // CS2 gravity
        predictedPos.z -= 0.5f * gravity * totalPredictionTime * totalPredictionTime;
        
        return predictedPos;
    }
    
    Vec3 ResolveTarget(const CEntity& localPlayer, const CEntity& entity, const Vec3& originalPos)
    {
        if (!AimbotCFG::AimbotCFG.ResolverEnabled) return originalPos;

        Vec3 resolved = originalPos;

        switch (AimbotCFG::AimbotCFG.ResolverMode)
        {
        case 1: // LBY Resolver
        {
            float lby = entity.Pawn.LowerBodyYaw;
            float eye = entity.Pawn.ViewAngle.y;
            float delta = NormalizeAngle(eye - lby);

            if (std::abs(delta) > 35.0f)
            {
                // Desync — корректируем позицию
                float offset = 30.0f * (delta > 0 ? 1 : -1);
                resolved.x += static_cast<float>(cos((eye + offset) * M_PI / 180.0f) * 10.0f);
                resolved.y += static_cast<float>(sin((eye + offset) * M_PI / 180.0f) * 10.0f);
            }
            break;
        }

        case 2: // Brute-force Resolver
        {
            static std::map<uint64_t, int> bruteIndex;
            uint64_t steamID = entity.Controller.SteamID;

            int& idx = bruteIndex[steamID];
            const float angles[] = { 0, 60, -60, 120, -120, 180 };
            int best = 0;
            float bestScore = -1;

            for (int i = 0; i < 6; i++)
            {
                Vec3 test = originalPos;
                float angle = entity.Pawn.ViewAngle.y + angles[i];
                test.x += static_cast<float>(cos(angle * M_PI / 180.0f) * 15.0f);
                test.y += static_cast<float>(sin(angle * M_PI / 180.0f) * 15.0f);

                // Проверяем видимость
                if (IsTargetVisible(localPlayer, entity, test))
                {
                    best = i;
                    break;
                }
            }

            idx = best;
            float finalAngle = entity.Pawn.ViewAngle.y + angles[best];
            resolved.x += static_cast<float>(cos(finalAngle * M_PI / 180.0f) * 15.0f);
            resolved.y += static_cast<float>(sin(finalAngle * M_PI / 180.0f) * 15.0f);
            break;
        }
        }

        return resolved;
    }

    void ClampAngle(Vec3& angle)
    {
        // Normalize pitch
        while (angle.x > 89.0f) angle.x = 89.0f;
        while (angle.x < -89.0f) angle.x = -89.0f;
        
        // Normalize yaw
        while (angle.y > 180.0f) angle.y -= 360.0f;
        while (angle.y < -180.0f) angle.y += 360.0f;
        
        // Reset roll
        angle.z = 0.0f;
    }

    float NormalizeAngle(float angle)
    {
        while (angle > 180.0f) angle -= 360.0f;
        while (angle < -180.0f) angle += 360.0f;
        return angle;
    }
    
    Vec3 GetHitboxPosition(const CEntity& entity, int hitbox, bool& isValid)
    {
        isValid = false;
        Vec3 hitboxPos = {0, 0, 0};
        
        // Get bone data from entity
        CBone boneData = entity.GetBone();
        
        // Map hitbox to bone
        int boneId = HEAD;
        switch (hitbox) {
        case HITBOX_HEAD:
            boneId = HEAD;
            break;
        case HITBOX_NECK:
            boneId = NECK;
            break;
        case HITBOX_CHEST:
        case HITBOX_UPPER_CHEST:
        case HITBOX_LOWER_CHEST:
            boneId = CHEST;
            break;
        case HITBOX_STOMACH:
            boneId = STOMACH;
            break;
        case HITBOX_PELVIS:
            boneId = PELVIS;
            break;
        default:
            // For limbs, use closest bone
            boneId = CHEST;
            break;
        }
        
        hitboxPos = boneData.GetBonePosition(boneId);
        isValid = (hitboxPos.x != 0 || hitboxPos.y != 0 || hitboxPos.z != 0);
        
        return hitboxPos;
    }
    
    bool CreateMove(float* inputX, float* inputY, bool* inAttack)
    {
        if (!AimbotCFG::AimbotCFG.CreateMoveHook || !AimbotCFG::AimbotCFG.AimbotEnabled) {
            return false;
        }
        
        // Check if aimbot key is active
        if (!IsAimbotKeyActive()) {
            return false;
        }
        
        // If we have a silent angle stored, apply it
        if (AimbotCFG::AimbotCFG.SilentAimInCreateMove && 
            (g_silentAngle.x != 0 || g_silentAngle.y != 0)) {
            
            *inputX = g_silentAngle.x;
            *inputY = g_silentAngle.y;
            
            // Clear the stored angle after use
            g_silentAngle = {0, 0, 0};
            
            // Set attack if triggerbot is enabled in CreateMove
            if (AimbotCFG::AimbotCFG.TriggerBotInCreateMove && g_shouldAttack) {
                *inAttack = true;
                g_shouldAttack = false;
            }
            
            return true;
        }
        
        return false;
    }
    
    Vec3 CalculateAngle(const Vec3& source, const Vec3& destination)
    {
        Vec3 delta;
        delta.x = destination.x - source.x;
        delta.y = destination.y - source.y;
        delta.z = destination.z - source.z;
        float hyp = std::sqrt(delta.x * delta.x + delta.y * delta.y);
        
        Vec3 angle;
        angle.x = -std::atan2(delta.z, hyp) * (180.0f / 3.14159265358979323846f);
        angle.y = std::atan2(delta.y, delta.x) * (180.0f / 3.14159265358979323846f);
        angle.z = 0.0f;
        
        return angle;
    }

    Vec3 ApplySmoothing(const Vec3& currentAngle, const Vec3& targetAngle, float smoothFactor)
    {
        if (!AimbotCFG::AimbotCFG.SmoothAim || smoothFactor <= 0.0f)
            return targetAngle;

        Vec3 delta;
        delta.x = targetAngle.x - currentAngle.x;
        delta.y = targetAngle.y - currentAngle.y;
        delta.z = 0.0f;

        // Normalize angles
        while (delta.y > 180.0f) delta.y -= 360.0f;
        while (delta.y < -180.0f) delta.y += 360.0f;
        
        // Apply smoothing with humanization
        float smoothValue = 1.0f - (AimbotCFG::AimbotCFG.SmoothValue * smoothFactor);
        smoothValue = (std::max)(0.1f, (std::min)(0.9f, smoothValue)); // Clamp between 0.1 and 0.9
        
        // Add randomization for human-like movement
        float randomFactor = 0.9f + (static_cast<float>(rand()) / RAND_MAX) * 0.2f; // Random between 0.9 and 1.1
        smoothValue *= randomFactor;
        
        // Apply curve for more natural movement (faster at start, slower at end)
        float curveFactor = std::pow(smoothValue, 1.5f); // Exponential curve
        
        Vec3 smoothedAngle;
        smoothedAngle.x = currentAngle.x + delta.x * curveFactor;
        smoothedAngle.y = currentAngle.y + delta.y * curveFactor;
        smoothedAngle.z = 0.0f;
        
        return smoothedAngle;
    }

    Vec3 ApplyRecoilControl(const Vec3& aimAngle, const CEntity& localPlayer)
    {
        if (!AimbotCFG::AimbotCFG.RCS)
            return aimAngle;

        // Get current punch angle
        // Convert Vec2 to Vec3 for punch angle
        Vec3 punchAngle;
        punchAngle.x = localPlayer.Pawn.AimPunchAngle.x;
        punchAngle.y = localPlayer.Pawn.AimPunchAngle.y;
        punchAngle.z = 0.0f;
        
        // Apply RCS strength
        punchAngle.x *= AimbotCFG::AimbotCFG.RCSStrength * 2.0f;
        punchAngle.y *= AimbotCFG::AimbotCFG.RCSStrength * 2.0f;
        
        // Subtract punch angle from aim angle
        Vec3 compensatedAngle = aimAngle;
        compensatedAngle.x -= punchAngle.x;
        compensatedAngle.y -= punchAngle.y;
        
        return compensatedAngle;
    }

    // Check if aimbot should be active (now only checks if enabled)
    bool IsAimbotKeyActive()
    {
        return AimbotCFG::AimbotCFG.AimbotEnabled;
    }


    void HandleAutoFire(CUserCmd* cmd, bool targetAcquired, const Vec3& targetPos)
    {
        if (!cmd) return;

        bool shouldFire = false;

        if (AimbotCFG::AimbotCFG.TriggerBot && targetAcquired)
        {
            static auto lastCheck = std::chrono::steady_clock::now();
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastCheck).count();
            
            if (elapsed > AimbotCFG::AimbotCFG.TriggerDelay)
            {
                shouldFire = true;
                lastCheck = now;
            }
        }
        else if (AimbotCFG::AimbotCFG.AutoFireMode == 1 && targetAcquired)
        {
            shouldFire = true;
        }

        if (shouldFire)
        {
            cmd->buttons |= IN_ATTACK;
            cmd->tick_count += dis(gen); // Use proper random generator
        }
    }

    static Vec3 storedSilentAngle = {0, 0, 0};

    void Run(CUserCmd* cmd, const CEntity& localPlayer, const std::vector<EntityResult>& entities)
    {
        // Skip if aimbot is disabled or menu is open
        if (!AimbotCFG::AimbotCFG.AimbotEnabled || MenuConfig::ShowMenu || !cmd)
            return;

        // Check if aimbot key is active
        if (!IsAimbotKeyActive())
            return;

        // Apply anti-aim if enabled and we're not aiming
        bool shouldSendPacket = true;
        if (MiscCFG::AntiAimEnabled && (!AimbotCFG::AimbotCFG.AimbotEnabled || !IsAimbotKeyActive())) {
            Vec3 antiAimAngle = ApplyAntiAim(localPlayer, cmd, shouldSendPacket);
            cmd->viewangles = antiAimAngle;
            
            // Don't send packets for choke when anti-aim is active
            if (!shouldSendPacket) {
                // This would need to be implemented in the actual hook
            }
            return;
        }

        // Find best target
        Vec3 targetAngle;
        Vec3 targetPos;
        int targetBone = HEAD;
        bool targetAcquired = GetBestTarget(localPlayer, entities, targetAngle, targetPos, targetBone);

        // Skip if no target found or if we're only targeting head and this isn't a head
        if (!targetAcquired || (AimbotCFG::AimbotCFG.StopWhenNoHead && AimbotCFG::AimbotCFG.TargetHeadOnly && targetBone != HEAD))
            return;

        // Convert Vec2 to Vec3 for view angle
        Vec3 currentViewAngle;
        currentViewAngle.x = localPlayer.Pawn.ViewAngle.x;
        currentViewAngle.y = localPlayer.Pawn.ViewAngle.y;
        currentViewAngle.z = 0.0f;

        Vec3 finalAngle = targetAngle;

        // RAGE MODE SPECIFIC LOGIC
        if (AimbotCFG::CurrentConfig == AimbotCFG::RAGE) {
            // Apply rage-specific features
            if (AimbotCFG::RageCFG.Multipoint) {
                Vec3 multipointPos;
                if (GetMultipointHitbox(entities[0].entity, HITBOX_HEAD, multipointPos, AimbotCFG::RageCFG.MultipointScale)) {
                    targetPos = multipointPos;
                    finalAngle = CalculateAngle(localPlayer.Pawn.CameraPos, targetPos);
                }
            }

            // Apply resolver bypass for rage mode
            targetPos = ApplyResolverBypass(entities[0].entity, targetPos);
            finalAngle = CalculateAngle(localPlayer.Pawn.CameraPos, targetPos);

            // Check autowall for rage mode
            if (AimbotCFG::RageCFG.AimThroughWalls) {
                float damage = 0.0f;
                if (CanPenetrateWall(localPlayer.Pawn.CameraPos, targetPos, damage)) {
                    // Can penetrate wall, continue with shot
                } else if (!IsVisibleForRage(localPlayer, entities[0].entity, targetPos, true)) {
                    return; // Can't penetrate and not visible
                }
            }

            // Apply perfect silent aim for rage mode
            if (AimbotCFG::RageCFG.PerfectSilent) {
                ApplyRageSilentAim(cmd, finalAngle, localPlayer.Pawn.CameraPos, targetPos);
            } else {
                cmd->viewangles = finalAngle;
            }

            // Force attack in rage mode
            if (AimbotCFG::RageCFG.ForceHeadshot || AimbotCFG::AimbotCFG.AutoFireMode == 2) {
                cmd->buttons |= IN_ATTACK;
                cmd->tick_count += 16; // Tick manipulation for instant hit
            }

            return; // Skip normal aimbot logic for rage mode
        }

        // NORMAL/SEMIRAGE MODE LOGIC
        // Apply smoothing
        if (AimbotCFG::AimbotCFG.SmoothAim) {
            finalAngle = ApplySmoothing(currentViewAngle, finalAngle, AimbotCFG::AimbotCFG.SmoothValue);
        }
        
        // Apply recoil control
        finalAngle = ApplyRecoilControl(finalAngle, localPlayer);
        
        // Apply sensitivity
        finalAngle.x *= AimbotCFG::AimbotCFG.Sensitivity;
        finalAngle.y *= AimbotCFG::AimbotCFG.Sensitivity;
        
        // Clamp angles to valid range
        ClampAngle(finalAngle);
        
        // Apply the aim
        if (AimbotCFG::AimbotCFG.SilentAim)
        {
            cmd->viewangles = finalAngle;
            cmd->tick_count += 1; // Anti-anti-aim
        }

        // Handle auto-fire
        HandleAutoFire(cmd, targetAcquired, targetPos);
    }

    void DrawFOV(const CEntity& localPlayer)
    {
        // Skip if FOV drawing is disabled or menu is open
        if (!AimbotCFG::AimbotCFG.ShowFov || MenuConfig::ShowMenu)
            return;

        // Skip if not in game
        if (localPlayer.Controller.TeamID == 0)
            return;

        ImDrawList* drawList = ImGui::GetBackgroundDrawList();
        ImVec2 screenSize = ImGui::GetIO().DisplaySize;
        ImVec2 screenCenter(screenSize.x / 2, screenSize.y / 2);

        // Calculate FOV radius in pixels
        float fovRadius = std::tan(AimbotCFG::AimbotCFG.FOVRadius * 0.017453f) / std::tan(90.0f * 0.017453f) * screenSize.x;

        // Animation and transparency
        static float animationTime = 0.0f;
        animationTime += 0.016f; // ~60 FPS
        
        // Pulsing effect
        float pulse = std::sin(animationTime * 2.0f) * 0.2f + 0.8f;
        
        // Get color with animated alpha
        ImU32 baseColor = ImColor(AimbotCFG::AimbotCFG.FOVCircleColor);
        ImU32 animatedColor = ImColor(
            (int)(baseColor & 0xFF),
            (int)((baseColor >> 8) & 0xFF),
            (int)((baseColor >> 16) & 0xFF),
            (int)((baseColor >> 24) * pulse)
        );

        // Draw FOV circle with animation
        drawList->AddCircle(
            screenCenter,
            fovRadius,
            animatedColor,
            100,  // Number of segments for smooth circle
            AimbotCFG::AimbotCFG.FOVCircleThickness * pulse
        );
        
        // Draw inner circle for visual effect
        drawList->AddCircle(
            screenCenter,
            fovRadius * 0.95f,
            ImColor(255, 255, 255, 50),
            60,
            1.0f
        );
    }

    // RAGE MODE FUNCTIONS
    bool ApplyRageSilentAim(CUserCmd* cmd, const Vec3& targetAngle, const Vec3& localPos, const Vec3& targetPos)
    {
        if (!cmd || AimbotCFG::CurrentConfig != AimbotCFG::RAGE) {
            return false;
        }

        // Perfect silent aim - instant angle change without visual movement
        cmd->viewangles = targetAngle;
        
        // Force instant hit for rage mode
        if (AimbotCFG::RageCFG.InstantHit) {
            cmd->tick_count += 16; // Force tick manipulation for instant hit
        }
        
        // Apply no spread compensation
        if (AimbotCFG::RageCFG.NoSpread) {
            cmd->viewangles = ApplyNoSpread(targetAngle, cmd);
        }
        
        return true;
    }

    bool CalculateInstantHit(const Vec3& source, const Vec3& target, Vec3& outAngle, float& outTime)
    {
        // Calculate perfect angle for instant hit
        outAngle = CalculateAngle(source, target);
        
        // Calculate bullet travel time (nearly instant for rage mode)
        float distance = source.DistanceTo(target);
        float bulletSpeed = 10000.0f; // Exaggerated speed for rage mode
        outTime = distance / bulletSpeed;
        
        // Compensate for bullet drop (minimal for rage mode)
        outAngle.x -= (0.5f * 800.0f * outTime * outTime) * (180.0f / 3.14159265359f) / distance;
        
        return true;
    }

    bool CanPenetrateWall(const Vec3& source, const Vec3& target, float& damage)
    {
        if (!AimbotCFG::RageCFG.AutoWall) {
            return false;
        }

        // Simple autowall calculation
        float distance = source.DistanceTo(target);
        float penetrationPower = AimbotCFG::RageCFG.WallPenetrationPower;
        
        // Calculate damage reduction based on wall thickness
        damage = 100.0f * (1.0f - (distance / 10000.0f) * (1.0f / penetrationPower));
        
        return damage >= AimbotCFG::RageCFG.MinDamage;
    }

    bool GetMultipointHitbox(const CEntity& entity, int hitbox, Vec3& outPos, float scale)
    {
        if (!AimbotCFG::RageCFG.Multipoint) {
            return false;
        }

        // Get base hitbox position
        bool isValid;
        outPos = GetHitboxPosition(entity, hitbox, isValid);
        
        if (!isValid) {
            return false;
        }

        // Add multipoint scaling for better hit registration
        Vec3 center = entity.Pawn.Pos;
        Vec3 direction = outPos - center;
        direction = direction * scale;
        outPos = center + direction;
        
        return true;
    }

    Vec3 ApplyAntiAim(const CEntity& localPlayer, CUserCmd* cmd, bool& shouldSend)
    {
        if (!MiscCFG::AntiAimEnabled) {
            return cmd->viewangles;
        }

        Vec3 antiAimAngle = cmd->viewangles;
        shouldSend = false; // Don't send packet for choke

        // Apply pitch based on mode
        switch (MiscCFG::AntiAimMode) {
        case 0: // Static
            antiAimAngle.x = MiscCFG::AntiAimPitch;
            break;
        case 1: // Jitter
            {
                static bool jitterSwitch = false;
                jitterSwitch = !jitterSwitch;
                antiAimAngle.x = jitterSwitch ? MiscCFG::AntiAimPitch : -MiscCFG::AntiAimPitch;
            }
            break;
        case 2: // Spin
            {
                static float spinAngle = 0.0f;
                spinAngle += MiscCFG::AntiAimSpinSpeed * 5.0f;
                if (spinAngle > 180.0f) spinAngle -= 360.0f;
                antiAimAngle.x = spinAngle;
            }
            break;
        case 3: // Random
            antiAimAngle.x = -89.0f + (static_cast<float>(rand()) / RAND_MAX) * 178.0f;
            break;
        }

        // Apply yaw offset
        float baseYaw = localPlayer.Pawn.ViewAngle.y + MiscCFG::AntiAimYawOffset;
        
        switch (MiscCFG::AntiAimMode) {
        case 0: // Static
            antiAimAngle.y = baseYaw;
            break;
        case 1: // Jitter
            {
                static bool yawJitter = false;
                yawJitter = !yawJitter;
                float jitter = yawJitter ? MiscCFG::AntiAimJitterRange : -MiscCFG::AntiAimJitterRange;
                antiAimAngle.y = baseYaw + jitter;
            }
            break;
        case 2: // Spin
            {
                static float spinYaw = 0.0f;
                spinYaw += MiscCFG::AntiAimSpinSpeed;
                if (spinYaw > 180.0f) spinYaw -= 360.0f;
                antiAimAngle.y = spinYaw;
            }
            break;
        case 3: // Random
            antiAimAngle.y = baseYaw + (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 180.0f;
            break;
        }

        // Apply LBY breaker
        if (MiscCFG::AntiAimLBYBreaker) {
            BreakLBY(cmd, antiAimAngle.y);
        }

        return antiAimAngle;
    }

    void BreakLBY(CUserCmd* cmd, float& yaw)
    {
        static float lastLBYUpdate = 0.0f;
        static bool shouldBreak = false;
        
        // Simulate LBY update timing
        float currentTime = GetTickCount64() / 1000.0f;
        if (currentTime - lastLBYUpdate > 1.1f) {
            shouldBreak = !shouldBreak;
            lastLBYUpdate = currentTime;
        }
        
        if (shouldBreak) {
            yaw += MiscCFG::AntiAimLBYOffset;
        }
    }

    Vec3 ApplyResolverBypass(const CEntity& entity, const Vec3& originalPos)
    {
        if (AimbotCFG::CurrentConfig != AimbotCFG::RAGE || !AimbotCFG::RageCFG.ResolverEnabled) {
            return originalPos;
        }

        Vec3 bypassedPos = originalPos;
        
        // Apply aggressive resolver bypass for rage mode
        switch (AimbotCFG::RageCFG.ResolverMode) {
        case 1: // Advanced
            {
                float lby = entity.Pawn.LowerBodyYaw;
                float eye = entity.Pawn.ViewAngle.y;
                float delta = NormalizeAngle(eye - lby);
                
                if (std::abs(delta) > 30.0f) {
                    // Force head position calculation
                    float offset = 45.0f * (delta > 0 ? 1 : -1);
                    bypassedPos.x += static_cast<float>(cos((eye + offset) * M_PI / 180.0f) * 20.0f);
                    bypassedPos.y += static_cast<float>(sin((eye + offset) * M_PI / 180.0f) * 20.0f);
                }
            }
            break;
        case 2: // Aggressive
            {
                // Brute force with multiple angles
                static std::map<uint64_t, int> bruteForceIndex;
                uint64_t steamID = entity.Controller.SteamID;
                
                int& index = bruteForceIndex[steamID];
                const float angles[] = { 0, 30, -30, 60, -60, 90, -90, 120, -120, 180 };
                
                index = (index + 1) % (sizeof(angles) / sizeof(angles[0]));
                float finalAngle = entity.Pawn.ViewAngle.y + angles[index];
                
                bypassedPos.x += static_cast<float>(cos(finalAngle * M_PI / 180.0f) * 25.0f);
                bypassedPos.y += static_cast<float>(sin(finalAngle * M_PI / 180.0f) * 25.0f);
            }
            break;
        }
        
        return bypassedPos;
    }

    Vec3 ApplyNoSpread(const Vec3& aimAngle, CUserCmd* cmd)
    {
        if (!AimbotCFG::RageCFG.NoSpread) {
            return aimAngle;
        }

        // Simple no spread compensation
        Vec3 compensatedAngle = aimAngle;
        
        // Add random seed manipulation for spread reduction
        static int seed = 0;
        seed = (seed + 1) % 256;
        cmd->random_seed = seed;
        
        // Apply spread compensation angle
        compensatedAngle.x += (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.1f;
        compensatedAngle.y += (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.1f;
        
        return compensatedAngle;
    }

    bool ShouldForceHeadshot(const CEntity& target, float weaponDamage)
    {
        if (!AimbotCFG::RageCFG.ForceHeadshot) {
            return false;
        }

        // Always force headshot in rage mode if damage is sufficient
        return weaponDamage >= AimbotCFG::RageCFG.MinDamage;
    }

    bool IsVisibleForRage(const CEntity& localPlayer, const CEntity& target, const Vec3& pos, bool ignoreWalls)
    {
        if (ignoreWalls || AimbotCFG::CurrentConfig == AimbotCFG::RAGE) {
            // In rage mode, ignore most visibility checks
            if (AimbotCFG::RageCFG.IgnoreSmoke && AimbotCFG::RageCFG.IgnoreFlash) {
                return true;
            }
        }

        // Fall back to normal visibility check
        return IsTargetVisible(localPlayer, target, pos);
    }
}
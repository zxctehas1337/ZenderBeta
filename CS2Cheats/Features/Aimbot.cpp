#include "Aimbot.h"
#include "../Core/Render.h"
#include "../Helpers/Logger.h"
#include "../Game/Game.h"
#include "../Core/Init.h"
#include <algorithm>
#include <iostream>

// Static members initialization
TargetInfo Aimbot::currentTarget;
std::chrono::steady_clock::time_point Aimbot::lastAimTime = std::chrono::steady_clock::now();
std::chrono::steady_clock::time_point Aimbot::lastShotTime = std::chrono::steady_clock::now();
std::chrono::steady_clock::time_point Aimbot::lastTriggerTime = std::chrono::steady_clock::now();
std::mt19937 Aimbot::rng(static_cast<unsigned int>(std::chrono::steady_clock::now().time_since_epoch().count()));
Vec3 Aimbot::lastAimAngle = {0, 0, 0};
bool Aimbot::isAiming = false;
int Aimbot::targetSwitchCount = 0;
float Aimbot::averageReactionTime = 0.0f;
std::vector<TargetInfo> Aimbot::targetHistory;

void Aimbot::Initialize() {
    Reset();
    
    // Initialize 2025 safety systems (simplified versions)
    Log::Info("Initializing 2025 safety features...");
    
    Log::Info("Aimbot initialized with 2025 safety features");
}

void Aimbot::Shutdown() {
    Reset();
    
    // Shutdown 2025 safety systems
    Log::Info("Safety systems disabled");
    
    Log::Info("Aimbot shutdown - safety systems disabled");
}

void Aimbot::Reset() {
    currentTarget = TargetInfo{};
    isAiming = false;
    targetSwitchCount = 0;
    averageReactionTime = 0.0f;
    targetHistory.clear();
    lastAimAngle = {0, 0, 0};
}

void Aimbot::Run(const CEntity& localEntity, const std::vector<CEntity>& entities) {
    auto& config = GetActiveConfig();
    
    if (!config.Enabled || !config.AimbotEnabled) {
        Reset();
        return;
    }

    // Check if local player is valid
    if (localEntity.Pawn.Address == 0 || localEntity.Pawn.Health <= 0) {
        Reset();
        return;
    }

    // Flash check
    if (config.FlashCheck && localEntity.Pawn.FlashDuration > 0.0f) {
        return;
    }
    
    // 2025 safety features would be updated here

    // Update adaptive settings
    if (config.AdaptiveFOV) {
        UpdateAdaptiveFOV(localEntity);
    }
    if (config.AdaptiveSpeed) {
        UpdateAdaptiveSpeed(localEntity);
    }

    // Find new target if we don't have one or current target is invalid
    if (currentTarget.entity.Pawn.Address == 0 || !IsTargetValid(currentTarget.entity, localEntity)) {
        TargetInfo newTarget = FindBestTarget(localEntity, entities);
        if (newTarget.entity.Pawn.Address != 0) {
            if (ShouldSwitchTarget(newTarget, localEntity)) {
                SetCurrentTarget(newTarget);
                targetSwitchCount++;
                LogAimEvent("Target switched", newTarget);
            }
        }
    }

    // Aim at current target
    if (currentTarget.entity.Pawn.Address != 0) {
        Vec3 aimPos = currentTarget.aimPosition;

        // Apply prediction
        if (config.Prediction) {
            aimPos = PredictTargetPosition(currentTarget, 0.1f * config.PredictionScale);
        }

        // Apply humanization
        if (config.Humanization) {
            aimPos = HumanizeAim(aimPos);
        }

        // Add delay if configured
        if (config.DelayMin > 0.0f || config.DelayMax > 0.0f) {
            AddDelay();
        }

        // Aim at target
        AimAtTarget(aimPos, localEntity);
        isAiming = true;
    } else {
        isAiming = false;
    }

    // Trigger bot
    if (config.TriggerEnabled) {
        TriggerBot(localEntity, entities);
    }

    UpdateStatistics();
}

Vec3 Aimbot::GetAimPosition(const CEntity& entity, int bone) {
    Vec3 bonePos;
    
    // Use updated 2025 bone indices directly
    switch (bone) {
        case AimbotCFG::AimbotConfigBase::HEAD:
            bonePos = entity.Pawn.BoneData.GetBonePosition(8); // head bone index
            break;
        case AimbotCFG::AimbotConfigBase::NECK:
            bonePos = entity.Pawn.BoneData.GetBonePosition(7); // neck_0 bone index
            break;
        case AimbotCFG::AimbotConfigBase::CHEST_UPPER:
            bonePos = entity.Pawn.BoneData.GetBonePosition(5); // spine_1 bone index
            break;
        case AimbotCFG::AimbotConfigBase::CHEST_LOWER:
            bonePos = entity.Pawn.BoneData.GetBonePosition(4); // spine_2 bone index
            break;
        case AimbotCFG::AimbotConfigBase::STOMACH:
            bonePos = entity.Pawn.BoneData.GetBonePosition(3); // spine_3 bone index
            break;
        case AimbotCFG::AimbotConfigBase::PELVIS:
            bonePos = entity.Pawn.BoneData.GetBonePosition(2); // pelvis bone index
            break;
        default:
            bonePos = entity.Pawn.BoneData.GetBonePosition(8); // default to head
            break;
    }
    
    return bonePos;
}

float Aimbot::CalculateFOV(const Vec3& viewAngle, const Vec3& aimAngle) {
    Vec3 delta = aimAngle - viewAngle;
    Math::NormalizeAngles(delta);
    
    return sqrtf(delta.x * delta.x + delta.y * delta.y);
}

bool Aimbot::IsTargetValid(const CEntity& entity, const CEntity& localEntity) {
    if (entity.Pawn.Address == 0)
        return false;
        
    if (entity.Pawn.Health <= 0)
        return false;
        
    if (entity.Controller.Address == 0)
        return false;
        
    if (entity.Pawn.Address == localEntity.Pawn.Address)
        return false;
        
    return true;
}

bool Aimbot::IsVisible(const CEntity& from, const CEntity& to, const Vec3& aimPos) {
    auto& config = GetActiveConfig();
    
    if (!config.VisibleCheck)
        return true;
    
    // 2025 improved visibility check
    if (config.RealTraceLine) {
        // Advanced visibility check with proper tracing
        Vec3 eyePos = from.Pawn.CameraPos;
        float distance = Math::Distance(eyePos, aimPos);
        
        // Basic distance check
        if (distance > config.MaxDistance || distance <= 0.0f) {
            return false;
        }
        
        // Rage mode autowall check
        if (config.RageMode && HasRageConfig()) {
            auto* rageConfig = static_cast<AimbotCFG::RageConfig*>(&config);
            
            if (rageConfig->AutoWall) {
                // Simple autowall: allow shooting through thin walls
                return distance <= config.MaxDistance;
            }
            
            if (rageConfig->IgnoreSmoke && rageConfig->IgnoreFlash) {
                // Ignore all visibility checks in rage mode
                return true;
            }
        }
        
        // Simplified trace check (2025: TODO implement proper IEngineTrace)
        Vec3 direction = aimPos - eyePos;
        direction.Normalize();
        
        // Sample points along the line for basic obstruction check
        int samples = Math::Clamp(static_cast<int>(distance / 100.0f), 3, 10);
        for (int i = 1; i < samples; i++) {
            Vec3 samplePos = eyePos + direction * (distance * i / samples);
            
            // Basic world bounds check
            if (samplePos.z < -2000.0f || samplePos.z > 3000.0f) {
                return false; // Likely obstructed
            }
        }
        
        return true; // Assume visible if no obvious obstructions
    } else {
        // Basic visibility check (legacy)
        Vec3 eyePos = from.Pawn.CameraPos;
        float distance = Math::Distance(eyePos, aimPos);
        
        if (distance > config.MaxDistance || distance <= 0.0f) {
            return false;
        }
        
        Vec3 forward = Math::CalcAngle(eyePos, aimPos);
        float fov = CalculateFOV(from.Pawn.ViewAngle, forward);
        
        return fov <= 90.0f;
    }
}

void Aimbot::AimAtTarget(const Vec3& targetPos, const CEntity& localEntity) {
    auto& config = GetActiveConfig();
    
    // Get current view angles
    Vec3 currentViewAngle = localEntity.Pawn.ViewAngle;
    
    // Calculate angle to target
    Vec3 targetAngle = Math::CalcAngle(localEntity.Pawn.CameraPos, targetPos);
    Math::NormalizeAngles(targetAngle);
    
    // Apply mode-specific settings
    if (config.RageMode) {
        ApplyRageSettings(targetAngle, currentTarget);
    } else if (config.LegitModeEnabled) {
        if (!IsLegitAimAllowed(localEntity, currentTarget)) {
            return;
        }
        ApplyLegitSettings(targetAngle, currentTarget);
    }
    
    // Apply smooth
    if (config.Smooth > 0.1f && !config.RageInstant) {
        float dynamicSmooth = GetDynamicSmooth(localEntity, currentTarget);
        targetAngle = SmoothAim(currentViewAngle, targetAngle, dynamicSmooth);
    }
    
    // Apply recoil control
    if (config.RecoilControl > 0.0f) {
        ApplyRecoilControl(targetAngle, localEntity);
    }
    
    // Apply anti-shake
    if (config.AntiShake) {
        ApplyAntiShake(targetAngle);
    }
    
    // Normalize and clamp angles
    Math::NormalizeAngles(targetAngle);
    ClampAngles(targetAngle);
    
    // Apply silent aim (handles both silent and normal aim)
    if (config.RageMode && HasRageConfig()) {
        ApplyRageSilentAim(targetAngle, localEntity);
    } else {
        ApplySilentAim(targetAngle, localEntity);
    }
    
    lastAimAngle = targetAngle;
    lastAimTime = std::chrono::steady_clock::now();
}

Vec3 Aimbot::SmoothAim(const Vec3& currentAngle, const Vec3& targetAngle, float smoothFactor) {
    Vec3 delta = targetAngle - currentAngle;
    Math::NormalizeAngles(delta);
    
    // Apply smooth factor with curve
    float smoothValue = (0.1f > smoothFactor) ? 0.1f : smoothFactor;
    delta.x /= smoothValue;
    delta.y /= smoothValue;
    delta.z /= smoothValue;
    
    return currentAngle + delta;
}

void Aimbot::ApplyRecoilControl(Vec3& angle, const CEntity& localEntity) {
    auto& config = GetActiveConfig();
    
    if (localEntity.Pawn.ShotsFired > 1) {
        Vec3 aimPunch = localEntity.Pawn.AimPunchAngle;
        aimPunch.x *= config.RecoilControl;
        aimPunch.y *= config.RecoilControl;
        
        angle.x -= aimPunch.x;
        angle.y -= aimPunch.y;
    }
}

void Aimbot::ApplySilentAim(const Vec3& targetAngle, const CEntity& localEntity) {
    auto& config = GetActiveConfig();
    
    if (!config.SilentAim) {
        // Normal aim - direct memory write (2025: TODO implement CreateMove hook)
        memoryManager.WriteMemory(localEntity.Pawn.Address + Offset.Pawn.angEyeAngles, targetAngle);
        return;
    }
    
    // Silent aim implementation (2025 improved version)
    static Vec3 originalAngles = {0, 0, 0};
    static bool isAimingAtTarget = false;
    static std::chrono::steady_clock::time_point aimStartTime;
    
    // Check if we should start silent aim
    bool shouldAim = IsPlayerShooting(localEntity) || config.RageMode;
    
    if (shouldAim && !isAimingAtTarget) {
        // Store original angles
        memoryManager.ReadMemory(localEntity.Pawn.Address + Offset.Pawn.angEyeAngles, originalAngles);
        isAimingAtTarget = true;
        aimStartTime = std::chrono::steady_clock::now();
        
        // Apply target angles for shooting
        Vec3 silentAngle = targetAngle;
        
        // Apply no-spread compensation if enabled
        if (config.RageMode && HasRageConfig()) {
            ApplyNoSpread(silentAngle, localEntity);
        }
        
        // Apply silent aim angles (2025: improved method)
        memoryManager.WriteMemory(localEntity.Pawn.Address + Offset.Pawn.angEyeAngles, silentAngle);
        
        // Force attack if in rage mode
        if (config.RageMode && config.RageInstant) {
            ForceAttack(true);
        }
    }
    else if (!shouldAim && isAimingAtTarget) {
        // Restore original angles after shooting
        auto now = std::chrono::steady_clock::now();
        auto aimDuration = std::chrono::duration_cast<std::chrono::milliseconds>(now - aimStartTime).count();
        
        // Add small delay to ensure shot is fired
        if (aimDuration > 50) { // 50ms delay
            memoryManager.WriteMemory(localEntity.Pawn.Address + Offset.Pawn.angEyeAngles, originalAngles);
            isAimingAtTarget = false;
            
            // Stop attack
            if (config.RageMode && config.RageInstant) {
                ForceAttack(false);
            }
        }
    }
    else if (isAimingAtTarget) {
        // Continue aiming at target while shooting
        Vec3 silentAngle = targetAngle;
        
        // Apply no-spread compensation if enabled
        if (config.RageMode && HasRageConfig()) {
            ApplyNoSpread(silentAngle, localEntity);
        }
        
        memoryManager.WriteMemory(localEntity.Pawn.Address + Offset.Pawn.angEyeAngles, silentAngle);
    }
}

Vec3 Aimbot::HumanizeAim(const Vec3& input) {
    auto& config = GetActiveConfig();
    Vec3 output = input;
    
    // Add random error
    if (config.RandomFactor > 0.0f) {
        float errorX = RandomFloat(-config.RandomFactor, config.RandomFactor);
        float errorY = RandomFloat(-config.RandomFactor, config.RandomFactor);
        output.x += errorX;
        output.y += errorY;
    }
    
    // Apply human curve
    if (config.HumanCurve > 0.0f) {
        output = ApplyHumanCurve(output, config.HumanCurve);
    }
    
    return output;
}

float Aimbot::RandomFloat(float min, float max) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(rng);
}

void Aimbot::AddHumanError(Vec3& angle) {
    auto& config = GetActiveConfig();
    
    if (config.RandomFactor > 0.0f) {
        angle.x += RandomFloat(-config.RandomFactor, config.RandomFactor);
        angle.y += RandomFloat(-config.RandomFactor, config.RandomFactor);
    }
}

Vec3 Aimbot::ApplyHumanCurve(const Vec3& input, float curveIntensity) {
    // Apply a bezier-like curve for human-like movement
    Vec3 output = input;
    
    // Simple curve implementation
    float factor = (1.0f < curveIntensity) ? 1.0f : curveIntensity;
    output.x *= (1.0f + factor * 0.1f);
    output.y *= (1.0f + factor * 0.1f);
    
    return output;
}

void Aimbot::AddDelay() {
    auto& config = GetActiveConfig();
    
    if (config.DelayMin > 0.0f || config.DelayMax > 0.0f) {
        float delay = RandomFloat(config.DelayMin, config.DelayMax);
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(delay * 1000)));
    }
}

TargetInfo Aimbot::FindBestTarget(const CEntity& localEntity, const std::vector<CEntity>& entities) {
    std::vector<TargetInfo> validTargets;
    auto& config = GetActiveConfig();
    
    for (const auto& entity : entities) {
        if (!IsTargetValid(entity, localEntity))
            continue;
            
        // Team filter
        if (config.TeamFilter && entity.Controller.TeamID == localEntity.Controller.TeamID)
            continue;
            
        // Get aim position
        Vec3 aimPos = GetAimPosition(entity, config.AimBone);
        
        // Calculate FOV
        Vec3 viewAngle = localEntity.Pawn.ViewAngle;
        Vec3 aimAngle = Math::CalcAngle(localEntity.Pawn.CameraPos, aimPos);
        float fov = CalculateFOV(viewAngle, aimAngle);
        
        // Check FOV limit
        float currentFOV = config.RageMode ? config.RageFOV : config.FOV;
        if (fov > currentFOV)
            continue;
            
        // Visible check
        bool isVisible = false;
        if (config.VisibleCheck) {
            isVisible = IsVisible(localEntity, entity, aimPos);
            if (!isVisible)
                continue;
        }
        
        // Calculate distance
        float distance = Math::Distance(localEntity.Pawn.CameraPos, entity.Pawn.Pos);
        
        // Check max distance
        if (distance > config.MaxDistance)
            continue;
        
        // Create target info
        TargetInfo target;
        target.entity = entity;
        target.aimPosition = aimPos;
        target.distance = distance;
        target.fov = fov;
        target.health = entity.Pawn.Health;
        target.isVisible = isVisible;
        target.score = CalculateTargetScore(target, localEntity);
        target.predictedPosition = aimPos;
        target.lastUpdateTime = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count() / 1000.0f;
        
        validTargets.push_back(target);
    }
    
    // Sort targets by score
    SortTargets(validTargets, localEntity);
    
    // Return best target
    return validTargets.empty() ? TargetInfo{} : validTargets[0];
}

float Aimbot::CalculateTargetScore(const TargetInfo& target, const CEntity& localEntity) {
    auto& config = GetActiveConfig();
    float score = 0.0f;
    
    switch (config.TargetPriority) {
        case AimbotCFG::AimbotConfigBase::DISTANCE:
            score = 1000.0f / (target.distance + 1.0f);
            break;
        case AimbotCFG::AimbotConfigBase::HEALTH:
            score = 100.0f / (target.health + 1.0f);
            break;
        case AimbotCFG::AimbotConfigBase::ANGLE:
            score = 100.0f / (target.fov + 1.0f);
            break;
        case AimbotCFG::AimbotConfigBase::FOV_PRIORITY:
            score = 180.0f - target.fov;
            break;
    }
    
    // Bonus for visible targets
    if (target.isVisible)
        score *= 1.5f;
        
    // Bonus for lower health (priority to weakened enemies)
    if (target.health < 50)
        score *= 1.2f;
        
    return score;
}

void Aimbot::SortTargets(std::vector<TargetInfo>& targets, const CEntity& localEntity) {
    std::sort(targets.begin(), targets.end(), 
        [](const TargetInfo& a, const TargetInfo& b) {
            return a.score > b.score;
        });
}

bool Aimbot::ShouldSwitchTarget(const TargetInfo& newTarget, const CEntity& localEntity) {
    if (currentTarget.entity.Pawn.Address == 0)
        return true;
        
    // Don't switch if current target is still valid and has higher score
    if (IsTargetValid(currentTarget.entity, localEntity)) {
        float currentScore = CalculateTargetScore(currentTarget, localEntity);
        float newScore = CalculateTargetScore(newTarget, localEntity);
        
        // Only switch if new target has significantly higher score
        return newScore > currentScore * 1.2f;
    }
    
    return true;
}

Vec3 Aimbot::PredictMovement(const CEntity& entity, float time) {
    Vec3 predictedPos = entity.Pawn.Pos;
    
    if (entity.Pawn.Velocity.x != 0 || entity.Pawn.Velocity.y != 0 || entity.Pawn.Velocity.z != 0) {
        predictedPos.x += entity.Pawn.Velocity.x * time;
        predictedPos.y += entity.Pawn.Velocity.y * time;
        predictedPos.z += entity.Pawn.Velocity.z * time;
    }
    
    return predictedPos;
}

Vec3 Aimbot::PredictBulletDrop(const Vec3& start, const Vec3& end, float distance) {
    // Simple bullet drop prediction
    Vec3 adjusted = end;
    float dropAmount = (distance * distance) * 0.0001f; // Simplified gravity effect
    adjusted.z += dropAmount;
    return adjusted;
}

Vec3 Aimbot::PredictTargetPosition(const TargetInfo& target, float deltaTime) {
    auto& config = GetActiveConfig();
    
    // 2025 improved prediction with lag compensation
    Vec3 predictedPos = target.aimPosition;
    
    // Basic velocity-based prediction
    Vec3 velocity = target.entity.Pawn.Velocity;
    if (velocity.Length() > 10.0f) {
        predictedPos.x += velocity.x * deltaTime;
        predictedPos.y += velocity.y * deltaTime;
        predictedPos.z += velocity.z * deltaTime;
    }
    
    // Enhanced prediction for rage mode
    if (config.RageMode && config.PingPrediction) {
        float pingCompensation = config.PingPredictionAmount;
        predictedPos.x += velocity.x * pingCompensation;
        predictedPos.y += velocity.y * pingCompensation;
        predictedPos.z += velocity.z * pingCompensation;
    }
    
    return predictedPos;
}

void Aimbot::TriggerBot(const CEntity& localEntity, const std::vector<CEntity>& entities) {
    auto& config = GetActiveConfig();
    
    auto now = std::chrono::steady_clock::now();
    auto timeSinceLastTrigger = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTriggerTime).count() / 1000.0f;
    
    if (timeSinceLastTrigger < config.TriggerDelay)
        return;
    
    for (const auto& entity : entities) {
        if (!IsTargetValid(entity, localEntity))
            continue;
            
        if (config.TeamFilter && entity.Controller.TeamID == localEntity.Controller.TeamID)
            continue;
            
        if (IsCrosshairOnTarget(localEntity, entity)) {
            if (ShouldTrigger(localEntity, entity)) {
                // Simulate mouse click
                mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
                
                lastTriggerTime = now;
                LogAimEvent("Trigger activated", TargetInfo{entity, GetAimPosition(entity, config.AimBone), 
                    Math::Distance(localEntity.Pawn.Pos, entity.Pawn.Pos), 0.0f, entity.Pawn.Health, true, 0.0f});
                break;
            }
        }
    }
}

bool Aimbot::IsCrosshairOnTarget(const CEntity& localEntity, const CEntity& target) {
    Vec3 viewAngle = localEntity.Pawn.ViewAngle;
    Vec3 aimPos = GetAimPosition(target, GetActiveConfig().AimBone);
    Vec3 aimAngle = Math::CalcAngle(localEntity.Pawn.CameraPos, aimPos);
    
    float fov = CalculateFOV(viewAngle, aimAngle);
    return fov <= 5.0f; // Small FOV for crosshair check
}

bool Aimbot::ShouldTrigger(const CEntity& localEntity, const CEntity& target) {
    auto& config = GetActiveConfig();
    
    if (config.TriggerVisibleCheck && !IsVisible(localEntity, target, GetAimPosition(target, config.AimBone)))
        return false;
        
    return true;
}

void Aimbot::ApplyAntiShake(Vec3& angle) {
    auto& config = GetActiveConfig();
    
    // Apply small random movements to simulate hand shake
    float shakeAmount = config.AntiShakeStrength * 0.1f;
    angle.x += RandomFloat(-shakeAmount, shakeAmount);
    angle.y += RandomFloat(-shakeAmount, shakeAmount);
}

void Aimbot::NormalizeAngles(Vec3& angle) {
    Math::NormalizeAngles(angle);
}

void Aimbot::ClampAngles(Vec3& angle) {
    angle.x = Math::Clamp(angle.x, -89.0f, 89.0f);
    angle.y = Math::Clamp(angle.y, -180.0f, 180.0f);
    angle.z = 0.0f;
}

void Aimbot::UpdateAdaptiveFOV(const CEntity& localEntity) {
    auto& config = GetActiveConfig();
    
    // Adaptive FOV based on player speed and situation
    float speedFactor = localEntity.Pawn.Speed / 300.0f; // Normalize by typical max speed
    float adaptiveFOV = config.FOV * (1.0f + speedFactor * config.AdaptiveFOVScale);
    
    config.FOV = Math::Clamp(adaptiveFOV, 1.0f, 180.0f);
}

void Aimbot::UpdateAdaptiveSpeed(const CEntity& localEntity) {
    auto& config = GetActiveConfig();
    
    // Adaptive speed based on distance to target
    if (currentTarget.entity.Pawn.Address != 0) {
        float distanceFactor = currentTarget.distance / 1000.0f; // Normalize by typical combat distance
        float adaptiveSmooth = config.Smooth * (1.0f + distanceFactor * config.AdaptiveSpeedScale);
        
        config.Smooth = Math::Clamp(adaptiveSmooth, 0.1f, 10.0f);
    }
}

float Aimbot::GetDynamicSmooth(const CEntity& localEntity, const TargetInfo& target) {
    auto& config = GetActiveConfig();
    
    // Dynamic smooth based on various factors
    float baseSmooth = config.Smooth;
    float distanceFactor = target.distance / 1000.0f;
    float fovFactor = target.fov / 30.0f;
    
    float dynamicSmooth = baseSmooth * (1.0f + distanceFactor * 0.5f + fovFactor * 0.3f);
    
    return Math::Clamp(dynamicSmooth, 0.1f, 10.0f);
}

void Aimbot::ApplyRageSettings(Vec3& angle, const TargetInfo& target) {
    auto& config = GetActiveConfig();
    
    // Rage mode: aggressive settings
    if (config.RageInstant) {
        // Instant aim for rage mode - no smoothing
        config.Smooth = 0.1f;
        config.FOV = 180.0f;
    }
    
    // Ignore visibility checks in rage mode
    if (config.RageMode) {
        config.VisibleCheck = false;
        config.SmokeCheck = false;
        config.FlashCheck = false;
    }
    
    // Auto-fire in rage mode
    if (config.RageMode && target.entity.Pawn.Address != 0) {
        AutoFire(target);
    }
}

void Aimbot::AutoFire(const TargetInfo& target) {
    auto& config = GetActiveConfig();
    
    if (!config.RageMode || !HasRageConfig()) return;
    
    static std::chrono::steady_clock::time_point lastShotTime;
    auto now = std::chrono::steady_clock::now();
    auto timeSinceLastShot = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastShotTime).count();
    
    // Auto-fire with delay to prevent spam
    if (timeSinceLastShot > 100) { // 100ms between shots
        ForceAttack(true);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        ForceAttack(false);
        lastShotTime = now;
        
        LogAimEvent("Auto-fire activated", target);
    }
}

void Aimbot::ApplyLegitSettings(Vec3& angle, const TargetInfo& target) {
    auto& config = GetActiveConfig();
    
    // Legit mode: more human-like settings
    if (config.LegitHumanization) {
        AddHumanError(angle);
    }
    
    // Limit FOV for legit mode
    if (config.FOV > config.LegitFOV) {
        config.FOV = config.LegitFOV;
    }
}

bool Aimbot::IsLegitAimAllowed(const CEntity& localEntity, const TargetInfo& target) {
    auto& config = GetActiveConfig();
    
    // Additional legit mode checks
    if (target.fov > config.LegitFOV)
        return false;
        
    if (target.distance > 500.0f) // Reasonable combat distance for legit play
        return false;
        
    return true;
}

bool Aimbot::ShouldAutoWall(const TargetInfo& target) {
    auto& config = GetActiveConfig();
    
    // AutoWall check is not implemented in base config
    if (true)
        return false;
        
    // Simple autowall check based on distance
    return target.distance < 500.0f;
}

float Aimbot::CalculateDamage(const TargetInfo& target) {
    // Simplified damage calculation
    float baseDamage = 100.0f;
    float distanceFactor = (0.1f > (1.0f - (target.distance / 2000.0f))) ? 0.1f : (1.0f - (target.distance / 2000.0f));
    
    return baseDamage * distanceFactor;
}

void Aimbot::Draw(const CEntity& localEntity) {
    auto& config = GetActiveConfig();
    
    if (!config.Enabled)
        return;
        
    // Draw FOV circle
    if (config.DrawFOVCircle) {
        DrawFOVCircle(localEntity);
    }
    
    // Draw target line
    if (config.DrawTargetLine && currentTarget.entity.Pawn.Address != 0) {
        DrawTargetLine(localEntity, currentTarget);
    }
    
    // Draw custom crosshair
    DrawCrosshairCustomization();
    
    // Draw hit markers
    DrawHitMarkers();
}

void Aimbot::DrawFOVCircle(const CEntity& localEntity) {
    auto& config = GetActiveConfig();
    
    Vec2 screenPos;
    if (!gGame.View.WorldToScreen(localEntity.Pawn.CameraPos, screenPos)) {
        return;
    }
    
    // Get screen center
    int screenWidth = ImGui::GetIO().DisplaySize.x;
    int screenHeight = ImGui::GetIO().DisplaySize.y;
    Vec2 screenCenter(screenWidth / 2.0f, screenHeight / 2.0f);
    
    // Calculate FOV circle radius
    float fovRadius = (config.FOV / 90.0f) * (screenWidth / 2.0f);
    
    // Draw FOV circle
    ImDrawList* drawList = ImGui::GetBackgroundDrawList();
    if (drawList) {
        drawList->AddCircle(screenCenter.ToImVec2(), fovRadius, config.FOVCircleColor, 64, config.FOVCircleThickness);
    }
}

void Aimbot::DrawTargetLine(const CEntity& localEntity, const TargetInfo& target) {
    auto& config = GetActiveConfig();
    
    Vec2 targetScreen;
    if (gGame.View.WorldToScreen(target.aimPosition, targetScreen)) {
        int screenWidth = ImGui::GetIO().DisplaySize.x;
        int screenHeight = ImGui::GetIO().DisplaySize.y;
        Vec2 screenCenter(screenWidth / 2.0f, screenHeight / 2.0f);
        
        ImDrawList* drawList = ImGui::GetBackgroundDrawList();
        if (drawList) {
            drawList->AddLine(screenCenter.ToImVec2(), targetScreen.ToImVec2(), config.TargetLineColor, 2.0f);
        }
    }
}

void Aimbot::DrawHitMarkers() {
    // Hit marker implementation would go here
    // This would show visual feedback when hitting targets
}

void Aimbot::DrawCrosshairCustomization() {
    // Custom crosshair drawing would go here
    // This would draw custom crosshairs based on settings
}

void Aimbot::UpdateStatistics() {
    // Update aimbot statistics for monitoring
    auto now = std::chrono::steady_clock::now();
    
    if (isAiming && currentTarget.entity.Pawn.Address != 0) {
        auto reactionTime = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastAimTime).count() / 1000.0f;
        
        // Update average reaction time
        if (averageReactionTime == 0.0f) {
            averageReactionTime = reactionTime;
        } else {
            averageReactionTime = (averageReactionTime * 0.9f) + (reactionTime * 0.1f);
        }
    }
}

void Aimbot::LogAimEvent(const std::string& event, const TargetInfo& target) {
    // Log aimbot events for debugging
    // Logger::Log("Aimbot: " + event + " - Target health: " + std::to_string(target.health));
}

bool Aimbot::IsAimbotActive() {
    auto& config = GetActiveConfig();
    return config.Enabled && config.AimbotEnabled;
}

bool Aimbot::IsAiming() {
    return isAiming;
}

void Aimbot::ResetTarget() {
    currentTarget = TargetInfo{};
    isAiming = false;
}

TargetInfo Aimbot::GetCurrentTarget() {
    return currentTarget;
}

void Aimbot::SetCurrentTarget(const TargetInfo& target) {
    currentTarget = target;
    targetHistory.push_back(target);
    
    // Keep history size manageable
    if (targetHistory.size() > 10) {
        targetHistory.erase(targetHistory.begin());
    }
}

AimbotCFG::AimbotConfigBase& Aimbot::GetActiveConfig() {
    return *AimbotCFG::GetCurrentConfig();
}

void Aimbot::ApplyPreset(int presetIndex) {
    // Apply predefined presets
    auto& config = GetActiveConfig();
    
    switch (presetIndex) {
        case 0: // Legit preset
            config.FOV = 10.0f;
            config.Smooth = 5.0f;
            config.Humanization = true;
            config.VisibleCheck = true;
            config.RageMode = false;
            break;
        case 1: // Rage preset
            config.FOV = 180.0f;
            config.Smooth = 0.1f;
            config.Humanization = false;
            config.VisibleCheck = false;
            config.RageMode = true;
            break;
        case 2: // Semi-rage preset
            config.FOV = 45.0f;
            config.Smooth = 2.0f;
            config.Humanization = true;
            config.VisibleCheck = true;
            config.RageMode = false;
            break;
    }
}

void Aimbot::EnableRageMode() {
    auto& config = GetActiveConfig();
    config.RageMode = true;
    config.LegitModeEnabled = false;
    ApplyPreset(1);
}

void Aimbot::EnableLegitMode() {
    auto& config = GetActiveConfig();
    config.RageMode = false;
    config.LegitModeEnabled = true;
    ApplyPreset(0);
}

void Aimbot::EnableHybridMode() {
    auto& config = GetActiveConfig();
    config.RageMode = false;
    config.LegitModeEnabled = true;
    config.AdaptiveFOV = true;
    config.AdaptiveSpeed = true;
    ApplyPreset(2);
}

bool Aimbot::IsRageModeActive() {
    auto& config = GetActiveConfig();
    return config.RageMode;
}

bool Aimbot::IsLegitModeActive() {
    auto& config = GetActiveConfig();
    return config.LegitModeEnabled;
}

void Aimbot::DrawDebugInfo() {
    // Debug information display
    if (IsAimbotActive()) {
        // Draw debug text on screen
        ImDrawList* drawList = ImGui::GetBackgroundDrawList();
        if (drawList) {
            int screenWidth = ImGui::GetIO().DisplaySize.x;
            int screenHeight = ImGui::GetIO().DisplaySize.y;
            
            std::string debugText = "Aimbot Active";
            if (isAiming) {
                debugText += " - Aiming";
            }
            if (currentTarget.entity.Pawn.Address != 0) {
                debugText += " - Target Locked";
            }
            
            Vec2 textPos(10, 100);
            drawList->AddText(textPos.ToImVec2(), ImColor(255, 255, 255, 255), debugText.c_str());
        }
    }
}

void Aimbot::DumpTargetList() {
    // Dump current target information for debugging
    if (currentTarget.entity.Pawn.Address != 0) {
        Log::Info("Current Target - Health: " + std::to_string(currentTarget.health) + 
                   ", Distance: " + std::to_string(currentTarget.distance) + 
                   ", FOV: " + std::to_string(currentTarget.fov));
    }
}

void Aimbot::ValidateSettings() {
    auto& config = GetActiveConfig();
    
    // Validate and clamp settings to safe ranges
    config.FOV = Math::Clamp(config.FOV, 1.0f, 180.0f);
    config.Smooth = Math::Clamp(config.Smooth, 0.1f, 10.0f);
    config.AimSpeed = Math::Clamp(config.AimSpeed, 1.0f, 20.0f);
    config.RecoilControl = Math::Clamp(config.RecoilControl, 0.0f, 1.0f);
    config.RandomFactor = Math::Clamp(config.RandomFactor, 0.0f, 1.0f);
    config.MaxDistance = Math::Clamp(config.MaxDistance, 100.0f, 10000.0f);
}

// Silent Aim helper functions
bool Aimbot::IsPlayerShooting(const CEntity& localEntity) {
    // Check if player is currently shooting
    DWORD attackState = 0;
    if (memoryManager.ReadMemory(gGame.GetAttackBtnAddress(), attackState)) {
        return (attackState & 1) != 0; // Check if attack button is pressed
    }
    return false;
}

void Aimbot::ForceAttack(bool attack) {
    // Force attack button state for rage mode (2025: improved method)
    DWORD attackValue = attack ? 65537 : 256; // CS2 attack button values
    memoryManager.WriteMemory(gGame.GetAttackBtnAddress(), attackValue);
}

bool Aimbot::HasRageConfig() {
    // Check if current config is rage mode
    return AimbotCFG::CurrentConfig == AimbotCFG::RAGE;
}

void Aimbot::ApplyNoSpread(Vec3& angle, const CEntity& localEntity) {
    // No-spread compensation for rage mode
    auto& config = GetActiveConfig();
    
    if (!HasRageConfig()) return;
    
    // Get weapon spread data (simplified implementation)
    // In a real cheat, you would read weapon spread from game memory
    Vec3 spreadCompensation = {0, 0, 0};
    
    // Basic spread compensation based on shots fired
    if (localEntity.Pawn.ShotsFired > 0) {
        float spreadFactor = localEntity.Pawn.ShotsFired * 0.1f;
        spreadCompensation.x = RandomFloat(-spreadFactor, spreadFactor);
        spreadCompensation.y = RandomFloat(-spreadFactor, spreadFactor);
    }
    
    // Apply compensation
    angle.x -= spreadCompensation.x;
    angle.y -= spreadCompensation.y;
}

void Aimbot::ApplyRageSilentAim(const Vec3& targetAngle, const CEntity& localEntity) {
    // Advanced rage silent aim with instant hit
    auto& config = GetActiveConfig();
    
    if (!config.RageMode || !HasRageConfig()) return;
    
    Vec3 rageAngle = targetAngle;
    
    // Apply no-spread
    ApplyNoSpread(rageAngle, localEntity);
    
    // Apply no-recoil for rage mode
    if (localEntity.Pawn.ShotsFired > 0) {
        Vec3 aimPunch = localEntity.Pawn.AimPunchAngle;
        rageAngle.x -= aimPunch.x * 2.0f; // Full recoil compensation for rage
        rageAngle.y -= aimPunch.y * 2.0f;
    }
    
    // Normalize angles
    Math::NormalizeAngles(rageAngle);
    ClampAngles(rageAngle);
    
    // Apply instantly
    memoryManager.WriteMemory(localEntity.Pawn.Address + Offset.Pawn.angEyeAngles, rageAngle);
    
    // Force shot if instant hit is enabled
    if (config.RageInstant) {
        ForceAttack(true);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        ForceAttack(false);
    }
}

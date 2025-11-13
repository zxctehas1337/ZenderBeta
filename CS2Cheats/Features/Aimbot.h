#pragma once
#include "../Game/Entity.h"
#include "../Core/Config.h"
#include "../Game/Bone.h"
#include "../Core/Globals.h"
#include "../Helpers/Math.h"
#include <chrono>
#include <random>
#include <vector>

struct TargetInfo {
    CEntity entity;
    Vec3 aimPosition;
    float distance;
    float fov;
    int health;
    bool isVisible;
    float score;
    Vec3 predictedPosition;
    float lastUpdateTime;
};

class Aimbot {
private:
    static TargetInfo currentTarget;
    static std::chrono::steady_clock::time_point lastAimTime;
    static std::chrono::steady_clock::time_point lastShotTime;
    static std::chrono::steady_clock::time_point lastTriggerTime;
    static std::mt19937 rng;
    static Vec3 lastAimAngle;
    static bool isAiming;
    static int targetSwitchCount;
    static float averageReactionTime;
    static std::vector<TargetInfo> targetHistory;

    // Core functions
    static Vec3 GetAimPosition(const CEntity& entity, int bone);
    static float CalculateFOV(const Vec3& viewAngle, const Vec3& aimAngle);
    static bool IsTargetValid(const CEntity& entity, const CEntity& localEntity);
    static bool IsVisible(const CEntity& from, const CEntity& to, const Vec3& aimPos);
    
    // Aim functions
    static void AimAtTarget(const Vec3& targetPos, const CEntity& localEntity);
    static Vec3 SmoothAim(const Vec3& currentAngle, const Vec3& targetAngle, float smoothFactor);
    static void ApplyRecoilControl(Vec3& angle, const CEntity& localEntity);
    static void ApplySilentAim(const Vec3& targetAngle, const CEntity& localEntity);
    
    // Humanization
    static Vec3 HumanizeAim(const Vec3& input);
    static float RandomFloat(float min, float max);
    static void AddHumanError(Vec3& angle);
    static Vec3 ApplyHumanCurve(const Vec3& input, float curveIntensity);
    static void AddDelay();
    
    // Target selection
    static TargetInfo FindBestTarget(const CEntity& localEntity, const std::vector<CEntity>& entities);
    static float CalculateTargetScore(const TargetInfo& target, const CEntity& localEntity);
    static void SortTargets(std::vector<TargetInfo>& targets, const CEntity& localEntity);
    static bool ShouldSwitchTarget(const TargetInfo& newTarget, const CEntity& localEntity);
    
    // Prediction
    static Vec3 PredictMovement(const CEntity& entity, float time);
    static Vec3 PredictBulletDrop(const Vec3& start, const Vec3& end, float distance);
    static Vec3 PredictTargetPosition(const TargetInfo& target, float deltaTime);
    
    // Trigger bot
    static void TriggerBot(const CEntity& localEntity, const std::vector<CEntity>& entities);
    static bool IsCrosshairOnTarget(const CEntity& localEntity, const CEntity& target);
    static bool ShouldTrigger(const CEntity& localEntity, const CEntity& target);
    
    // Anti-detection
    static void ApplyAntiShake(Vec3& angle);
    static void NormalizeAngles(Vec3& angle);
    static void ClampAngles(Vec3& angle);
    
    // Adaptive settings
    static void UpdateAdaptiveFOV(const CEntity& localEntity);
    static void UpdateAdaptiveSpeed(const CEntity& localEntity);
    static float GetDynamicSmooth(const CEntity& localEntity, const TargetInfo& target);
    
    // Rage mode features
    static void ApplyRageSettings(Vec3& angle, const TargetInfo& target);
    static bool ShouldAutoWall(const TargetInfo& target);
    static float CalculateDamage(const TargetInfo& target);
    
    // Legit mode features
    static void ApplyLegitSettings(Vec3& angle, const TargetInfo& target);
    static bool IsLegitAimAllowed(const CEntity& localEntity, const TargetInfo& target);
    
    // Visual functions
    static void DrawFOVCircle(const CEntity& localEntity);
    static void DrawTargetLine(const CEntity& localEntity, const TargetInfo& target);
    static void DrawHitMarkers();
    static void DrawCrosshairCustomization();
    
    // Statistics and monitoring
    static void UpdateStatistics();
    static void LogAimEvent(const std::string& event, const TargetInfo& target);
    static float GetAverageAccuracy();
    static int GetTotalShots();
    static int GetTotalHits();

public:
    // Main functions
    static void Run(const CEntity& localEntity, const std::vector<CEntity>& entities);
    static void Draw(const CEntity& localEntity);
    static void Initialize();
    static void Shutdown();
    static void Reset();
    
    // Utilities
    static bool IsAimbotActive();
    static bool IsAiming();
    static void ResetTarget();
    static TargetInfo GetCurrentTarget();
    static void SetCurrentTarget(const TargetInfo& target);
    
    // Configuration helpers
    static AimbotCFG::AimbotConfigBase& GetActiveConfig();
    static void ApplyPreset(int presetIndex);
    static void SaveCurrentSettings();
    static void LoadSettings();
    
    // Advanced features
    static void EnableRageMode();
    static void EnableLegitMode();
    static void EnableHybridMode();
    static bool IsRageModeActive();
    static bool IsLegitModeActive();
    
    // Debug functions
    static void DrawDebugInfo();
    static void DumpTargetList();
    static void ValidateSettings();
    
    // Silent Aim functions
    static bool IsPlayerShooting(const CEntity& localEntity);
    static void ForceAttack(bool attack);
    static bool HasRageConfig();
    static void ApplyNoSpread(Vec3& angle, const CEntity& localEntity);
    static void ApplyRageSilentAim(const Vec3& targetAngle, const CEntity& localEntity);
    static void AutoFire(const TargetInfo& target);
};

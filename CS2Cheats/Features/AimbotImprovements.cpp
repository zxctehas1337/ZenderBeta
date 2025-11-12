#include "AimbotImprovements.h"
#include "../Helpers/Logger.h"
#include <algorithm>
#include <chrono>
#include <cmath>

namespace AimbotImprovements
{
    // Dynamic FOV Implementation
    float DynamicFOV::CalculateDynamicFOV(const CEntity& localEntity, const CEntity& target, float baseFOV)
    {
        if (!settings.enabled)
            return baseFOV;

        float distance = localEntity.Pawn.Pos.DistanceTo(target.Pawn.Pos);
        float targetSpeed = target.Pawn.Speed;

        // Calculate dynamic FOV based on distance and speed
        float distanceFactor = 1.0f - (distance * settings.distanceMultiplier);
        float speedFactor = 1.0f - (targetSpeed * settings.speedMultiplier);
        
        float combinedFactor = (distanceFactor + speedFactor) / 2.0f;
        float dynamicFOV = baseFOV * combinedFactor;

        // Clamp to min/max values
        return std::clamp(dynamicFOV, settings.minFOV, settings.maxFOV);
    }

    void DynamicFOV::UpdateSettings()
    {
        // Settings can be updated from GUI
    }

    // Humanization Implementation
    Humanization::HumanizedAimData Humanization::CalculateHumanization(const CEntity& localEntity, const CEntity& target)
    {
        HumanizedAimData data;

        if (!settings.enabled) {
            data.reactionDelay = 0.0f;
            data.smoothFactor = 1.0f;
            data.mistakeOffset = {0, 0, 0};
            data.shouldApplyMistake = false;
            return data;
        }

        // Random reaction time
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_real_distribution<float> reactionDist(settings.reactionTimeMin, settings.reactionTimeMax);
        data.reactionDelay = reactionDist(gen);

        // Random smooth variation
        std::uniform_real_distribution<float> smoothDist(1.0f - settings.smoothVariation, 1.0f + settings.smoothVariation);
        data.smoothFactor = smoothDist(gen);

        // Random mistake
        std::uniform_real_distribution<float> mistakeDist(0.0f, 1.0f);
        data.shouldApplyMistake = mistakeDist(gen) < settings.mistakeChance;

        if (data.shouldApplyMistake) {
            std::uniform_real_distribution<float> angleDist(-settings.mistakeAmount, settings.mistakeAmount);
            data.mistakeOffset.x = angleDist(gen);
            data.mistakeOffset.y = angleDist(gen);
            data.mistakeOffset.z = 0;
        }

        return data;
    }

    void Humanization::UpdateSettings()
    {
        // Settings can be updated from GUI
    }

    // Multi-point Implementation
    std::vector<MultiPoint::PointData> MultiPoint::GetMultiPoints(const CEntity& entity, int bone)
    {
        std::vector<PointData> points;

        if (!settings.enabled) {
            // Return single point if multi-point is disabled
            PointData singlePoint;
            singlePoint.position = entity.GetBone().GetBonePosition(bone);
            singlePoint.score = 1.0f;
            singlePoint.visible = true;
            points.push_back(singlePoint);
            return points;
        }

        Vec3 centerPos = entity.GetBone().GetBonePosition(bone);
        
        // Generate grid of points around the center
        for (int x = -(settings.pointsX / 2); x <= (settings.pointsX / 2); x++) {
            for (int y = -(settings.pointsY / 2); y <= (settings.pointsY / 2); y++) {
                if (x == 0 && y == 0) continue; // Skip center point (will be added separately)

                PointData point;
                point.position = centerPos;
                point.position.x += x * settings.pointSpacing;
                point.position.y += y * settings.pointSpacing;
                
                // Calculate score based on distance from center
                float distance = std::sqrt(x * x + y * y);
                point.score = 1.0f / (1.0f + distance * 0.1f);
                
                // Visibility would need to be checked with actual trace function
                point.visible = true; // Placeholder
                
                points.push_back(point);
            }
        }

        // Add center point with highest priority if head prioritization is enabled
        if (settings.prioritizeHead && bone == HEAD) {
            PointData centerPoint;
            centerPoint.position = centerPos;
            centerPoint.score = 2.0f; // Higher score for head
            centerPoint.visible = true;
            points.insert(points.begin(), centerPoint);
        } else {
            PointData centerPoint;
            centerPoint.position = centerPos;
            centerPoint.score = 1.0f;
            centerPoint.visible = true;
            points.push_back(centerPoint);
        }

        return points;
    }

    MultiPoint::PointData MultiPoint::GetBestPoint(const CEntity& localEntity, const std::vector<PointData>& points)
    {
        if (points.empty()) {
            return PointData{{0, 0, 0}, 0.0f, false};
        }

        PointData bestPoint = points[0];
        
        for (const auto& point : points) {
            if (!point.visible) continue;
            
            // Calculate angle to point
            Vec3 angle = Aimbot::CalculateAngle(localEntity.Pawn.CameraPos, point.position);
            float fov = std::hypot(angle.x - localPlayer.Pawn.ViewAngle.x, 
                                  angle.y - localPlayer.Pawn.ViewAngle.y);
            
            // Adjust score based on FOV
            float adjustedScore = point.score / (1.0f + fov * 0.01f);
            
            if (adjustedScore > bestPoint.score || !bestPoint.visible) {
                bestPoint = point;
                bestPoint.score = adjustedScore;
            }
        }

        return bestPoint;
    }

    void MultiPoint::UpdateSettings()
    {
        // Settings can be updated from GUI
    }

    // Adaptive Smoothing Implementation
    float AdaptiveSmoothing::CalculateAdaptiveSmooth(const CEntity& localEntity, const CEntity& target, 
                                                     float currentAngle, float targetAngle)
    {
        if (!settings.enabled)
            return settings.baseSmooth;

        float distance = localEntity.Pawn.Pos.DistanceTo(target.Pawn.Pos);
        float targetSpeed = target.Pawn.Speed;
        float angleDiff = std::abs(targetAngle - currentAngle);

        // Calculate adaptive factors
        float distanceFactor = std::clamp(1.0f - (distance * settings.distanceFactor), 0.0f, 1.0f);
        float speedFactor = std::clamp(1.0f - (targetSpeed * settings.speedFactor), 0.0f, 1.0f);
        float angleFactor = std::clamp(1.0f - (angleDiff * settings.angleFactor / 180.0f), 0.0f, 1.0f);

        // Combine factors
        float adaptiveFactor = (distanceFactor + speedFactor + angleFactor) / 3.0f;
        float adaptiveSmooth = settings.baseSmooth * (1.0f + adaptiveFactor);

        return std::clamp(adaptiveSmooth, settings.minSmooth, settings.maxSmooth);
    }

    void AdaptiveSmoothing::UpdateSettings()
    {
        // Settings can be updated from GUI
    }

    // Advanced Prediction Implementation
    Vec3 AdvancedPrediction::PredictTargetPosition(const CEntity& target, float predictionTime)
    {
        if (!settings.enabled)
            return target.Pawn.Pos;

        Vec3 predictedPos = target.Pawn.Pos;
        
        // Basic velocity prediction
        predictedPos.x += target.Pawn.Velocity.x * predictionTime;
        predictedPos.y += target.Pawn.Velocity.y * predictionTime;
        predictedPos.z += target.Pawn.Velocity.z * predictionTime;

        // Apply gravity if enabled
        if (settings.useGravity) {
            predictedPos.z -= 0.5f * settings.gravityCompensation * predictionTime * predictionTime;
        }

        // Apply acceleration if enabled (simplified)
        if (settings.useAcceleration) {
            // This would need actual acceleration data from the game
            // For now, we'll use a simple deceleration model
            float deceleration = 0.95f; // 5% deceleration per second
            float speedFactor = std::pow(deceleration, predictionTime);
            predictedPos.x = target.Pawn.Pos.x + (predictedPos.x - target.Pawn.Pos.x) * speedFactor;
            predictedPos.y = target.Pawn.Pos.y + (predictedPos.y - target.Pawn.Pos.y) * speedFactor;
        }

        return predictedPos;
    }

    Vec3 AdvancedPrediction::ApplyBulletDrop(const Vec3& startPos, const Vec3& targetPos, float bulletSpeed)
    {
        if (!settings.useBulletDrop || bulletSpeed <= 0)
            return targetPos;

        Vec3 direction = targetPos - startPos;
        float distance = direction.Length();
        
        if (distance <= 0)
            return targetPos;

        // Normalize direction
        direction = direction / distance;

        // Calculate time to target
        float timeToTarget = distance / bulletSpeed;

        // Calculate bullet drop
        float bulletDrop = 0.5f * settings.gravityCompensation * timeToTarget * timeToTarget;

        // Apply drop to target position
        Vec3 adjustedPos = targetPos;
        adjustedPos.z += bulletDrop;

        return adjustedPos;
    }

    void AdvancedPrediction::UpdateSettings()
    {
        // Settings can be updated from GUI
    }

    // Main Integration
    void Initialize()
    {
        // Initialize all systems
        DynamicFOV::UpdateSettings();
        Humanization::UpdateSettings();
        MultiPoint::UpdateSettings();
        AdaptiveSmoothing::UpdateSettings();
        AdvancedPrediction::UpdateSettings();
    }

    void Update()
    {
        // Update all systems if needed
    }

    Vec3 ApplyImprovements(const CEntity& localEntity, const CEntity& target, 
                           const Vec3& targetPos, float& smoothFactor)
    {
        Vec3 improvedPos = targetPos;

        // Apply advanced prediction
        if (AdvancedPrediction::settings.enabled) {
            improvedPos = AdvancedPrediction::PredictTargetPosition(target, AdvancedPrediction::settings.predictionTime);
            improvedPos = AdvancedPrediction::ApplyBulletDrop(localEntity.Pawn.CameraPos, improvedPos);
        }

        // Apply adaptive smoothing
        if (AdaptiveSmoothing::settings.enabled) {
            Vec3 currentAngle = localEntity.Pawn.ViewAngle;
            Vec3 targetAngle = Aimbot::CalculateAngle(localEntity.Pawn.CameraPos, improvedPos);
            smoothFactor = AdaptiveSmoothing::CalculateAdaptiveSmooth(localEntity, target, 
                                                                      currentAngle.x, targetAngle.x);
        }

        // Apply humanization
        if (Humanization::settings.enabled) {
            Humanization::HumanizedAimData humanData = Humanization::CalculateHumanization(localEntity, target);
            
            // Apply reaction delay (this would need to be handled in the main aimbot loop)
            // Apply smooth variation
            smoothFactor *= humanData.smoothFactor;
            
            // Apply mistake offset
            if (humanData.shouldApplyMistake) {
                Vec3 angle = Aimbot::CalculateAngle(localEntity.Pawn.CameraPos, improvedPos);
                angle.x += humanData.mistakeOffset.x;
                angle.y += humanData.mistakeOffset.y;
                
                // Convert back to position
                float distance = improvedPos.DistanceTo(localEntity.Pawn.CameraPos);
                float pitch = angle.x * 3.14159265f / 180.0f;
                float yaw = angle.y * 3.14159265f / 180.0f;
                
                improvedPos.x = localEntity.Pawn.CameraPos.x + distance * std::cos(pitch) * std::cos(yaw);
                improvedPos.y = localEntity.Pawn.CameraPos.y + distance * std::cos(pitch) * std::sin(yaw);
                improvedPos.z = localEntity.Pawn.CameraPos.z + distance * std::sin(pitch);
            }
        }

        return improvedPos;
    }

    void RenderDebugInfo()
    {
        // Render debug information for all improvement systems
        // This would show current FOV, smoothing values, prediction data, etc.
    }
}

#pragma once
#include "../Core/Render.h"
#include "../Core/Config.h"
#include "../Game/Entity.h"
#include <vector>
#include <random>

namespace AimbotImprovements
{
    // Dynamic FOV System
    namespace DynamicFOV {
        struct DynamicFOVSettings {
            bool enabled = false;
            float baseFOV = 120.0f;
            float minFOV = 30.0f;
            float maxFOV = 180.0f;
            float distanceMultiplier = 0.01f; // How much FOV changes with distance
            float speedMultiplier = 0.5f; // How much FOV changes with target speed
        };

        inline DynamicFOVSettings settings;

        float CalculateDynamicFOV(const CEntity& localEntity, const CEntity& target, float baseFOV);
        void UpdateSettings();
    }

    // Humanization System
    namespace Humanization {
        struct HumanizationSettings {
            bool enabled = false;
            float reactionTimeMin = 0.1f; // Minimum reaction time in seconds
            float reactionTimeMax = 0.3f; // Maximum reaction time in seconds
            float mistakeChance = 0.05f; // 5% chance to make a "mistake"
            float mistakeAmount = 15.0f; // How many degrees the mistake can be
            float smoothVariation = 0.2f; // Variation in smoothing
            float aimSpeedVariation = 0.3f; // Variation in aim speed
        };

        inline HumanizationSettings settings;

        struct HumanizedAimData {
            float reactionDelay;
            float smoothFactor;
            Vec3 mistakeOffset;
            bool shouldApplyMistake;
        };

        HumanizedAimData CalculateHumanization(const CEntity& localEntity, const CEntity& target);
        void UpdateSettings();
    }

    // Multi-point Targeting
    namespace MultiPoint {
        struct MultiPointSettings {
            bool enabled = false;
            int pointsX = 3; // Number of points horizontally
            int pointsY = 3; // Number of points vertically
            float pointSpacing = 2.0f; // Spacing between points in units
            bool prioritizeHead = true;
        };

        inline MultiPointSettings settings;

        struct PointData {
            Vec3 position;
            float score;
            bool visible;
        };

        std::vector<PointData> GetMultiPoints(const CEntity& entity, int bone);
        PointData GetBestPoint(const CEntity& localEntity, const std::vector<PointData>& points);
        void UpdateSettings();
    }

    // Adaptive Smoothing
    namespace AdaptiveSmoothing {
        struct AdaptiveSmoothingSettings {
            bool enabled = false;
            float baseSmooth = 0.5f;
            float minSmooth = 0.1f;
            float maxSmooth = 0.9f;
            float distanceFactor = 0.3f; // How much distance affects smoothing
            float speedFactor = 0.4f; // How much target speed affects smoothing
            float angleFactor = 0.3f; // How much angle difference affects smoothing
        };

        inline AdaptiveSmoothingSettings settings;

        float CalculateAdaptiveSmooth(const CEntity& localEntity, const CEntity& target, 
                                     float currentAngle, float targetAngle);
        void UpdateSettings();
    }

    // Advanced Prediction
    namespace AdvancedPrediction {
        struct PredictionSettings {
            bool enabled = false;
            bool useAcceleration = true;
            bool useGravity = true;
            bool useBulletDrop = true;
            float predictionTime = 0.1f;
            float gravityCompensation = 800.0f; // CS2 gravity
        };

        inline PredictionSettings settings;

        Vec3 PredictTargetPosition(const CEntity& target, float predictionTime);
        Vec3 ApplyBulletDrop(const Vec3& startPos, const Vec3& targetPos, float bulletSpeed = 1000.0f);
        void UpdateSettings();
    }

    // Main Integration
    void Initialize();
    void Update();
    Vec3 ApplyImprovements(const CEntity& localEntity, const CEntity& target, 
                           const Vec3& targetPos, float& smoothFactor);
    void RenderDebugInfo();
}

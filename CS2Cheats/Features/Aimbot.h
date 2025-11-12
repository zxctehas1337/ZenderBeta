#pragma once
#include "../Core/Config.h"
#include "../Game/Game.h"
#include "../Game/Bone.h"
#include "../Game/Entity.h"
#include "../Helpers/Math.h"
#include <Windows.h>
#include <cmath>

// Forward declaration for EntityResult
struct EntityResult;

namespace Aimbot
{
    // Bone mapping for different body parts
    enum class AimBone : int
    {
        Head = 0,
        Neck = 1,
        Chest = 2,
        Stomach = 3,
        Pelvis = 4,
        Auto = 5  // Automatically choose best bone
    };

    // Target information structure
    struct TargetInfo
    {
        CEntity entity;
        Vec3 headPos;
        Vec3 bodyPos;
        float distance;
        float fov;
        bool isVisible;
        AimBone bestBone;
    };

    // AutoFire modes
    enum class AutoFireMode : int
    {
        Off = 0,
        OnSight = 1,    // Fire when enemy is in sight
        AutoAim = 2     // Fire when aiming at enemy
    };

    // Internal state
    static bool isAimbotActive = false;
    static bool wasKeyPressed = false;
    static TargetInfo currentTarget;
    static DWORD64 lastShootTime = 0;
    static const DWORD SHOOT_DELAY = 100; // 100ms between shots

    // Helper functions
    Vec3 GetBonePosition(const CEntity& entity, AimBone bone)
    {
        Vec3 result = entity.Pawn.Pos;
        const CBone& boneData = entity.Pawn.BoneData;
        
        // Get bone index based on the requested bone
        int boneIndex = -1;
        switch (bone)
        {
        case AimBone::Head:
            boneIndex = BONEINDEX::head;
            break;
        case AimBone::Neck:
            boneIndex = BONEINDEX::neck_0;
            break;
        case AimBone::Chest:
            boneIndex = BONEINDEX::spine_2; // Using spine_2 as chest
            break;
        case AimBone::Stomach:
            boneIndex = BONEINDEX::spine_0; // Using spine_0 as stomach
            break;
        case AimBone::Pelvis:
            boneIndex = BONEINDEX::pelvis;
            break;
        case AimBone::Auto:
            // Try head first, then chest, then stomach
            if (boneData.BonePosList.size() > BONEINDEX::head)
                result = boneData.BonePosList[BONEINDEX::head].Pos;
            else if (boneData.BonePosList.size() > BONEINDEX::spine_2)
                result = boneData.BonePosList[BONEINDEX::spine_2].Pos;
            else if (boneData.BonePosList.size() > BONEINDEX::spine_0)
                result = boneData.BonePosList[BONEINDEX::spine_0].Pos;
            return result;
        }
        
        // Get the bone position if the index is valid
        if (boneIndex != -1 && boneData.BonePosList.size() > boneIndex)
        {
            result = boneData.BonePosList[boneIndex].Pos;
        }

        return result;
    }

    float CalculateFOV(const Vec2& viewAngle, const Vec3& targetPos, const Vec3& localPos)
    {
        Vec3 aimAngle = Math::CalcAngle(localPos, targetPos);
        Vec2 deltaAngle = Vec2(aimAngle.x - viewAngle.x, aimAngle.y - viewAngle.y);
        
        // Normalize angles
        while (deltaAngle.x > 180.0f) deltaAngle.x -= 360.0f;
        while (deltaAngle.x < -180.0f) deltaAngle.x += 360.0f;
        while (deltaAngle.y > 180.0f) deltaAngle.y -= 360.0f;
        while (deltaAngle.y < -180.0f) deltaAngle.y += 360.0f;
        
        return sqrtf(deltaAngle.x * deltaAngle.x + deltaAngle.y * deltaAngle.y);
    }

    bool IsTargetValid(const CEntity& entity, const CEntity& localEntity, const Vec3& localPos, const Vec2& viewAngle)
    {
        // Team check
        if (AimbotCFG::TeamCheck && entity.Controller.TeamID == localEntity.Controller.TeamID)
            return false;

        // Distance check
        float distance = entity.Pawn.Pos.DistanceTo(localPos);
        if (distance > AimbotCFG::MaxDistance * 100.0f)  // Convert to game units
            return false;

        // FOV check
        Vec3 targetPos = GetBonePosition(entity, Aimbot::AimBone::Head);
        float fov = CalculateFOV(viewAngle, targetPos, localPos);
        if (fov > AimbotCFG::FOVRadius)
            return false;

        // Visible check
        if (AimbotCFG::VisibleCheck && entity.Pawn.bSpottedByMask == 0)
            return false;

        return true;
    }

    AimBone GetBestBone(const CEntity& entity)
    {
        if (AimbotCFG::TargetHeadOnly || AimbotCFG::HeadOnly)
            return AimBone::Head;

        // Use configured bone selection
        switch (AimbotCFG::AimBone)
        {
        case 0: return AimBone::Head;
        case 1: return AimBone::Neck;
        case 2: return AimBone::Chest;
        case 3: return AimBone::Stomach;
        case 4: return AimBone::Pelvis;
        case 5: return AimBone::Auto;
        default: return AimBone::Head;
        }
    }

    TargetInfo FindBestTarget(const CEntity& localEntity, const std::vector<EntityResult>& entities)
    {
        TargetInfo bestTarget{};
        bestTarget.fov = 999999.0f;
        
        Vec3 localPos = localEntity.Pawn.Pos;
        Vec3 viewAngle = Vec3(localEntity.Pawn.ViewAngle.x, localEntity.Pawn.ViewAngle.y, 0.0f);

        for (const auto& result : entities)
        {
            if (!result.isValid || !result.entity.IsAlive())
                continue;

            const CEntity& entity = result.entity;

            Vec2 viewAngle2D(viewAngle.x, viewAngle.y);
            if (!IsTargetValid(entity, localEntity, localPos, viewAngle2D))
                continue;

            // Get target positions for different bones
            Vec3 headPos = GetBonePosition(entity, AimBone::Head);
            Vec3 bodyPos = GetBonePosition(entity, AimBone::Chest);
            
            float distance = entity.Pawn.Pos.DistanceTo(localPos);
            Vec2 viewAngle2D2(viewAngle.x, viewAngle.y);
            float headFOV = CalculateFOV(viewAngle2D2, headPos, localPos);
            float bodyFOV = CalculateFOV(viewAngle2D2, bodyPos, localPos);
            
            // Check if we should stop when no head is in radius
            if (AimbotCFG::StopWhenNoHead && headFOV > AimbotCFG::FOVRadius)
                continue;
            
            // Choose best bone based on FOV and settings
            float bestFOV = headFOV;
            AimBone bestBone = AimBone::Head;
            
            if (bodyFOV < bestFOV && !AimbotCFG::HeadOnly && !AimbotCFG::TargetHeadOnly)
            {
                bestFOV = bodyFOV;
                bestBone = AimBone::Chest;
            }

            // Update best target if this one is better
            if (bestFOV < bestTarget.fov)
            {
                bestTarget.entity = entity;
                bestTarget.headPos = headPos;
                bestTarget.bodyPos = bodyPos;
                bestTarget.distance = distance;
                bestTarget.fov = bestFOV;
                bestTarget.isVisible = (entity.Pawn.bSpottedByMask != 0);
                bestTarget.bestBone = bestBone;
            }
        }

        return bestTarget;
    }

    void AimAtTarget(const CEntity& localEntity, const TargetInfo& target)
    {
        if (target.entity.Controller.Address == 0)
            return;

        Vec3 targetPos = GetBonePosition(target.entity, target.bestBone);
        
        // Predict movement if enabled
        if (AimbotCFG::PredictMovement && g_globalVars)
        {
            Vec3 targetVelocity = {0, 0, 0};
            float interval = 1.0f / 64.0f * 2.0f;
            targetPos.x += targetVelocity.x * interval;
            targetPos.y += targetVelocity.y * interval;
            targetPos.z += targetVelocity.z * interval;
        }

        Vec3 localPos = localEntity.Pawn.Pos;
        Vec3 aimAngle = Math::CalcAngle(localPos, targetPos);

        // Apply RCS if enabled
        if (AimbotCFG::RCS)
        {
            Vec2 aimPunch = localEntity.Pawn.AimPunchAngle;
            aimAngle.x -= aimPunch.x * AimbotCFG::RCSStrength;
            aimAngle.y -= aimPunch.y * AimbotCFG::RCSStrength;
        }

        // Smooth aim if enabled
        if (AimbotCFG::SmoothAim)
        {
            Vec3 currentAngle = Vec3(localEntity.Pawn.ViewAngle.x, localEntity.Pawn.ViewAngle.y, 0.0f);
            Vec3 deltaAngle;
            deltaAngle.x = aimAngle.x - currentAngle.x;
            deltaAngle.y = aimAngle.y - currentAngle.y;
            deltaAngle.z = aimAngle.z - currentAngle.z;
            
            // Normalize delta
            while (deltaAngle.x > 180.0f) deltaAngle.x -= 360.0f;
            while (deltaAngle.x < -180.0f) deltaAngle.x += 360.0f;
            while (deltaAngle.y > 180.0f) deltaAngle.y -= 360.0f;
            while (deltaAngle.y < -180.0f) deltaAngle.y += 360.0f;
            
            // Apply smoothing
            float smoothFactor = 1.0f - AimbotCFG::SmoothValue;
            aimAngle.x = currentAngle.x + deltaAngle.x * smoothFactor;
            aimAngle.y = currentAngle.y + deltaAngle.y * smoothFactor;
        }

        // Apply aim angles - both silent and regular aim write to the same address
        memoryManager.WriteMemory(localEntity.Pawn.Address + Offset.Pawn.angEyeAngles, aimAngle);
    }

    void AutoFire(const CEntity& localEntity, const TargetInfo& target)
    {
        if (AimbotCFG::AutoFireMode == static_cast<int>(AutoFireMode::Off))
            return;

        DWORD currentTime = GetTickCount64();
        if (currentTime - lastShootTime < SHOOT_DELAY)
            return;

        bool shouldFire = false;

        if (AimbotCFG::AutoFireMode == static_cast<int>(AutoFireMode::OnSight))
        {
            // Fire when enemy is visible and in FOV
            shouldFire = target.isVisible && target.fov <= AimbotCFG::FOVRadius;
        }
        else if (AimbotCFG::AutoFireMode == static_cast<int>(AutoFireMode::AutoAim))
        {
            // Fire when aiming at enemy
            shouldFire = target.entity.Controller.Address != 0 && target.fov <= 5.0f;
        }

        if (shouldFire)
        {
            // Simulate mouse click
            mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
            Sleep(10);
            mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
            lastShootTime = currentTime;
        }
    }

    void Run(const CEntity& localEntity, const std::vector<EntityResult>& entities)
    {
        if (!AimbotCFG::AimbotEnabled)
        {
            currentTarget = {};
            return;
        }

        // Check aimbot key
        bool keyPressed = (GetAsyncKeyState(AimbotCFG::AimbotKey) & 0x8000) != 0;
        
        // Toggle mode handling
        if (AimbotCFG::ToggleMode)
        {
            if (keyPressed && !wasKeyPressed)
            {
                isAimbotActive = !isAimbotActive;
            }
        }
        else
        {
            isAimbotActive = keyPressed;
        }
        
        wasKeyPressed = keyPressed;

        if (!isAimbotActive)
        {
            currentTarget = {};
            return;
        }

        // Find best target
        TargetInfo newTarget = FindBestTarget(localEntity, entities);
        
        // Update current target if we found a better one or lost the old one
        if (newTarget.entity.Controller.Address != 0 && 
            (currentTarget.entity.Controller.Address == 0 || newTarget.fov < currentTarget.fov))
        {
            currentTarget = newTarget;
        }
        else if (currentTarget.entity.Controller.Address != 0)
        {
            // Check if current target is still valid
            Vec2 viewAngle2D(localEntity.Pawn.ViewAngle.x, localEntity.Pawn.ViewAngle.y);
            if (!IsTargetValid(currentTarget.entity, localEntity, localEntity.Pawn.Pos, viewAngle2D))
            {
                currentTarget = {};
            }
        }

        // Aim at current target
        if (currentTarget.entity.Controller.Address != 0)
        {
            AimAtTarget(localEntity, currentTarget);
            AutoFire(localEntity, currentTarget);
        }
    }

    void DrawFOV(const CEntity& localEntity)
    {
        if (!AimbotCFG::ShowFov || !AimbotCFG::AimbotEnabled)
            return;

        if (localEntity.Controller.TeamID == 0)
            return;

        ImDrawList* drawList = ImGui::GetBackgroundDrawList();
        if (!drawList)
            return;

        // Get screen center
        ImVec2 screenSize = ImGui::GetIO().DisplaySize;
        ImVec2 screenCenter(screenSize.x / 2.0f, screenSize.y / 2.0f);

        // Calculate FOV circle radius based on game FOV
        float fovPixels = (screenSize.y / 90.0f) * AimbotCFG::FOVRadius;

        // Draw FOV circle
        ImColor fovColor = ImColor(1.0f, 0.0f, 0.0f, 0.5f);
        drawList->AddCircle(screenCenter, fovPixels, fovColor, 64, AimbotCFG::FOVCircleThickness);

        // Draw filled circle with transparency if DrawAimbotFOV is enabled
        if (AimbotCFG::DrawAimbotFOV)
        {
            ImColor fovFillColor = ImColor(AimbotCFG::FOVCircleColor.Value.x, AimbotCFG::FOVCircleColor.Value.y, AimbotCFG::FOVCircleColor.Value.z, 0.2f);
            drawList->AddCircleFilled(screenCenter, fovPixels, fovFillColor, 64);
        }
    }

    // Get current aimbot status for GUI
    bool IsAimbotActive() { return isAimbotActive; }
    TargetInfo GetCurrentTarget() { return currentTarget; }
}

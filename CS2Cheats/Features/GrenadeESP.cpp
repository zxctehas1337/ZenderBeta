#include "GrenadeESP.h"
#include "../Helpers/Logger.h"
#include "../Game/Game.h"
#include <algorithm>
#include <cmath>

namespace GrenadeESP
{
    std::vector<GrenadeInfo> GrenadeManager::activeGrenades;
    std::vector<GrenadeTrajectoryPoint> GrenadeManager::trajectoryPoints;
    std::chrono::steady_clock::time_point GrenadeManager::lastUpdate;

    void GrenadeManager::Update()
    {
        auto now = std::chrono::steady_clock::now();
        float deltaTime = std::chrono::duration<float>(now - lastUpdate).count();
        lastUpdate = now;

        // Update existing grenades
        for (auto& grenade : activeGrenades) {
            if (grenade.isActive) {
                grenade.timer -= deltaTime;
                
                // Update position based on velocity
                if (grenade.velocity.x != 0 || grenade.velocity.y != 0 || grenade.velocity.z != 0) {
                    grenade.position.x += grenade.velocity.x * deltaTime;
                    grenade.position.y += grenade.velocity.y * deltaTime;
                    grenade.position.z += grenade.velocity.z * deltaTime;
                    
                    // Apply gravity
                    grenade.velocity.z -= 800.0f * deltaTime; // CS2 gravity
                }

                // Deactivate expired grenades
                if (grenade.timer <= 0) {
                    grenade.isActive = false;
                }
            }
        }

        // Remove inactive grenades
        activeGrenades.erase(
            std::remove_if(activeGrenades.begin(), activeGrenades.end(),
                [](const GrenadeInfo& g) { return !g.isActive; }),
            activeGrenades.end()
        );
    }

    void GrenadeManager::Render(const CEntity& localEntity)
    {
        if (!Config::EnableGrenadeESP)
            return;

        for (const auto& grenade : activeGrenades) {
            if (!grenade.isActive)
                continue;

            DrawGrenade(grenade, localEntity);
        }
    }

    void GrenadeManager::AddGrenade(const Vec3& pos, const Vec3& vel, const std::string& type, DWORD64 owner)
    {
        GrenadeInfo grenade;
        grenade.position = pos;
        grenade.velocity = vel;
        grenade.type = type;
        grenade.owner = owner;
        grenade.color = GetGrenadeColor(type);
        grenade.radius = GetGrenadeRadius(type);
        grenade.isActive = true;
        grenade.timer = GetGrenadeTimer(type);
        grenade.spawnTime = std::chrono::steady_clock::now();

        activeGrenades.push_back(grenade);
    }

    void GrenadeManager::ClearGrenades()
    {
        activeGrenades.clear();
        trajectoryPoints.clear();
    }

    std::vector<Vec3> GrenadeManager::PredictTrajectory(const Vec3& startPos, const Vec3& startVel, const std::string& type)
    {
        std::vector<Vec3> trajectory;
        Vec3 currentPos = startPos;
        Vec3 currentVel = startVel;
        float timeStep = 0.016f; // ~60 FPS
        float maxTime = 5.0f;

        for (float t = 0; t < maxTime; t += timeStep) {
            trajectory.push_back(currentPos);

            // Update position
            currentPos.x += currentVel.x * timeStep;
            currentPos.y += currentVel.y * timeStep;
            currentPos.z += currentVel.z * timeStep;

            // Apply gravity
            currentVel.z -= 800.0f * timeStep;

            // Check if hit ground
            if (currentPos.z <= 0) {
                break;
            }
        }

        return trajectory;
    }

    float GrenadeManager::GetGrenadeRadius(const std::string& type)
    {
        if (type == "weapon_hegrenade") return 350.0f;
        if (type == "weapon_molotov" || type == "weapon_incgrenade") return 180.0f;
        if (type == "weapon_flashbang") return 400.0f;
        if (type == "weapon_smokegrenade") return 250.0f;
        return 100.0f;
    }

    ImColor GrenadeManager::GetGrenadeColor(const std::string& type)
    {
        if (type == "weapon_hegrenade") return Config::HEColor;
        if (type == "weapon_molotov" || type == "weapon_incgrenade") return Config::MolotovColor;
        if (type == "weapon_flashbang") return Config::FlashbangColor;
        if (type == "weapon_smokegrenade") return Config::SmokeColor;
        return ImColor(255, 255, 255, 200);
    }

    bool GrenadeManager::IsGrenadeDangerous(const Vec3& grenadePos, const Vec3& playerPos, const std::string& type, float timeToExplosion)
    {
        float distance = grenadePos.DistanceTo(playerPos);
        float radius = GetGrenadeRadius(type);
        
        return distance <= radius && timeToExplosion <= Config::WarningTime;
    }

    void Initialize()
    {
        GrenadeManager::ClearGrenades();
    }

    void RenderGrenadeESP(const CEntity& localEntity)
    {
        GrenadeManager::Update();
        GrenadeManager::Render(localEntity);
    }

    void DrawGrenade(const GrenadeInfo& grenade, const CEntity& localEntity)
    {
        // Convert 3D position to 2D screen coordinates
        Vec2 screenPos;
        if (!gGame.View.WorldToScreen(grenade.position, screenPos))
            return;

        ImDrawList* drawList = ImGui::GetBackgroundDrawList();

        // Draw grenade icon/circle
        drawList->AddCircle(
            ImVec2(screenPos.x, screenPos.y),
            8.0f,
            grenade.color,
            12,
            2.0f
        );

        // Draw grenade name
        if (Config::ShowGrenadeName) {
            std::string displayName = GetGrenadeDisplayName(grenade.type);
            ImVec2 textSize = ImGui::CalcTextSize(displayName.c_str());
            drawList->AddText(
                ImVec2(screenPos.x - textSize.x / 2, screenPos.y - 20),
                ImColor(255, 255, 255, 255),
                displayName.c_str()
            );
        }

        // Draw timer
        if (Config::ShowGrenadeTimer && grenade.timer > 0) {
            char timerText[32];
            sprintf_s(timerText, "%.1fs", grenade.timer);
            ImVec2 textSize = ImGui::CalcTextSize(timerText);
            drawList->AddText(
                ImVec2(screenPos.x - textSize.x / 2, screenPos.y + 12),
                ImColor(255, 255, 0, 255),
                timerText
            );
        }

        // Draw explosion radius
        if (Config::ShowExplosionRadius) {
            float distance = grenade.position.DistanceTo(localEntity.Pawn.Pos);
            bool isDangerous = GrenadeManager::IsGrenadeDangerous(grenade.position, localEntity.Pawn.Pos, grenade.type, grenade.timer);
            DrawExplosionRadius(grenade.position, grenade.radius, grenade.color, isDangerous);
        }

        // Draw trajectory prediction
        if (Config::ShowTrajectory) {
            auto trajectory = GrenadeManager::PredictTrajectory(grenade.position, grenade.velocity, grenade.type);
            DrawTrajectory(trajectory);
        }

        // Draw danger warning
        if (Config::ShowDangerWarning) {
            float distance = grenade.position.DistanceTo(localEntity.Pawn.Pos);
            if (distance <= Config::WarningDistance) {
                DrawDangerWarning(grenade.position, grenade.type, grenade.timer);
            }
        }
    }

    void DrawTrajectory(const std::vector<Vec3>& trajectory)
    {
        if (trajectory.size() < 2)
            return;

        ImDrawList* drawList = ImGui::GetBackgroundDrawList();
        
        for (size_t i = 1; i < trajectory.size(); ++i) {
            Vec2 screenPos1, screenPos2;
            if (gGame.View.WorldToScreen(trajectory[i-1], screenPos1) && 
                gGame.View.WorldToScreen(trajectory[i], screenPos2)) {
                
                // Fade trajectory based on distance
                float alpha = 1.0f - (static_cast<float>(i) / trajectory.size()) * 0.7f;
                ImColor color = ImColor(
                    Config::TrajectoryColor.Value.x,
                    Config::TrajectoryColor.Value.y,
                    Config::TrajectoryColor.Value.z,
                    Config::TrajectoryColor.Value.w * alpha
                );
                
                drawList->AddLine(
                    ImVec2(screenPos1.x, screenPos1.y),
                    ImVec2(screenPos2.x, screenPos2.y),
                    color,
                    1.5f
                );
            }
        }
    }

    void DrawExplosionRadius(const Vec3& position, float radius, const ImColor& color, bool isDangerous)
    {
        Vec2 screenPos;
        if (!gGame.View.WorldToScreen(position, screenPos))
            return;

        ImDrawList* drawList = ImGui::GetBackgroundDrawList();
        
        // Calculate screen radius based on distance
        float screenRadius = radius / position.DistanceTo(Vec3(0, 0, 0)) * 1000.0f;
        
        ImColor radiusColor = isDangerous ? Config::DangerColor : color;
        radiusColor.Value.w = 100; // Make it semi-transparent
        
        drawList->AddCircle(
            ImVec2(screenPos.x, screenPos.y),
            screenRadius,
            radiusColor,
            64,
            1.5f
        );
    }

    void DrawDangerWarning(const Vec3& position, const std::string& type, float timeToExplosion)
    {
        Vec2 screenPos;
        if (!gGame.View.WorldToScreen(position, screenPos))
            return;

        ImDrawList* drawList = ImGui::GetBackgroundDrawList();
        
        // Draw warning text
        std::string warning = "DANGER!";
        ImVec2 textSize = ImGui::CalcTextSize(warning.c_str());
        
        // Pulsing effect
        float pulse = std::sin(ImGui::GetTime() * 5.0f) * 0.3f + 0.7f;
        ImColor warningColor = ImColor(
            255,
            static_cast<int>(50 * pulse),
            50,
            255
        );
        
        drawList->AddText(
            ImVec2(screenPos.x - textSize.x / 2, screenPos.y - 35),
            warningColor,
            warning.c_str()
        );
        
        // Draw warning icon
        drawList->AddCircle(
            ImVec2(screenPos.x, screenPos.y),
            15.0f,
            warningColor,
            8,
            3.0f
        );
    }

    std::string GetGrenadeDisplayName(const std::string& weaponType)
    {
        if (weaponType == "weapon_hegrenade") return "HE";
        if (weaponType == "weapon_molotov") return "Molotov";
        if (weaponType == "weapon_incgrenade") return "Incendiary";
        if (weaponType == "weapon_flashbang") return "Flash";
        if (weaponType == "weapon_smokegrenade") return "Smoke";
        return "Grenade";
    }

    float GetGrenadeTimer(const std::string& type)
    {
        if (type == "weapon_hegrenade") return 1.5f;
        if (type == "weapon_molotov" || type == "weapon_incgrenade") return 3.0f;
        if (type == "weapon_flashbang") return 1.5f;
        if (type == "weapon_smokegrenade") return 1.5f;
        return 2.0f;
    }
}

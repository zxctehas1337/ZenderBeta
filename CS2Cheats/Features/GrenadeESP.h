#pragma once
#include "../Core/Render.h"
#include "../Core/Config.h"
#include "../Core/Cheats.h"
#include "../Game/Entity.h"
#include <vector>
#include <string>
#include <chrono>

namespace GrenadeESP
{
    struct GrenadeInfo {
        Vec3 position;
        Vec3 velocity;
        std::string type;
        float timer;
        DWORD64 owner;
        ImColor color;
        float radius;
        bool isActive;
        std::chrono::steady_clock::time_point spawnTime;
    };

    struct GrenadeTrajectoryPoint {
        Vec3 position;
        float time;
        bool valid;
    };

    class GrenadeManager {
    private:
        static std::vector<GrenadeInfo> activeGrenades;
        static std::vector<GrenadeTrajectoryPoint> trajectoryPoints;
        static std::chrono::steady_clock::time_point lastUpdate;

    public:
        static void Update();
        static void Render(const CEntity& localEntity);
        static void AddGrenade(const Vec3& pos, const Vec3& vel, const std::string& type, DWORD64 owner);
        static void ClearGrenades();
        static std::vector<Vec3> PredictTrajectory(const Vec3& startPos, const Vec3& startVel, const std::string& type);
        static float GetGrenadeRadius(const std::string& type);
        static ImColor GetGrenadeColor(const std::string& type);
        static bool IsGrenadeDangerous(const Vec3& grenadePos, const Vec3& playerPos, const std::string& type, float timeToExplosion);
    };

    namespace Config {
        inline bool EnableGrenadeESP = true;
        inline bool ShowTrajectory = true;
        inline bool ShowExplosionRadius = true;
        inline bool ShowDangerWarning = true;
        inline bool ShowGrenadeTimer = true;
        inline bool ShowGrenadeName = true;
        inline float WarningDistance = 500.0f;
        inline float WarningTime = 3.0f;
        inline ImColor HEColor = ImColor(255, 100, 100, 200);
        inline ImColor MolotovColor = ImColor(255, 150, 50, 200);
        inline ImColor FlashbangColor = ImColor(200, 200, 255, 200);
        inline ImColor SmokeColor = ImColor(150, 150, 150, 200);
        inline ImColor DangerColor = ImColor(255, 0, 0, 255);
        inline ImColor TrajectoryColor = ImColor(255, 255, 255, 150);
    }

    void Initialize();
    void RenderGrenadeESP(const CEntity& localEntity);
    void DrawGrenade(const GrenadeInfo& grenade, const CEntity& localEntity);
    void DrawTrajectory(const std::vector<Vec3>& trajectory);
    void DrawExplosionRadius(const Vec3& position, float radius, const ImColor& color, bool isDangerous);
    void DrawDangerWarning(const Vec3& position, const std::string& type, float timeToExplosion);
    std::string GetGrenadeDisplayName(const std::string& weaponType);
    float GetGrenadeTimer(const std::string& type);
}

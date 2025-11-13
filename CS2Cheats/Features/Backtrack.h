#pragma once
#include "../OS-ImGui/OS-ImGui_Struct.h"
#include "../Game/Entity.h"
#include <deque>
#include <chrono>
#include <unordered_map>

// Backtrack record for entity history
struct BacktrackRecord {
    Vec3 position;
    Vec3 angles;
    Vec3 velocity;
    Vec3 origin;
    float simulationTime;
    int tickCount;
    bool valid;
    std::chrono::steady_clock::time_point recordTime;
};

// Backtrack system for lag compensation
class Backtrack {
private:
    // Entity history storage (12 ticks ~200ms as recommended)
    static std::unordered_map<int, std::deque<BacktrackRecord>> entityHistory;
    static const int MAX_RECORDS = 12; // ~200ms at 64 tick
    static const float MAX_BACKTRACK_TIME; // 200ms maximum
    
    static bool enabled;
    static float backtrackTime;
    static int validRecords;
    
public:
    // Initialize backtrack system
    static void Initialize();
    static void Shutdown();
    static bool IsEnabled() { return enabled; }
    
    // Main update function (call every frame)
    static void Update(const std::vector<CEntity>& entities);
    static void UpdateEntity(const CEntity& entity);
    
    // Backtrack query functions
    static bool GetBestRecord(int entityIndex, BacktrackRecord& record);
    static bool GetRecordAtTime(int entityIndex, float time, BacktrackRecord& record);
    static std::deque<BacktrackRecord>* GetEntityRecords(int entityIndex);
    
    // Validation functions
    static bool IsRecordValid(const BacktrackRecord& record);
    static bool IsTimeValid(float time);
    static float GetLatency();
    
    // Utility functions
    static void ClearHistory();
    static void ClearEntityHistory(int entityIndex);
    static int GetRecordCount(int entityIndex);
    static float GetOldestRecordTime(int entityIndex);
    
    // Configuration
    static void SetEnabled(bool enable) { enabled = enable; }
    static void SetBacktrackTime(float time);
    static float GetBacktrackTime() { return backtrackTime; }
    static int GetValidRecordCount() { return validRecords; }
    
    // Advanced functions
    static Vec3 PredictPosition(const BacktrackRecord& record, float deltaTime);
    static bool ShouldBacktrack(const CEntity& entity, const Vec3& aimPos);
    static BacktrackRecord InterpolateRecords(const BacktrackRecord& older, const BacktrackRecord& newer, float factor);
    
    // Debug functions
    static void DrawDebugInfo();
    static void LogBacktrackInfo(int entityIndex);
    
private:
    // Internal functions
    static void AddRecord(int entityIndex, const CEntity& entity);
    static void CleanupOldRecords();
    static void CleanupEntityRecords(int entityIndex);
    static BacktrackRecord CreateRecord(const CEntity& entity);
    static bool ValidateRecord(const BacktrackRecord& record, const CEntity& entity);
};

// Global backtrack instance
extern Backtrack g_Backtrack;
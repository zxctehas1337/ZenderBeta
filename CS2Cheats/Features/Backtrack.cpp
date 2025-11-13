#include "Backtrack.h"
#include "../Helpers/Logger.h"
#include "../Helpers/Math.h"
#include "../Core/GlobalVars.h"

// Static member initialization
std::unordered_map<int, std::deque<BacktrackRecord>> Backtrack::entityHistory;
const float Backtrack::MAX_BACKTRACK_TIME = 0.2f; // 200ms
bool Backtrack::enabled = false;
float Backtrack::backtrackTime = 0.15f; // 150ms default
int Backtrack::validRecords = 0;
Backtrack g_Backtrack;

void Backtrack::Initialize() {
    entityHistory.clear();
    enabled = true;
    validRecords = 0;
    Log::Info("Backtrack system initialized");
}

void Backtrack::Shutdown() {
    ClearHistory();
    enabled = false;
    Log::Info("Backtrack system shutdown");
}

void Backtrack::Update(const std::vector<CEntity>& entities) {
    if (!enabled) {
        return;
    }
    
    validRecords = 0;
    
    // Update all entities
    for (const auto& entity : entities) {
        if (entity.Controller.Address != 0 && entity.Pawn.Address != 0) {
            UpdateEntity(entity);
        }
    }
    
    // Cleanup old records
    CleanupOldRecords();
}

void Backtrack::UpdateEntity(const CEntity& entity) {
    if (!enabled || entity.Controller.Address == 0) {
        return;
    }
    
    int entityIndex = static_cast<int>(entity.Controller.Address); // Use controller address as unique ID
    AddRecord(entityIndex, entity);
}

bool Backtrack::GetBestRecord(int entityIndex, BacktrackRecord& record) {
    if (!enabled) {
        return false;
    }
    
    auto it = entityHistory.find(entityIndex);
    if (it == entityHistory.end() || it->second.empty()) {
        return false;
    }
    
    // Find the best record (usually the most recent valid one)
    for (auto& rec : it->second) {
        if (IsRecordValid(rec)) {
            record = rec;
            return true;
        }
    }
    
    return false;
}

bool Backtrack::GetRecordAtTime(int entityIndex, float time, BacktrackRecord& record) {
    if (!enabled || !IsTimeValid(time)) {
        return false;
    }
    
    auto it = entityHistory.find(entityIndex);
    if (it == entityHistory.end() || it->second.empty()) {
        return false;
    }
    
    // Find record closest to the requested time
    BacktrackRecord* bestRecord = nullptr;
    float bestTimeDiff = FLT_MAX;
    
    for (auto& rec : it->second) {
        if (!IsRecordValid(rec)) {
            continue;
        }
        
        float timeDiff = abs(rec.simulationTime - time);
        if (timeDiff < bestTimeDiff) {
            bestTimeDiff = timeDiff;
            bestRecord = &rec;
        }
    }
    
    if (bestRecord) {
        record = *bestRecord;
        return true;
    }
    
    return false;
}

std::deque<BacktrackRecord>* Backtrack::GetEntityRecords(int entityIndex) {
    auto it = entityHistory.find(entityIndex);
    if (it != entityHistory.end()) {
        return &it->second;
    }
    return nullptr;
}

bool Backtrack::IsRecordValid(const BacktrackRecord& record) {
    if (!record.valid) {
        return false;
    }
    
    // Check if record is within valid time range
    auto now = std::chrono::steady_clock::now();
    auto recordAge = std::chrono::duration_cast<std::chrono::milliseconds>(now - record.recordTime).count() / 1000.0f;
    
    return recordAge <= MAX_BACKTRACK_TIME;
}

bool Backtrack::IsTimeValid(float time) {
    if (!g_globalVars) {
        return false;
    }
    
    float currentTime = g_globalVars->g_fCurrentTime;
    float timeDiff = currentTime - time;
    
    return timeDiff >= 0.0f && timeDiff <= MAX_BACKTRACK_TIME;
}

float Backtrack::GetLatency() {
    // TODO: Get actual network latency
    // For now, return estimated latency
    return 0.05f; // 50ms estimated latency
}

void Backtrack::ClearHistory() {
    entityHistory.clear();
    validRecords = 0;
}

void Backtrack::ClearEntityHistory(int entityIndex) {
    auto it = entityHistory.find(entityIndex);
    if (it != entityHistory.end()) {
        it->second.clear();
    }
}

int Backtrack::GetRecordCount(int entityIndex) {
    auto it = entityHistory.find(entityIndex);
    if (it != entityHistory.end()) {
        return static_cast<int>(it->second.size());
    }
    return 0;
}

float Backtrack::GetOldestRecordTime(int entityIndex) {
    auto it = entityHistory.find(entityIndex);
    if (it != entityHistory.end() && !it->second.empty()) {
        return it->second.back().simulationTime;
    }
    return 0.0f;
}

void Backtrack::SetBacktrackTime(float time) {
    backtrackTime = Math::Clamp(time, 0.0f, MAX_BACKTRACK_TIME);
}

Vec3 Backtrack::PredictPosition(const BacktrackRecord& record, float deltaTime) {
    // Simple linear prediction based on velocity
    Vec3 predictedPos = record.position;
    predictedPos.x += record.velocity.x * deltaTime;
    predictedPos.y += record.velocity.y * deltaTime;
    predictedPos.z += record.velocity.z * deltaTime;
    
    return predictedPos;
}

bool Backtrack::ShouldBacktrack(const CEntity& entity, const Vec3& aimPos) {
    if (!enabled) {
        return false;
    }
    
    // Check if entity is moving fast enough to benefit from backtrack
    float speed = entity.Pawn.Velocity.Length();
    if (speed < 50.0f) { // Moving too slowly
        return false;
    }
    
    // Check if we have enough records
    int entityIndex = static_cast<int>(entity.Controller.Address);
    if (GetRecordCount(entityIndex) < 3) {
        return false;
    }
    
    return true;
}

BacktrackRecord Backtrack::InterpolateRecords(const BacktrackRecord& older, const BacktrackRecord& newer, float factor) {
    BacktrackRecord interpolated;
    
    // Interpolate position
    interpolated.position = older.position + (newer.position - older.position) * factor;
    interpolated.angles = older.angles + (newer.angles - older.angles) * factor;
    interpolated.velocity = older.velocity + (newer.velocity - older.velocity) * factor;
    interpolated.origin = older.origin + (newer.origin - older.origin) * factor;
    
    // Interpolate time values
    interpolated.simulationTime = older.simulationTime + (newer.simulationTime - older.simulationTime) * factor;
    interpolated.tickCount = static_cast<int>(older.tickCount + (newer.tickCount - older.tickCount) * factor);
    
    interpolated.valid = older.valid && newer.valid;
    interpolated.recordTime = newer.recordTime;
    
    return interpolated;
}

void Backtrack::DrawDebugInfo() {
    if (!enabled) {
        return;
    }
    
    // TODO: Draw debug visualization
    // This would show backtrack records as colored dots or lines
}

void Backtrack::LogBacktrackInfo(int entityIndex) {
    auto records = GetEntityRecords(entityIndex);
    if (records) {
        Log::Info("Entity " + std::to_string(entityIndex) + " has " + std::to_string(records->size()) + " backtrack records");
    }
}

void Backtrack::AddRecord(int entityIndex, const CEntity& entity) {
    BacktrackRecord record = CreateRecord(entity);
    
    if (!ValidateRecord(record, entity)) {
        return;
    }
    
    auto& records = entityHistory[entityIndex];
    
    // Add new record to front
    records.push_front(record);
    
    // Remove old records if we have too many
    while (records.size() > MAX_RECORDS) {
        records.pop_back();
    }
    
    validRecords++;
}

void Backtrack::CleanupOldRecords() {
    auto now = std::chrono::steady_clock::now();
    
    for (auto& pair : entityHistory) {
        CleanupEntityRecords(pair.first);
    }
}

void Backtrack::CleanupEntityRecords(int entityIndex) {
    auto it = entityHistory.find(entityIndex);
    if (it == entityHistory.end()) {
        return;
    }
    
    auto& records = it->second;
    auto now = std::chrono::steady_clock::now();
    
    // Remove records that are too old
    records.erase(
        std::remove_if(records.begin(), records.end(),
            [now](const BacktrackRecord& record) {
                auto age = std::chrono::duration_cast<std::chrono::milliseconds>(now - record.recordTime).count() / 1000.0f;
                return age > MAX_BACKTRACK_TIME;
            }),
        records.end()
    );
}

BacktrackRecord Backtrack::CreateRecord(const CEntity& entity) {
    BacktrackRecord record;
    
    record.position = entity.Pawn.Pos;
    record.angles = entity.Pawn.ViewAngle;
    record.velocity = entity.Pawn.Velocity;
    record.origin = entity.Pawn.Pos; // Same as position for now
    
    // Get simulation time from global vars
    if (g_globalVars) {
        record.simulationTime = g_globalVars->g_fCurrentTime;
        record.tickCount = g_globalVars->g_iTickCount;
    } else {
        record.simulationTime = 0.0f;
        record.tickCount = 0;
    }
    
    record.valid = true;
    record.recordTime = std::chrono::steady_clock::now();
    
    return record;
}

bool Backtrack::ValidateRecord(const BacktrackRecord& record, const CEntity& entity) {
    // Basic validation
    if (!record.valid) {
        return false;
    }
    
    // Check if entity is alive
    if (entity.Pawn.Health <= 0) {
        return false;
    }
    
    // Check if position is reasonable
    if (record.position.Length() > 50000.0f) { // Sanity check
        return false;
    }
    
    return true;
}
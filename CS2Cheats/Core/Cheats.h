#pragma once
#include "../Game/Game.h"
#include "../Game/Entity.h"
#include "../Features/Radar.h"
#include "../Features/Misc.h"
#include <vector>

// processed entity data and results
struct EntityResult {
	int entityIndex = -1;
	CEntity entity;
	ImVec4 espRect;
	int distance = 0;
	bool isInScreen = false;
	bool isValid = false;
};

namespace Cheats
{
	static DWORD m_currentTick = 0;
	static DWORD m_previousTick = 0;
	static std::vector<EntityResult> entityDataBuffer[2]; // Two buffers for double buffering
	static std::atomic<int> currentBufferIndex = 0; // Atomically switch between buffers

	void Run();
	
	std::vector<std::pair<int, CEntity>> CollectEntityData(CEntity& localEntity, int& localPlayerControllerIndex);
	std::vector<EntityResult> ProcessEntities(CEntity& localEntity, int& localPlayerControllerIndex, const std::vector<std::pair<int, CEntity>>& collectedEntities);
	void HandleEnts(const std::vector<EntityResult>& entities, CEntity& localEntity, 
		int localPlayerControllerIndex, Base_Radar& gameRadar);
	std::string GetCurrentMapName();
}

// Forward declaration for MiscFuncs to accept renderEntities
void MiscFuncs(CEntity& LocalEntity, const std::vector<EntityResult>& renderEntities);

struct {
	ImFont* normal15px = nullptr;
} fonts;
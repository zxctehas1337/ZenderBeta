
#include <string>
#include <thread>
#include <future>
#include <iostream>

#include "Cheats.h"
#include "Render.h"
#include "../Core/Config.h"

#include "../Core/Init.h"

#include "../Features/ESP.h"
#include "../Core/GUI.h"
#include "../Features/BombTimer.h"
#include "../Features/SpectatorList.h"
#include "../Helpers/Logger.h"
#include "../Features/SoundESP.h"
#include "../Features/Aimbot.h"

int PreviousTotalHits = 0;

void Menu();
void Visual(const CEntity&);
void Radar(Base_Radar, const CEntity&);
void MiscFuncs(CEntity&, const std::vector<EntityResult>& renderEntities);
void RenderCrosshair(ImDrawList*, const CEntity&);
void RadarSetting(Base_Radar&);

void Cheats::Run()
{	
	Menu();

	Misc::AutoAccept::UpdateAutoAccept();

#ifndef DBDEBUG
	if (!Init::Client::isGameWindowActive() && !MenuConfig::ShowMenu) {
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
		return;
	}
#endif

	// Update matrix
	if (!memoryManager.ReadMemory(gGame.GetMatrixAddress(), gGame.View.Matrix,64))
		return;

	// Update EntityList Entry
	gGame.UpdateEntityListEntry();

	DWORD64 LocalControllerAddress = 0;
	DWORD64 LocalPawnAddress = 0;

	if (!memoryManager.ReadMemory(gGame.GetLocalControllerAddress(), LocalControllerAddress))
		return;
	if (!memoryManager.ReadMemory(gGame.GetLocalPawnAddress(), LocalPawnAddress))
		return;

	if (LocalPawnAddress == 0 || LocalControllerAddress == 0) {
        g_globalVars->UpdateGlobalvars();
        entityDataBuffer[0].clear();
        entityDataBuffer[1].clear();
        currentBufferIndex.store(0);
        return;
    }

	// Update global variables every frame
	g_globalVars->UpdateGlobalvars();

	// LocalEntity
	CEntity LocalEntity;
	int LocalPlayerControllerIndex = 0;
	LocalEntity.UpdateClientData();
	if (!LocalEntity.UpdateController(LocalControllerAddress))
		return;
	if (!LocalEntity.UpdatePawn(LocalPawnAddress) && !MenuConfig::WorkInSpec)
		return;

	// Update only on new tick for other entities
	int currentTick = g_globalVars->g_iTickCount;
	if (currentTick != m_currentTick)
	{
		m_previousTick = m_currentTick;
		m_currentTick = currentTick;

		int writeBufferIndex = 1 - currentBufferIndex.load(); // Get the index of the buffer to write to
		entityDataBuffer[writeBufferIndex].clear(); // Clear the buffer before filling

		// Collect and process entities for the write buffer
		auto collectedEntities = CollectEntityData(LocalEntity, LocalPlayerControllerIndex);
		entityDataBuffer[writeBufferIndex] = ProcessEntities(LocalEntity, LocalPlayerControllerIndex, collectedEntities);

		// Atomically swap buffers for rendering
		currentBufferIndex.store(writeBufferIndex);
	}

	// Use the read buffer for rendering
	const std::vector<EntityResult>& renderEntities = entityDataBuffer[currentBufferIndex.load()];

	// radar data
	Base_Radar GameRadar;
	if ((RadarCFG::ShowRadar && LocalEntity.Controller.TeamID != 0) || (RadarCFG::ShowRadar && MenuConfig::ShowMenu))
		RadarSetting(GameRadar);
	
	// render entities
	HandleEnts(renderEntities, LocalEntity, LocalPlayerControllerIndex, GameRadar);

	Visual(LocalEntity);
	Radar(GameRadar, LocalEntity);
	MiscFuncs(LocalEntity, renderEntities); // Pass renderEntities

	// Run aimbot visualization (actual aimbot logic runs in CreateMove hook)
	Aimbot::DrawFOV(LocalEntity);

	int currentFPS = static_cast<int>(ImGui::GetIO().Framerate);
	if (currentFPS > MenuConfig::RenderFPS)
	{
		int FrameWait = static_cast<int>(round(1000.0f / MenuConfig::RenderFPS));
		std::this_thread::sleep_for(std::chrono::milliseconds(FrameWait));
	}
	
	// run trigger & aim every new tick
	if (m_currentTick != m_previousTick)
	{
		std::vector<CEntity> allEntities;
		for (const auto& result : renderEntities) { // Use renderEntities
			allEntities.push_back(result.entity);
		}
		SpecList::GetSpectatorList(allEntities, LocalEntity);
	}
}

// collect entity data
std::vector<std::pair<int, CEntity>> Cheats::CollectEntityData(CEntity& localEntity, int& localPlayerControllerIndex)
{
	// update only on new tick
	//if (m_currentTick == m_previousTick)
	//{
	//	return cachedResults;
	//}

	std::vector<EntityBatchData> batchData;
	batchData.reserve(64);

	// collect entity addresses
	for (int entityIndex = 0; entityIndex < 64; ++entityIndex)
	{
		DWORD64 entityAddress = 0;
		if (!memoryManager.ReadMemory<DWORD64>(gGame.GetEntityListEntry() + (entityIndex + 1) * 0x70, entityAddress))
		{
			continue;
		}

		// skip local player
		if (entityAddress == localEntity.Controller.Address)
		{
			localPlayerControllerIndex = entityIndex;
			continue;
		}

		// get pawn address
		CEntity tempEntity;
		tempEntity.Controller.Address = entityAddress;
		DWORD64 pawnAddress = tempEntity.Controller.GetPlayerPawnAddress();
		
		if (pawnAddress != 0)
		{
			batchData.emplace_back(entityIndex, entityAddress, pawnAddress);
		}
	}

	if (batchData.empty())
	{
		return {};
	}

	// process all entities in batch
	std::vector<std::pair<int, CEntity>> entities;
	EntityBatchProcessor processor;
	if (!processor.ProcessAllEntities(entities, batchData))
	{
		return {};
	}

	// update cache - NO LONGER NEEDED WITH DOUBLE BUFFERING
	// cachedResults = entities;

	return entities; // Return directly
}

// process, prepare results
std::vector<EntityResult> Cheats::ProcessEntities(CEntity& localEntity, int& localPlayerControllerIndex, const std::vector<std::pair<int, CEntity>>& collectedEntities)
{
	// get batch-processed entities - REMOVED, now passed as argument
	// auto entities = CollectEntityData(localEntity, localPlayerControllerIndex);
	std::vector<EntityResult> results;
	results.reserve(collectedEntities.size());

	// process each entity
	for (auto& [entityIndex, entity] : collectedEntities)
	{
		EntityResult result;
		result.entityIndex = entityIndex;
		result.entity = entity;

		if (!entity.IsAlive())
			continue;

		// skip teammates if team check enabled
		if (MenuConfig::TeamCheck && entity.Controller.TeamID == localEntity.Controller.TeamID)
			continue;

		// check if in screen
		result.isInScreen = entity.IsInScreen();

		// calculate distance
		result.distance = static_cast<int>(entity.Pawn.Pos.DistanceTo(localEntity.Pawn.Pos) / 100);

		// calculate esp box rect
		if (ESPConfig::ESPenabled && result.isInScreen)
			result.espRect = ESP::GetBoxRect(entity, ESPConfig::BoxType);

		// sound esp
		if (ESPConfig::ESPenabled && ESPConfig::EnemySound && result.entity.Controller.Address != localEntity.Controller.Address)
			SoundESP::ProcessSound(result.entity, localEntity);

		result.isValid = true;
		results.push_back(result);
	}
	
	return results;
}

// render, collect aim data
void Cheats::HandleEnts(const std::vector<EntityResult>& entities, CEntity& localEntity, 
	int localPlayerControllerIndex, Base_Radar& gameRadar)
{
	// healthbar map (static)
	static std::map<DWORD64, Render::HealthBar> HealthBarMap;

	float MaxAimDistance = 100000;

	for (const auto& result : entities)
	{
		if (!result.isValid)
		{
			if (HealthBarMap.count(result.entity.Controller.Address))
				HealthBarMap.erase(result.entity.Controller.Address);
			continue;
		}

		const auto& entity = result.entity;
		const int entityIndex = result.entityIndex;

		// add entity to radar
		if (RadarCFG::ShowRadar && localEntity.Controller.TeamID != 0)
		{
			gameRadar.AddPoint(localEntity.Pawn.Pos, localEntity.Pawn.ViewAngle.y, 
				entity.Pawn.Pos, ImColor(237, 85, 106, 200), RadarCFG::RadarType, entity.Pawn.ViewAngle.y);
		}


        // skip not in screen
		if (!result.isInScreen)
		{
			continue;
		}

		// render esp
		if (ESPConfig::ESPenabled && (!ESPConfig::FlashCheck || localEntity.Pawn.FlashDuration < 0.1f))
		{
			const ImVec4& Rect = result.espRect;
			const int distance = result.distance;

			if (MenuConfig::RenderDistance == 0 || (distance <= MenuConfig::RenderDistance && MenuConfig::RenderDistance > 0))
			{
				ESP::RenderPlayerESP(localEntity, entity, Rect, localPlayerControllerIndex, entityIndex);
				Render::DrawDistance(localEntity, entity, Rect);

				// healthbar
				if (ESPConfig::ShowHealthBar)
				{
					ImVec2 HealthBarPos = { Rect.x - 6.f, Rect.y };
					ImVec2 HealthBarSize = { 4, Rect.w };
					Render::DrawHealthBar(static_cast<DWORD>(entity.Controller.Address), 100.0f, static_cast<float>(entity.Pawn.Health), HealthBarPos, HealthBarSize);
				}

				// ammo
				// When player is using knife or nade, Ammo = -1.
				if (ESPConfig::AmmoBar && entity.Pawn.Ammo != -1)
				{
					ImVec2 AmmoBarPos = { Rect.x, Rect.y + Rect.w + 2 };
					ImVec2 AmmoBarSize = { Rect.z, 4 };
					Render::DrawAmmoBar(static_cast<DWORD>(entity.Controller.Address), static_cast<float>(entity.Pawn.Ammo + entity.Pawn.ShotsFired), 
						static_cast<float>(entity.Pawn.Ammo), AmmoBarPos, AmmoBarSize);
				}

				// armor
				// It is meaningless to render a empty bar
				if (ESPConfig::ArmorBar && entity.Pawn.Armor > 0)
				{
					bool HasHelmet;
					ImVec2 ArmorBarPos;
					memoryManager.ReadMemory(entity.Controller.Address + Offset.PlayerController.HasHelmet, HasHelmet);
					
					if (ESPConfig::ShowHealthBar)
						ArmorBarPos = { Rect.x - 10.f, Rect.y };
					else
						ArmorBarPos = { Rect.x - 6.f, Rect.y };
					
					ImVec2 ArmorBarSize = { 4.f, Rect.w };
					Render::DrawArmorBar(static_cast<DWORD>(entity.Controller.Address), static_cast<float>(entity.Pawn.Armor), 100.0f, HasHelmet, ArmorBarPos, ArmorBarSize);
				}
			}
		}
	}
}

void Menu() 
{
	if (MenuConfig::ShowMenu)
		GUI::DrawGui();
}

void Visual(const CEntity& LocalEntity)
{
	// Fov circle
	if (LocalEntity.Controller.TeamID != 0 && !MenuConfig::ShowMenu)
		Render::DrawFovCircle(ImGui::GetBackgroundDrawList(), LocalEntity);

	// HeadShoot Line
	Render::HeadShootLine(LocalEntity, MiscCFG::HeadShootLineColor);

	RenderCrosshair(ImGui::GetBackgroundDrawList(), LocalEntity);
}

void Radar(Base_Radar Radar, const CEntity& LocalEntity)
{
	// Radar render
	if ((RadarCFG::ShowRadar && LocalEntity.Controller.TeamID != 0) || (RadarCFG::ShowRadar && MenuConfig::ShowMenu))
	{
		Radar.Render();

		MenuConfig::RadarWinPos = ImGui::GetWindowPos();
		ImGui::End();
	}
}

void MiscFuncs(CEntity& LocalEntity, const std::vector<EntityResult>& renderEntities)
{
    SpecList::SpectatorWindowList(LocalEntity);
    bmb::RenderWindow(LocalEntity.Controller.TeamID);
    SoundESP::Render();

    Misc::HitManager(LocalEntity, PreviousTotalHits);
    Misc::AutoShop(LocalEntity);
    
    // Check for AutoShop hotkey
    if (MiscCFG::AutoShop && GetAsyncKeyState(MiscCFG::AutoShopKey) & 1) {
        Misc::BuyNow(LocalEntity);
    }
    
    // Wallbang Indicator - convert EntityResult to CEntity vector
    std::vector<CEntity> entitiesList;
    entitiesList.reserve(renderEntities.size());
    for (const auto& r : renderEntities) entitiesList.push_back(r.entity);
    Misc::WallbangIndicator(LocalEntity, entitiesList);
    
    Misc::BunnyHop(LocalEntity);
    Misc::Watermark(LocalEntity);
    Misc::FastStop();
    Misc::AntiAFKKickUpdate();

    if (MiscCFG::AutoKnife && !MenuConfig::ShowMenu) {
        std::vector<CEntity> enemyList;
        enemyList.reserve(renderEntities.size());
        for (const auto& r : renderEntities) enemyList.push_back(r.entity);
        Misc::KnifeBot(LocalEntity, enemyList);
    }
    if (MiscCFG::AutoZeus && !MenuConfig::ShowMenu) {
        Misc::ZeusBot(LocalEntity);
    }

    // AntiFlash: убираем флэш у себя
    if (MiscCFG::AntiFlash) {
        LocalEntity.Pawn.FlashDuration = 0.0f;
        // Также записываем в память игры для надежности
        if (LocalEntity.Pawn.Address != 0) {
            memoryManager.WriteMemory(LocalEntity.Pawn.Address + Offset.Pawn.flFlashDuration, 0.0f);
        }
    }
}

void RadarSetting(Base_Radar& Radar)
{
	// Radar window
	ImGui::SetNextWindowBgAlpha(RadarCFG::RadarBgAlpha);
	ImGui::Begin("Radar", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
	ImGui::SetWindowSize({ RadarCFG::RadarRange * 2,RadarCFG::RadarRange * 2 });
	ImGui::SetWindowPos(MenuConfig::RadarWinPos, ImGuiCond_Once);

	if (MenuConfig::RadarWinChengePos)
	{
		ImGui::SetWindowPos("Radar", MenuConfig::RadarWinPos);
		MenuConfig::RadarWinChengePos = false;
	}

	if (!RadarCFG::customRadar)
	{
		RadarCFG::ShowRadarCrossLine = false;
		RadarCFG::Proportion = 2700.f;
		RadarCFG::RadarPointSizeProportion = 1.f;
		RadarCFG::RadarRange = 125.f;
		RadarCFG::RadarBgAlpha = 0.1f;
	}


	// Radar.SetPos({ Gui.Window.Size.x / 2,Gui.Window.Size.y / 2 });
	Radar.SetDrawList(ImGui::GetWindowDrawList());
	Radar.SetPos({ ImGui::GetWindowPos().x + RadarCFG::RadarRange, ImGui::GetWindowPos().y + RadarCFG::RadarRange });
	Radar.SetProportion(RadarCFG::Proportion);
	Radar.SetRange(RadarCFG::RadarRange);
	Radar.SetSize(RadarCFG::RadarRange * 2);
	Radar.SetCrossColor(RadarCFG::RadarCrossLineColor);

	Radar.ArcArrowSize *= RadarCFG::RadarPointSizeProportion;
	Radar.ArrowSize *= RadarCFG::RadarPointSizeProportion;
	Radar.CircleSize *= RadarCFG::RadarPointSizeProportion;

	Radar.ShowCrossLine = RadarCFG::ShowRadarCrossLine;
	Radar.Opened = true;
}

void RenderCrosshair(ImDrawList* drawList, const CEntity& LocalEntity)
{
	if (!MiscCFG::SniperCrosshair || LocalEntity.Controller.TeamID == 0 || MenuConfig::ShowMenu)
		return;

	bool isScoped;
	memoryManager.ReadMemory<bool>(LocalEntity.Pawn.Address + Offset.Pawn.isScoped, isScoped);

	if (!isScoped)
		Render::DrawCrossHair(drawList, ImVec2(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2), MiscCFG::SniperCrosshairColor);
}

std::string Cheats::GetCurrentMapName() {
    if (!g_globalVars || !g_globalVars->g_cCurrentMap) {
        return "";
    }

    char currentMap[256] = { 0 };
    if (!memoryManager.ReadMemory(reinterpret_cast<DWORD64>(g_globalVars->g_cCurrentMap),
        currentMap, sizeof(currentMap) - 1)) {
        return "";
    }

    currentMap[255] = '\0';
    return std::string(currentMap);
}
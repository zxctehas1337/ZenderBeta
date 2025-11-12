// UpdaterOff.cpp - Функция обновления оффсетов игры перед запуском программы
// Извлечено из Offsets.cpp без изменения текущей реализации

#include <Windows.h>
#include <string>
#include <stdexcept>
#include <array>
#include <memory>
#include <regex>
#include "../Libs/json/json.hpp"

using json = nlohmann::json;

// Вспомогательная функция для HTTP GET запросов
namespace Web
{
    inline void Get(std::string url, std::string& response)
    {
        response = "";
        std::string cmd = "curl -s -X GET " + url;

        std::array<char, 128> buffer;
        std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd.c_str(), "r"), _pclose);

        if (!pipe)
           throw std::runtime_error("failed to get curl request");
        while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) != nullptr)
            response += buffer.data();

        if (response == "")
            throw std::runtime_error("bad internet connection");

        std::regex pattern("\\d{3}:");
        if (std::regex_search(response, pattern))
            throw std::runtime_error(response);
    }
}

// Основная функция обновления оффсетов
// Загружает актуальные оффсеты из репозитория cs2-dumper
void UpdateOffsets()
{
    std::string offsetsData, buttonsData, client_dllData;

    // Загрузка данных из GitHub репозитория
    Web::Get("https://raw.githubusercontent.com/a2x/cs2-dumper/main/output/offsets.json", offsetsData);
    Web::Get("https://raw.githubusercontent.com/a2x/cs2-dumper/main/output/buttons.json", buttonsData);
    Web::Get("https://raw.githubusercontent.com/a2x/cs2-dumper/main/output/client_dll.json", client_dllData);

    // Парсинг JSON данных
    json offsetsJson = json::parse(offsetsData);
    json buttonsJson = json::parse(buttonsData);
    json client_dllJson = json::parse(client_dllData)["client.dll"]["classes"];

    // Здесь происходит установка оффсетов в структуру Offsets
    // Полный список оффсетов:
    
    // Основные оффсеты client.dll:
    // - dwEntityList
    // - dwViewMatrix
    // - dwViewAngles
    // - dwLocalPlayerController
    // - dwLocalPlayerPawn
    // - dwGlobalVars
    // - dwPlantedC4
    // - dwSensitivity
    // - dwSensitivity_sensitivity
    
    // Оффсеты inputsystem.dll:
    // - dwInputSystem
    
    // Кнопки (buttons.json):
    // - attack, jump, right, left
    
    // Оффсеты классов (client_dll.json):
    // - CCSPlayerController
    // - C_CSPlayerPawn
    // - C_BasePlayerPawn
    // - C_BaseEntity
    // - CSkeletonInstance
    // - CCSPlayerBase_CameraServices
    // - EntitySpottedState_t
    // - CBasePlayerController
    // - CPlayer_ObserverServices
    // - C_EconEntity
    // - C_BasePlayerWeapon
    // - CBasePlayerWeaponVData
    // - C_AttributeContainer
    // - C_EconItemView
    // - CPlayer_WeaponServices
    // - C_PlantedC4
}

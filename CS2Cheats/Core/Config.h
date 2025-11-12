#pragma once
#include "../Game/Bone.h"
#include "../Game/Game.h"

namespace MenuConfig
{
	inline std::string name = "Zender";
	inline std::string version = "1";
	inline std::string author = "nihmadev";

	inline std::string path = "";
	inline std::string docPath = "";
	inline int RenderFPS = 1000;
	inline int AimDelay = 1;
	inline int BunnyHopDelay = 25;
	inline int RenderDistance = 1000;

	struct {
		// 0: Visual 1: Misc 2: Config
		int MenuPage = 0;
		ImVec2 MainWinSize = ImVec2(600.f, 500.f);
		ImVec2 LogoPos = ImVec2(0.f, 0.f);
		ImVec2 Button1Pos = ImVec2(0.f, 59.f);
		ImVec2 Button2Pos = ImVec2(0.f, 118.f);
		ImVec2 Button3Pos = ImVec2(0.f, 177.f);
		ImVec2 Button4Pos = ImVec2(0.f, 236.f);
		ImVec2 ChildPos = ImVec2(60.f, 0.f);
		ImVec2 ChildSize = ImVec2(540.f, 500.f);
	} WCS;	// Window Component Settings

	inline bool defaultConfig = false;

	// 0: Window 1: Collapse
	inline int WindowStyle = 0;
	inline bool ShowMenu = true;
	inline bool TeamCheck = true;
	inline bool BypassOBS = false;
	inline bool WorkInSpec = true;

	inline ImVec2 MarkWinPos;
	inline ImVec2 RadarWinPos;
	inline ImVec2 SpecWinPos;
	inline ImVec2 BombWinPos;
	inline bool MarkWinChengePos = false;
	inline bool RadarWinChengePos = false;
	inline bool SpecWinChengePos = false;
	inline bool BombWinChengePos = false;
	inline int HotKey = VK_END;
}

namespace ESPConfig
{
	inline int HotKey = VK_F6;
	inline bool ESPenabled = true;
	inline bool AmmoBar = true;
	inline bool ShowBoneESP = true;
	inline bool ShowBoxESP = true;
	inline bool ShowHealthBar = true;
	inline bool ShowWeaponESP = true;
	inline bool ShowEyeRay = true;
	inline bool ShowPlayerName = true;
	inline bool DrawFov = true;
	inline bool ShowDistance = true;
	inline bool ShowHealthNum = true;
	inline bool ArmorBar = true;
	inline bool ShowArmorNum = true;
	inline bool ShowHeadBox = true;
	inline bool ShowPreview = true;
	inline bool VisibleCheck = true;
	inline bool FilledBox = false;
	inline bool FilledVisBox = false;
	inline bool MultiColor = false;
	inline bool OutLine = true;
	inline bool ShowIsScoped = true;
	inline bool ShowIsBlind = true;
	inline bool FlashCheck = false;
	// 0: normal 1: Corner
	inline int BoxType = 0;
	inline float BoxRounding = 5;
	inline bool ShowLineToEnemy = false;
	inline ImColor LineToEnemyColor = ImColor(59, 71, 148, 180);
	// 0: Top 1: Center 2: Bottom
	inline int LinePos = 0;

	inline ImColor BoneColor = ImColor(131, 137, 150, 180);
	inline ImColor BoxColor = ImColor(59, 71, 148, 180);
	inline ImColor EyeRayColor = ImColor(0, 98, 98, 255);
	inline ImColor HeadBoxColor = ImColor(131, 137, 150, 180);
	inline ImColor VisibleColor = ImColor(59, 71, 148, 180);
	inline ImColor FilledColor = ImColor(59, 71, 148, 128);
	inline ImColor BoxFilledVisColor = ImColor(0, 98, 98, 128);
	inline ImColor FilledColor2 = ImColor(59, 71, 148, 128);

    inline bool ShowOutOfFOVArrow = false;
    inline float OutOfFOVRadiusFactor = 0.45f;
    inline ImColor OutOfFOVArrowColor = ImColor(255, 180, 50, 230);

	inline bool EnemySound = false;
	inline ImColor EnemySoundColor = ImColor(255, 255, 255, 255);
	
	inline bool ShowWeaponInfo = true;
	inline bool ShowBombCarrier = true;
	
	inline bool ShowArmorBar = true;
}

//namespace CrosshairsCFG
//{
//	inline bool ShowCrossHair = false;
//
//	inline float CrossHairSize = 75;
//	inline bool drawDot = true;
//	inline bool drawCrossline = true;
//	inline bool tStyle = false;
//	inline bool drawCircle = false;
//	inline bool drawOutLine = true;
//	inline bool DynamicGap = false;
//	inline bool TeamCheck = true;
//
//	inline int crosshairPreset = 0;
//	inline int Gap = 8;
//	inline int HorizontalLength = 6;
//	inline int VerticalLength = 6;
//	inline int Thickness = 1.0f;
//	inline float DotSize = 1.0f;
//	inline float CircleRadius = 3.f;
//
//	inline bool isAim = false;
//	inline bool isJump = false;
//
//	inline ImColor CrossHairColor = ImColor(0, 255, 0, 255);
//	inline ImColor TargetedColor = ImColor(255, 0, 0, 255);
// 
// inline bool TargetingCrosshairs = false;
//}

namespace RadarCFG
{
	inline bool ShowRadar = false;
	inline float RadarRange = 125;
	inline float RadarPointSizeProportion = 1.f;
	inline bool ShowRadarCrossLine = false;
	inline ImColor RadarCrossLineColor = ImColor(131, 137, 150, 180);
	// 0: circle 1: arrow 2: circle with arrow
	inline int RadarType = 2;
	inline float Proportion = 2700.f;
	inline bool customRadar = false;
	inline float RadarBgAlpha = 0.1f;
}

namespace AimbotCFG
{
    inline struct {
        bool AimbotEnabled = false;
        bool ToggleMode = false;
        int AimbotKey = VK_RBUTTON;
        bool SilentAim = false;
        float FOVRadius = 120.0f;
        float FOVRadiusMin = 10.0f;
        float FOVRadiusMax = 180.0f;
        bool ShowFov = true;
        bool DrawAimbotFOV = true;
        ImU32 FOVCircleColor = ImColor(255, 255, 255, 180);
        float FOVCircleThickness = 1.0f;
        bool VisibleCheck = true;
        bool TeamCheck = true;
        float MaxDistance = 5000.0f;
        int AimBone = 0;
        bool TargetHeadOnly = false;
        bool StopWhenNoHead = false;
        bool SmoothAim = true;
        float SmoothValue = 0.5f;
        bool RCS = true;
        float RCSStrength = 1.0f;
        float Sensitivity = 1.0f;
        int AutoFireMode = 0;
        bool TriggerBot = false;
        int TriggerDelay = 50;
        
        // Advanced features
        bool RealTraceLine = true;
        bool PingPrediction = true;
        float PingPredictionAmount = 1.0f;
        bool HitboxBasedTargeting = false;
        int HitboxPriority = 0; // 0: Head, 1: Chest, 2: Stomach, 3: Auto
        bool ResolverEnabled = false;
        int ResolverMode = 0; // 0: Basic, 1: Advanced, 2: Aggressive
        bool CreateMoveHook = true;
        bool SilentAimInCreateMove = true;
        bool TriggerBotInCreateMove = false;
    } AimbotCFG;
}

namespace MiscCFG
{
	inline bool BunnyHop = false;
	inline bool WaterMark = true;
	inline bool SniperCrosshair = true;
	inline ImColor SniperCrosshairColor = ImColor(32, 178, 170, 255);
	inline int	HitSound = 0;
	inline bool HitMarker = false;
	inline bool bmbTimer = true;
	inline bool FastStop = false;
	inline int FastStopDelay = 120;
	inline int FastStopOffset = 0;
	inline bool SpecList = true;
	inline ImColor BombTimerCol = ImColor(131, 137, 150, 255);
	inline bool ShowHeadShootLine = false;
	inline ImColor HeadShootLineColor = ImColor(131, 137, 150, 200);

    inline bool AutoAccept = false;

    inline bool AutoKnife = false;
    inline float AutoKnifeDistance = 70.0f;

    inline bool AutoZeus = false;
    inline float AutoZeusDistance = 130.0f;

    inline bool AntiAFKKick = false;
    inline bool AntiFlash = false;
    inline bool FatalitySound = false;
    
    // Auto-Shop settings
    inline bool AutoShop = false;
    inline bool BuyAK47 = false;
    inline bool BuyM4A4 = false;
    inline bool BuyM4A1S = false;
    inline bool BuyAWP = false;
    inline bool BuyDeagle = false;
    inline bool BuyFlash = false;
    inline bool BuyHE = false;
    inline bool BuySmoke = false;
    inline bool BuyMolotov = false;
    inline bool BuyKevlar = false;
    inline bool BuyHelmet = false;
    inline bool BuyDefuse = false;
    inline int AutoShopKey = VK_F6;  // F6 key by default
    
    // Wallbang Indicator settings
    inline bool WallbangIndicator = false;
    inline float WallbangSensitivity = 0.5f;  // 0.0 to 1.0
    inline ImColor WallbangColorPenetrable = ImColor(255, 0, 0, 255);  // Red - can penetrate
    inline ImColor WallbangColorNotPenetrable = ImColor(0, 255, 0, 255);  // Green - cannot penetrate
}

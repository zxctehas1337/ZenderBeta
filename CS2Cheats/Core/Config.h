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
    // LEGIT CONFIG
    inline struct {
        bool AimbotEnabled = false;
        bool SilentAim = false;
        float FOVRadius = 15.0f;
        float FOVRadiusMin = 5.0f;
        float FOVRadiusMax = 30.0f;
        bool ShowFov = true;
        bool DrawAimbotFOV = true;
        ImU32 FOVCircleColor = ImColor(255, 255, 255, 180);
        float FOVCircleThickness = 1.0f;
        bool VisibleCheck = true;
        bool TeamCheck = true;
        float MaxDistance = 500.0f;
        int AimBone = 0;
        bool TargetHeadOnly = true;
        bool StopWhenNoHead = true;
        bool SmoothAim = true;
        float SmoothValue = 0.85f;
        bool RCS = false;
        float RCSStrength = 0.5f;
        float Sensitivity = 1.0f;
        int AutoFireMode = 0;
        bool TriggerBot = false;
        int TriggerDelay = 150;
        
        // Advanced features
        bool RealTraceLine = true;
        bool PingPrediction = false;
        float PingPredictionAmount = 0.3f;
        bool HitboxBasedTargeting = false;
        int HitboxPriority = 0;
        bool ResolverEnabled = false;
        int ResolverMode = 0;
        bool CreateMoveHook = false;
        bool SilentAimInCreateMove = false;
        bool TriggerBotInCreateMove = false;
    } LegitCFG;

    // SEMIRAGE CONFIG
    inline struct {
        bool AimbotEnabled = false;
        bool SilentAim = true;
        float FOVRadius = 45.0f;
        float FOVRadiusMin = 20.0f;
        float FOVRadiusMax = 80.0f;
        bool ShowFov = true;
        bool DrawAimbotFOV = true;
        ImU32 FOVCircleColor = ImColor(255, 200, 0, 180);
        float FOVCircleThickness = 1.5f;
        bool VisibleCheck = true;
        bool TeamCheck = true;
        float MaxDistance = 1500.0f;
        int AimBone = 0;
        bool TargetHeadOnly = false;
        bool StopWhenNoHead = false;
        bool SmoothAim = true;
        float SmoothValue = 0.4f;
        bool RCS = true;
        float RCSStrength = 0.8f;
        float Sensitivity = 1.0f;
        int AutoFireMode = 1;
        bool TriggerBot = true;
        int TriggerDelay = 80;
        
        // Advanced features
        bool RealTraceLine = true;
        bool PingPrediction = true;
        float PingPredictionAmount = 0.7f;
        bool HitboxBasedTargeting = true;
        int HitboxPriority = 0;
        bool ResolverEnabled = true;
        int ResolverMode = 1;
        bool CreateMoveHook = true;
        bool SilentAimInCreateMove = true;
        bool TriggerBotInCreateMove = false;
    } SemiRageCFG;

    // RAGE CONFIG
    inline struct {
        bool AimbotEnabled = false;
        bool SilentAim = true;
        float FOVRadius = 180.0f;
        float FOVRadiusMin = 90.0f;
        float FOVRadiusMax = 180.0f;
        bool ShowFov = false;
        bool DrawAimbotFOV = false;
        ImU32 FOVCircleColor = ImColor(255, 0, 0, 180);
        float FOVCircleThickness = 2.0f;
        bool VisibleCheck = false;
        bool TeamCheck = true;
        float MaxDistance = 10000.0f;
        int AimBone = 0;
        bool TargetHeadOnly = false;
        bool StopWhenNoHead = false;
        bool SmoothAim = false;
        float SmoothValue = 0.0f;
        bool RCS = true;
        float RCSStrength = 2.0f;
        float Sensitivity = 1.0f;
        int AutoFireMode = 2;
        bool TriggerBot = true;
        int TriggerDelay = 10;
        
        // Advanced features
        bool RealTraceLine = true;
        bool PingPrediction = true;
        float PingPredictionAmount = 1.5f;
        bool HitboxBasedTargeting = true;
        int HitboxPriority = 0;
        bool ResolverEnabled = true;
        int ResolverMode = 2;
        bool CreateMoveHook = true;
        bool SilentAimInCreateMove = true;
        bool TriggerBotInCreateMove = true;
        
        // RAGE SPECIFIC
        bool ForceHeadshot = true;
        bool IgnoreSmoke = true;
        bool IgnoreFlash = true;
        bool AutoWall = true;
        float MinDamage = 20.0f;
        bool AimThroughWalls = true;
        float WallPenetrationPower = 2.0f;
        bool PerfectSilent = true;
        bool NoSpread = true;
        bool InstantHit = true;
        bool Multipoint = true;
        float MultipointScale = 0.8f;
    } RageCFG;

    // Current active config
    enum ConfigMode {
        LEGIT = 0,
        SEMIRAGE = 1,
        RAGE = 2
    };
    inline int CurrentConfig = LEGIT;

    // Get current config reference
    inline auto& GetCurrentConfig() {
        switch (CurrentConfig) {
            case LEGIT: return LegitCFG;
            case SEMIRAGE: return SemiRageCFG;
            case RAGE: return RageCFG;
            default: return LegitCFG;
        }
    }

    // Legacy compatibility
    inline auto& AimbotCFG = GetCurrentConfig();
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

    // Grenade ESP settings
    inline bool GrenadeESP = true;
    inline bool ShowGrenadeTrajectory = true;
    inline bool ShowGrenadeRadius = true;
    inline bool ShowGrenadeWarning = true;
    inline bool ShowGrenadeTimer = true;
    inline bool ShowGrenadeName = true;
    inline float GrenadeWarningDistance = 500.0f;
    inline float GrenadeWarningTime = 3.0f;
    inline ImColor GrenadeHEColor = ImColor(255, 100, 100, 200);
    inline ImColor GrenadeMolotovColor = ImColor(255, 150, 50, 200);
    inline ImColor GrenadeFlashColor = ImColor(200, 200, 255, 200);
    inline ImColor GrenadeSmokeColor = ImColor(150, 150, 150, 200);
    inline ImColor GrenadeDangerColor = ImColor(255, 0, 0, 255);
    inline ImColor GrenadeTrajectoryColor = ImColor(255, 255, 255, 150);

    // Anti-Aim (Крутилка) settings
    inline bool AntiAimEnabled = false;
    inline int AntiAimMode = 0; // 0: Static, 1: Jitter, 2: Spin, 3: Random
    inline float AntiAimPitch = -89.0f; // -89 (up), 0 (center), 89 (down)
    inline float AntiAimYawOffset = 180.0f; // 180 (backward), 90 (left), -90 (right)
    inline float AntiAimSpinSpeed = 5.0f; // Speed for spin mode
    inline float AntiAimJitterRange = 30.0f; // Jitter range in degrees
    inline bool AntiAimAtTargets = false; // Aim at enemies when visible
    inline bool AntiAimOnMove = true; // Only activate when moving
    inline bool AntiAimOnKey = false; // Activate on key press
    inline int AntiAimKey = VK_LSHIFT; // Key to toggle anti-aim
    inline bool AntiAimReal = false; // Real angle (for desync)
    inline bool AntiAimFake = true; // Fake angle (for resolver bypass)
    inline float AntiAimFakeOffset = 30.0f; // Offset for fake angle
    inline bool AntiAimLBYBreaker = true; // Break LBY for anti-resolver
    inline float AntiAimLBYOffset = 120.0f; // LBY breaker offset
}

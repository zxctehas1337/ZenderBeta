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
    // Base struct for all aimbot configs
    struct AimbotConfigBase {
        // Main toggles
        bool Enabled = false;
        bool AimbotEnabled = false;
        bool TriggerBot = false;
        bool Aimlock = false;
        bool SilentAim = false;
        
        // Aim settings
        enum AimBone {
            HEAD = 8,        // Updated for CS2 2025 - head bone
            NECK = 7,        // neck_0 bone  
            CHEST_UPPER = 5, // spine_1 bone
            CHEST_LOWER = 4, // spine_2 bone
            STOMACH = 3,     // spine_3 bone
            PELVIS = 2       // pelvis bone
        };
        
        int AimBone = HEAD;
        float FOV = 15.0f;           // 1-180 degrees
        float Smooth = 2.0f;         // 0.1-10.0
        float AimSpeed = 5.0f;       // 1-20
        float RecoilControl = 0.5f;  // 0-1
        
        // Visual settings
        bool DrawFOVCircle = true;
        ImColor FOVCircleColor = ImColor(255, 255, 255, 100);
        float FOVCircleThickness = 1.0f;
        bool DrawTargetLine = false;
        ImColor TargetLineColor = ImColor(255, 0, 0, 255);
        
        // Safety settings
        bool VisibleCheck = true;
        bool FlashCheck = true;
        bool SmokeCheck = true;
        bool TeamFilter = true;
        bool LegitMode = true;
        
        // Advanced settings
        enum TargetPriority {
            DISTANCE = 0,
            HEALTH,
            ANGLE,
            FOV_PRIORITY
        };
        
        int TargetPriority = DISTANCE;
        bool Prediction = false;
        float PredictionScale = 1.0f;
        bool AntiShake = true;
        float AntiShakeStrength = 0.5f;
        
        // Humanization
        bool Humanization = true;
        float RandomFactor = 0.1f;    // 0-1
        float DelayMin = 0.1f;        // seconds
        float DelayMax = 0.3f;        // seconds
        float HumanCurve = 1.0f;      // curve intensity
        
        // Trigger bot
        bool TriggerEnabled = false;
        float TriggerDelay = 0.0f;    // 0-1 second
        int TriggerKey = VK_LBUTTON;  // mouse button
        bool TriggerVisibleCheck = true;
        
        // Rage mode
        bool RageMode = false;
        float RageFOV = 180.0f;
        float RageSmooth = 0.1f;
        bool RageInstant = true;
        
        // Legit mode
        bool LegitModeEnabled = true;
        float LegitFOV = 10.0f;
        float LegitSmooth = 5.0f;
        bool LegitHumanization = true;
        
        // Kill delay
        bool KillDelay = false;
        float KillDelayTime = 0.5f;   // seconds
        
        // Adaptive settings
        bool AdaptiveFOV = false;
        float AdaptiveFOVScale = 1.0f;
        bool AdaptiveSpeed = false;
        float AdaptiveSpeedScale = 1.0f;
        
        // Legacy compatibility
        bool ToggleMode = false;
        int AimbotKey = VK_RBUTTON;
        float FOVRadius = 15.0f;
        float FOVRadiusMin = 5.0f;
        float FOVRadiusMax = 30.0f;
        bool ShowFov = true;
        bool DrawAimbotFOV = true;
        ImU32 FOVCircleColorLegacy = ImColor(255, 255, 255, 180);
        bool TeamCheck = true;
        float MaxDistance = 500.0f;
        bool TargetHeadOnly = true;
        bool StopWhenNoHead = true;
        bool SmoothAim = true;
        float SmoothValue = 0.85f;
        bool RCS = false;
        float RCSStrength = 0.5f;
        float Sensitivity = 1.0f;
        int AutoFireMode = 0;
        // TriggerDelay is already defined in base class as float
        
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
    };
    
    // LEGIT CONFIG
    inline struct LegitConfig : AimbotConfigBase {
        // Legit mode inherits all base settings, just override defaults
        LegitConfig() {
            AimbotEnabled = false;
            SilentAim = false;
            FOVRadius = 15.0f;
            FOVRadiusMin = 5.0f;
            FOVRadiusMax = 30.0f;
            ShowFov = true;
            DrawAimbotFOV = true;
            FOVCircleColorLegacy = ImColor(255, 255, 255, 180);
            FOVCircleThickness = 1.0f;
            VisibleCheck = true;
            TeamCheck = true;
            MaxDistance = 500.0f;
            AimBone = 0;
            TargetHeadOnly = true;
            StopWhenNoHead = true;
            SmoothAim = true;
            SmoothValue = 0.85f;
            RCS = false;
            RCSStrength = 0.5f;
            Sensitivity = 1.0f;
            AutoFireMode = 0;
            TriggerBot = false;
            TriggerDelay = 150;
            
            // Advanced features
            RealTraceLine = true;
            PingPrediction = false;
            PingPredictionAmount = 0.3f;
            HitboxBasedTargeting = false;
            HitboxPriority = 0;
            ResolverEnabled = false;
            ResolverMode = 0;
            CreateMoveHook = false;
            SilentAimInCreateMove = false;
            TriggerBotInCreateMove = false;
        }
    } LegitCFG;

    // SEMIRAGE CONFIG
    inline struct SemiRageConfig : AimbotConfigBase {
        // SemiRage mode inherits all base settings, just override defaults
        SemiRageConfig() {
            AimbotEnabled = false;
            SilentAim = true;
            FOVRadius = 45.0f;
            FOVRadiusMin = 20.0f;
            FOVRadiusMax = 80.0f;
            ShowFov = true;
            DrawAimbotFOV = true;
            FOVCircleColorLegacy = ImColor(255, 200, 0, 180);
            FOVCircleThickness = 1.5f;
            VisibleCheck = true;
            TeamCheck = true;
            MaxDistance = 1500.0f;
            AimBone = 0;
            TargetHeadOnly = false;
            StopWhenNoHead = false;
            SmoothAim = true;
            SmoothValue = 0.4f;
            RCS = true;
            RCSStrength = 0.8f;
            Sensitivity = 1.0f;
            AutoFireMode = 1;
            TriggerBot = true;
            TriggerDelay = 80;
            
            // Advanced features
            RealTraceLine = true;
            PingPrediction = true;
            PingPredictionAmount = 0.7f;
            HitboxBasedTargeting = true;
            HitboxPriority = 0;
            ResolverEnabled = true;
            ResolverMode = 1;
            CreateMoveHook = true;
            SilentAimInCreateMove = true;
            TriggerBotInCreateMove = false;
        }
    } SemiRageCFG;

    // RAGE CONFIG
    inline struct RageConfig : AimbotConfigBase {
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
        
        // Rage mode inherits all base settings, just override defaults
        RageConfig() {
            AimbotEnabled = false;
            SilentAim = true;
            FOVRadius = 180.0f;
            FOVRadiusMin = 90.0f;
            FOVRadiusMax = 180.0f;
            ShowFov = false;
            DrawAimbotFOV = false;
            FOVCircleColorLegacy = ImColor(255, 0, 0, 180);
            FOVCircleThickness = 2.0f;
            VisibleCheck = false;
            TeamCheck = true;
            MaxDistance = 10000.0f;
            AimBone = 0;
            TargetHeadOnly = false;
            StopWhenNoHead = false;
            SmoothAim = false;
            SmoothValue = 0.0f;
            RCS = true;
            RCSStrength = 2.0f;
            Sensitivity = 1.0f;
            AutoFireMode = 2;
            TriggerBot = true;
            TriggerDelay = 10;
            
            // Advanced features
            RealTraceLine = true;
            PingPrediction = true;
            PingPredictionAmount = 1.5f;
            HitboxBasedTargeting = true;
            HitboxPriority = 0;
            ResolverEnabled = true;
            ResolverMode = 2;
            CreateMoveHook = true;
            SilentAimInCreateMove = true;
            TriggerBotInCreateMove = true;
        }
    } RageCFG;

    // Current active config
    enum ConfigMode {
        LEGIT = 0,
        SEMIRAGE = 1,
        RAGE = 2
    };
    inline int CurrentConfig = LEGIT;

    // Get current config reference
    inline AimbotConfigBase* GetCurrentConfig() {
        switch (CurrentConfig) {
            case LEGIT: return static_cast<AimbotConfigBase*>(&LegitCFG);
            case SEMIRAGE: return static_cast<AimbotConfigBase*>(&SemiRageCFG);
            case RAGE: return static_cast<AimbotConfigBase*>(&RageCFG);
            default: return static_cast<AimbotConfigBase*>(&LegitCFG);
        }
    }

    // Legacy compatibility - use pointer dereference
    inline auto& AimbotCFG = *GetCurrentConfig();
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

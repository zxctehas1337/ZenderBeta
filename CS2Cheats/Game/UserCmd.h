#pragma once
#include "../OS-ImGui/OS-ImGui_Struct.h"

// CS2 2025 UserCmd structure (based on cs2-dumper July 2025)
struct CUserCmd {
    // Basic command info
    int command_number;
    int tick_count;
    
    // View angles - CRITICAL for Silent Aim
    Vec3 viewangles;
    Vec3 aimdirection;
    
    // Movement
    Vec3 forwardmove;
    Vec3 sidemove;
    Vec3 upmove;
    
    // Buttons - for auto fire
    uint64_t buttons;
    uint8_t impulse;
    
    // Weapon selection
    int weaponselect;
    int weaponsubtype;
    
    // Mouse movement
    int mousedx;
    int mousedy;
    
    // Prediction
    bool hasbeenpredicted;
    
    // Padding for alignment
    char pad[24];
};

// Button flags for CS2 2025
enum ButtonFlags : uint64_t {
    IN_ATTACK = (1ULL << 0),
    IN_JUMP = (1ULL << 1),
    IN_DUCK = (1ULL << 2),
    IN_FORWARD = (1ULL << 3),
    IN_BACK = (1ULL << 4),
    IN_USE = (1ULL << 5),
    IN_CANCEL = (1ULL << 6),
    IN_LEFT = (1ULL << 7),
    IN_RIGHT = (1ULL << 8),
    IN_MOVELEFT = (1ULL << 9),
    IN_MOVERIGHT = (1ULL << 10),
    IN_ATTACK2 = (1ULL << 11),
    IN_RUN = (1ULL << 12),
    IN_RELOAD = (1ULL << 13),
    IN_ALT1 = (1ULL << 14),
    IN_ALT2 = (1ULL << 15),
    IN_SCORE = (1ULL << 16),
    IN_SPEED = (1ULL << 17),
    IN_WALK = (1ULL << 18),
    IN_ZOOM = (1ULL << 19),
    IN_WEAPON1 = (1ULL << 20),
    IN_WEAPON2 = (1ULL << 21),
    IN_BULLRUSH = (1ULL << 22),
    IN_GRENADE1 = (1ULL << 23),
    IN_GRENADE2 = (1ULL << 24),
    IN_LOOKSPIN = (1ULL << 25),
};

// Global pointer to current user command (set by CreateMove hook)
extern CUserCmd* g_pCurrentCmd;
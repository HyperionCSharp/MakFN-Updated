#pragma once
#include <cstdint>

namespace offsets
{
    uint64_t game_instance = 0x1A8;
    uint64_t local_players = 0x38;
    uint64_t player_controller = 0x30;
    uint64_t acknowledged_pawn = 0x310; //mb 0x2A8
    uint64_t levels = 0x160;
    uint64_t persistent_level = 0x30;
    uint64_t player_camera_manager = 0x328;
    uint64_t aactors = 0x98;
    uint64_t actor_count = 0xA0;
    uint64_t is_reloading = 0x311;
    uint64_t display_name = 0x90;

    uint64_t revivefrom_dbno = 0x3EE8;
    uint64_t mesh = 0x2F0; //mb 288
    uint64_t relative_loc = 0x128;
    uint64_t player_state = 0x290;
    uint64_t team_index = 0x1048;
    uint64_t root_component = 0x188;
    uint64_t bADSWhileNotOnGround = 0x4525;

    uint64_t item_definition = 0x18;
    uint64_t primary_pick_up_item_entry = 0x2F8;
    uint64_t tier = 0x74;
    uint64_t balreadysearched = 0xf51;
    uint64_t is_dying = 0x680;
    uint64_t is_dbno = 0x6f2;
    uint64_t CurrWeapon = 0x7f0;
    uint64_t weapondata = 0x3d8;
    uint64_t LastFireTime = 0xa70;
    uint64_t LastFireTimeVerified = 0xa74;
    
}
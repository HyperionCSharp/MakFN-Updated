#pragma once

namespace settings {
    bool aimbot = false;
    bool fovcircle = true;
    int aim_delay = 0;
    bool prediction = false;
    bool magicbullet = true;
    bool playerweapon = false;
    bool aimline = false;
    bool enemyindicator = false;
    bool aim_delay_bool = false;
    int aimspeed = 2;
    int aimfov = 100;
    float lock = 0.0f;
    bool playerindicator = false;
    int hitbox = 0;
    bool controllersupport = false;
    bool weaponconfig = false;
    bool box = false;
    bool corner_box = false;
    bool threed_box = false;
    bool skel = false;
    bool vischeck = false;
    bool name = false;
    bool aimdowned = false;
    bool lines = false;
    bool playerchams = false;
    bool outline = false;
    bool playerweaponesp = false;
    bool show_head = true;
    bool crosshair = true;
    bool chestesp = false;
    bool ammoboxesp = false;
    bool lootesp = false;
    bool vehicle = false;
    bool performancemode = false;

    int boxMode = 0;
    int skelMode = 0;
    int lineMode = 0;

    bool kill_switch = false;

    int MaxSkeletonDrawDistance = 50;
    int MaxDistance = 300;
    bool radar;



    int msdelay = 1;
    int box_thick = 1;
    float crosshair_size = 10.f;

    bool flyzip;
    bool instantrevive;
    bool gliderexploit;
    bool aiminair;
    bool rapidfire;
    bool instantreload;
    bool hitboxextend;
    bool usernamehider;
    bool godmode;
    bool fovchanger;
    bool antiaim;

    int fovval = 90;
    int other_smooth = 4;
    int ar_smooth = 2;
    int shotty_smooth = 1;
    int smg_smooth = 5;

    int smg_hitbox = 68;
    int shotgun_hitbox = 68;
    int ar_hitbox = 68;
    int other_hitbox = 68;
}

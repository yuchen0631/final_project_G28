#ifndef DATACENTER_H_INCLUDED
#define DATACENTER_H_INCLUDED

#include <map>
#include <vector>
#include <allegro5/keycodes.h>
#include <allegro5/mouse.h>
#include "../shapes/Point.h"

#include "DialogManager.h"
#include "../FragmentObject.h"

// forward declarations（可以保留 Player / Level / Monster）
class Player;
class Level;
class Monster;
class Bullet;
class NPC;
class Boss;


class DataCenter
{
public:
    static DataCenter *get_instance() {
        static DataCenter DC;
        return &DC;
    }
    ~DataCenter();

public:
    double FPS;
    int window_width, window_height;
    int game_field_length;

    bool key_state[ALLEGRO_KEY_MAX];
    bool prev_key_state[ALLEGRO_KEY_MAX];
    Point mouse;
    bool mouse_state[ALLEGRO_MOUSE_MAX_EXTRA_AXES];
    bool prev_mouse_state[ALLEGRO_MOUSE_MAX_EXTRA_AXES];

public:
    Player *player;
    Level *level;

    std::vector<Monster*> monsters;
    std::vector<Bullet*> towerBullets;

    NPC* forest_npc = nullptr;
    DialogManager* dialog = nullptr;

    int slime_kill_count = 0;
    int slime_kill_target = 5;

    int fragment_collected = 0;
    int fragment_total = 5;

    // ★ 使用 FragmentObject，需要完整定義
    std::vector<FragmentObject*> fragments;

    bool stage_cleared = false;
public:
    Boss* final_boss = nullptr;
    bool boss_spawned = false;
    double boss_spawn_timer = 0;
    bool slimes_cleared_after_boss = false;


private:
    DataCenter();
};

#endif



#ifndef BOSS_H_INCLUDED
#define BOSS_H_INCLUDED

#include "../shapes/Rectangle.h"
#include <allegro5/allegro.h>

class Boss {
public:
    Boss(const char* img_path, double wx, double wy);

    void update();
    void draw();
    bool overlap_player();
    void take_damage(bool key_item);

public:
    ALLEGRO_BITMAP* img = nullptr;
    Rectangle* shape = nullptr;
    double x = 0, y = 0;

    int HP = 20;
    bool is_dead = false;

    double speed = 20;

    // ★ 攻擊冷卻
    double atk_cooldown = 0;
    double atk_interval = 1.0;   // 每 1 秒最多打一次玩家
};

#endif



#include "Monster.h"
#include "../data/DataCenter.h"
#include "../data/ImageCenter.h"
#include "../Player.h"
#include "../Level.h"
#include <cmath>
#include <cstdlib>
#include <allegro5/allegro_primitives.h>

Monster::Monster(double cx, double cy, const char* img_root) {
    sprintf(img_path, "%s", img_root);

    double w = 32;
    double h = 32;

    shape = new Rectangle(cx - w/2, cy - h/2, cx + w/2, cy + h/2);
}

Monster* Monster::create_monster(MonsterType type) {
    double x = rand() % 960;
    double y = rand() % 540;

    const char* img = nullptr;
    switch(type) {
        case MonsterType::SLIME:
            img = "./assets/image/slime.png";
            break;
    }

    return new Monster(x, y, img);
}

void Monster::update() {
    random_move();
    try_attack_player();

    ImageCenter* IC = ImageCenter::get_instance();
    ALLEGRO_BITMAP* bmp = IC->get(img_path);
    if (!bmp) return;

    int w = al_get_bitmap_width(bmp);
    int h = al_get_bitmap_height(bmp);

    double cx = shape->center_x();
    double cy = shape->center_y();

    shape->x1 = cx - w * 0.4;
    shape->y1 = cy - h * 0.4;
    shape->x2 = cx + w * 0.4;
    shape->y2 = cy + h * 0.4;
}

void Monster::random_move() {
    DataCenter* DC = DataCenter::get_instance();
    Level* LV = DC->level;

    double now = al_get_time();

    if (now - last_change_dir >= change_dir_interval) {
        last_change_dir = now;

        dir_x = (rand() % 3) - 1;
        dir_y = (rand() % 3) - 1;

        if (dir_x == 0 && dir_y == 0)
            dir_x = 1;
    }

    double len = std::sqrt(dir_x*dir_x + dir_y*dir_y);
    if (len == 0) len = 1;

    double vx = (dir_x / len) * speed / DC->FPS;
    double vy = (dir_y / len) * speed / DC->FPS;

    shape->update_center_x(shape->center_x() + vx);
    shape->update_center_y(shape->center_y() + vy);

    int world_w = LV->get_world_width();
    int world_h = LV->get_world_height();

    if (shape->x1 < 0 || shape->x2 > world_w) dir_x = -dir_x;
    if (shape->y1 < 0 || shape->y2 > world_h) dir_y = -dir_y;
}

void Monster::try_attack_player() {
    DataCenter* DC = DataCenter::get_instance();
    Player* p = DC->player;

    double mx = shape->center_x();
    double my = shape->center_y();
    double px = p->shape->center_x();
    double py = p->shape->center_y();

    double dist = std::sqrt((mx - px)*(mx - px) + (my - py)*(my - py));

    // ★★★ Debug：確認 slime 與玩家距離 ★★★
    debug_log("[SLIME CHECK] dist=%.2f  attack_range=%.2f  HP=%d\n",
              dist, attack_range, HP);

    if (dist <= attack_range)
        attack_player();
}

void Monster::attack_player() {
    double now = al_get_time();
    if (now - last_attack_time < attack_cooldown)
        return;

    last_attack_time = now;

    DataCenter* DC = DataCenter::get_instance();
    Player* p = DC->player;

    // ========= 無敵判定 =========
    if (p->invincible_timer > 0) {
        debug_log("[SLIME] Player is invincible. No damage.\n");
        return;
    }

    // ========= 扣血 =========
    int before = p->HP;
    p->HP -= 1;

    // ========= 給玩家無敵 1 秒 =========
    p->invincible_timer = p->invincible_duration;

    debug_log("[SLIME ATTACK] Player HP %d -> %d  (invincible for %.2f sec)\n",
              before, p->HP, p->invincible_duration);
}


void Monster::draw() {
    DataCenter* DC = DataCenter::get_instance();
    Level* LV = DC->level;

    ImageCenter* IC = ImageCenter::get_instance();
    ALLEGRO_BITMAP* bmp = IC->get(img_path);
    if (!bmp) return;

    double sx = shape->center_x() - LV->get_cam_x();
    double sy = shape->center_y() - LV->get_cam_y();

    al_draw_bitmap(
        bmp,
        sx - al_get_bitmap_width(bmp) / 2,
        sy - al_get_bitmap_height(bmp) / 2,
        0
    );
}

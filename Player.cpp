#include "Player.h"
#include "data/DataCenter.h"
#include "data/ImageCenter.h"
#include "Utils.h"
#include "Level.h"
#include <allegro5/allegro_primitives.h>
#include <cmath>

Player::Player(){
    shape = new Rectangle{32*1, 32*1, 32*1+32, 32*1+32};
    HP = 10;

    attack_cooldown = 0;
    attack_interval = 0.3; 
    v = 150;
}

void Player::update() {

    DataCenter *DC = DataCenter::get_instance();
    Level* LV = DC->level;

    // ⭐ 每幀降低攻擊冷卻
    if (attack_cooldown > 0)
        attack_cooldown -= 1.0 / DC->FPS;

    double movement = v / DC->FPS;

    double cx = shape->center_x();
    double cy = shape->center_y();

    double nx = cx;
    double ny = cy;

    // movement
    if (DC->key_state[ALLEGRO_KEY_W]) ny -= movement;
    if (DC->key_state[ALLEGRO_KEY_S]) ny += movement;
    if (DC->key_state[ALLEGRO_KEY_A]) nx -= movement;
    if (DC->key_state[ALLEGRO_KEY_D]) nx += movement;

    // collision X
    if (!LV->is_blocked_pixel(nx, cy))
        shape->update_center_x(nx);

    // collision Y
    if (!LV->is_blocked_pixel(cx, ny))
        shape->update_center_y(ny);

    // 移動 camera（用 Game 裡的 update 已經可以，這裡不會衝突）
    LV->update(
        shape->center_x() / TILE_SIZE,
        shape->center_y() / TILE_SIZE
    );

    // ⭐ 攻擊判定
    detect_and_attack();
}

void Player::detect_and_attack() {
    DataCenter *DC = DataCenter::get_instance();

    // ⭐ 改為「按下這一瞬間」
    if (!(DC->key_state[ALLEGRO_KEY_SPACE] && !DC->prev_key_state[ALLEGRO_KEY_SPACE]))
        return;

    if (attack_cooldown > 0)
        return;

    Point p(shape->center_x(), shape->center_y());

    for (Monster* m : DC->monsters) {
        if (!m) continue;

        Point mc(m->shape->center_x(), m->shape->center_y());
        double dist = Point::dist(p, mc);

        if (dist <= 30.0) {
            attack(m);
            attack_cooldown = attack_interval;
            return;
        }
    }
}

void Player::attack(Monster* target) {
    if (!target) return;
    debug_log("<Player> Attack!\n");
    target->HP -= 1;
}

void Player::draw() {

    DataCenter* DC = DataCenter::get_instance();
    Level* LV = DC->level;

    double sx = shape->center_x() - LV->get_cam_x();
    double sy = shape->center_y() - LV->get_cam_y();

    al_draw_filled_rectangle(
        sx - 10,
        sy - 20,
        sx + 10,
        sy,
        al_map_rgb(255, 255, 255)
    );
}

void Player::reset_position(double x, double y) {
    if (!shape) return;

    double w = shape->x2 - shape->x1;
    double h = shape->y2 - shape->y1;

    shape->x1 = x;
    shape->y1 = y;
    shape->x2 = x + w;
    shape->y2 = y + h;
}



#include "Player.h"
#include "data/DataCenter.h"
#include "data/ImageCenter.h"
#include "Level.h"
#include "monsters/Monster.h"
#include "monsters/Boss.h"
#include "Utils.h"
#include <allegro5/allegro_primitives.h>
#include <cmath>
#include <string>

Player::Player(){
    shape = new Rectangle(40, 40, 80, 80);
    HP = 10;

    attack_cooldown = 0;
    attack_interval = 0.3;

    invincible_timer = 0;
    invincible_duration = 1.0;

    v = 150;

    dir = PlayerDir::DOWN;
    state = PlayerState::IDLE;

    anim_frame = 0;
    anim_timer = 0;
    anim_interval = 0.15;

    load_images();
}

void Player::load_images() {
    ImageCenter* IC = ImageCenter::get_instance();
    idle_img[(int)PlayerDir::DOWN]  = IC->get("./assets/image/player/idle_down.png");
    idle_img[(int)PlayerDir::UP]    = IC->get("./assets/image/player/idle_up.png");
    idle_img[(int)PlayerDir::LEFT]  = IC->get("./assets/image/player/idle_left.png");
    idle_img[(int)PlayerDir::RIGHT] = IC->get("./assets/image/player/idle_right.png");

    const char* dir_name[4] = { "down", "up", "left", "right" };
    for (int d = 0; d < 4; d++) {
        for (int i = 0; i < 4; i++) {
            std::string path = "./assets/image/player/move_" + std::string(dir_name[d]) + "_" + std::to_string(i) + ".png";
            move_img[d][i] = IC->get(path.c_str());
        }
        for (int i = 0; i < 3; i++) {
            std::string path = "./assets/image/player/attack_" + std::string(dir_name[d]) + "_" + std::to_string(i) + ".png";
            attack_img[d][i] = IC->get(path.c_str());
        }
    }
}

void Player::update()
{
    DataCenter *DC = DataCenter::get_instance();

    //——— 玩家無敵時間倒數
    if (invincible_timer > 0) {
        invincible_timer -= 1.0 / DC->FPS;
        debug_log("[PLAYER] invincible=%.2f\n", invincible_timer);
    }
    //——— 攻擊冷卻
    if (attack_cooldown > 0)
        attack_cooldown -= 1.0 / DC->FPS;

    double movement = v / DC->FPS;
    bool moved = false;

    if (DC->key_state[ALLEGRO_KEY_W]) {
        shape->update_center_y(shape->center_y() - movement);
        dir = PlayerDir::UP;
        moved = true;
    }
    if (DC->key_state[ALLEGRO_KEY_S]) {
        shape->update_center_y(shape->center_y() + movement);
        dir = PlayerDir::DOWN;
        moved = true;
    }
    if (DC->key_state[ALLEGRO_KEY_A]) {
        shape->update_center_x(shape->center_x() - movement);
        dir = PlayerDir::LEFT;
        moved = true;
    }
    if (DC->key_state[ALLEGRO_KEY_D]) {
        shape->update_center_x(shape->center_x() + movement);
        dir = PlayerDir::RIGHT;
        moved = true;
    }

    if (state != PlayerState::ATTACK) {
        state = moved ? PlayerState::MOVE : PlayerState::IDLE;
    }

    detect_and_attack();
    update_animation();
}

void Player::detect_and_attack()
{
    DataCenter *DC = DataCenter::get_instance();

    if (!(DC->key_state[ALLEGRO_KEY_SPACE] && !DC->prev_key_state[ALLEGRO_KEY_SPACE]))
        return;

    if (attack_cooldown > 0)
        return;

    Point p(shape->center_x(), shape->center_y());

    // ———攻擊 Boss———
    if (DC->boss_spawned && DC->final_boss && !DC->final_boss->is_dead)
    {
        Boss* B = DC->final_boss;

        double dist = Point::dist(p, Point(B->shape->center_x(), B->shape->center_y()));

        if (dist <= 30.0)
        {
            bool hasItem = (DC->fragment_collected == DC->fragment_total);
            B->take_damage(hasItem);
            attack_cooldown = attack_interval;
            return;
        }
    }

    // ———攻擊小怪———
    state = PlayerState::ATTACK;
    anim_frame = 0;
    anim_timer = 0;

    attack_cooldown = attack_interval;

    for (Monster* m : DC->monsters)
    {
        if (!m) continue;

        double dist = Point::dist(p, Point(m->shape->center_x(), m->shape->center_y()));
        if (dist <= 30.0)
        {
            attack(m);
            return;
        }
    }
}

void Player::update_animation() {
    DataCenter* DC = DataCenter::get_instance();

    anim_timer += 1.0 / DC->FPS;
    if (anim_timer < anim_interval) return;

    anim_timer = 0;
    anim_frame++;

    if (state == PlayerState::ATTACK && anim_frame >= 3) {
        anim_frame = 0;
    }

    if (state == PlayerState::MOVE && anim_frame >= 4)
        anim_frame = 0;
}

void Player::attack(Monster* target)
{
    if (!target) return;

    debug_log("<Player> Attack!\n");
    target->HP -= 1;

    if (target->HP <= 0)
    {
        DataCenter* DC = DataCenter::get_instance();
        DC->slime_kill_count++;

        debug_log("Slime kill = %d / %d\n",
                  DC->slime_kill_count,
                  DC->slime_kill_target);
    }
}

void Player::draw()
{
    DataCenter* DC = DataCenter::get_instance();
    Level* LV = DC->level;

    double sx = shape->center_x() - LV->get_cam_x();
    double sy = shape->center_y() - LV->get_cam_y();

    ALLEGRO_BITMAP* img = nullptr;

    if (state == PlayerState::IDLE)
        img = idle_img[(int)dir];
    else if (state == PlayerState::MOVE)
        img = move_img[(int)dir][anim_frame];
    else if (state == PlayerState::ATTACK)
        img = attack_img[(int)dir][anim_frame];

    if (img) {
        al_draw_bitmap(img, sx - 20, sy - 20, 0);
    } else {
        //debug
        al_draw_rectangle(
            sx - 16, sy - 16,
            sx + 16, sy + 16,
            al_map_rgb(255, 0, 0),
            2
        );
    }
}

void Player::reset_position(double x, double y)
{
    if (!shape) return;

    double w = shape->x2 - shape->x1;
    double h = shape->y2 - shape->y1;

    shape->x1 = x;
    shape->y1 = y;
    shape->x2 = x + w;
    shape->y2 = y + h;
}

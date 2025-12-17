#include "Player.h"
#include "data/DataCenter.h"
#include "Level.h"
#include "monsters/Monster.h"
#include "monsters/Boss.h"
#include "Utils.h"
#include <allegro5/allegro_primitives.h>
#include <cmath>

Player::Player(){
    shape = new Rectangle(32, 32, 64, 64);
    HP = 10;

    attack_cooldown = 0;
    attack_interval = 0.3;

    invincible_timer = 0;
    invincible_duration = 1.0;

    v = 150;
}

void Player::update()
{
    DataCenter *DC = DataCenter::get_instance();
    Level* LV = DC->level;

    //——— 玩家無敵時間倒數
    if (invincible_timer > 0)
        invincible_timer -= 1.0 / DC->FPS;
        debug_log("[PLAYER] invincible=%.2f\n", invincible_timer);

    //——— 攻擊冷卻
    if (attack_cooldown > 0)
        attack_cooldown -= 1.0 / DC->FPS;

    double movement = v / DC->FPS;

    double cx = shape->center_x();
    double cy = shape->center_y();

    double nx = cx, ny = cy;

    if (DC->key_state[ALLEGRO_KEY_W]) ny -= movement;
    if (DC->key_state[ALLEGRO_KEY_S]) ny += movement;
    if (DC->key_state[ALLEGRO_KEY_A]) nx -= movement;
    if (DC->key_state[ALLEGRO_KEY_D]) nx += movement;

    if (!LV->is_blocked_pixel(nx, cy))
        shape->update_center_x(nx);

    if (!LV->is_blocked_pixel(cx, ny))
        shape->update_center_y(ny);

    LV->update(shape->center_x() / TILE_SIZE, shape->center_y() / TILE_SIZE);

    detect_and_attack();
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
    for (Monster* m : DC->monsters)
    {
        if (!m) continue;

        double dist = Point::dist(p, Point(m->shape->center_x(), m->shape->center_y()));
        if (dist <= 30.0)
        {
            attack(m);
            attack_cooldown = attack_interval;
            return;
        }
    }
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

    al_draw_filled_rectangle(
        sx - 10,
        sy - 20,
        sx + 10,
        sy,
        al_map_rgb(255, 255, 255)
    );
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



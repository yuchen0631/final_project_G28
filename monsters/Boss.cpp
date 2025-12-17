#include "Boss.h"
#include "../data/DataCenter.h"
#include "../data/ImageCenter.h"
#include "../Player.h"
#include "../Level.h"
#include <cmath>

Boss::Boss(const char* img_path, double wx, double wy)
{
    img = ImageCenter::get_instance()->get(img_path);

    int w = al_get_bitmap_width(img);
    int h = al_get_bitmap_height(img);

    x = wx;
    y = wy;

    shape = new Rectangle(wx + 10, wy + 10, wx + w - 10, wy + h - 10);

    atk_cooldown = 0;   // ★ 一定要初始化！
}


void Boss::update()
{
    if (is_dead) return;

    DataCenter* DC = DataCenter::get_instance();
    Player* P = DC->player;

    // 攻擊冷卻
    atk_cooldown -= 1.0 / DC->FPS;
    if (atk_cooldown < 0) atk_cooldown = 0;

    // 玩家無敵時間倒數
    if (P->invincible_timer > 0)
        P->invincible_timer -= 1.0 / DC->FPS;

    // === Debug ===
    debug_log("[BOSS] atk_cd=%.2f  Player invincible=%.2f  Player HP=%d\n",
              atk_cooldown, P->invincible_timer, P->HP);

    // 追蹤
    double dx = (P->shape->center_x() > shape->center_x()) ? 1 : -1;
    double dy = (P->shape->center_y() > shape->center_y()) ? 1 : -1;
    double dt = 1.0 / DC->FPS;
    x += dx * speed * dt;
    y += dy * speed * dt;


    shape->update_center_x(x + al_get_bitmap_width(img)/2);
    shape->update_center_y(y + al_get_bitmap_height(img)/2);

    // ====== 攻擊判定 ======
    if (overlap_player())
    {
        debug_log("[BOSS] Overlap detected\n");

        if (atk_cooldown == 0)
        {
            if (P->invincible_timer <= 0)
            {
                P->HP -= 2;
                P->invincible_timer = P->invincible_duration;

                debug_log("[BOSS HIT] Player HP=%d → Invincible for %.2f sec\n",
                          P->HP, P->invincible_duration);
            }
            else
            {
                debug_log("[BOSS] Player is invincible, no damage.\n");
            }

            atk_cooldown = atk_interval; // 例如 1 秒
        }
        else
        {
            debug_log("[BOSS] atk_cooldown>0, cannot attack.\n");
        }
    }
}


bool Boss::overlap_player()
{
    return shape->overlap(*DataCenter::get_instance()->player->shape);
}

void Boss::draw()
{
    if (is_dead) return;

    Level* LV = DataCenter::get_instance()->level;
    al_draw_bitmap(img, x - LV->get_cam_x(), y - LV->get_cam_y(), 0);
}


void Boss::take_damage(bool key_item)
{
    if (is_dead) return;

    int dmg = key_item ? 5 : 1;
    HP -= dmg;

    debug_log("[Boss Damaged] -%d HP=%d\n", dmg, HP);

    if (HP <= 0)
    {
        debug_log("=== BOSS DEFEATED ===\n");
        is_dead = true;
    }
}




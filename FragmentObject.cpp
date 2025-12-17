#include "FragmentObject.h"
#include "data/DataCenter.h"
#include "data/ImageCenter.h"
#include "Player.h"
#include "Level.h"
#include "Utils.h"
#include <cmath>

FragmentObject::FragmentObject(int wx_, int wy_, const char* img_path)
{
    // 儲存世界座標（不會再被覆寫）
    wx = wx_;
    wy = wy_;

    debug_log("[CONSTRUCTOR] wx=%d wy=%d\n", wx, wy);

    bmp = ImageCenter::get_instance()->get(img_path);

    tx = wx / TILE_SIZE;
    ty = wy / TILE_SIZE;

    // shape 用於碰撞，不用來當座標
    rectShape = new Rectangle(
        wx,
        wy,
        wx + draw_size,
        wy + draw_size
    );

    shape.reset(rectShape);
}

void FragmentObject::draw()
{
    if (taken) return;

    Level* LV = DataCenter::get_instance()->level;

    // ★ 使用我們真正的世界座標 wx, wy
    int sx = wx - LV->get_cam_x();
    int sy = wy - LV->get_cam_y();

    static int dbg = 0;
    if (dbg < 20) {
        debug_log("[GEM] world(%d,%d) screen(%d,%d)\n", wx, wy, sx, sy);
        dbg++;
    }

    int bw = al_get_bitmap_width(bmp);
    int bh = al_get_bitmap_height(bmp);

    // 縮放至 tile 大小
    al_draw_scaled_bitmap(
        bmp,
        0, 0, bw, bh,
        sx, sy,
        draw_size,
        draw_size,
        0
    );

    static int dbg2 = 0;
    if (dbg2 < 10) {
        debug_log("[CAM] cam_x=%d cam_y=%d\n", LV->get_cam_x(), LV->get_cam_y());
        dbg2++;
}
}

bool FragmentObject::is_adjacent_to_player()
{
    Player* P = DataCenter::get_instance()->player;

    int px = P->shape->center_x() / TILE_SIZE;
    int py = P->shape->center_y() / TILE_SIZE;

    debug_log("[ADJ?] Player(%d,%d) Gem(%d,%d)\n", px, py, tx, ty);
    return (abs(px - tx) + abs(py - ty) <= 1);
}





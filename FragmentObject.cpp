#include "FragmentObject.h"
#include "data/DataCenter.h"
#include "data/ImageCenter.h"
#include "Player.h"
#include "Level.h"
#include <cmath>
#include "Utils.h"


FragmentObject::FragmentObject(int wx, int wy, const char* img_path)
{
    debug_log("[CONSTRUCTOR] wx=%d wy=%d\n", wx, wy);
    bmp = ImageCenter::get_instance()->get(img_path);

    tx = wx / TILE_SIZE;
    ty = wy / TILE_SIZE;

    // ★ 讓 gem shape 是 32×32（tile 大小）
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
    static int draw_count = 0;

    if (taken) return;

    Level* LV = DataCenter::get_instance()->level;

    int sx = rectShape->x1 - LV->get_cam_x();
    int sy = rectShape->y1 - LV->get_cam_y();

    if (draw_count < 30) {  // 只印前 30 筆
        debug_log("[GEM #%d] wx=%d wy=%d  sx=%d sy=%d\n",
                  draw_count, rectShape->x1, rectShape->y1, sx, sy);
        draw_count++;
    }

    int bw = al_get_bitmap_width(bmp);
    int bh = al_get_bitmap_height(bmp);

    al_draw_scaled_bitmap(
        bmp,
        0, 0, bw, bh,
        sx, sy,
        draw_size,
        draw_size,
        0
    );
}


bool FragmentObject::is_adjacent_to_player()
{
    Player* P = DataCenter::get_instance()->player;

    int px = P->shape->center_x() / TILE_SIZE;
    int py = P->shape->center_y() / TILE_SIZE;

    return (abs(px - tx) + abs(py - ty) == 1);
}




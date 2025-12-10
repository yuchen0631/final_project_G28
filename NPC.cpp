#include "NPC.h"
#include "Level.h"
#include "Player.h"
#include "FragmentObject.h"
#include "data/DataCenter.h"
#include "data/ImageCenter.h"

NPC::NPC(double x, double y, const char* img_path) {
    img = ImageCenter::get_instance()->get(img_path);

    int w = al_get_bitmap_width(img);
    int h = al_get_bitmap_height(img);

    shape = new Rectangle(
        x - w / 2,
        y - h / 2,
        x + w / 2,
        y + h / 2
    );
}

void NPC::draw() {
    DataCenter* DC = DataCenter::get_instance();
    Level* LV = DC->level;

    double sx = shape->center_x() - LV->get_cam_x();
    double sy = shape->center_y() - LV->get_cam_y();

    al_draw_bitmap(
        img,
        sx - al_get_bitmap_width(img) / 2,
        sy - al_get_bitmap_height(img) / 2,
        0
    );
}

bool NPC::is_adjacent_to_player() {
    DataCenter* DC = DataCenter::get_instance();
    Player* P = DC->player;

    int px = P->shape->center_x() / TILE_SIZE;
    int py = P->shape->center_y() / TILE_SIZE;

    int nx = shape->center_x() / TILE_SIZE;
    int ny = shape->center_y() / TILE_SIZE;

    return (abs(px - nx) + abs(py - ny) == 1);
}



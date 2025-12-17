#include "DialogManager.h"
#include "DataCenter.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>

void DialogManager::show(const std::string& t)
{
    text = t;
    active = true;
}

void DialogManager::close()
{
    active = false;
}

// Zelda style：黑底 → 白框 → 白字
void DialogManager::draw()
{
    if (!active) return;

    DataCenter* DC = DataCenter::get_instance();

    float w = DC->window_width;
    float h = DC->window_height;

    float box_w = w - 80;
    float box_h = 160;
    float x1 = 40;
    float y1 = h - box_h - 40;
    float x2 = w - 40;
    float y2 = h - 40;

    // 半透明黑底
    al_draw_filled_rectangle(
        x1, y1, x2, y2,
        al_map_rgba(0, 0, 0, 200)
    );

    // 白框
    al_draw_rectangle(
        x1, y1, x2, y2,
        al_map_rgb(255, 255, 255),
        3
    );

    // 字體
    ALLEGRO_FONT* font = al_create_builtin_font();
    al_draw_multiline_text(
        font,
        al_map_rgb(255, 255, 255),
        x1 + 20,
        y1 + 20,
        box_w - 40,
        20,
        0,
        text.c_str()
    );
}

#ifndef FRAGMENT_OBJECT_H_INCLUDED
#define FRAGMENT_OBJECT_H_INCLUDED

#include "Object.h"
#include "shapes/Rectangle.h"
#include <allegro5/allegro.h>

class FragmentObject : public Object
{
public:
    FragmentObject(int wx, int wy, const char* img_path);

    void draw() override;
    bool is_adjacent_to_player();

public:
    ALLEGRO_BITMAP* bmp = nullptr;

    // ★ 真正的世界座標（永遠不要再用 rectShape 來當世界座標）
    int wx, wy;

    // tile 座標（用於判斷相鄰）
    int tx, ty;

    bool taken = false;

    int draw_size = 24; // gem 顯示大小

    // only for collision
    Rectangle* rectShape = nullptr;
};

#endif



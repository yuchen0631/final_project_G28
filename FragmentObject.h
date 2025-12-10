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
    Rectangle* rectShape = nullptr;
    int tx, ty;           // tile 座標
    bool taken = false;

    int draw_size = 48;   // ★ gem 顯示大小（可調成 48 或 64）
};

#endif


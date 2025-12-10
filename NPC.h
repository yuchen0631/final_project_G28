#ifndef NPC_H_INCLUDED
#define NPC_H_INCLUDED

#include "shapes/Rectangle.h"
#include <allegro5/allegro.h>

class NPC {
public:
    Rectangle* shape;
    ALLEGRO_BITMAP* img;

    NPC(double x, double y, const char* img_path);

    int tile_x, tile_y;


    bool is_adjacent_to_player();
    void draw();
};

#endif

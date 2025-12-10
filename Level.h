#ifndef LEVEL_H_INCLUDED
#define LEVEL_H_INCLUDED

#include <vector>
#include <string>
#include <allegro5/allegro.h>
#include "algif5/json.hpp"
using json = nlohmann::json;
#define TILE_SIZE 32

class Level {
public:
    Level();

    double camera_zoom;
    bool load_map(const char* path);
    void update(int px_tile, int py_tile);
    void draw();

    bool is_blocked_tile(int tx, int ty) const;
    bool is_blocked_pixel(double px, double py) const;

    int get_cam_x() const { return cam_x; }
    int get_cam_y() const { return cam_y; }

    int get_world_width() const { return pixel_w; }
    int get_world_height() const { return pixel_h; }


private:
    struct TilesetInfo {
        int firstgid = 0;
        int columns = 0;
        int tilecount = 0;
        int tilewidth = 0;
        int tileheight = 0;
        ALLEGRO_BITMAP* bmp = nullptr;
    };

    struct TileLayer {
        std::vector<int> data;
        bool visible = true;
        int offset_x = 0;
        int offset_y = 0;
    };

    // map info
    int map_w = 0;
    int map_h = 0;
    int pixel_w = 0;
    int pixel_h = 0;

    // camera
    int cam_x = 0;
    int cam_y = 0;

    // layers
    std::vector<TileLayer> layers;

    // tilesets
    std::vector<TilesetInfo> tilesets;

    // collision
    std::vector<std::vector<char>> blocked;

private:
    void init();
    void load_tilesets(const json& j, const std::string& base_dir);
    void build_blockmap(const json& j);
    void update_camera(int tx, int ty);

    void draw_layer(const TileLayer& L);
    void draw_tile_with_flip(ALLEGRO_BITMAP* tsbmp,
                             int sx, int sy,
                             int dx, int dy,
                             bool flip_h, bool flip_v, bool flip_d);
};

#endif




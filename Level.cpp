#include "Level.h"
#include "Utils.h"
#include <allegro5/allegro_image.h>
#include "data/DataCenter.h"
#include <fstream>
#include <iostream>

using namespace std;

static inline int strip_gid(int gid) {
    return gid & 0x1FFFFFFF;   // 去掉翻轉 bits
}

Level::Level() {
    camera_zoom = 1.0;

}

void Level::init() {
    layers.clear();
    tilesets.clear();
    blocked.clear();

    map_w = map_h = 0;
    pixel_w = pixel_h = 0;

    cam_x = cam_y = 0;
}

bool Level::load_map(const char* path) {
    init();

    ifstream f(path);
    if (!f.is_open()) {
        debug_log("<Level> Cannot open map: %s\n", path);
        return false;
    }

    json j;
    f >> j;

    map_w = j["width"];
    map_h = j["height"];
    pixel_w = map_w * TILE_SIZE;
    pixel_h = map_h * TILE_SIZE;

    debug_log("map_w=%d map_h=%d\n", map_w, map_h);

    string map_path = path;
    size_t slash = map_path.find_last_of("/\\");
    string base_dir = map_path.substr(0, slash);

    // 1. 載入 tileset
    load_tilesets(j, base_dir);

    // 2. 載入 tile layers
    for (auto& layer : j["layers"]) {
        if (layer["type"] != "tilelayer") continue;

        TileLayer L;
        L.visible = layer["visible"];
        L.offset_x = layer.value("offsetx", 0);
        L.offset_y = layer.value("offsety", 0);

        for (int gid : layer["data"]) {
            L.data.push_back(gid);
        }
        layers.push_back(L);
    }

    // 3. 建立碰撞
    build_blockmap(j);

    debug_log("<Level> collision map done.\n");

    return true;
}

//
// ---- Tileset Loader ----
//
void Level::load_tilesets(const json& j, const string& base_dir) {
    for (auto& ts : j["tilesets"]) {

        TilesetInfo info;
        info.firstgid = ts["firstgid"];

        string tsx_rel = ts["source"];
        string tsx_path = base_dir + "/" + tsx_rel;

        // 處理 "../"
        while (true) {
            size_t pos = tsx_path.find("/../");
            if (pos == string::npos) break;

            size_t prev = tsx_path.rfind("/", pos - 1);
            tsx_path.erase(prev, pos + 3 - prev);
        }

        ifstream in(tsx_path);
        if (!in.is_open()) {
            debug_log("<Level> cannot open TSX: %s\n", tsx_path.c_str());
            continue;
        }

        string xml((istreambuf_iterator<char>(in)),
                    istreambuf_iterator<char>());

        // 解析 PNG 路徑
        auto pos = xml.find("image source=");
        pos = xml.find("\"", pos);
        auto pos2 = xml.find("\"", pos + 1);
        string png_rel = xml.substr(pos + 1, pos2 - pos - 1);

        string tsx_dir = tsx_path.substr(0, tsx_path.find_last_of("/\\"));
        string png_full = tsx_dir + "/" + png_rel;

        info.bmp = al_load_bitmap(png_full.c_str());
        if (!info.bmp) {
            debug_log("<Level> cannot load PNG: %s\n", png_full.c_str());
            continue;
        }

        auto getInt = [&](string key) {
            auto p = xml.find(key);
            p = xml.find("\"", p);
            auto p2 = xml.find("\"", p + 1);
            return stoi(xml.substr(p + 1, p2 - p - 1));
        };

        info.columns    = getInt("columns=");
        info.tilecount  = getInt("tilecount=");
        info.tilewidth  = getInt("tilewidth=");
        info.tileheight = getInt("tileheight=");

        tilesets.push_back(info);
    }
}

//
// ---- Collision System ----
//
void Level::build_blockmap(const json& j) {
    blocked.assign(map_h, vector<char>(map_w, 0));

    static vector<string> bl = {"house","ob1","ob2","ob3","tree"};

    for (auto& layer : j["layers"]) {
        if (layer["type"] != "tilelayer") continue;

        string name = layer["name"];
        if (find(bl.begin(), bl.end(), name) == bl.end()) continue;

        const auto& data = layer["data"];
        int idx = 0;
        for (int y = 0; y < map_h; y++)
        for (int x = 0; x < map_w; x++, idx++)
            if (strip_gid(data[idx]) != 0)
                blocked[y][x] = 1;
    }
}

bool Level::is_blocked_tile(int tx, int ty) const {
    if (tx < 0 || ty < 0 || tx >= map_w || ty >= map_h)
        return true;
    return blocked[ty][tx] != 0;
}

bool Level::is_blocked_pixel(double wx, double wy) const {
    int tx = wx / TILE_SIZE;
    int ty = wy / TILE_SIZE;

    if (tx < 0 || tx >= map_w || ty < 0 || ty >= map_h)
        return true;

    return blocked[ty][tx] != 0;
}

//
// ---- Update Camera ----
//
void Level::update(int px_tile, int py_tile) {
    update_camera(px_tile, py_tile);
}

void Level::update_camera(int tx, int ty)
{
    DataCenter* DC = DataCenter::get_instance();

    double zoom = camera_zoom;
    double view_w = DC->window_width / zoom;
    double view_h = DC->window_height / zoom;

    // 把玩家放在視野中心
    cam_x = tx * TILE_SIZE - view_w / 2;
    cam_y = ty * TILE_SIZE - view_h / 2;

    // ---- Clamp Camera (避免鏡頭超出地圖) ----
    if (cam_x < 0) cam_x = 0;
    if (cam_y < 0) cam_y = 0;

    if (cam_x > pixel_w - view_w) cam_x = pixel_w - view_w;
    if (cam_y > pixel_h - view_h) cam_y = pixel_h - view_h;
}


//
// ---- Drawing ----
//
void Level::draw() {
    for (auto& L : layers)
        if (L.visible) draw_layer(L);
}

void Level::draw_tile_with_flip(ALLEGRO_BITMAP* bmp,
                                int sx, int sy,
                                int dx, int dy,
                                bool flip_h, bool flip_v, bool flip_d)
{
    ALLEGRO_TRANSFORM trans;
    al_identity_transform(&trans);

    // 移動到 tile 中心
    al_translate_transform(&trans, -sx/2, -sy/2);

    if (flip_d)
        al_rotate_transform(&trans, ALLEGRO_PI / 2);
    if (flip_h)
        al_scale_transform(&trans, -1, 1);
    if (flip_v)
        al_scale_transform(&trans, 1, -1);

    al_translate_transform(&trans, dx + sx/2, dy + sy/2);

    al_use_transform(&trans);

    al_draw_bitmap_region(bmp, 0, 0, sx, sy, 0, 0, 0);

    al_identity_transform(&trans);
    al_use_transform(&trans);
}

void Level::draw_layer(const TileLayer& L) {

    for (int y = 0; y < map_h; y++) {
        for (int x = 0; x < map_w; x++) {

            int gid_raw = L.data[y * map_w + x];
            if (gid_raw == 0) continue;

            bool flip_h = gid_raw & 0x80000000;
            bool flip_v = gid_raw & 0x40000000;
            bool flip_d = gid_raw & 0x20000000;
            int gid = strip_gid(gid_raw);

            // 找 tileset
            const TilesetInfo* ts = nullptr;
            for (auto& t : tilesets) {
                if (gid >= t.firstgid && gid < t.firstgid + t.tilecount) {
                    ts = &t;
                    break;
                }
            }
            if (!ts) continue;

            int local = gid - ts->firstgid;
            int src_x = (local % ts->columns) * ts->tilewidth;
            int src_y = (local / ts->columns) * ts->tileheight;

            int dx = x * TILE_SIZE - cam_x + L.offset_x;
            int dy = y * TILE_SIZE - cam_y + L.offset_y;

            if (flip_h || flip_v || flip_d)
            {
                ALLEGRO_BITMAP* region =
                    al_create_sub_bitmap(ts->bmp, src_x, src_y,
                                         ts->tilewidth, ts->tileheight);

                draw_tile_with_flip(region,
                                    ts->tilewidth, ts->tileheight,
                                    dx, dy,
                                    flip_h, flip_v, flip_d);

                al_destroy_bitmap(region);
            }
            else {
                al_draw_bitmap_region(
                    ts->bmp,
                    src_x, src_y,
                    ts->tilewidth, ts->tileheight,
                    dx, dy,
                    0
                );
            }
        }
    }
}



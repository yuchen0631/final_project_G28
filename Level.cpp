#include "Level.h"
#include <string>
#include "Utils.h"
#include "monsters/Monster.h"
#include "data/DataCenter.h"
#include <allegro5/allegro_primitives.h>
#include "shapes/Point.h"
#include "shapes/Rectangle.h"
#include <array>

using namespace std;

void Level::init() {
    level = -1;
    background_path.clear();
}

// 載入關卡
void Level::load_level(int lvl) {
    level = lvl;
    DataCenter* DC = DataCenter::get_instance();

    // 設定背景圖片（假設有命名規則 LEVEL1.png, LEVEL2.png）
    char buf[50];
    sprintf(buf, "./assets/level/LEVEL%d.png", lvl);
    background_path = buf;

    // 清空怪物列表
    DC->monsters.clear();

    // 一開始生成 10 隻小怪物
    for(int i = 0; i < 10; i++) {
        DC->monsters.emplace_back(Monster::create_monster(MonsterType::SLIME));
    }

    debug_log("<Level> load level %d, spawn 20 small monsters.\n", lvl);
}

// update 目前只更新怪物或其他需要的邏輯
void Level::update() {
    // 可以迴圈更新怪物位置
    // for(auto &monster : DataCenter::get_instance()->monsters) { monster.update(); }
}

// draw 畫背景圖片
void Level::draw() {
    if(background_path.empty()) return;

    ALLEGRO_BITMAP* bg = al_load_bitmap(background_path.c_str());
    if(bg) {
        al_draw_bitmap(bg, 0, 0, 0);
        al_destroy_bitmap(bg);
    }
}
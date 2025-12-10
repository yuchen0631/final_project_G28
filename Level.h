#ifndef LEVEL_H_INCLUDED
#define LEVEL_H_INCLUDED

#include <string>
#include <vector>
#include "monsters/Monster.h"

class Level {
public:
    Level() { init(); }

    void init();                 // 初始化
    void load_level(int lvl);    // 載入關卡（生成怪物）
    void update();               // 更新怪物或其他需要更新的東西
    void draw();                 // 畫出背景圖片（如果有）

    std::string get_background() const { return background_path; }

private:
    int level;
    std::string background_path; // 背景圖片路徑
};

#endif

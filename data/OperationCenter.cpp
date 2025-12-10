#include "OperationCenter.h"
#include "DataCenter.h"
#include "../monsters/Monster.h"
#include "../Player.h"

void OperationCenter::update() {
    _update_monster();
    _update_monster_player();
}

void OperationCenter::_update_monster() {
    DataCenter* DC = DataCenter::get_instance();
    auto& monsters = DC->monsters;

    for (Monster* m : monsters)
        m->update();
}

void OperationCenter::_update_monster_player() {
    DataCenter* DC = DataCenter::get_instance();
    auto& monsters = DC->monsters;
    Player* player = DC->player;

    for (size_t i = 0; i < monsters.size(); ++i) {
        Monster* m = monsters[i];

        if (m->HP <= 0) {
            delete m;
            monsters.erase(monsters.begin() + i);
            --i;
        }
    }
}

void OperationCenter::draw() {
    _draw_monster();
}

void OperationCenter::_draw_monster() {
    auto& monsters = DataCenter::get_instance()->monsters;
    for (Monster* m : monsters)
        m->draw();
}

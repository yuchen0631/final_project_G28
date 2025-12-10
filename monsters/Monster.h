#ifndef MONSTER_H_INCLUDED
#define MONSTER_H_INCLUDED

#include "../shapes/Rectangle.h"
#include "../Utils.h"

class Player;

enum class MonsterType {
    SLIME,
};
class Monster {
public:
    Rectangle* shape;
    int HP = 3;

    double speed = 40;                 
    double attack_range = 2.0;         
    double attack_cooldown = 0.5; // attack cooldown in seconds
    double last_attack_time = 0;

    double dir_x = 0;
    double dir_y = 0;
    double last_change_dir = 0;
    double change_dir_interval = 2.0;  // change direction every 2 seconds

    char img_path[80]; 

    Monster(double x, double y, const char* img_root);

    void update();
    void random_move();
    void try_attack_player();
    void attack_player();
    void draw();

	static Monster* create_monster(MonsterType type);
};

#endif
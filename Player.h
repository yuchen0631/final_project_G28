#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

#include "./shapes/Rectangle.h"
#include "./shapes/Point.h"
#include "./monsters/Monster.h"
class Player {
public:
    Player();

    void update();
    void draw();

    void detect_and_attack();
    void attack(Monster* target);

    Rectangle* shape;
    int HP;

    double v;   // movement speed (pixels per second)

    double attack_cooldown;  // attack cooldown timer
    double attack_interval;  // attack interval in seconds
};
#endif

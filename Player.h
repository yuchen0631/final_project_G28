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

    void reset_position(double x, double y);


    Rectangle* shape;
    int HP;

    double v;   // movement speed (pixels per second)

    double attack_cooldown;  // attack cooldown timer
    double attack_interval;  // attack interval in seconds
    
    double invincible_timer = 0;     // 玩家被擊中後的無敵時間
    double invincible_duration = 1.0;  // 1 秒無敵

};
#endif

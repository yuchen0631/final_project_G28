#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

#include "./shapes/Rectangle.h"
#include "./shapes/Point.h"
#include "./monsters/Monster.h"
#include <allegro5/allegro.h>

enum class PlayerDir {
    DOWN,
    UP,
    LEFT,
    RIGHT
};

enum class PlayerState {
    IDLE,
    MOVE,
    ATTACK
};

class Monster;
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

    void update_animation();
    void load_images();

    // state
    PlayerDir dir;
    PlayerState state;

    // animations
    ALLEGRO_BITMAP* idle_img[4];
    ALLEGRO_BITMAP* move_img[4][4];
    ALLEGRO_BITMAP* attack_img[4][3];

    int anim_frame;
    double anim_timer;
    double anim_interval;

    double v;   // movement speed (pixels per second)

    double attack_cooldown;  // attack cooldown timer
    double attack_interval;  // attack interval in seconds
    
    double invincible_timer = 0;     // 玩家被擊中後的無敵時間
    double invincible_duration = 1.0;  // 1 秒無敵

};
#endif

#include "Player.h"
#include "data/DataCenter.h"
#include "data/ImageCenter.h"
#include "Utils.h"
#include <allegro5/allegro_primitives.h>
#include <cmath>

Player::Player(){
    shape = new Rectangle{300, 300, 320, 320}; // initialize player position
    HP = 10; 
    attack_cooldown = 0;
    attack_interval = 0.3; // attack every 0.3 seconds
    v = 150; // movement speed (pixels per second)
}

void Player::update(){
    DataCenter *DC = DataCenter::get_instance();
    double movement = v / DC->FPS; // movement per frame

    // movement control
    if(DC->key_state[ALLEGRO_KEY_W]) shape->update_center_y(shape->center_y() - movement);
    if(DC->key_state[ALLEGRO_KEY_S]) shape->update_center_y(shape->center_y() + movement);
    if(DC->key_state[ALLEGRO_KEY_A]) shape->update_center_x(shape->center_x() - movement);
    if(DC->key_state[ALLEGRO_KEY_D]) shape->update_center_x(shape->center_x() + movement);

    // update attack cooldown
    if(attack_cooldown > 0) attack_cooldown -= 1.0 / DC->FPS;

    int screen_w = DC->window_width;  
    int screen_h = DC->window_height;
    if (shape->x1 < 0) {
        shape->update_center_x(shape->center_x() - shape->x1); // push slime back into screen
        //dir_x = -dir_x; // invert X direction (bounce back)
    }

    else if (shape->x2 > screen_w) {
        shape->update_center_x(shape->center_x() - (shape->x2 - screen_w));
        //dir_x = -dir_x; 
    }

    if (shape->y1 < 0) {
        shape->update_center_y(shape->center_y() - shape->y1); 
        //dir_y = -dir_y; 
    }

    else if (shape->y2 > screen_h) {
        shape->update_center_y(shape->center_y() - (shape->y2 - screen_h)); 
        //dir_y = -dir_y; 
    }

    detect_and_attack();
}

void Player::detect_and_attack(){
    DataCenter *DC = DataCenter::get_instance();

    if(!DC->key_state[ALLEGRO_KEY_SPACE]) return; 

    if(attack_cooldown > 0) return;

    Point player_center{
        shape->center_x(),
        shape->center_y()
    };

    for(Monster* m : DC->monsters){
        if(!m) continue;

        Point monster_center{
            m->shape->center_x(),
            m->shape->center_y()
        };

        double dist = Point::dist(player_center, monster_center);

        if(dist <= 30.0){
            attack(m);
            attack_cooldown = attack_interval; // reset CD
            return; // attack only one monster per input
        }
    }
}

void Player::attack(Monster* target) {
    if(!target) return;

    debug_log("<Player> Attack!\n");

    target->HP -= 1;

}

void Player::draw() {
    al_draw_filled_rectangle(
        shape->center_x() - 10,
        shape->center_y() - 20,
        shape->center_x() + 10,
        shape->center_y(),
        al_map_rgb(255, 255, 255)
    );
}

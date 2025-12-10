#include "UI.h"
#include "Utils.h"
#include "data/DataCenter.h"
#include "data/ImageCenter.h"
#include "data/FontCenter.h"
#include <algorithm>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>
#include "shapes/Point.h"
#include "shapes/Rectangle.h"
#include "Player.h"
#include "Level.h"

// fixed settings
constexpr char love_img_path[] = "./assets/image/love.png";
constexpr int love_img_padding = 5;

void
UI::init() {
	ImageCenter *IC = ImageCenter::get_instance();
	love = IC->get(love_img_path);
}

void
UI::update() {

}

void
UI::draw() {
	DataCenter *DC = DataCenter::get_instance();
	ALLEGRO_FONT* font = al_create_builtin_font();
	// 左上角擊殺計數
    al_draw_textf(font,
                  al_map_rgb(255, 255, 255),
                  10, 10, 0,
                  "Slime kill: %d / %d",
                  DC->slime_kill_count,
                  DC->slime_kill_target);
	// draw HP
	const int &game_field_length = DC->game_field_length;
	const int &player_HP = DC->player->HP;
	int love_width = al_get_bitmap_width(love);
	for(int i = 1; i <= player_HP; ++i) {
		al_draw_bitmap(love, game_field_length - (love_width + love_img_padding) * i, love_img_padding, 0);
	}
}

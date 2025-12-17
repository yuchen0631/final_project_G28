#include "DataCenter.h"
#include "DialogManager.h"
#include <cstring>
#include "../Level.h"
#include "../Player.h"
#include "../monsters/Monster.h"

// fixed settings
namespace DataSetting {
	constexpr double FPS = 60;
	constexpr int window_width = 960;
	constexpr int window_height = 540;
	constexpr int game_field_length = 600;
}

DataCenter::DataCenter() {

	debug_log("DataCenter created\n");
	this->FPS = DataSetting::FPS;
	this->window_width = DataSetting::window_width;
	this->window_height = DataSetting::window_height;
	this->game_field_length = DataSetting::game_field_length;

	memset(key_state, false, sizeof(key_state));
	memset(prev_key_state, false, sizeof(prev_key_state));
	memset(mouse_state, false, sizeof(mouse_state));
	memset(prev_mouse_state, false, sizeof(prev_mouse_state));

	mouse = Point(0, 0);

	player = new Player();
	level = new Level();

	forest_npc = nullptr;
    fragments.clear();

    slime_kill_count = 0;
    slime_kill_target = 5;
    fragment_collected = 0;
    fragment_total = 5;
	slimes_cleared_after_boss = false;


    stage_cleared = false;

	debug_log("DataCenter created\n");

	dialog = new DialogManager();
	debug_log("DialogManager allocated at %p\n", dialog);

}

DataCenter::~DataCenter() {
	delete player;
	delete level;
	for(Monster *&m : monsters) {
		delete m;
	}
}

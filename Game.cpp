#include "Game.h"
#include "Utils.h"
#include "data/DataCenter.h"
#include "data/OperationCenter.h"
#include "data/SoundCenter.h"
#include "data/ImageCenter.h"
#include "data/FontCenter.h"
#include "Player.h"
#include "Level.h"

#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_acodec.h>
#include <cstring>

// fixed paths
constexpr char game_icon_img_path[] = "./assets/image/game_icon.png";
constexpr char menu_bg_img_path[]  = "./assets/image/menu.jpg";
constexpr char game_start_sound_path[] = "./assets/sound/growl.wav";
constexpr char background_sound_path[] = "./assets/sound/BackgroundMusic.ogg";
constexpr char win_img_path[]  = "./assets/image/win.png";
constexpr char lose_img_path[] = "./assets/image/lose.png";
// ───────────────────────────────────────────────
// Draw UI button (for START / WIN / LOSE)
// ───────────────────────────────────────────────
void draw_button(const char *text, float cx, float cy) {

    const float w = 280;
    const float h = 70;

    // button background
    al_draw_filled_rectangle(
        cx - w/2, cy - h/2,
        cx + w/2, cy + h/2,
        al_map_rgba(50, 100, 200, 220)
    );

    // button border
    al_draw_rectangle(
        cx - w/2, cy - h/2,
        cx + w/2, cy + h/2,
        al_map_rgb(255, 255, 255),
        3
    );

    // draw text
    FontCenter *FC = FontCenter::get_instance();
    al_draw_text(
        FC->caviar_dreams[FontSize::LARGE],
        al_map_rgb(255, 255, 255),
        cx, cy - 20,
        ALLEGRO_ALIGN_CENTRE,
        text
    );
}

// ───────────────────────────────────────────────
// Utility: UI button hover region
// ───────────────────────────────────────────────
bool inside_button(float mx, float my, float cy) {
    float cx = DataCenter::get_instance()->window_width / 2;
    float w = 300, h = 70;
    return (mx >= cx - w/2 && mx <= cx + w/2 &&
            my >= cy - h/2 && my <= cy + h/2);
}

// ───────────────────────────────────────────────
// Constructor
// ───────────────────────────────────────────────
Game::Game(bool testMode) {
    DataCenter *DC = DataCenter::get_instance();
    GAME_ASSERT(al_init(), "failed to initialize allegro");

    // addons
    GAME_ASSERT(al_init_primitives_addon(), "fail primitives");
    GAME_ASSERT(al_init_font_addon(), "fail font");
    GAME_ASSERT(al_init_ttf_addon(), "fail ttf");
    GAME_ASSERT(al_init_image_addon(), "fail image");
    GAME_ASSERT(al_init_acodec_addon(), "fail acodec");
    
    if(testMode) {
        timer = nullptr;
        event_queue = nullptr;
        display = nullptr;
        return;
    }

    GAME_ASSERT(al_install_keyboard(), "keyboard fail");
    GAME_ASSERT(al_install_mouse(), "mouse fail");
    GAME_ASSERT(al_install_audio(), "audio fail");

    al_set_new_display_flags(ALLEGRO_WINDOWED | ALLEGRO_RESIZABLE);

    display = al_create_display(DC->window_width, DC->window_height);
    GAME_ASSERT(display, "display fail");
    
    timer = al_create_timer(1.0 / DC->FPS);
    event_queue = al_create_event_queue();
    GAME_ASSERT(timer && display && event_queue, "allegro init fail");

    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_mouse_event_source());
    al_register_event_source(event_queue, al_get_timer_event_source(timer));

    game_init();
}

Game::~Game() {
    if(display) al_destroy_display(display);
    if(timer) al_destroy_timer(timer);
    if(event_queue) al_destroy_event_queue(event_queue);
}

// ───────────────────────────────────────────────
// Initialization
// ───────────────────────────────────────────────
void Game::game_init() {

    DataCenter *DC = DataCenter::get_instance();
    debug_log("Game Init: Player = %p\n", DC->player);
    ImageCenter *IC = ImageCenter::get_instance();
    SoundCenter *SC = SoundCenter::get_instance();
    FontCenter *FC = FontCenter::get_instance();

    // icon
    game_icon = IC->get(game_icon_img_path);
    al_set_display_icon(display, game_icon);

    // events
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_mouse_event_source());
    al_register_event_source(event_queue, al_get_timer_event_source(timer));

    // init centers
    SC->init();
    FC->init();

    // UI
    ui = new UI();
    ui->init();

    // images
    background = IC->get(menu_bg_img_path);
    win_img = IC->get(win_img_path);
    lose_img = IC->get(lose_img_path);

    state = STATE::START;
    al_start_timer(timer);
}

// ───────────────────────────────────────────────
// Main Loop
// ───────────────────────────────────────────────
void Game::execute() {
    DataCenter *DC = DataCenter::get_instance();
    bool run = true;

    while(run) {
        al_wait_for_event(event_queue, &event);

        switch(event.type) {
            case ALLEGRO_EVENT_TIMER:
                run &= game_update();
                game_draw();
                break;
            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                run = false;
                break;
            case ALLEGRO_EVENT_KEY_DOWN:
                DC->key_state[event.keyboard.keycode] = true;
                break;
            case ALLEGRO_EVENT_KEY_UP:
                DC->key_state[event.keyboard.keycode] = false;
                break;
            case ALLEGRO_EVENT_MOUSE_AXES:
                DC->mouse.x = event.mouse.x;
                DC->mouse.y = event.mouse.y;
                break;
            case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
                DC->mouse_state[event.mouse.button] = true;
                break;
            case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
                DC->mouse_state[event.mouse.button] = false;
                break;
        }
    }
}

// ───────────────────────────────────────────────
// Update
// ───────────────────────────────────────────────
bool Game::game_update() {
    DataCenter *DC = DataCenter::get_instance();
    SoundCenter *SC = SoundCenter::get_instance();
    OperationCenter *OC = OperationCenter::get_instance();
    static ALLEGRO_SAMPLE_INSTANCE* bgm = nullptr;

    switch(state) {

    // ─────────── START MENU ───────────
    case STATE::START: {
        static bool played = false;
        if(!played) {
            SC->play(game_start_sound_path, ALLEGRO_PLAYMODE_ONCE);
            played = true;
        }

        float mx = DC->mouse.x;
        float my = DC->mouse.y;

        if(DC->mouse_state[1]) {
            if(inside_button(mx, my, 270)) {
                // load tile map ONCE
                if(!map_loaded) {
                    DC->level->load_map("./assets/map/forest.tmj");
                    map_loaded = true;
                }
                al_flush_event_queue(event_queue);
                al_rest(0.02);
                    al_flush_event_queue(event_queue);

                memset(DC->key_state, 0, sizeof(DC->key_state));
                memset(DC->prev_key_state, 0, sizeof(DC->prev_key_state));
                memset(DC->mouse_state, 0, sizeof(DC->mouse_state));
                memset(DC->prev_mouse_state, 0, sizeof(DC->prev_mouse_state));

                state = STATE::LEVEL;
            }
            if(inside_button(mx, my, 450)) return false;
        }
        break;
    }

    case STATE::LEVEL: {

        static bool spawned = false;
        if (!spawned) {
            debug_log("Spawning player & monsters...\n");

            DC->player->reset_position(32 * 20, 32 * 10);

            DC->monsters.clear();
            for (int i = 0; i < 10; i++) {
                DC->monsters.push_back(
                    new Monster(
                        32 * 20 + i * 30,   // 世界座標靠近玩家
                        32 * 10 + i * 20,
                        "./assets/image/slime.png"
                    )
                );
            }

            spawned = true;
        }
        // ─────────────────────────────────────────────


        // ─────────────────────────────────────────────
        // BGM
        // ─────────────────────────────────────────────
        static bool bgm_started = false;
        if (!bgm_started) {
            bgm = SC->play(background_sound_path, ALLEGRO_PLAYMODE_LOOP);
            bgm_started = true;
        }

        // 暫停
        if (DC->key_state[ALLEGRO_KEY_P] && !DC->prev_key_state[ALLEGRO_KEY_P]) {
            SC->toggle_playing(bgm);
            state = STATE::PAUSE;
        }


        // ─────────────────────────────────────────────
        // 更新 camera = player tile
        // ─────────────────────────────────────────────
        int tx = DC->player->shape->center_x() / TILE_SIZE;
        int ty = DC->player->shape->center_y() / TILE_SIZE;
        DC->level->update(tx, ty);


        // ─────────────────────────────────────────────
        // 更新玩家 & 怪物
        // ─────────────────────────────────────────────
        DC->player->update();
        OC->update();


        // ─────────────────────────────────────────────
        // 死亡判定
        // ─────────────────────────────────────────────
        if (DC->player->HP <= 0)
            state = STATE::LOSE;

        break;
    }



    // ─────────── PAUSE ───────────
    case STATE::PAUSE: {
        if(DC->key_state[ALLEGRO_KEY_P] && !DC->prev_key_state[ALLEGRO_KEY_P]) {
            SC->toggle_playing(bgm);
            state = STATE::LEVEL;
        }
        break;
    }


    // ─────────── LOSE ───────────
    case STATE::LOSE: {
        float mx = DC->mouse.x;
        float my = DC->mouse.y;

        if(DC->mouse_state[1]) {
            if(inside_button(mx, my, 385))
                state = STATE::START;
            if(inside_button(mx, my, 470))
                return false;
        }
        break;
    }


    // ─────────── WIN ───────────
    case STATE::WIN: {
        float mx = DC->mouse.x;
        float my = DC->mouse.y;

        if(DC->mouse_state[1]) {
            if(inside_button(mx, my, 370))
                state = STATE::START;
            if(inside_button(mx, my, 450))
                return false;
        }
        break;
    }
    }

    // update prev key/mouse
    memcpy(DC->prev_key_state, DC->key_state, sizeof(DC->key_state));
    memcpy(DC->prev_mouse_state, DC->mouse_state, sizeof(DC->mouse_state));

    return true;
}

// ───────────────────────────────────────────────
// Draw
// ───────────────────────────────────────────────
void Game::game_draw() {
    DataCenter *DC = DataCenter::get_instance();
    OperationCenter *OC = OperationCenter::get_instance();
    FontCenter *FC = FontCenter::get_instance();

    al_clear_to_color(al_map_rgb(80, 80, 80));

    switch(state) {

    case STATE::START: {
        al_draw_bitmap(background, 0, 0, 0);
        draw_button("ENTER", DC->window_width/2, 270);
        draw_button("EXIT", DC->window_width/2, 450);
        break;
    }

    case STATE::LEVEL: {
        DC->level->draw();
        DC->player->draw();
        ui->draw();
        OC->draw();
        break;
    }

    case STATE::PAUSE: {
        DC->level->draw();
        DC->player->draw();
        ui->draw();
        al_draw_filled_rectangle(0,0,DC->window_width,DC->window_height,
                                 al_map_rgba(0,0,0,120));
        al_draw_text(
            FC->caviar_dreams[FontSize::LARGE],
            al_map_rgb(255,255,255),
            DC->window_width/2, DC->window_height/2,
            ALLEGRO_ALIGN_CENTRE, "PAUSE");
        break;
    }

    case STATE::LOSE: {
        al_draw_bitmap(lose_img, 200, 20, 0);
        draw_button("BACK TO MENU", DC->window_width/2, 385);
        draw_button("EXIT", DC->window_width/2, 470);
        break;
    }

    case STATE::WIN: {
        al_draw_bitmap(win_img, 260, 20, 0);
        draw_button("BACK TO MENU", DC->window_width/2, 370);
        draw_button("EXIT", DC->window_width/2, 450);
        break;
    }
    }

    al_flip_display();
}


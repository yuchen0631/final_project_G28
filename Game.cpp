#include "Game.h"
#include "Utils.h"
#include "data/DataCenter.h"
#include "data/OperationCenter.h"
#include "data/SoundCenter.h"
#include "data/ImageCenter.h"
#include "data/FontCenter.h"
#include "Player.h"
#include "Level.h"
#include "NPC.h"
#include "FragmentObject.h"
#include "data/DialogManager.h"
#include "monsters/Boss.h"





#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_acodec.h>
#include <cstring>
std::string dialog_text = "";


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

void Game::spawn_fragments()
{
    debug_log("SPAWN fragments!!\n");
    DataCenter* DC = DataCenter::get_instance();

    // 清空舊資料（保險）
    for (auto* f : DC->fragments) delete f;
    DC->fragments.clear();

    const char* gem_path = "./assets/image/gem.png";

    struct GemPos { int x, y; };
    GemPos gempos[5] = {
        {4, 15},
        {15, 8},
        {23, 15},
        {17, 15},
        {20, 15}
    };

    for (int i = 0; i < 5; i++) {
        DC->fragments.push_back(
            new FragmentObject(
                gempos[i].x * TILE_SIZE,
                gempos[i].y * TILE_SIZE,
                gem_path
            )
        );
    }

    DC->fragment_total = 5;
    DC->fragment_collected = 0;
}

void show_dialog(const char* msg)
{
    dialog_text = msg;
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

    debug_log("Game Init Start\n");

    debug_log("display = %p\n", display);
    debug_log("event_queue = %p\n", event_queue);
    debug_log("timer = %p\n", timer);


    DataCenter *DC = DataCenter::get_instance();
    debug_log("player = %p\n", DC->player);
    debug_log("level = %p\n", DC->level);
    debug_log("npc = %p\n", DC->forest_npc);
    debug_log("fragments.size = %d\n", (int)DC->fragments.size());

    ImageCenter *IC = ImageCenter::get_instance();
    SoundCenter *SC = SoundCenter::get_instance();
    FontCenter *FC = FontCenter::get_instance();

    // ---------------------------------------------------
    // ★★★ 先初始化音效中心、字體中心（非常重要）
    // ---------------------------------------------------
    SC->init();
    FC->init();

    // ---------------------------------------------------
    // ★★★ 現在才可以載圖片和 UI（避免使用未載入字體）
    // ---------------------------------------------------

    // icon
    game_icon = IC->get(game_icon_img_path);

    // events
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_mouse_event_source());
    al_register_event_source(event_queue, al_get_timer_event_source(timer));

    // UI（此時 font 已載入，所以不會 crash）
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

    bool interact = DC->key_state[ALLEGRO_KEY_E] &&
                    !DC->prev_key_state[ALLEGRO_KEY_E];

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
                if(!map_loaded) {
                    DC->level->load_map("./assets/map/forest.tmj");
                    map_loaded = true;

                    // ★★★ NPC 一開始就生成 ★★★
                    DC->forest_npc = new NPC(
                        15 * TILE_SIZE,
                        15 * TILE_SIZE,
                        "./assets/image/npc.png"
                    );
                }

                // 清除按鍵
                al_flush_event_queue(event_queue);
                memset(DC->key_state, 0, sizeof(DC->key_state));
                memset(DC->prev_key_state, 0, sizeof(DC->prev_key_state));

                state = STATE::LEVEL;
            }
            if(inside_button(mx, my, 450))
                return false;
        }
        break;
    }

    // ─────────── LEVEL ───────────
    case STATE::LEVEL: {

        static bool spawned = false;

        if (!spawned) {
            DC->player->reset_position(32 * 20, 32 * 10);

            // 生成小怪
            DC->monsters.clear();
            for (int i = 0; i < 10; i++) {
                DC->monsters.push_back(
                    new Monster(
                        32 * 20 + i * 30,
                        32 * 10 + i * 20,
                        "./assets/image/slime.png"
                    )
                );
            }

            spawned = true;
        }

        // BGM
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

        // 更新 camera
        int tx = DC->player->shape->center_x() / TILE_SIZE;
        int ty = DC->player->shape->center_y() / TILE_SIZE;
        DC->level->update(tx, ty);

        // 更新玩家 & 怪物
        DC->player->update();
        OC->update();

        // =======================================================
        // ★ Step 1. 玩家走到指定 tile → 3 秒後生成 Boss
        // =======================================================
        int px_tile = DC->player->shape->center_x() / TILE_SIZE;
        int py_tile = DC->player->shape->center_y() / TILE_SIZE;

        // 假設 Boss 生成點是 (23, 5)
        if (!DC->boss_spawned && px_tile == 20 && py_tile == 1)
        {
            DC->boss_spawn_timer += 1.0 / DC->FPS;

            if (DC->boss_spawn_timer >= 3.0)
            {
                DC->final_boss = new Boss(
                    "./assets/image/monster/DemonNinja/DOWN_0.png",
                    25 * TILE_SIZE,
                    5 * TILE_SIZE
                );

                DC->boss_spawned = true;
                debug_log("=== BOSS SPAWNED ===\n");
            }
        }
        else {
            DC->boss_spawn_timer = 0;
        }


        // -------- 玩家擊殺達成 → 進入 WIN_HINT 視窗 --------
        if (!DC->stage_cleared &&
            DC->slime_kill_count >= DC->slime_kill_target)
        {
            DC->stage_cleared = true;
            state = STATE::WIN_HINT;
        }

        // -------- NPC互動（根據任務階段）---------
        if (DC->forest_npc && DC->forest_npc->is_adjacent_to_player() && interact) {

            if (DC->slime_kill_count < DC->slime_kill_target) {
        
            show_dialog("There seem to be a lot more little slimes in the village lately...\nBut I'm missing the key evidence.");
            state = STATE::DIALOG;
            }
            else if (DC->fragment_collected < DC->fragment_total) {

                show_dialog("This is the clue I've been looking for!\nCould you help me collect five magic gems?");
                if (DC->fragments.empty()) spawn_fragments();
                state = STATE::DIALOG;
            }
            else {

                show_dialog("Great! With this, I can craft a weapon to seal the Great Demon King.\nHead north!");
                state = STATE::DIALOG;
            }
        }


        // -------- 玩家拾取碎片 --------
        for (auto* f : DC->fragments) {
            if (!f->taken && f->is_adjacent_to_player() && interact) {
                f->taken = true;
                DC->fragment_collected++;
            }
        }
        // =======================================================
        // ★ Step 2. Boss 更新（追蹤 + 攻擊）
        // =======================================================
        if (DC->boss_spawned && DC->final_boss)
        {
            // ---- A. 只在 Boss 剛剛出現時清除 slime ----
            if (!DC->slimes_cleared_after_boss)
            {
                for (Monster* m : DC->monsters)
                delete m;
                DC->monsters.clear();

                DC->slimes_cleared_after_boss = true;  // ★ 確保只執行一次
                debug_log("=== SLIMES CLEARED (Boss Phase Begin) ===\n");
            }

            // ---- B. 更新 Boss 行為（真正攻擊玩家的程式在 Boss.cpp 裡）----
            DC->final_boss->update();

            // ---- C. 檢查 Boss 是否死亡 ----
            if (DC->final_boss->is_dead)
            {
                debug_log("=== BOSS DEFEATED ===\n");
                state = STATE::WIN;
            }
}


        // 死亡
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

    // ─────────── WIN_HINT（殺滿小怪後）───────────
    case STATE::WIN_HINT: {

        if (DC->key_state[ALLEGRO_KEY_ENTER] &&
            !DC->prev_key_state[ALLEGRO_KEY_ENTER])
        {
            // 清空全部史萊姆
            for (Monster* m : DC->monsters) delete m;
            DC->monsters.clear();

            state = STATE::LEVEL;
        }
        break;
    }
    case STATE::DIALOG:{

        if (DC->key_state[ALLEGRO_KEY_ENTER] &&
            !DC->prev_key_state[ALLEGRO_KEY_ENTER])
        {
            dialog_text = "";     // 清空
            state = STATE::LEVEL; 
        }
        break;
    }


    // ─────────── LOSE ───────────
    case STATE::LOSE:
        // 原本你的程式碼
        break;

    } // ← switch 結束

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
    int tx = 20;
    int ty = 1;
    int px = tx * TILE_SIZE - DC->level->get_cam_x();
        int py = ty * TILE_SIZE - DC->level->get_cam_y();

    al_draw_rectangle(px, py, px + TILE_SIZE, py + TILE_SIZE,
                    al_map_rgb(255, 0, 0), 3);


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

        if (DC->forest_npc)
            DC->forest_npc->draw();

        for (auto* f : DC->fragments)
            f->draw();

        // =======================================================
        // ★ Step 3. 畫 Boss
        // =======================================================
        if (DC->boss_spawned && DC->final_boss)
            DC->final_boss->draw();

        // === DEBUG: draw tile (21,2) ===
        {
            DataCenter* DC = DataCenter::get_instance();
            Level* LV = DC->level;

            int tx = 20;
            int ty = 1;

            int world_x = tx * TILE_SIZE;
            int world_y = ty * TILE_SIZE;

            int cam_x = LV->get_cam_x();
            int cam_y = LV->get_cam_y();

            int sx = world_x - cam_x;
            int sy = world_y - cam_y;

            // debug print
            debug_log("[MARK] world(%d,%d) screen(%d,%d) cam(%d,%d)\n",
              world_x, world_y, sx, sy, cam_x, cam_y);

            // 如果框完全不在畫面裡，就先畫個背景框確認（必定可見）
            if (sx < 0 || sy < 0 || sx > DC->window_width || sy > DC->window_height)
            {
                al_draw_filled_rectangle(10, 10, 60, 60, al_map_rgba(255,0,0,120));
                al_draw_textf(
                FontCenter::get_instance()->caviar_dreams[FontSize::SMALL],
                    al_map_rgb(255,255,255),
                    15, 15, 0,
                    "(20,1) off screen"
                );
            }
            else
            {
                al_draw_rectangle(
                    sx, sy, sx + TILE_SIZE, sy + TILE_SIZE,
                    al_map_rgb(255, 0, 0),
                    4
                );
            }
        }

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

    case STATE::WIN_HINT: {

        // 半透明黑背景
        al_draw_filled_rectangle(
            0, 0,
            DC->window_width,
            DC->window_height,
            al_map_rgba(0, 0, 0, 180)
        );

        ALLEGRO_FONT* font = al_create_builtin_font();

        // 白底彈窗
        al_draw_filled_rectangle(
            DC->window_width/2 - 200,
            DC->window_height/2 - 100,
            DC->window_width/2 + 200,
            DC->window_height/2 + 100,
            al_map_rgb(255,255,255)
        );

        al_draw_rectangle(
            DC->window_width/2 - 200,
            DC->window_height/2 - 100,
            DC->window_width/2 + 200,
            DC->window_height/2 + 100,
            al_map_rgb(0,0,0),
            2
        );

        al_draw_text(font,
            al_map_rgb(0,0,0),
            DC->window_width/2,
            DC->window_height/2 - 40,
            ALLEGRO_ALIGN_CENTRE,
            "CONGRATULATIONS!");

        al_draw_text(font,
            al_map_rgb(0,0,0),
            DC->window_width/2,
            DC->window_height/2 + 10,
            ALLEGRO_ALIGN_CENTRE,
            "you got the hint");

        al_draw_text(font,
            al_map_rgb(0,0,0),
            DC->window_width/2,
            DC->window_height/2 + 50,
            ALLEGRO_ALIGN_CENTRE,
            "(press ENTER to continue)");
        break;
    }
    case STATE::DIALOG: {
        // 先畫遊戲畫面（地圖、玩家、NPC…）
        DC->level->draw();
        DC->player->draw();
        if (DC->forest_npc) DC->forest_npc->draw();
        // 你也可以畫 monsters、fragments…

        // —— 對話框 —— (Zelda 黑底白框)
        al_draw_filled_rectangle(50, 350, 910, 520, al_map_rgba(0,0,0,200));
        al_draw_rectangle(50, 350, 910, 520, al_map_rgb(255,255,255), 4);

        FontCenter* FC = FontCenter::get_instance();
        al_draw_multiline_text(
            FC->caviar_dreams[FontSize::MEDIUM],
            al_map_rgb(255,255,255),
            80, 380,
            800, 30,
            0,
            dialog_text.c_str()
        );

        break;
    }

    }
    // 對話框繪製 (全狀態都適用)
    if (DC->dialog)
        DC->dialog->draw();
    al_flip_display();
}


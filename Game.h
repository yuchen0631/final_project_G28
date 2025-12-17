#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include <allegro5/allegro.h>
#include "UI.h"

class Game {
public:
    Game(bool testMode = false);
    ~Game();

    void execute();
    void game_init();
    bool game_update();
    void game_draw();
    void spawn_fragments();


private:
    enum class STATE {
        START,
        LEVEL,
        PAUSE,
        LOSE,
        WIN,
        WIN_HINT,
        DIALOG
    };

    STATE state;

    // allegro
    ALLEGRO_DISPLAY *display;
    ALLEGRO_TIMER *timer;
    ALLEGRO_EVENT_QUEUE *event_queue;
    ALLEGRO_EVENT event;

    // images
    ALLEGRO_BITMAP *game_icon;
    ALLEGRO_BITMAP *background;   // menu background
    ALLEGRO_BITMAP *win_img;
    ALLEGRO_BITMAP *lose_img;

    UI *ui;

    bool map_loaded = false;      // ensure map loads only once
};

#endif


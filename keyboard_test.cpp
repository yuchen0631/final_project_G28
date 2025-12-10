#define ALLEGRO_MAIN  // ★必要：讓 Allegro 接管 main()
#include <allegro5/allegro.h>
#include <iostream>

int main(int argc, char **argv) {
    if (!al_init()) {
        std::cout << "al_init failed\n";
        return 0;
    }

    if (!al_install_keyboard()) {
        std::cout << "keyboard install failed\n";
        return 0;
    }

    al_set_new_display_flags(ALLEGRO_WINDOWED);
    ALLEGRO_DISPLAY* disp = al_create_display(400, 200);
    if (!disp) {
        std::cout << "display failed\n";
        return 0;
    }

    ALLEGRO_EVENT_QUEUE* q = al_create_event_queue();
    al_register_event_source(q, al_get_keyboard_event_source());

    std::cout << "Press any key. ESC exits.\n";

    while (true) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(q, &ev);

        if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
            std::cout << "KEY DOWN: " << ev.keyboard.keycode << "\n";
            if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
                break;
        }
    }

    return 0;
}



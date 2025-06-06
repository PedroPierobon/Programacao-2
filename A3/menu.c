#include "menu.h"
#include "core.h"
#include "setting.h"
#include <allegro5/allegro_primitives.h>

static int selected_option = 0;

GameState menu_run() {
    const int total_options = 3;
    const char* options[] = {"Start", "Controls", "Exit"};

    ALLEGRO_EVENT_QUEUE* queue = core_get_event_queue();
    ALLEGRO_FONT* font = core_get_font();

    while(true) {
        ALLEGRO_EVENT event;
        al_wait_for_event(queue, &event);

        if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) return EXIT;
        else if(event.type == ALLEGRO_EVENT_KEY_DOWN) {
            switch(event.keyboard.keycode) {
                case ALLEGRO_KEY_UP:
                    // Ex: 2 - 1 = 1 + 3 = 4 % 3 = 1
                    selected_option = (selected_option - 1 + total_options) % total_options;
                    break;
                case ALLEGRO_KEY_DOWN:
                    // Ex: 2 + 1  = 3 % 3 = 0 volta pra cima
                    selected_option = (selected_option + 1) % total_options;
                    break;    
                case ALLEGRO_KEY_ENTER:
                    if(selected_option == 0) return PLAYING;
                    if(selected_option == 1) return CONTROLS;
                    if(selected_option == 2) return EXIT;
                    break;

            }
        }

        if(al_is_event_queue_empty(queue)){
            al_clear_to_color(al_map_rgb(20, 20, 40));

            const GameSettings *settings = settings_get();
            float screen_w = settings->screen_width;
            float screen_h = settings->screen_height;

            for(int i = 0; i < total_options; i++) {
                ALLEGRO_COLOR color;
                if(i == selected_option) color = al_map_rgb(255, 255, 0);
                else color = al_map_rgb(255, 255, 255);

                al_draw_text(font, color, screen_w / 2, (screen_h / 2) + (i * 40), ALLEGRO_ALIGN_CENTER, options[i]);
            }
            al_flip_display();
        }
    }
}
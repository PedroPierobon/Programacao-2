#include "controls.h"
#include "assets.h"
#include <allegro5/allegro_primitives.h>
#include "game_states.h"
#include "setting.h"

GameControls game_controls = {
    .moveUp = ALLEGRO_KEY_W,
    .moveDown = ALLEGRO_KEY_S,
    .moveLeft = ALLEGRO_KEY_A,
    .moveRight = ALLEGRO_KEY_D,
    .action1 = ALLEGRO_KEY_SPACE,
    .action2 = ALLEGRO_KEY_E
};

static int selected_action = 0;

GameState controls_run() {
    const char* actions_names[] = {"Cima", "Baixo", "Esquerda", "Direita", "Ki Blast", "Especial"};
    const int num_actions = sizeof(actions_names) / sizeof(actions_names[0]);

    bool is_remapping = false;

    ALLEGRO_EVENT_QUEUE* queue = core_get_event_queue();
    ALLEGRO_FONT* font = assets_get_font_menu();
    
    while (true) {
        ALLEGRO_EVENT event;
        al_wait_for_event(queue, &event);
        
        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            return EXIT;
        }

        if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (is_remapping) {
                if (event.keyboard.keycode != ALLEGRO_KEY_ESCAPE) {
                    int* control_to_change = &game_controls.moveUp;
                    control_to_change[selected_action] = event.keyboard.keycode;
                }
                is_remapping = false;
            } else {
                switch (event.keyboard.keycode) {
                    case ALLEGRO_KEY_UP:
                        selected_action = (selected_action - 1 + num_actions) % num_actions;
                        break;
                    case ALLEGRO_KEY_DOWN:
                        selected_action = (selected_action + 1) % num_actions;
                        break;
                    case ALLEGRO_KEY_ENTER:
                        is_remapping = true; // Ativa o modo de remapeamento
                        break;
                    case ALLEGRO_KEY_ESCAPE:
                        return MENU; // Volta para o menu principal
                }
            }
        }

        if(al_is_event_queue_empty(queue)) {
            al_clear_to_color(al_map_rgb(0, 0, 0));

            const GameSettings *settings = settings_get();
            float screen_w = settings->screen_width;
            float screen_h = settings->screen_height;
            
            al_draw_text(font, al_map_rgb(255, 255, 255), screen_w / 2, 100, ALLEGRO_ALIGN_CENTRE, "REMAPEAR CONTROLES");
            al_draw_text(font, al_map_rgb(200, 200, 200), screen_w / 2, screen_h - 100, ALLEGRO_ALIGN_CENTRE, "Pressione ESC para voltar");

            int* control_key = &game_controls.moveUp;

            for(int i = 0; i < num_actions; i++) {
                ALLEGRO_COLOR color = (i == selected_action) ? al_map_rgb(255, 255, 0) : al_map_rgb(255, 255, 255);

                al_draw_text(font, color, screen_w / 2 - 400, (screen_h / 2.5) + i * 60, ALLEGRO_ALIGN_LEFT, actions_names[i]);
                
                bool should_draw_key = true;
                if (is_remapping && i == selected_action) {
                    // Se o tempo é par ele apaga
                    if((int)(al_get_time() * 2) % 2 == 0) {
                        should_draw_key = false;
                    }
                }

                if (should_draw_key ){
                    const char* key_name = al_keycode_to_name(control_key[i]);
                    al_draw_text(font, color, screen_w / 2 + 300, (screen_h / 2.5) + i * 60, ALLEGRO_ALIGN_LEFT, key_name);
                } else {
                    // al_draw_text(font, color, screen_w / 2 + 50, (screen_h / 2.5) + i * 60, ALLEGRO_ALIGN_LEFT, "...");
                }
            }
            al_flip_display();
        }
        
    }
}
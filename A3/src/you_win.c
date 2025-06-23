#include "you_win.h"
#include "core.h"
#include "assets.h"
#include "setting.h"

GameState you_win_run() {
    ALLEGRO_EVENT_QUEUE* queue = core_get_event_queue();
    ALLEGRO_FONT* font = assets_get_font_menu();
    
    // Pega as dimensões da tela para centralizar o texto
    const GameSettings* settings = settings_get();
    float screen_w = settings->screen_width;
    float screen_h = settings->screen_height;

    while (true) {
        ALLEGRO_EVENT event;
        al_wait_for_event(queue, &event);

        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            return EXIT;
        }
        // Se qualquer tecla for pressionada, volta para o menu
        if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (event.keyboard.keycode == ALLEGRO_KEY_ENTER || event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                return MENU;
            }
        }

        // Lógica de desenho simples
        al_clear_to_color(al_map_rgb(0, 0, 0));
        al_draw_text(font, al_map_rgb(0, 255, 0), screen_w / 2, screen_h / 2 - 40, ALLEGRO_ALIGN_CENTER, "YOU WIN");
        al_draw_text(font, al_map_rgb(255, 255, 255), screen_w / 2, screen_h / 2 + 40, ALLEGRO_ALIGN_CENTER, "Pressione ENTER para voltar ao menu");
        al_flip_display();
    }
}
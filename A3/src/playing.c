#include "playing.h"
#include "core.h"
#include "player.h"
#include "assets.h"

GameState playing_run() {
    ALLEGRO_EVENT_QUEUE* queue = core_get_event_queue();
    ALLEGRO_BITMAP* bg_img = assets_get_1level();
    ALLEGRO_DISPLAY* display = core_get_display();
    bool redraw = true;
    bool running_state = true;

    float background_x = 0.0;
    const float scroll_speed = 1.5;
    
    float screen_w = al_get_display_width(display);
    float screen_h = al_get_display_height(display);
    
    int bg_w = al_get_bitmap_width(bg_img);
    int bg_h = al_get_bitmap_height(bg_img);

    float scaled_height = screen_h;
    float scaled_width = ((float)bg_w / bg_h) * scaled_height;

    Player* player1 = player_create();

    while (running_state) {
        ALLEGRO_EVENT event;
        al_wait_for_event(queue, &event);
        
        player_handle_input(player1, &event);

        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            player_destroy(player1);
            return EXIT;
        }
        if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                running_state = false;
            }
        }
        if (event.type == ALLEGRO_EVENT_TIMER) {
            player_update(player1);
            if(player1->x > screen_w / 2) background_x -= 10;
            redraw = true;
        }

        if (redraw && al_is_event_queue_empty(queue)) {
            redraw = false;
            
            al_clear_to_color(al_map_rgb(0, 0, 0));

            
            al_draw_scaled_bitmap(bg_img,
                                    0, 0, bg_w, bg_h, // Região de origem (imagem inteira)
                                    background_x, 0,                        // Posição de destino na tela
                                    scaled_width, scaled_height,             // Tamanho de destino na tela
                                    0);
            //al_draw_scaled_bitmap(bg_img, 0, 0, bg_w, bg_h, 0, 0, screen_w, screen_h, 0);
            //al_draw_bitmap(bg_img, 0, 0, 0);

            player_draw(player1);

            al_flip_display();
        }
    }

    player_destroy(player1);
    return MENU;
}
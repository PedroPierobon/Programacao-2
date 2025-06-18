#include "playing.h"
#include "core.h"
#include "player.h"
#include "assets.h"
#include "bullet.h"
#include "enemy.h"

GameState playing_run() {
    ALLEGRO_EVENT_QUEUE* queue = core_get_event_queue();
    ALLEGRO_BITMAP* bg_img = assets_get_1level();
    ALLEGRO_DISPLAY* display = core_get_display();
    bool redraw = true;
    bool running_state = true;

    float screen_w = al_get_display_width(display);
    float screen_h = al_get_display_height(display);
    
    int bg_w = al_get_bitmap_width(bg_img);
    int bg_h = al_get_bitmap_height(bg_img);
    
    float scaled_height = screen_h;
    float scaled_width = ((float)bg_w / bg_h) * scaled_height;
    
    float camera_x = 0.0f;
    const float dead_zone_left = screen_w * 0.2f;
    const float dead_zone_right = screen_w * 0.5f;

    struct EnemiesSpawn {
        float trigger_x;
        bool triggered;
    };
    
    struct EnemiesSpawn triggers[] = {
        {1500, false},
        {2800, false},
        {4000, false}
    };
    int num_triggers = sizeof(triggers) / sizeof (triggers[0]);

    Player* player1 = player_create(screen_h);
    bullets_init();
    enemies_init();

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
            bullets_update_all(camera_x, screen_w);
            enemies_update_all(player1, camera_x, screen_w);
            
            if (player1->x > camera_x + dead_zone_right){
                camera_x = player1->x - dead_zone_right;
            } else if (player1->x < camera_x + dead_zone_left) {
                camera_x = player1->x - dead_zone_left;
            }

            if (camera_x < 0) camera_x = 0;

            for (int i = 0; i < num_triggers; i++) {
                if(!triggers[i].triggered && camera_x >= triggers[i].trigger_x) {
                    triggers[i].triggered = true;

                    float x = triggers[i].trigger_x + screen_w + 30;
                    float des_x = x - 430;
                    float y = player1->ground_level_y;
                    
                    enemies_spawn(x, y, des_x);
                }
            }

            redraw = true;
        }

        if (redraw && al_is_event_queue_empty(queue)) {
            redraw = false;
            al_clear_to_color(al_map_rgb(0, 0, 0));
            
            ALLEGRO_TRANSFORM camera_transform;
            al_identity_transform(&camera_transform);
            al_translate_transform(&camera_transform, -camera_x, 0);
            al_use_transform(&camera_transform);
            
            al_draw_scaled_bitmap(bg_img,
                                    0, 0, bg_w, bg_h, // Região de origem (imagem inteira)
                                    0, 0,                        // Posição de destino na tela
                                    scaled_width, scaled_height,             // Tamanho de destino na tela
                                    0);
            
            player_draw(player1);
            bullets_draw_all();
            enemies_draw_all();
                
            al_identity_transform(&camera_transform);
            al_use_transform(&camera_transform);
            al_flip_display();
        }
    }

    player_destroy(player1);
    return MENU;
}
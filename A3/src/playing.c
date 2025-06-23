#include "playing.h"
#include "core.h"
#include "player.h"
#include "assets.h"
#include "bullet.h"
#include "enemy.h"
#include "boss.h"
#include "ui.h"
#include <stdio.h>

#define LEVEL_WIDTH 10618.27f

bool check_collision(float x1, float y1, float w1, float h1
                    ,float x2, float y2, float w2, float h2) {
    if (x1 + w1 < x2 || // x1 ta na esquerda
        x1 > x2 + w2 || // x1 na direita
        y1 + h1 < y2 || // x1 em cima
        y1 > y2 + h2){  // x1 embaixo
            return false;
    }
    return true; // se não está em volta, então tem colisão
}

void collisions(Player* player, int* enemies_to_defeat) {
    Bullet* bullet_pool = bullets_get_pool();
    Enemy* enemy_pool = enemies_get_pool();
    Boss* boss = get_boss();

    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!bullet_pool[i].active) continue;

        if (bullet_pool[i].owner == PLAYER) {
            for (int j = 0; j < MAX_ENEMIES; j++) {
                if (!enemy_pool[j].active) continue;
                
                float bullet_w = al_get_bitmap_width(assets_get_kiblast());
                float bullet_h = al_get_bitmap_height(assets_get_kiblast());
                
                float enemy_w = enemy_pool[j].frame_width * enemy_pool[j].scale;
                float enemy_h = enemy_pool[j].frame_height * enemy_pool[j].scale;
                
                if (check_collision(bullet_pool[i].x, bullet_pool[i].y, bullet_w, bullet_h,
                                    enemy_pool[j].x, enemy_pool[j].y, enemy_w, enemy_h)) {
                    bullet_pool[i].active = false;
                    enemy_pool[j].health -= 10;
                    if (enemy_pool[j].health <= 0) {
                        enemy_pool[j].active = false;
                        (*enemies_to_defeat)--;
                    }
                    break;
                }
            }
            if (boss->active) {
                float bullet_w = al_get_bitmap_width(assets_get_kiblast());
                float bullet_h = al_get_bitmap_height(assets_get_kiblast());

                float boss_w = boss->frame_width * boss->scale;
                float boss_h = boss->frame_height * boss->scale;

                if (check_collision(bullet_pool[i].x, bullet_pool[i].y, bullet_w, bullet_h,
                                    boss->x, boss->y, boss_w, boss_h))
                {
                    bullet_pool[i].active = false;
                    boss->health -= 10;
                    printf("BOSS ATINGIDO! Vida: %d\n", boss->health);

                    if (boss->health <= 0) {
                        boss->active = false;
                        printf("BOSS DERROTADO!\n");
                        // Aqui você poderia ativar um estado de vitória!
                    }
                }
            }
        }
        else {
            float player_w = player->frame_width * player->scale;
            float player_h = player->frame_height * player->scale;
            float player_y = player->y;
            if (player->state == SQUATTING) {
                player_h /= 2;
                player_y += player_h;
            }
            
            float bullet_w = al_get_bitmap_width(assets_get_enemy_kiblast());
            float bullet_h = al_get_bitmap_height(assets_get_enemy_kiblast());

            if (check_collision(bullet_pool[i].x, bullet_pool[i].y, bullet_w, bullet_h,
                                player->x, player_y, player_w, player_h)) {
                bullet_pool[i].active = false;
                player->health -= 10;
                printf("JOGADOR ATINGIDO!\n");
                if (player->health <= 0) {
                    player->active = false; // << Adicione uma flag 'active' ao Player
                    printf("GAME OVER\n");
                }                       
            }
        }
    }
}

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

    bool boss_arena_active = false;
    bool boss_spawned = false;
    int enemies_to_defeat = 0;
    struct EnemiesSpawn {
        float trigger_x;
        bool triggered;
    };
    
    struct EnemiesSpawn triggers[] = {
        {(int)(LEVEL_WIDTH * 0.05), false},
        {(int)(LEVEL_WIDTH * 0.20), false},
        {(int)(LEVEL_WIDTH * 0.35), false},
        {(int)(LEVEL_WIDTH * 0.50), false},
        {(int)(LEVEL_WIDTH * 0.65), false},
        {(int)(LEVEL_WIDTH * 0.80), false}
    };
    int num_triggers = sizeof(triggers) / sizeof (triggers[0]);

    Player* player1 = player_create(screen_h);
    bullets_init();
    enemies_init();
    boss_init();

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
            player_update(player1, boss_arena_active, camera_x, screen_w);
            bullets_update_all(camera_x, screen_w, screen_h);
            enemies_update_all(player1, camera_x, screen_w);
            boss_update(camera_x, screen_w);
            collisions(player1, &enemies_to_defeat);

            if (!player1->active) {
                return GAME_OVER; // ...encerra o estado de 'playing' e retorna GAME_OVER
            }

            if (boss_spawned && !boss_is_active()) {
                return YOU_WIN; // ...encerra e retorna YOU_WIN
            }

            if (camera_x >= LEVEL_WIDTH - screen_w && !boss_arena_active) {
                float right_world_limit = LEVEL_WIDTH - (player1->frame_width * player1->scale);
                if (player1->x > right_world_limit) {
                    player1->x = right_world_limit;
                }
            }

            if (!boss_arena_active) {
                if (player1->x > camera_x + dead_zone_right){
                    camera_x = player1->x - dead_zone_right;
                } else if (player1->x < camera_x + dead_zone_left) {
                    camera_x = player1->x - dead_zone_left;
                }
            }

            if (camera_x < 0) camera_x = 0;
            
            if (camera_x > LEVEL_WIDTH - screen_w) {
                camera_x = LEVEL_WIDTH - screen_w;
            }

            for (int i = 0; i < num_triggers; i++) {
                if(!triggers[i].triggered && camera_x >= triggers[i].trigger_x) {
                    triggers[i].triggered = true;
                    float x = triggers[i].trigger_x + screen_w + 30;
                    float des_x = x - 430;
                    enemies_spawn(x, player1->ground_level_y, des_x);
                    enemies_to_defeat++;
                }
            }
            if (!boss_spawned && camera_x >= LEVEL_WIDTH - screen_w && enemies_to_defeat == 0) {
                boss_spawned = true;
                boss_arena_active = true;
                boss_spawn(camera_x + screen_w + 30, 50, camera_x, screen_w); 
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
            boss_draw();
                
            al_identity_transform(&camera_transform);
            al_use_transform(&camera_transform);
            
            ui_draw(player1);
            
            al_flip_display();
        }
    }

    player_destroy(player1);
    return MENU;
}
#include "enemy.h"
#include "assets.h"
#include "bullet.h"
#include "player.h"

#define MAX_ENEMIES 10
#define ENEMY_SPEED 3.0f
#define SHOOT_DELAY 2.0f

static Enemy enemy_pool[MAX_ENEMIES];

void update_single_enemy(Enemy* e, Player* player, float camera_x, float screen_w) {
    if (!e->active) return;

    switch (e->state) {
        case MOVING:
            if (e->destination_x < e->x) {
                e->x -= ENEMY_SPEED;
                if (e->x <= e->destination_x) {
                    e->x = e->destination_x;
                    e->state = ATTACKING;
                }
            }
            e->facing_right = (e->destination_x > e->x);
            break;
        case ATTACKING:
            e->facing_right = (player->x > e->x);
            e->shoot_frames += 1.0 / 30.0; // 1 frame

            if (e->shoot_timer >= SHOOT_DELAY) {
                e->shoot_frames = 0;

                BulletDirection dir = e->facing_right ? DIR_RIGHT : DIR_LEFT;
                float spawn_x = e->facing_right ? e->x + e->frame_width : e->x - 40;
                float spawn_y = e->y + (e->frame_height / 2.0f);
                bullets_spawn(spawn_x, spawn_y, dir);
            }
            break;
        case INACTIVE:
            break;
    }

    //if(e->state == MOVING){
    //    e->current_frame = (e->current_frame + 1) % e->run_frames;
    //} else if (e->state == ATTACKING) {
    //    e->current_frame = (e->current_frame + 1) % e->shoot_frames;
    //}
}

void draw_single_enemy(Enemy* e) {
    if (!e->active) return;
    // Lógica de desenho similar à do jogador
    // Por enquanto, vamos desenhar um retângulo simples
    ALLEGRO_COLOR color = al_map_rgb(255, 0, 255); // Magenta para teste
    al_draw_filled_rectangle(e->x, e->y, e->x + 500, e->y + 1000, color);
}

void enemies_init() {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        enemy_pool[i].active = false;
    }
}

void enemies_spawn(float x, float y, float dest_x) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemy_pool[i].active) {
            Enemy* e = &enemy_pool[i];
            e->active = true;
            e->x = x;
            e->y = y;
            e->destination_x = dest_x;
            e->state = MOVING;
            // Configurar outros valores como sprites e contagem de frames aqui...
            e->frame_width = 44; // Exemplo
            e->frame_height = 44; // Exemplo
            return;
        }
    }
}

void enemies_update_all(Player* player, float camera_x, float screen_w) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        update_single_enemy(&enemy_pool[i], player, camera_x, screen_w);
    }
}

void enemies_draw_all() {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        draw_single_enemy(&enemy_pool[i]);
    }
}
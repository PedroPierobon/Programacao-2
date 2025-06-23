#include "boss.h"
#include "assets.h"
#include "bullet.h"
#include "player.h"
#include <math.h>

#define BOSS_SPEED 6.0f
#define SHOOT_DELAY 1.0f

static Boss boss;

void boss_init() {
    boss.active = false;
    boss.sprite_sheets = assets_get_boss_spritesheet();
}

void boss_spawn(float x, float y, float camera_x, float screen_w) {
    boss.active = true;
    boss.x = x;
    boss.y = y;
    boss.speed = BOSS_SPEED;
    boss.state = BOSS_MOVING;
    boss.destination_x = camera_x + (screen_w * 0.8f); // Começa indo para a direita
    boss.facing_right = false;
    boss.shoot_timer = 0.0f;
    boss.frame_height = 49;
    boss.frame_width = 38;
    boss.current_frame = 0;
    boss.scale = 6.0f;
    boss.health = 200;
    boss.max_health = 200;
}

void boss_update(float camera_x, float screen_w) {
    if (!boss.active) return;

    float patrol_left_limit = camera_x + (screen_w * 0.1f);
    float patrol_right_limit = camera_x + (screen_w * 0.9f);

    switch (boss.state) {
        case BOSS_MOVING:
            if (boss.destination_x < boss.x){
                boss.x -= boss.speed;
                if (boss.x <= boss.destination_x){
                    boss.x = boss.destination_x;
                    boss.state = BOSS_ATTACKING;
                    boss.destination_x = patrol_left_limit;
                    //boss.shoot_timer = 0; // Prepara para o primeiro tiro
                }
            }
            break;
        
        case BOSS_ATTACKING:
            if (boss.destination_x > boss.x) {
                boss.x += boss.speed;
            } else {
                boss.x -= boss.speed;
            }
            
            if (fabs(boss.x - boss.destination_x) < boss.speed) {
                boss.destination_x = (boss.destination_x == patrol_right_limit) 
                                     ? patrol_left_limit 
                                     : patrol_right_limit;
            }

            // --- Lógica de Ataque (Tiro Contínuo) ---
            // Esta lógica roda EM PARALELO com a de movimento.
            boss.shoot_timer += 1.0 / 30.0; // Adiciona tempo
            if (boss.shoot_timer >= SHOOT_DELAY) {
                boss.shoot_timer = 0; // Reseta o timer

                // Atira para baixo
                float spawn_x = boss.x + 100; // Posição do canhão (ajustar)
                float spawn_y = boss.y + 200; // Posição do canhão (ajustar)
                bullets_spawn(spawn_x, spawn_y, DIR_DOWN, BOSS);
            }
            break;
        
        case BOSS_INACTIVE:
            break;
    }
    
    // Define a direção do sprite (pode ser útil para a animação)
    boss.facing_right = (boss.destination_x > boss.x);
}

void boss_draw() {
    if (!boss.active) return;
    int animation_row = 0;

    int source_x = boss.current_frame * boss.frame_width;
    int source_y = animation_row * boss.frame_height;

    int flip_flag = boss.facing_right ? 0: ALLEGRO_FLIP_HORIZONTAL;

    al_draw_scaled_bitmap(
        boss.sprite_sheets,
        source_x, source_y,
        boss.frame_width, boss.frame_height,
        boss.x, boss.y,
        boss.frame_width * boss.scale,
        boss.frame_height * boss.scale,
        flip_flag
    );
}

bool boss_is_active() {
    return boss.active;
}

Boss* get_boss() {
    return &boss;
}
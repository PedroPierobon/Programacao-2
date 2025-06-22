#include "enemy.h"
#include "assets.h"
#include "bullet.h"
#include "player.h"

#define ENEMY_SPEED 6.0f
#define SHOOT_DELAY 1.0f

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

            if (e->shoot_timer >= SHOOT_DELAY) {
                e->shoot_timer = 0;

                BulletDirection dir = e->facing_right ? DIR_RIGHT : DIR_LEFT;
                float spawn_x = e->facing_right ? e->x + e->frame_width : e->x - 40;
                float spawn_y = e->y + (e->frame_height / 2.0f);
                bullets_spawn(spawn_x, spawn_y, dir, ENEMY);
            }
            break;
        case INACTIVE:
            break;
    }

    e->animation_timer += 1.0;
    if (e->animation_timer >= (30 / e->animation_speed)) {
        e->animation_timer = 0.0;
        if (e->state == ATTACKING) {
            e->current_frame = (e->current_frame + 1) % e->shoot_frames;
        }
    }
}

void draw_single_enemy(Enemy* e) {
    if (!e->active) return;

    int animation_row = 0;
    if (e->state == ATTACKING) animation_row = 1;

    float sx = e->current_frame * e->frame_width;
    float sy = animation_row * e->frame_height;

    int flip_flag = e->facing_right ? 0: ALLEGRO_FLIP_HORIZONTAL;

    al_draw_scaled_bitmap(
        e->sprite_sheets,
        sx, sy,
        e->frame_width, e->frame_height,
        e->x, e->y,
        e->frame_width * e->scale,
        e->frame_height * e->scale,
        flip_flag
    );

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
            e->facing_right = false;

            e->sprite_sheets = assets_get_enemy_spritesheet();
            e->frame_width = 44; 
            e->frame_height = 44; 
            e->scale = 6.0f;

            e->current_frame = 0;
            e->animation_timer = 0.0f;
            e->animation_speed = 8.0f;

            e->run_frames = 1;
            e->shoot_frames = 2;
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

Enemy* enemies_get_pool(){
    return enemy_pool;
}
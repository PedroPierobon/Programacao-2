#ifndef ENEMY_H
#define ENEMY_H

#include <allegro5/allegro5.h>
#include "player.h"

#define MAX_ENEMIES 10

typedef enum {
    INACTIVE,
    MOVING,
    ATTACKING
} EnemyState;

typedef struct {
    float x, y;
    float speed;
    bool active;
    bool facing_right;

    ALLEGRO_BITMAP* sprite_sheets;
    int frame_width, frame_height;
    int current_frame;
    float animation_timer, animation_speed;
    int run_frames, shoot_frames;
    float scale;

    EnemyState state;
    float destination_x;
    float shoot_timer;

    int health;
    int max_health;
} Enemy;

void enemies_init();
void enemies_spawn(float x, float y, float dest_x);
void enemies_update_all(Player* player, float camera_x, float screen_w);
void enemies_draw_all();
Enemy* enemies_get_pool();

#endif
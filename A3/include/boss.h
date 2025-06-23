#ifndef BOSS_H
#define BOSS_H

#include <allegro5/allegro5.h>
#include "player.h"

#define MAX_ENEMIES 10

typedef enum {
    BOSS_INACTIVE,
    BOSS_MOVING,
    BOSS_ATTACKING
} BossState;

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

    BossState state;
    float destination_x;
    float shoot_timer;

    int health;
    int max_health;
} Boss;

void boss_init();
void boss_spawn(float x, float y, float camera_x, float screen_w);
void boss_update(float camera_x, float screen_w);
void boss_draw();
bool boss_active();
Boss* get_boss();

#endif
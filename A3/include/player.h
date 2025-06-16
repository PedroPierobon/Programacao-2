#ifndef PLAYER_H
#define PLAYER_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

typedef enum {
    IDLE,
    WALKING,
    JUMPING,
    SQUATTING,
    SHOOTING
} PlayerState;

typedef struct {
    float x, y;
    float speed;
    float jump_initial_speed;
    float current_vy;
    float ground_level_y;
    bool is_on_ground;

    PlayerState state;

    ALLEGRO_BITMAP* sprite_sheets;
    int frame_width;
    int frame_height;
    float scale;

    int current_frame;
    int walk_animation_frames;
    int idle_animation_frames;
    int jump_animation_frames;
    int shoot_animation_frames;
    int squat_animation_frames;


    float animation_timer;
    float animation_speed;

    bool facing_right;
} Player;

Player* player_create(float screen_height);

void player_destroy(Player* p);

void player_handle_input(Player* p, ALLEGRO_EVENT* event);

void player_update(Player* p);

void player_draw(Player* p);

#endif


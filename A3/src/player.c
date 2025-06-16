#include "player.h"
#include <stdlib.h>
#include "controls.h"
#include "assets.h"
#include "bullet.h"

#define GRAVITY 9.0f
//#define GROUND_Y 500.0f

static bool key_up = false;
static bool key_down = false;
static bool key_left = false;
static bool key_right = false;
static bool key_shoot = false;

Player* player_create(float screen_height) {
    Player* p = malloc(sizeof(Player));
    if (p) {
        p->jump_initial_speed = -100.0f;
        p->current_vy = 0.0f;
        p->is_on_ground = true;
        
        p->state = IDLE;
        
        p->sprite_sheets = assets_get_player_spritesheet();
        
        p->frame_width = 44;
        p->frame_height = 44;
        p->scale = 6.0f;
        
        p->ground_level_y = screen_height - 1.5 * p->frame_height * p->scale;
        
        p->x = 100;
        p->y = p->ground_level_y;
        p->speed = 25.0f;

        p->current_frame = 0;
        p->walk_animation_frames = 8;
        p->idle_animation_frames = 5;
        p->jump_animation_frames = 8;
        p->shoot_animation_frames = 6;
        p->squat_animation_frames = 2;

        p->animation_timer = 0.0;
        p->animation_speed = 8.0;

        p->facing_right = true;
    }
    return p;
}

void player_destroy(Player* p) {
    if (p) {
        free(p);
    }
}

void player_handle_input(Player* p, ALLEGRO_EVENT* event) {
    if (event->type == ALLEGRO_EVENT_KEY_DOWN) {
        int keycode = event->keyboard.keycode;

        if (keycode == game_controls.moveUp) {
            key_up = true;
        } else if (keycode == game_controls.moveDown) {
            key_down = true;
        } else if (keycode == game_controls.moveLeft) {
            key_left = true;
        } else if (keycode == game_controls.moveRight) {
            key_right = true;
        } else if (keycode == game_controls.action1) {
            key_shoot = true;
        }
    } else if (event->type == ALLEGRO_EVENT_KEY_UP) {
        int keycode = event->keyboard.keycode;

        if (keycode == game_controls.moveUp) {
            key_up = false;
        } else if (keycode == game_controls.moveDown) {
            key_down = false;
        } else if (keycode == game_controls.moveLeft) {
            key_left = false;
        } else if (keycode == game_controls.moveRight) {
            key_right = false;
        } else if (keycode == game_controls.action1) {
            key_shoot = false;
        }
    }
}

void player_update(Player* p) {
    PlayerState old_state = p->state;

    float move_direction = 0.0f;
    if (key_left){
        move_direction -= 1.0f;
        p->facing_right = false;
    }
    if (key_right){
        move_direction += 1.0f;
        p->facing_right = true;
    }
    p->x += move_direction * p->speed;
    
    if (!p->is_on_ground) {
        p->state = key_shoot ? SHOOTING : JUMPING;
        p->current_vy += GRAVITY;
        p->y += p->current_vy;
        
        if (p->y >= p->ground_level_y) {
            p->y = p->ground_level_y;
            p->is_on_ground = true;
            p->current_vy = 0;
        }
    }
    else {
        if (key_up) {
            p->is_on_ground = false;
            p->current_vy = p->jump_initial_speed;
            p->state = JUMPING;
        }
        else if (key_down) {
            p->state = SQUATTING;
        }
        else if (key_shoot) {
            p->state = SHOOTING;
        }
        else if (move_direction != 0) {
            p->state = WALKING;
        }
        else {
            p->state = IDLE;
        }
    }

    if (p->state != old_state) p->current_frame = 0;

    float speed_modifier = (p->state == SHOOTING) ? 2.0f : 1.0f;

    p->animation_timer += 1.0;
    if(p->animation_timer >= (30.0 / (p->animation_speed * speed_modifier))) {
        p->animation_timer = 0.0;

        switch (p->state)
        {
        case IDLE:
            p->current_frame = (p->current_frame + 1) % p->idle_animation_frames;
            break;
        case WALKING:
            p->current_frame = (p->current_frame + 1) % p->walk_animation_frames;
            break;
        case JUMPING:
            if(p->current_frame < p->jump_animation_frames - 1) {
                p->current_frame++;
            }
            break;
        case SHOOTING:
            p->current_frame = (p->current_frame + 1) % p->shoot_animation_frames;
            if (p->current_frame == 2 || p->current_frame == 5) {
                float spawn_x = p->facing_right
                                ? p->x + (p->frame_width * p->scale) - 10
                                : p->x + 10;
                float spawn_y = p->y + (p->frame_height * p->scale / 2.0f);

                bullets_spawn(spawn_x, spawn_y, p->facing_right ? 1 : -1);
            }
            break;
        case SQUATTING:
            if (key_shoot) {
                p->current_frame = (p->current_frame + 1) % p->squat_animation_frames;
                float spawn_x = p->facing_right
                                ? p->x + (p->frame_width * p->scale) - 10
                                : p->x + 10;
                float spawn_y = p->y + (p->frame_height * p->scale / 2.0f) + 20;

                bullets_spawn(spawn_x, spawn_y, p->facing_right ? 1 : -1);
            } else {
                p->current_frame = 0;
            }
            break;
        }
    }
}

void player_draw(Player* p) {
    int animation_row = 0;
    if(p->state == WALKING) {
        animation_row = 1;
    } else if(p->state == JUMPING) {
        animation_row = 2;
    } else if(p->state == SHOOTING) {
        animation_row = 3;
    } else if(p->state == SQUATTING) {
        animation_row = 4;
    }

    int source_x = p->current_frame * p->frame_width;
    int source_y = animation_row * p->frame_height;

    int flip_flag = p->facing_right ? 0: ALLEGRO_FLIP_HORIZONTAL;

    al_draw_scaled_bitmap(
        p->sprite_sheets,
        source_x, source_y,
        p->frame_width, p->frame_height,
        p->x, p->y,
        p->frame_width * p->scale,
        p->frame_height * p->scale,
        flip_flag
    );
}
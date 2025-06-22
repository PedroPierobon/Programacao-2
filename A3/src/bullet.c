#include "bullet.h"
#include "assets.h"

#define BULLET_SPEED 60.0f

static Bullet bullet_pool[MAX_BULLETS];

static ALLEGRO_BITMAP* player_kiblast_sprite = NULL;
static ALLEGRO_BITMAP* enemy_kiblast_sprite = NULL;
static ALLEGRO_BITMAP* enemy_kiblast_down_sprite = NULL;

void update_single_bullet(Bullet* b, float camera_x, float screen_w, float screen_h) {
    if(!b->active) return;
    switch (b->direction) {
        case DIR_RIGHT:
            b->x += BULLET_SPEED;
            break;
        case DIR_LEFT:
            b->x -= BULLET_SPEED;
            break;
        case DIR_UP:
            b->y -= BULLET_SPEED;
            break;
        case DIR_DOWN:
            b->y += BULLET_SPEED;
            break;
    }
    if (b->x > camera_x + screen_w || 
        b->x < camera_x || b->y < 0 || b->y > screen_h - 44){
        b->active = false;
    }
}

void draw_single_bullet(Bullet* b) {
    if (!b->active) return;
    
    ALLEGRO_BITMAP* sprite;
    if  (b->owner == PLAYER){
        sprite = player_kiblast_sprite;
    } else if (b->direction == DIR_DOWN) {
        sprite = enemy_kiblast_down_sprite;
    } else {
        sprite = enemy_kiblast_sprite;
    }
    if (sprite) al_draw_bitmap(sprite, b->x, b->y, 0);
}

void bullets_init() {
    player_kiblast_sprite = assets_get_kiblast();
    enemy_kiblast_sprite = assets_get_enemy_kiblast();
    enemy_kiblast_down_sprite = assets_get_enemy_kiblast_down();

    for (int i = 0; i < MAX_BULLETS; i++) {
        bullet_pool[i].active = false;
    }
}

void bullets_spawn(float x, float y, BulletDirection dir, BulletOwner owner) {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if(!bullet_pool[i].active) {
            bullet_pool[i].active = true;
            bullet_pool[i].x = x;
            bullet_pool[i].y = y;
            bullet_pool[i].direction = dir;
            bullet_pool[i].owner = owner;
            return;
        }
    }
}

void bullets_update_all(float camera_x, float screen_w, float screen_h) {
    for (int i = 0; i < MAX_BULLETS; i++) {
        update_single_bullet(&bullet_pool[i], camera_x, screen_w, screen_h);
    }
}

void bullets_draw_all() {
    for (int i = 0; i < MAX_BULLETS; i++) {
        draw_single_bullet(&bullet_pool[i]);
    }
}

Bullet* bullets_get_pool() {
    return bullet_pool;
}
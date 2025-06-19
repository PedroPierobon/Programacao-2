#include "bullet.h"
#include "assets.h"

#define BULLET_SPEED 15.0f

static Bullet bullet_pool[MAX_BULLETS];

static ALLEGRO_BITMAP* player_kiblast_sprite = NULL;
static ALLEGRO_BITMAP* enemy_kiblast_sprite = NULL;

void update_single_bullet(Bullet* b, float camera_x, float screen_w) {
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
    }
    if(b->x > camera_x + screen_w || b->x < camera_x || b->y < 0){
        b->active = false;
    }
}

void draw_single_bullet(Bullet* b) {
    if (!b->active) return;
    
    ALLEGRO_BITMAP* sprite = (b->owner == PLAYER) ? player_kiblast_sprite : enemy_kiblast_sprite;
    if (sprite) al_draw_bitmap(sprite, b->x, b->y, 0);
}

void bullets_init() {
    player_kiblast_sprite = assets_get_kiblast();
    enemy_kiblast_sprite = assets_get_enemy_kiblast();

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

void bullets_update_all(float camera_x, float screen_w) {
    for (int i = 0; i < MAX_BULLETS; i++) {
        update_single_bullet(&bullet_pool[i], camera_x, screen_w);
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
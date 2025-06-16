#include "bullet.h"
#include "assets.h"

#define MAX_BULLETS 20
#define BULLET_SPEED 15.0f

static Bullet bullet_pool[MAX_BULLETS];

void update_single_bullet(Bullet* b, float screen_w) {
    if(!b->active) return;
    b->x += BULLET_SPEED * b->direction;
    if(b->x >= screen_w || b->x < 0){
        b->active = false;
    }
    
    // colisão
}

void draw_single_bullet(Bullet* b) {
    if (!b->active) return;
    al_draw_bitmap(b->sprite, b->x, b->y, 0);
}

void bullets_init() {
    ALLEGRO_BITMAP* bullet_sprite = assets_get_kiblast();
    for (int i = 0; i < MAX_BULLETS; i++) {
        bullet_pool[i].active = false;
        bullet_pool[i].sprite = bullet_sprite;
    }
}

void bullets_spawn(float x, float y, int direction) {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if(!bullet_pool[i].active) {
            bullet_pool[i].active = true;
            bullet_pool[i].x = x;
            bullet_pool[i].y = y;
            bullet_pool[i].direction = direction;
            return;
        }
    }
}

void bullets_update_all(float screen_w) {
    for (int i = 0; i < MAX_BULLETS; i++) {
        update_single_bullet(&bullet_pool[i], screen_w);
    }
}

void bullets_draw_all() {
    for (int i = 0; i < MAX_BULLETS; i++) {
        draw_single_bullet(&bullet_pool[i]);
    }
}
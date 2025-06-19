#ifndef BULLET_H
#define BULLET_H

#define MAX_BULLETS 20
#include <allegro5/allegro5.h>

typedef enum {
    DIR_RIGHT,
    DIR_LEFT,
    DIR_UP
} BulletDirection;

typedef enum {
    PLAYER,
    ENEMY
} BulletOwner;

typedef struct {
    float x, y;
    BulletDirection direction;
    bool active;
    BulletOwner owner;
} Bullet;

void bullets_init();

void bullets_spawn(float x, float y, BulletDirection dir, BulletOwner owner);

void bullets_update_all(float camera_x, float screen_w);
void bullets_draw_all();

Bullet* bullets_get_pool();

#endif
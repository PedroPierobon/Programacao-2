#ifndef BULLET_H
#define BULLET_H

#include <allegro5/allegro5.h>

typedef enum {
    DIR_RIGHT,
    DIR_LEFT,
    DIR_UP
} BulletDirection;

typedef struct {
    float x, y;
    BulletDirection direction;
    bool active;
    ALLEGRO_BITMAP* sprite;
} Bullet;

void bullets_init();

void bullets_spawn(float x, float y, BulletDirection dir);

void bullets_update_all(float camera_x, float screen_w);
void bullets_draw_all();

#endif
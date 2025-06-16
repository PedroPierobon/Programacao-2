#ifndef BULLET_H
#define BULLET_H

#include <allegro5/allegro5.h>

typedef struct {
    float x, y;
    float speed;
    int direction;
    bool active;
    ALLEGRO_BITMAP* sprite;
} Bullet;

void bullets_init();

void bullets_spawn(float x, float y, int direction);

void bullets_update_all(float screen_w);
void bullets_draw_all();

#endif
#ifndef PLAYER_H
#define PLAYER_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

typedef struct {
    float x, y;
    float speed;
    float size;
    ALLEGRO_COLOR color;
} Player;

Player* player_create();

void player_destroy(Player* p);

void player_handle_input(Player* p, ALLEGRO_EVENT* event);

void player_update(Player* p);

void player_draw(Player* p);

#endif


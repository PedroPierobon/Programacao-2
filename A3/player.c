#include "player.h"
#include <stdlib.h>

static bool key_up = false;
static bool key_down = false;
static bool key_left = false;
static bool key_right = false;

Player* player_create() {
    Player* p = malloc(sizeof(Player));
    if (p) {
        p->x = 100;
        p->y = 100;
        p->speed = 4.0f;
        p->size = 20.0f;
        p->color = al_map_rgb(255, 0, 0); // Vermelho
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
        switch (event->keyboard.keycode) {
            case ALLEGRO_KEY_UP: key_up = true; break;
            case ALLEGRO_KEY_DOWN: key_down = true; break;
            case ALLEGRO_KEY_LEFT: key_left = true; break;
            case ALLEGRO_KEY_RIGHT: key_right = true; break;
        }
    } else if (event->type == ALLEGRO_EVENT_KEY_UP) {
        switch (event->keyboard.keycode) {
            case ALLEGRO_KEY_UP: key_up = false; break;
            case ALLEGRO_KEY_DOWN: key_down = false; break;
            case ALLEGRO_KEY_LEFT: key_left = false; break;
            case ALLEGRO_KEY_RIGHT: key_right = false; break;
        }
    }
}

void player_update(Player* p) {
    if (key_up) p->y -= p->speed;
    if (key_down) p->y += p->speed;
    if (key_left) p->x -= p->speed;
    if (key_right) p->x += p->speed;
}

void player_draw(Player* p) {
    al_draw_filled_rectangle(p->x, p->y, p->x + p->size, p->y + p->size, p->color);
}
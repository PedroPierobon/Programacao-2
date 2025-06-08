#ifndef CONTROLS_H
#define CONTROLS_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include "core.h"
#include "game_states.h"

typedef struct {
    int moveUp;
    int moveDown;
    int moveLeft;
    int moveRight;
    int action1; // Atirar
    int action2; // Especial
} GameControls;

// Está nas settings
extern GameControls game_controls;

GameState controls_run();

#endif

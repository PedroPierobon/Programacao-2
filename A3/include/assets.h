#ifndef ASSETS_H
#define ASSETS_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>

bool assets_init();

void assets_shutdown();

ALLEGRO_BITMAP* assets_get_background();
ALLEGRO_FONT* assets_get_font_menu();

#endif

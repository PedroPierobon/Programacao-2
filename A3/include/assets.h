#ifndef ASSETS_H
#define ASSETS_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>

bool assets_init();

void assets_shutdown();

ALLEGRO_BITMAP* assets_get_background();
ALLEGRO_BITMAP* assets_get_player_spritesheet();
ALLEGRO_BITMAP* assets_get_enemy_spritesheet();
ALLEGRO_BITMAP* assets_get_boss_spritesheet();
ALLEGRO_BITMAP* assets_get_enemy_kiblast();
ALLEGRO_BITMAP* assets_get_enemy_kiblast_down();
ALLEGRO_BITMAP* assets_get_kiblast();
ALLEGRO_BITMAP* assets_get_1level();
ALLEGRO_FONT* assets_get_font_menu();

#endif

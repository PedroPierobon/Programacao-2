#include "assets.h"
#include <allegro5/allegro_image.h>
#include <stdio.h>

static ALLEGRO_BITMAP* background = NULL;
static ALLEGRO_BITMAP* goku = NULL;
static ALLEGRO_BITMAP* enemy = NULL;
static ALLEGRO_BITMAP* enemy_kiblast = NULL;
static ALLEGRO_BITMAP* kiblast = NULL;
static ALLEGRO_BITMAP* level1 = NULL;
static ALLEGRO_FONT* font_menu = NULL;

bool assets_init() {
    background = al_load_bitmap("assets/images/backgroundMenu.png");
    if (!background) {
        fprintf(stderr, "Falhou em carregar backgroundMenu.png\n");
        return false;
    }
    
    level1 = al_load_bitmap("assets/images/BGFirst.jpg");
    if (!level1) {
        fprintf(stderr, "alhou em carregar BGFirst.jpg\n");
        return false;
    }

    goku = al_load_bitmap("assets/images/GokuSpriteSheet.png");
    if (!goku) {
        fprintf(stderr, "Falhou em carregar GokuSpriteSheet.png");
        return false;
    }
    
    enemy = al_load_bitmap("assets/images/EnemySpriteSheet.png");
    if (!enemy) {
        fprintf(stderr, "Falhou em carregar EnemySpriteSheet.png");
        return false;
    }

    kiblast = al_load_bitmap("assets/images/KiblastSprite.png");
    if (!kiblast) {
        fprintf(stderr, "Falhou em carregar KiblastSprite.png");
        return false;
    }
    
    enemy_kiblast = al_load_bitmap("assets/images/EnemyKiblast.png");
    if (!enemy_kiblast) {
        fprintf(stderr, "Falhou em carregar EnemyKiblast.png");
        return false;
    }

    font_menu = al_load_font("assets/fonts/PressStart2P-Regular.ttf", 32, 0);
    if(!font_menu){
        fprintf(stderr, "Falha em carregar fonte\n");
        return false;
    }

    return true;
}

void assets_shutdown() {
    if (background) al_destroy_bitmap(background);
    if (goku) al_destroy_bitmap(goku);
    if (enemy) al_destroy_bitmap(enemy);
    if (enemy_kiblast) al_destroy_bitmap(enemy_kiblast);
    if (kiblast) al_destroy_bitmap(kiblast);
    if (level1) al_destroy_bitmap(level1);
    if (font_menu) al_destroy_font(font_menu);
}

ALLEGRO_BITMAP* assets_get_background() {
    return background;
}

ALLEGRO_BITMAP* assets_get_player_spritesheet() {
    return goku;
}

ALLEGRO_BITMAP* assets_get_enemy_spritesheet() {
    return enemy;
}

ALLEGRO_BITMAP* assets_get_kiblast() {
    return kiblast;
}

ALLEGRO_BITMAP* assets_get_enemy_kiblast() {
    return enemy_kiblast;
}

ALLEGRO_FONT* assets_get_font_menu(){
    return font_menu;
}

ALLEGRO_BITMAP* assets_get_1level(){
    return level1;
}
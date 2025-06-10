#include "assets.h"
#include <allegro5/allegro_image.h>
#include <stdio.h>

static ALLEGRO_BITMAP* background = NULL;
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
        if(background) al_destroy_bitmap(background);
        return false;
    }

    font_menu = al_load_font("assets/fonts/PressStart2P-Regular.ttf", 32, 0);
    if(!font_menu){
        fprintf(stderr, "Falha em carregar fonte\n");
        if(background) al_destroy_bitmap(background);
        if(level1) al_destroy_bitmap(level1);
        return false;
    }

    return true;
}

void assets_shutdown() {
    if (background) al_destroy_bitmap(background);
    if(level1) al_destroy_bitmap(level1);
    if (font_menu) al_destroy_font(font_menu);
}

ALLEGRO_BITMAP* assets_get_background() {
    return background;
}

ALLEGRO_FONT* assets_get_font_menu(){
    return font_menu;
}

ALLEGRO_BITMAP* assets_get_1level(){
    return level1;
}
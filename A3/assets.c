#include "assets.h"
#include <allegro5/allegro_image.h>
#include <stdio.h>

static ALLEGRO_BITMAP* background = NULL;
static ALLEGRO_FONT* font_menu = NULL;

bool assets_init() {
    background = al_load_bitmap("assets/backgroundMenu.png");
    if (!background) {
        fprintf(stderr, "Falhou em carregar backgroundMenu.png\n");
        return false;
    }

    font_menu = al_load_font("assets/PressStart2P-Regular.ttf", 32, 0);
    if(!font_menu){
        fprintf(stderr, "Falha em carregar fonte\n");
        if(background) al_destroy_bitmap(background);
    }

    return true;
}

void assets_shutdown() {
    // Libera a memória das imagens
    if (background) al_destroy_bitmap(background);
    if (font_menu) al_destroy_font(font_menu);
}

// Implementação dos getters
ALLEGRO_BITMAP* assets_get_background() {
    return background;
}

ALLEGRO_FONT* assets_get_font_menu(){
    return font_menu;
}

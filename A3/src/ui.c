#include "ui.h"
#include <allegro5/allegro_primitives.h>

void ui_draw(Player* player) {
    float health_percentage = (float)player->health / player->max_health;
    if (health_percentage < 0) health_percentage = 0;
    
    float bar_width = 300;
    float bar_height = 30;
    float bar_x = 20;
    float bar_y = 20;
    
    // Desenha o fundo da barra (vermelho escuro)
    al_draw_filled_rectangle(bar_x, bar_y, bar_x + bar_width, bar_y + bar_height, al_map_rgb(100, 0, 0));
    // Desenha a vida atual (verde)
    al_draw_filled_rectangle(bar_x, bar_y, bar_x + (bar_width * health_percentage), bar_y + bar_height, al_map_rgb(0, 255, 0));
    // Desenha a moldura
    al_draw_rectangle(bar_x, bar_y, bar_x + bar_width, bar_y + bar_height, al_map_rgb(255, 255, 255), 2.0);
}
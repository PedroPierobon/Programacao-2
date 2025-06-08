#include <stdio.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_ttf.h>

#include "core.h"
#include "setting.h"

#define FPS 30

static ALLEGRO_DISPLAY* display = NULL;
static ALLEGRO_EVENT_QUEUE* queue = NULL;
static ALLEGRO_TIMER* timer = NULL;
static ALLEGRO_FONT* font = NULL;

bool core_init() {
    if (!al_init()) {
        fprintf(stderr, "Falhou em inicializar o Allegro.\n");
        return false;
    }
    if (!al_install_keyboard()) {
        fprintf(stderr, "Falhou em inicializar o teclado.\n");
        return false;
    }

    timer = al_create_timer(1.0 / FPS);
    if (!timer) {
        fprintf(stderr, "Falhou em criar o timer.\n");
        return false;
    }

    al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);

    ALLEGRO_DISPLAY_MODE disp_data;

    al_get_display_mode(0, &disp_data);

    display = al_create_display(disp_data.width, disp_data.height);
    if (!display) {
        fprintf(stderr, "Falhou em criar a janela.\n");
        al_destroy_timer(timer);
        return false;
    }

    settings_set_res(disp_data.width, disp_data.height);

    queue = al_create_event_queue();
    if (!queue) {
        fprintf(stderr, "Falhou em criar a fila de eventos.\n");
        al_destroy_display(display);
        al_destroy_timer(timer);
        return false;
    }

    al_init_font_addon();
    al_init_ttf_addon();

    if (!al_init_image_addon()) {
        fprintf(stderr, "Falhou em inicializar o addon de imagens.\n");
        al_destroy_event_queue(queue);
        al_destroy_display(display);
        al_destroy_timer(timer);
    }

    if(!al_init_primitives_addon()) {
        fprintf(stderr, "Falhou em inicializar o addon de primitivas.\n");
        al_destroy_event_queue(queue);
        al_destroy_display(display);
        al_destroy_timer(timer);
        return false;
    }

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    return true;
}

void core_shutdown() {
    if (font) al_destroy_font(font);
    if (queue) al_destroy_event_queue(queue);
    if (display) al_destroy_display(display);
    if (timer) al_destroy_timer(timer);
}

ALLEGRO_DISPLAY* core_get_display() {
    return display;
}

ALLEGRO_EVENT_QUEUE* core_get_event_queue() {
    return queue;
}

ALLEGRO_TIMER* core_get_timer() {
    return timer;
}

ALLEGRO_FONT* core_get_font() {
    return font;
}
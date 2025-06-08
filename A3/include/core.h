#ifndef CORE_H
#define CORE_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>

bool core_init();

void core_shutdown();

ALLEGRO_DISPLAY* core_get_display();
ALLEGRO_EVENT_QUEUE* core_get_event_queue();
ALLEGRO_TIMER* core_get_timer();
ALLEGRO_FONT* core_get_font();

#endif
#include <allegro5/allegro.h>

#include "core.h"
#include "game_states.h"
//#include "menu.h"
//#include "joystick.h"

int main(){
  if(!core_init()) return -1;

  GameState current_state = MENU;
  bool running = true;

  al_start_timer(core_get_timer());

  while(running) {
    ALLEGRO_EVENT event;
    al_wait_for_event(core_get_event_queue(), &event);

    switch (current_state) {
      case MENU:
        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
          current_state = EXIT;
        }

        break;
      case JOYSTICK:
        //adicionar lógica do joystick
        break;
      case PLAYING:
        //adicionar lógica do jogo
        break;
      case GAME_OVER:
        //adicionar lógica de game over
        break;  
      case EXIT:
        running = false;
        break;
    }
    if (al_is_event_queue_empty(core_get_event_queue())) {
      al_clear_to_color(al_map_rgb(0, 0, 0));

      if (current_state == MENU) {
           al_draw_text(core_get_font(), al_map_rgb(255, 255, 255), 400, 300, ALLEGRO_ALIGN_CENTER, "MENU SCREEN - PRESS X TO CLOSE");
      }
      
      al_flip_display();
    }
  }
  core_shutdown();
  return 0;
}

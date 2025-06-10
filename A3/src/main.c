// Compilar com: gcc main.c core.c setting.c menu.c assets.c player.c playing.c -o game $(pkg-config --cflags --libs allegro-5 allegro_font-5 allegro_primitives-5 allegro_image-5 allegro_ttf-5 allegro_main-5)
#include <allegro5/allegro.h>
#include <stdio.h>

#include "core.h"
#include "setting.h"
#include "game_states.h"
#include "menu.h"
#include "playing.h"
#include "assets.h"
#include "controls.h"


int main(){
  settings_init();
  if(!core_init()) return -1;
  if(!assets_init()) {
    core_shutdown();
    return -1;
  }
  GameState current_state = MENU;
  bool running = true;

  al_start_timer(core_get_timer());

  while(running) {

    switch (current_state) {
      case MENU:
        current_state = menu_run();
        break;

      case PLAYING:
        current_state = playing_run();
        break;

      case CONTROLS:
        current_state = controls_run();
        break;

      case EXIT:
        running = false;
        break;

      default:
        printf("Unknown game state, exiting.\n");
        current_state = EXIT;
        break;
    }
  }
  assets_shutdown();
  core_shutdown();
  return 0;
}

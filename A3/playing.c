#include "playing.h"
#include "core.h"
#include "player.h"

GameState playing_run() {
    ALLEGRO_EVENT_QUEUE* queue = core_get_event_queue();
    bool redraw = true;
    bool running_state = true;


    Player* player1 = player_create();

    while (running_state) {
        ALLEGRO_EVENT event;
        al_wait_for_event(queue, &event);

        player_handle_input(player1, &event);

        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            player_destroy(player1);
            return EXIT;
        }
        if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                running_state = false;
            }
        }
        if (event.type == ALLEGRO_EVENT_TIMER) {
            player_update(player1);
            redraw = true;
        }

        if (redraw && al_is_event_queue_empty(queue)) {
            redraw = false;
            al_clear_to_color(al_map_rgb(0, 0, 0));

            player_draw(player1);

            al_flip_display();
        }
    }

    player_destroy(player1);
    return MENU;
}
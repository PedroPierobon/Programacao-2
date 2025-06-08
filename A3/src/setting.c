#include "setting.h"

// Global somente dentro deste arquivo
static GameSettings settings;

void settings_init() {
    settings.screen_height = 720;
    settings.screen_width = 1080;
}

void settings_set_res(int width, int height){
    settings.screen_height = height;
    settings.screen_width = width;
}

const GameSettings* settings_get() {
    return &settings;
}
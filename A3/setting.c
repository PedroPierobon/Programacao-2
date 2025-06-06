#include "setting.h"

// Global somente dentro deste arquivo
static GameSettings settings;

void settings_init() {
    settings.screen_width = 800;
    settings.screen_height = 600;
}

const GameSettings* settings_get() {
    return &settings;
}
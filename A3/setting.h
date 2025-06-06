#ifndef SETTING_H
#define SETTING_H

typedef struct {
    int screen_width;
    int screen_height;
} GameSettings;

void settings_init();

// Const para não poder ser alterada
const GameSettings* settings_get();

#endif
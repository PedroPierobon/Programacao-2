#ifndef SETTING_H
#define SETTING_H

typedef struct {
    int screen_width;
    int screen_height;
} GameSettings;

void settings_init();

void settings_set_res(int width, int height);

// Const para não poder ser alterada
const GameSettings* settings_get();

#endif
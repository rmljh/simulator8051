#ifndef LED_DISPLAY_H
#define LED_DISPLAY_H

#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../include/inst.h"
#include "../include/memory.h"
#include "../include/hexfile_load.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 200

#define LED_WIDTH 6
#define LED_HEIGHT 8
#define LED_COUNT 8
#define LED_GAP 6
#define LED_X_START 255
#define LED_Y 90


void updateLEDs(SDL_Renderer* renderer, word_t ledMask);

#endif // LED_DISPLAY_H


#ifndef SEG7_DISPLAY_H
#define SEG7_DISPLAY_H

#include <SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include "../include/inst.h"
#include "../include/memory.h"
#include "../include/hexfile_load.h"
#define SCREEN_WIDTH  1200
#define SCREEN_HEIGHT 680

#define SEG_WIDTH      16
#define SEG_HEIGHT     16
#define SEG_THICKNESS  3
#define SEG_SPACING    3

#define SEG_X_START    200
    

#define NUM_DIGITS     8

extern word_t seg7_cache[8];
void drawDigit(SDL_Renderer* renderer, word_t digit, int x, int y);
void drawNumber(SDL_Renderer* renderer, int x, int y);

#endif	// SEG7_DISPLAY_H

#include "seg7.h"

word_t seg7_cache[8] = { 0x00 };
const int segments[11][7] = {
    {1, 1, 1, 1, 1, 1, 0}, // 0
    {0, 1, 1, 0, 0, 0, 0}, // 1
    {1, 1, 0, 1, 1, 0, 1}, // 2
    {1, 1, 1, 1, 0, 0, 1}, // 3
    {0, 1, 1, 0, 0, 1, 1}, // 4
    {1, 0, 1, 1, 0, 1, 1}, // 5
    {1, 0, 1, 1, 1, 1, 1}, // 6
    {1, 1, 1, 0, 0, 0, 0}, // 7
    {1, 1, 1, 1, 1, 1, 1}, // 8
    {1, 1, 1, 1, 0, 1, 1}, // 9
    {0, 0, 0, 0, 0, 0, 0}, // null
};

void drawDigit(SDL_Renderer* renderer, word_t digit, int x, int y) {
    SDL_Rect segmentsRects[8] = {
        {x + SEG_SPACING, y, SEG_WIDTH, SEG_THICKNESS},
        {x + SEG_WIDTH + SEG_SPACING, y + SEG_SPACING, SEG_THICKNESS, SEG_HEIGHT},
        {x + SEG_WIDTH + SEG_SPACING, y + SEG_HEIGHT + 2 * SEG_SPACING, SEG_THICKNESS, SEG_HEIGHT},
        {x + SEG_SPACING, y + 2 * SEG_HEIGHT + 2 * SEG_SPACING, SEG_WIDTH, SEG_THICKNESS},
        {x, y + SEG_HEIGHT + 2 * SEG_SPACING, SEG_THICKNESS, SEG_HEIGHT},
        {x, y + SEG_SPACING, SEG_THICKNESS, SEG_HEIGHT},
        {x + SEG_SPACING, y + SEG_HEIGHT + SEG_SPACING, SEG_WIDTH, SEG_THICKNESS},
        {x + 3 * SEG_SPACING + SEG_WIDTH, y + 2 * SEG_HEIGHT + 2 * SEG_SPACING, SEG_THICKNESS, SEG_THICKNESS}
    };

    for (int i = 0; i < 9; ++i) {
        if (BITS(digit, i, i)) {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red
        } else {
            SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255); // Dark grey
        }
        SDL_RenderFillRect(renderer, &segmentsRects[i]);
    }
}

void drawNumber(SDL_Renderer* renderer, int x, int y) {
    
    for (int i = 0; i < 8; ++i) {
    	int posX = x + (SEG_WIDTH + SEG_SPACING) * 2 * i;
    	drawDigit(renderer, seg7_cache[i], posX, y);
    }
}

/*
int main(int argc, char* argv[]) {
    char* filename = "../difftest/demo1.hex";
    word_t* code;
    printf("begin test: %s\n", filename);
    code = hexfile_load(filename);
    mcu_init(&mcu);
    memory.code = code;

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("7-Segment Display", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    bool quit = false;
    SDL_Event e;
    word_t number = 0;
    word_t digitPosition = 0;
    
    while (!quit) {
    
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }
	inst_exec_once(&inst_encode);
	digitPosition = Mr(0xA0, MEM_TYPE_IRAM);
	number   = Mr(0x80, MEM_TYPE_IRAM);
	printf("P0 = %x, P2 = %x\n", number, digitPosition);
	if (seg7_cache[digitPosition] == number) continue;
	seg7_cache[digitPosition] = number;
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        drawNumber(renderer, SCREEN_WIDTH / 2 - (NUM_DIGITS * (SEG_WIDTH + SEG_SPACING)) / 2, SCREEN_HEIGHT / 2 - SEG_HEIGHT / 2);
        SDL_RenderPresent(renderer);

    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
*/

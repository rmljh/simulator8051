#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../include/inst.h"
#include "../include/memory.h"
#include "../include/hexfile_load.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 200

#define LED_WIDTH 40
#define LED_HEIGHT 40
#define LED_COUNT 8
#define LED_GAP 20
#define LED_X_START 100
#define LED_Y 80

// Function to update the LEDs based on the input mask
void updateLEDs(SDL_Renderer* renderer, word_t ledMask) {
    for (int i = 0; i < LED_COUNT; i++) {
        SDL_Rect ledRect = {LED_X_START + i * (LED_WIDTH + LED_GAP), LED_Y, LED_WIDTH, LED_HEIGHT};
        if (ledMask & (1 << (LED_COUNT - 1 - i))) {
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Green for active LED
        } else {
            SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255); // Gray for inactive LEDs
        }
        SDL_RenderFillRect(renderer, &ledRect);
    }
}

int main(int argc, char* argv[]) {
    char* filename = "../difftest/demo.hex";
    word_t* code;
    printf("begin test: %s\n", filename);
    code = hexfile_load(filename);
    mcu_init(&mcu);
    memory.code = code;
    
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    word_t ledMask = 0xAA; // Default mask, can be overridden by command-line argument

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    // Create window
    window = SDL_CreateWindow("LED Flow Lights", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        fprintf(stderr, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
    }

    // Create renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        fprintf(stderr, "Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return EXIT_FAILURE;
    }

    int running = 1;
    SDL_Event e;

    while (running) {
        // Handle events on queue
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                running = 0;
            }
        }
	inst_exec_once(&inst_encode);
	if (ledMask == Mr(0x80, MEM_TYPE_IRAM)) continue;
	ledMask = Mr(0x80, MEM_TYPE_IRAM);
      	// printf("%x\n", ledMask);
        // Clear screen
        
    	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    	SDL_RenderClear(renderer);
        // Update LEDs based on the current mask
        updateLEDs(renderer, ledMask);

        // Update screen
        SDL_RenderPresent(renderer);

        // Small delay to keep the window responsive
        // SDL_Delay(100);
    }

    // Clean up
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}
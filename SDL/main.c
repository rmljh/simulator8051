#include "led.h"
#include "seg7.h"
#include <unistd.h>

#define CORE_X      270
#define CORE_Y      160
#define CORE_WIDTH  60
#define CORE_HEIGHT 60

#define KEY0_X      400
#define KEY0_Y      350
#define KEY0_WIDTH  25
#define KEY0_HEIGHT 25

bool isInsideCore(int x, int y) {
  return (x >= CORE_X && x <= CORE_X + CORE_WIDTH && y >= CORE_Y && y <= CORE_Y + CORE_HEIGHT);
}

bool isInsideKey0(int x, int y) {
  return (x >= KEY0_X && x <= KEY0_X + KEY0_WIDTH && y >= KEY0_Y && y <= KEY0_Y + KEY0_HEIGHT);
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
    return EXIT_FAILURE;
  }
  char* filename = argv[1];
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    return EXIT_FAILURE;
  }

  SDL_Window* window = SDL_CreateWindow("7-Segment Display", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 500, 400, SDL_WINDOW_SHOWN);
  if (!window) {
    fprintf(stderr, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
    SDL_Quit();
    return EXIT_FAILURE;
  }

  SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (!renderer) {
    fprintf(stderr, "Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_FAILURE;
  }

  SDL_Surface* background = SDL_LoadBMP("./SDL/background.bmp");
  SDL_Texture* backgroundTexture = SDL_CreateTextureFromSurface(renderer, background);
  SDL_FreeSurface(background);

  word_t* code;
  printf("begin test: %s\n", filename);
  code = hexfile_load(filename);
  mcu_init(&mcu);
  memory.code = code;
  
  word_t number = 0;
  word_t digitPosition = 0;
  word_t led_cache = 0;
  
  SDL_Event e;
  SDL_Rect backgroundRect = {0, 0, 500, 400};
  bool quit = false;
  while (!quit) {
    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_QUIT) {
        quit = true;
      } else if (e.type == SDL_MOUSEBUTTONDOWN) {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        if (e.button.button == SDL_BUTTON_LEFT && isInsideCore(mouseX, mouseY)) {
            // Execute another executable
          if (fork() == 0) {
            execl("./Gtkmonitor", "Gtkmoitor", filename, NULL);
            // printf("child");
            exit(0);
          }
        } else if (e.button.button == SDL_BUTTON_LEFT && isInsideKey0(mouseX, mouseY)) {
          printf("key0 press\n");
          Mw(0xB0, ~Mr(0xB0, MEM_TYPE_IRAM), MEM_TYPE_IRAM);
        }
      }
    }
	
    inst_exec_once(&inst_encode);
    digitPosition = Mr(0xA0, MEM_TYPE_IRAM);
    number        = Mr(0x80, MEM_TYPE_IRAM);
    // printf("P0 = %x, P2 = %x\n", number, digitPosition);
    word_t sbtLedSel = BITS(digitPosition, 3, 3);
    // printf("P0 = %x, P2 = %x\n", number, digitPosition);
    if (number == 0) continue;
    if (!sbtLedSel && seg7_cache[digitPosition] == number) continue;
    if (!sbtLedSel) seg7_cache[digitPosition] = number;
    if (sbtLedSel) led_cache = number;
	// printf("Led = %x", led_cache);
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, backgroundTexture, NULL, &backgroundRect); // Render background texture
    // Draw blue COREangle as background for seg7 and LEDs
    // SDL_CORE backgroundRect = {140, 0, 310, 60};  // Define the COREangle
    // SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);  // Set COREangle color to blue
    // SDL_RenderFillCORE(renderer, &backgroundRect);  // Fill the COREangle

    drawNumber(renderer, 155, 12); // Example usage
    updateLEDs(renderer, led_cache); // Example usage
    

    SDL_RenderPresent(renderer);
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}


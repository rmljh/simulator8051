#include <SDL.h>
#include <stdio.h>

#define SCREEN_WIDTH  1000
#define SCREEN_HEIGHT 480

#define SEG_WIDTH      30
#define SEG_HEIGHT     60
#define SEG_THICKNESS  10
#define SEG_SPACING    10

#define NUM_DIGITS     3


// 7段数码管的绘制模板
const int segments[10][7] = {
    {1, 1, 1, 1, 1, 1, 0}, // 0
    {0, 1, 1, 0, 0, 0, 0}, // 1
    {1, 1, 0, 1, 1, 0, 1}, // 2
    {1, 1, 1, 1, 0, 0, 1}, // 3
    {0, 1, 1, 0, 0, 1, 1}, // 4
    {1, 0, 1, 1, 0, 1, 1}, // 5
    {1, 0, 1, 1, 1, 1, 1}, // 6
    {1, 1, 1, 0, 0, 0, 0}, // 7
    {1, 1, 1, 1, 1, 1, 1}, // 8
    {1, 1, 1, 1, 0, 1, 1}  // 9
};

// 绘制单个数字的7段数码管
void drawDigit(SDL_Renderer* renderer, int digit, int x, int y) {
    // 数码管的7个段的坐标位置
    SDL_Rect segmentsRects[7] = {
        {x + SEG_SPACING, y, SEG_WIDTH, SEG_THICKNESS},                     // A
        {x + SEG_WIDTH + 2 * SEG_SPACING, y + SEG_SPACING, SEG_THICKNESS, SEG_HEIGHT}, // B
        {x + SEG_WIDTH + 2 * SEG_SPACING, y + SEG_HEIGHT + 2 * SEG_SPACING, SEG_THICKNESS, SEG_HEIGHT}, // C
        {x + SEG_SPACING, y + 2 * SEG_HEIGHT + 3 * SEG_SPACING, SEG_WIDTH, SEG_THICKNESS}, // D
        {x, y + SEG_HEIGHT + 2 * SEG_SPACING, SEG_THICKNESS, SEG_HEIGHT}, // E
        {x, y + SEG_SPACING, SEG_THICKNESS, SEG_HEIGHT},                  // F
        {x + SEG_SPACING, y + SEG_HEIGHT + SEG_SPACING, SEG_WIDTH, SEG_THICKNESS} // G
    };

    // 绘制7段数码管的每个段
    for (int i = 0; i < 7; ++i) {
        if (segments[digit][i]) {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // 设置为红色
        } else {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // 设置为黑色
        }
        SDL_RenderFillRect(renderer, &segmentsRects[i]);
    }
}

// 绘制数字字符串
void drawNumber(SDL_Renderer* renderer, int number, int x, int y) {
    char numberStr[NUM_DIGITS];
    sprintf(numberStr, "%03d", number); // 将数字格式化为3位数的字符串

    for (int i = 0; i < NUM_DIGITS; ++i) {
        int digit = numberStr[i] - '0'; // 获取当前数字字符的整数值
        drawDigit(renderer, digit, x + (SEG_WIDTH + SEG_SPACING) * 2 * i, y);
    }
}

int main(int argc, char* argv[]) {
    // 初始化SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    // 创建窗口和渲染器
    SDL_Window* window = SDL_CreateWindow("7-Segment Display", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL) {
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // 主循环
    bool quit = false;
    SDL_Event e;
    int number = 0;

    while (!quit) {
        // 处理事件
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }

        // 清空屏幕
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // 绘制数字
        drawNumber(renderer, number, SCREEN_WIDTH / 2 - (NUM_DIGITS * (SEG_WIDTH + SEG_SPACING)) / 2, SCREEN_HEIGHT / 2 - SEG_HEIGHT / 2);

        // 显示绘制结果
        SDL_RenderPresent(renderer);

        // 延迟一段时间
        SDL_Delay(1000);
        
        // 数字加1，循环显示0到999
        number = (number + 1) % 1000;
    }

    // 清理资源
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}

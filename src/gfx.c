#include "gfx.h"


SDL_Renderer *renderer;
SDL_Window *window;
SDL_Event e;

uint8_t init_sdl()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL could not be initialised. %s.\n", SDL_GetError());
        return 1;
    } else {
        SDL_Log("Initialised SDL.\n");
    }

    if(SDL_CreateWindowAndRenderer(WIN_COLS, WIN_ROWS, SDL_WINDOW_RESIZABLE | SDL_RENDERER_ACCELERATED, &window, &renderer) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not create window or renderer. %s.\n", SDL_GetError());
        return 1;
    } else {
        SDL_SetWindowTitle(window, "Chip-8 Emulator");
        SDL_Log("Created window and renderer.\n");
    }

    return 0;
}

#ifdef DEBUG
void print(bool display[DIS_ROWS][DIS_COLS])
{
    for(int i = 0; i < DIS_COLS; i++) {
        printf("[");
        for(int j = 0; j < DIS_ROWS; j++)
            printf("%d ", display[i][j]);
        printf("]\n");
    }
}
#endif

void render(bool display[DIS_ROWS][DIS_COLS])
{
    SDL_SetRenderDrawColor(renderer, 0x0, 0x0, 0x0, 0xFF);
    SDL_RenderClear(renderer);

    const uint16_t SCALED_ROWS = WIN_ROWS / DIS_ROWS;
    const uint16_t SCALED_COLS = WIN_COLS / DIS_COLS;

    for(uint8_t y = 0; y < DIS_ROWS; ++y) {
        for(uint8_t x = 0; x < DIS_COLS; ++x) {
            if(display[y][x] == 1) {
                SDL_Rect rect = { x * SCALED_COLS, y * SCALED_ROWS, SCALED_ROWS, SCALED_COLS };
                SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }
    SDL_RenderPresent(renderer);
}

void handle_inputs(bool *quit, bool key[ARR_SIZE])
{
    memset(key, 0, sizeof(*key));
    while(SDL_PollEvent(&e) != 0) {
        switch(e.type) {
            case SDL_KEYDOWN:
                switch(e.key.keysym.sym) {
                    case SDLK_0:
                        key[0x0] = 1;
                        break;
                    case SDLK_1:
                        key[0x1] = 1;
                        break;
                    case SDLK_2:
                        key[0x2] = 1;
                        break;
                    case SDLK_3:
                        key[0x3] = 1;
                        break;
                    case SDLK_4:
                        key[0x4] = 1;
                        break;
                    case SDLK_5:
                        key[0x5] = 1;
                        break;
                    case SDLK_6:
                        key[0x6] = 1;
                        break;
                    case SDLK_7:
                        key[0x7] = 1;
                        break;
                    case SDLK_8:
                        key[0x8] = 1;
                        break;
                    case SDLK_9:
                        key[0x9] = 1;
                        break;
                    case SDLK_a:
                        key[0xA] = 1;
                        break;
                    case SDLK_b:
                        key[0xB] = 1;
                        break;
                    case SDLK_c:
                        key[0xC] = 1;
                        break;
                    case SDLK_d:
                        key[0xD] = 1;
                        break;
                    case SDLK_e:
                        key[0xE] = 1;
                        break;
                    case SDLK_f:
                        key[0xF] = 1;
                        break;
                    case SDLK_ESCAPE: case SDLK_q:
                        *quit = true;
                        break;
            }
                break;
        }
    }
}

void close_sdl()
{
    puts("Destroying window and renderer...");
    SDL_DestroyRenderer(renderer);
    renderer = NULL;
    SDL_DestroyWindow(window);
    window = NULL;
    puts("Done! Closing SDL...");
    SDL_Quit();
    puts("Goodbye!");
}

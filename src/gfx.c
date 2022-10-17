#include "gfx.h"
#include "util.h"


SDL_Renderer *renderer;
SDL_Window *window;
SDL_Event e;

int init_sdl()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL could not be initialised. %s.\n", SDL_GetError());
        return 1;
    } else {
        SDL_Log("Initialised SDL!\n");
    }

    if(SDL_CreateWindowAndRenderer(WIN_ROWS, WIN_COLS, SDL_WINDOW_RESIZABLE, &window, &renderer) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not create window or renderer. %s.\n", SDL_GetError());
        return 1;
    } else {
        SDL_Log("Created window and renderer!\n");
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

void render(bool (*display)[DIS_COLS])
{
    SDL_SetRenderDrawColor(renderer, 0x0, 0x0, 0x0, 0xFF);
    SDL_RenderClear(renderer);

    uint16_t w = WIN_ROWS / DIS_ROWS;
    uint16_t h = WIN_COLS / DIS_COLS;

    for(uint8_t y = 0x0; y < DIS_ROWS; ++y) {
        for(uint8_t x = 0x0; x < DIS_COLS; ++x) {
            if(display[x][y] == 1) {
                SDL_Rect rect = { x * w, y * h, w, h };
                SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }

    SDL_RenderPresent(renderer);
}

void handle_inputs(bool *quit)
{
    while(SDL_PollEvent(&e) != 0) {
        switch(e.type) {
            case SDL_KEYDOWN:
                switch(e.key.keysym.sym) {
                    case SDLK_0:
                        SDL_Log("Pressed 0");
                        break;
                    case SDLK_1:
                        SDL_Log("Pressed 1");
                        break;
                    case SDLK_2:
                        SDL_Log("Pressed 2");
                        break;
                    case SDLK_3:
                        SDL_Log("Pressed 3");
                        break;
                    case SDLK_4:
                        SDL_Log("Pressed 4");
                        break;
                    case SDLK_5:
                        SDL_Log("Pressed 5");
                        break;
                    case SDLK_6:
                        SDL_Log("Pressed 6");
                        break;
                    case SDLK_7:
                        SDL_Log("Pressed 7");
                        break;
                    case SDLK_8:
                        SDL_Log("Pressed 8");
                        break;
                    case SDLK_9:
                        SDL_Log("Pressed 9");
                        break;
                    case SDLK_a:
                        SDL_Log("Pressed A");
                        break;
                    case SDLK_b:
                        SDL_Log("Pressed B");
                        break;
                    case SDLK_c:
                        SDL_Log("Pressed C");
                        break;
                    case SDLK_d:
                        SDL_Log("Pressed D");
                        break;
                    case SDLK_e:
                        SDL_Log("Pressed E");
                        break;
                    case SDLK_f:
                        SDL_Log("Pressed F");
                        break;
                    case SDLK_ESCAPE: case SDLK_q:
                        *quit = true;
                        break;
            }
        }
    }
}

void close_sdl()
{
    puts("Destroying window and renderer...");
    SDL_DestroyWindow(window);
    window = NULL;
    SDL_DestroyRenderer(renderer);
    renderer = NULL;
    puts("Done! Closing SDL...");
    SDL_Quit();
    puts("Goodbye!");
}

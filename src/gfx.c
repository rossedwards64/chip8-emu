#include "gfx.h"

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

    if(SDL_CreateWindowAndRenderer(64, 32, SDL_WINDOW_RESIZABLE, &window, &renderer) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not create window or renderer. %s.\n", SDL_GetError());
        return 1;
    } else {
        SDL_Log("Created window and renderer!\n");
    }

    return 0;
}

void update()
{

}

void render()
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
}

void handle_inputs(bool *quit)
{
    while(SDL_PollEvent(&e) != 0) {
        if(e.type == SDL_QUIT) { *quit = true; }
        switch(e.type) {
            case SDL_KEYDOWN:
                switch(e.key.keysym.sym) {
                    case SDLK_w:
                        SDL_Log("Pressed W");
                        break;
                    case SDLK_s:
                        SDL_Log("Pressed S");
                        break;
                    case SDLK_a:
                        SDL_Log("Pressed A");
                        break;
                    case SDLK_d:
                        SDL_Log("Pressed D");
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

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdio.h>

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
TTF_Font *font = NULL;
SDL_Texture *texture = NULL;

void quit();
bool initRenderer() {
  {

    if (SDL_Init(SDL_INIT_VIDEO) == false) {
      SDL_Log("SDL_Init failed: %s", SDL_GetError());
    }

    if (TTF_Init() == false) {
      SDL_Log("TTF_Init failed: %s", SDL_GetError());
    }

    if (SDL_CreateWindowAndRenderer("Example Renderer Clear", 640, 480, 0,
                                    &window, &renderer) == false) {
      SDL_Log("SDL_CreateWindowAndRenderer failed: %s", SDL_GetError());
    }

    font = TTF_OpenFont(
        "/home/theo/.local/share/fonts/FiraCodeNerdFont-Regular.ttf", 16);
    if (!font) {
      SDL_Log("TTF_OpenFont failed: %s", SDL_GetError());
    }

    SDL_Color white = {255, 255, 255, SDL_ALPHA_OPAQUE};
    SDL_Surface *text_surface =
        TTF_RenderText_Blended(font, "Hello World!", 12, white);
    if (!text_surface) {
      SDL_Log("TTF_RenderText_Blended failed: %s", SDL_GetError());
    }

    texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    SDL_DestroySurface(text_surface);
    if (!texture) {
      SDL_Log("SDL_CreateTextureFromSurface failed: %s", SDL_GetError());
    }


    return 0;
  }
}

bool renderFrame() {
      bool shouldStop = false;
      SDL_Event event;
      while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT)
          shouldStop = true;
        if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_Q)
          shouldStop = true;
      }

      int w, h;
      SDL_GetRenderOutputSize(renderer, &w, &h);
      float tex_w, tex_h;
      SDL_GetTextureSize(texture, &tex_w, &tex_h);
      SDL_FRect dst = {.x = (w - tex_w) / 2.0f,
                       .y = (h - tex_h) / 2.0f,
                       .w = (float)tex_w,
                       .h = (float)tex_h};

      SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
      SDL_RenderClear(renderer);
      SDL_RenderTexture(renderer, texture, NULL, &dst);
      SDL_RenderPresent(renderer);
      return shouldStop;
  
}


void quit() {
    if (texture)
      SDL_DestroyTexture(texture);
    if (font)
      TTF_CloseFont(font);
    if (renderer)
      SDL_DestroyRenderer(renderer);
    if (window)
      SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
  
}

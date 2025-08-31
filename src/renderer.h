#include "../includes/clay.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdbool.h>
bool initRenderer();
bool renderFrame(Clay_RenderCommandArray commands);

void quit();

typedef struct {
  SDL_Renderer *renderer;
  TTF_TextEngine *textEngine;
  TTF_Font **fonts;
} Clay_SDL3RendererData;
void SDL_Clay_RenderClayCommands(Clay_SDL3RendererData *rendererData,
                                 Clay_RenderCommandArray *rcommands);

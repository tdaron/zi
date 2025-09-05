#ifndef RENDERER_H
#define RENDERER_H
#include <stdbool.h>
#include <clay.h>
bool render(Clay_RenderCommandArray commands);
void init_renderer();
void quit_renderer();
int renderer_height();
int renderer_width();

#endif

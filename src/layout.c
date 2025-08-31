#include "../includes/clay.h"

void drawBufferLine() {
  const int buffers = 4;

  CLAY({.id = CLAY_ID("buffers"),
        .layout = {.childGap = 8,
                    .padding = 10,
                   .childAlignment = {.y = CLAY_ALIGN_Y_CENTER},
                   .sizing = {CLAY_SIZING_GROW(0), CLAY_SIZING_FIT(30)}},
        .backgroundColor = {45, 42, 54, 255}}) {
    for (int i = 0; i < buffers; i++) {
      CLAY({.layout = {.sizing = {CLAY_SIZING_FIT(100), CLAY_SIZING_FIT(20)}},

            .backgroundColor = {149, 165, 166, 255}});
    }
  }
}

Clay_RenderCommandArray DrawLayout() {
  Clay_BeginLayout();
  CLAY({.id = CLAY_ID("root"),
        .layout = {.sizing = {CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0)}},
        .backgroundColor = {44, 62, 80, 255}}) {
    drawBufferLine();
  }
  Clay_RenderCommandArray renderCommands = Clay_EndLayout();
  return renderCommands;
}

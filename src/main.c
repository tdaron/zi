#include "renderer.h"
#include <stdio.h>
#define CLAY_IMPLEMENTATION
#include "../includes/clay.h"
#include "layout.h"

void HandleClayErrors(Clay_ErrorData errorData) {
    // See the Clay_ErrorData struct for more information
    printf("%s", errorData.errorText.chars);
}

int main() {
  initRenderer();
  int running = 1;
  uint64_t totalMemorySize = Clay_MinMemorySize();
  Clay_Arena arena = Clay_CreateArenaWithCapacityAndMemory(
      totalMemorySize, malloc(totalMemorySize));

  // Note: screenWidth and screenHeight will need to come from your environment,
  // Clay doesn't handle window related tasks
  Clay_Initialize(arena, (Clay_Dimensions){640, 480},
                  (Clay_ErrorHandler){HandleClayErrors, NULL});
  while (running) {
    Clay_RenderCommandArray render_commands = DrawLayout();
    running = !renderFrame(render_commands);
  }
  quit();
}

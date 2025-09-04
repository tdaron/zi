#include "buffer.h"
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

void renderer_init() {
  InitWindow(640, 480, "Zi Editor");
  SetTargetFPS(60);
}

bool renderer_draw(Buffer *buffer) {
  BeginDrawing();
  DrawRectangle(0, 0, 640, 480, BLACK);
  if (IsKeyPressed(KEY_LEFT)) {
    move_cursor(buffer, -1);
  }
  if (IsKeyPressed(KEY_RIGHT)) {
    move_cursor(buffer, 1);
  }
  if (IsKeyPressed(KEY_BACKSPACE)) {
    delete_chars(buffer, 1);
  }
  int key = GetCharPressed();
  while (key > 0) {
    insert_char(buffer, (char)key);
    // key is Unicode codepoint
    key = GetCharPressed();
  }

  char* content = get_raw_content(buffer);
  DrawText(content, 10, 10, 16, WHITE);
  EndDrawing();
  free(content);
  return true;
}

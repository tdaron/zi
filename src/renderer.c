#include <buffer.h>
#include <raylib.h>
#include <stdlib.h>

Font font;

void renderer_init() {
  InitWindow(640, 480, "Zi Editor");
  SetTargetFPS(60);
  font = LoadFontEx("/home/theo/.local/share/fonts/FiraCodeNerdFont-Regular.ttf", 16, 0, 250);
  SetTextureFilter(font.texture, TEXTURE_FILTER_POINT);
}


void draw_buffer(Buffer *b, Font font, int start_x, int start_y, int font_size) {
    int x = start_x;
    int y = start_y;

    char *s1, *s2;
    int len1, len2;
    get_contents(b, &s1, &len1, &s2, &len2);

    int cursor_index = get_cursor_position(b); // position en caractères
    int char_index = 0; // compteur global pour trouver où placer le curseur
    int cursor_x = start_x;
    int cursor_y = start_y;

    // rendu du premier segment
    for (int i = 0; i < len1; i++) {
        char c = s1[i];

        if (char_index == cursor_index) {
            cursor_x = x;
            cursor_y = y;
        }

        if (c == '\n') {
            x = start_x;
            y += font_size + 4;
        } else {
            int cp = (unsigned char)c;
            DrawTextCodepoint(font, cp, (Vector2){x, y}, font_size, BLACK);
            GlyphInfo gi = GetGlyphInfo(font, cp);
            x += gi.advanceX;
        }

        char_index++;
    }

    // rendu du second segment
    for (int i = 0; i < len2; i++) {
        char c = s2[i];

        if (char_index == cursor_index) {
            cursor_x = x;
            cursor_y = y;
        }

        if (c == '\n') {
            x = start_x;
            y += font_size + 4;
        } else {
            int cp = (unsigned char)c;
            DrawTextCodepoint(font, cp, (Vector2){x, y}, font_size, BLACK);
            GlyphInfo gi = GetGlyphInfo(font, cp);
            x += gi.advanceX;
        }

        char_index++;
    }

    // si le curseur est à la fin du texte
    if (char_index == cursor_index) {
        cursor_x = x;
        cursor_y = y;
    }

    // dessine le curseur (barre verticale)
    DrawRectangle(cursor_x, cursor_y, 2, font_size, RED);
}

bool renderer_draw(Buffer *buffer) {
  BeginDrawing();
  DrawRectangle(0, 0, 640, 480, BEIGE);
  if (IsKeyDown(KEY_LEFT)) {
    move_cursor(buffer, -1);
  }
  if (IsKeyDown(KEY_RIGHT)) {
    move_cursor(buffer, 1);
  }
  if (IsKeyDown(KEY_BACKSPACE)) {
    delete_chars(buffer, 1);
  }
  if (IsKeyDown(KEY_ENTER)) {
    insert_char(buffer, '\n');
  }
  int key = GetCharPressed();
  while (key > 0) {
    insert_char(buffer, (char)key);
    // key is Unicode codepoint
    key = GetCharPressed();
  }

  char* content = get_raw_content(buffer);
  // DrawTextEx(font, content, (Vector2){10, 10}, 16, 0, BLACK);
  draw_buffer(buffer, font, 10, 10, 16);
  EndDrawing();
  free(content);
  return true;
}

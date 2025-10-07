#ifndef WINDOW_H
#include <buffer.h>
#define WINDOW_H

typedef struct {
  Buffer* buffer;
  int cursorX;
  int cursorY;
  // char file_type; // Speculation about what Window could contain
} View;

#endif

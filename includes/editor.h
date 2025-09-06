#ifndef EDITOR_H
#define EDITOR_H

#include "buffer.h"

typedef enum {
  NORMAL_MODE,
  INSERT_MODE
} EDITOR_MODE;

typedef struct {
  Buffer* buffers[10];
  int buffersCount;
  int currentBuffer;
  char* message;
  EDITOR_MODE mode;
} Editor;

extern Editor editor;

void editor_open_buffer(Editor* e, char* name);
void editor_set_current_buffer(Editor* e, int buffer);
void free_editor(Editor* e);
void editor_next_buffer(Editor* e);
#endif

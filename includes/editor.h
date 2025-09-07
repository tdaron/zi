#ifndef EDITOR_H
#define EDITOR_H

#include "buffer.h"
#include "termgfx.h"
#include <stdbool.h>

typedef enum { NORMAL_MODE, INSERT_MODE, COMMAND_MODE } EDITOR_MODE;

typedef struct {
    Buffer* buffers[10];
    int buffersCount;
    int currentBuffer;
    char* message;
    EDITOR_MODE mode;
    bool shouldClose;
} Editor;

extern Editor editor;

void editor_open_buffer(char* name);
void editor_set_current_buffer(int buffer);
void free_editor();
void editor_next_buffer();
void editor_handle_event(tg_event* event);

#endif

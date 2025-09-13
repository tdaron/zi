#ifndef EDITOR_H
#define EDITOR_H

#include <io.h>
#include <buffer.h>

#include <termgfx.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sv.h>

typedef enum { NORMAL_MODE, INSERT_MODE, INPUT_MODE } EDITOR_MODE;

typedef void (*CALLBACK_FN)(String_View input);


typedef struct {
    String_View prefix;
    char input[256];
    int  length;
    CALLBACK_FN callback;
    
} UserInput;

typedef struct {
    Buffer* buffers[10];
    int buffersCount;
    int currentBuffer;
    char* message;
    EDITOR_MODE mode;
    bool shouldClose;
    UserInput userInput;

    FILE* logFileHandle; // Passed to the buffers on init 
} Editor;

extern Editor editor;

void editor_open_buffer(char* name);
void editor_set_current_buffer(int buffer);
void free_editor();
void editor_next_buffer();
void editor_handle_event(tg_event* event);
void editor_input_mode(String_View prefix, CALLBACK_FN callback);

#endif

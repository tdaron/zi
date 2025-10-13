#ifndef EDITOR_H
#define EDITOR_H

#include <io.h>
#include <buffer.h>

#include <termgfx.h>
#include <stdbool.h>
#include <stdio.h>
#include <view.h>
#include <sv.h>
#include <vec.h>

typedef struct Editor Editor;
typedef struct KeyBinding KeyBinding;
typedef struct Mode Mode;
typedef void (*CALLBACK_FN)(String_View input);
typedef bool (*EVENT_HANDLER)(Mode* mode, tg_event* ev);
typedef void (*KEYBIND_HANDLER)(Mode* mode, tg_event* ev);


void editor_new_view(Buffer* buf);
void editor_set_current_view(int view_id);
void free_editor();
void editor_next_buffer();
void editor_handle_event(tg_event* event);
void editor_input_mode(String_View prefix, CALLBACK_FN callback);
void editor_init(Editor* editor);
Mode* editor_mode_lookup(char* name, int* idx);
bool editor_switch_mode(char* name);
bool editor_bind_key(char* name, KeyBinding binding);


typedef struct KeyBinding{
    const char* key;
    KEYBIND_HANDLER handler;
} KeyBinding;

typedef vec_t(KeyBinding) keybinding_vec_t;

typedef struct Mode {
    char* name;
    char* short_name;
    EVENT_HANDLER events_handler;
    keybinding_vec_t keybindings;
} Mode;

#define DEFINE_MODE(NAME, SHORT, HANDLER) (Mode){NAME, SHORT, HANDLER, {0}}

typedef vec_t(Mode) mode_vec_t;



typedef struct {
    String_View prefix;
    char input[256];
    int  length;
    CALLBACK_FN callback;
} UserInput;

typedef struct Editor {
    // TODO: Either make a DA or a linked list
    View views[10];
    int viewsCount;
    int currentView;
    char* message;
    mode_vec_t modes;
    int current_mode;
    bool shouldClose;
    UserInput userInput;

    FILE* logFileHandle; // Passed to the buffers on init 
} Editor;

extern Editor editor;

#endif

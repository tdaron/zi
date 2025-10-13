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
#include <uthash.h>

typedef struct Mode Mode;
typedef void (*CALLBACK_FN)(String_View input);
typedef void (*EVENT_HANDLER)(Mode* mode, tg_event* ev);


typedef struct {
    const char* key;
    EVENT_HANDLER handler;
    UT_hash_handle hh;
} KeyBinding;

#define STATIC_KEYBINDING(name, handler) {name, handler, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0 };

typedef struct Mode {
    char* name;
    char* short_name;
    EVENT_HANDLER events_handler;
    KeyBinding* keymap;
} Mode;

typedef vec_t(Mode) mode_vec_t;



typedef struct {
    String_View prefix;
    char input[256];
    int  length;
    CALLBACK_FN callback;
    
} UserInput;

typedef struct {
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

void editor_new_view(Buffer* buf);
void editor_set_current_view(int view_id);
void free_editor();
void editor_next_buffer();
void editor_handle_event(tg_event* event);
void editor_input_mode(String_View prefix, CALLBACK_FN callback);
void editor_init(Editor* editor);
Mode* editor_mode_lookup(char* name, int* idx);
#endif

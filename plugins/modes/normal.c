#include "termgfx.h"
#include <editor.h>
#include <vec.h>

void normal_handle_events(Mode* mode, tg_event* ev)
{
    KeyBinding* kb;
    HASH_FIND(hh, mode->keymap, &ev->ch, 1, kb);
    if (kb != NULL) {
        kb->handler(mode, ev);
    }
}

void handle_quit(Mode* mode, tg_event* ev) { editor.shouldClose = true; }

Mode normal_mode = { .name = "normal", .short_name = "NOR", .events_handler = normal_handle_events, .keymap = NULL };
KeyBinding quit = STATIC_KEYBINDING("q", handle_quit);

void normal_init()
{

    HASH_ADD_KEYPTR(hh, normal_mode.keymap, quit.key, strlen(quit.key), &quit);

    vec_push(&editor.modes, normal_mode);
    int idx;
    editor_mode_lookup("normal", &idx);

    editor.current_mode = idx;
}

void normal_apply() {}

#include "termgfx.h"
#include <editor.h>
#include <vec.h>

void normal_handle_events(Mode* mode, tg_event* ev)
{
    if (ev->type == TG_EV_KEY) {
        if (ev->data.key == TG_KEY_CHAR) {


            // Lookup in keybindings
            for (int i = 0; i < mode->keybindings.length; i++) {
                if (*mode->keybindings.data[i].key == ev->ch) mode->keybindings.data[i].handler(mode, ev);
            }
        }
    }
}

void handle_quit(Mode* mode, tg_event* ev) { editor.shouldClose = true; }

Mode normal_mode = { .name = "normal", .short_name = "NOR", .events_handler = normal_handle_events, .keybindings = {0} };
KeyBinding quit = {"q", handle_quit};
void normal_init()
{

    // HASH_ADD_KEYPTR(hh, normal_mode.keymap, quit.key, strlen(quit.key), &quit);

    vec_push(&normal_mode.keybindings, quit);
    vec_push(&editor.modes, normal_mode);
    int idx;
    editor_mode_lookup("normal", &idx);

    editor.current_mode = idx;
}

void normal_apply() {}

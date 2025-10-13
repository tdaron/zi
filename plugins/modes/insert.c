#include "termgfx.h"
#include <buffer.h>
#include <editor.h>
#include <utf8.h>
#include <vec.h>

void insert_handle_events(Mode* mode, tg_event* ev)
{
    Buffer* current_buffer = editor.views[editor.currentView].buffer;
    if (ev->type == TG_EV_KEY) {
        if (ev->data.key == TG_KEY_ESC) {
            int idx;
            editor_mode_lookup("normal", &idx);
            editor.current_mode = idx;
        } else {
            if (ev->data.key == TG_KEY_CHAR) {
                insert_char_bytes(current_buffer, ev->fullChar, CHAR_LEN(ev->fullChar[0]));
            }
        }
    }
    KeyBinding* kb;
    HASH_FIND(hh, mode->keymap, &ev->ch, 1, kb);
    if (kb != NULL) {
        kb->handler(mode, ev);
    }
}

void go_to_insert(Mode* m, tg_event* ev)
{
    int idx;
    editor_mode_lookup("insert", &idx);
    editor.current_mode = idx;
}

Mode insert_mode = { .name = "insert", .short_name = "INS", .events_handler = insert_handle_events, .keymap = NULL };
KeyBinding go_to_insert_kb = STATIC_KEYBINDING("i", go_to_insert);

void insert_init()
{

    vec_push(&editor.modes, insert_mode);

}

void insert_apply()
{

    int idx;
    Mode* normal = editor_mode_lookup("normal", &idx);
    HASH_ADD_KEYPTR(hh, normal->keymap, go_to_insert_kb.key, strlen(go_to_insert_kb.key), &go_to_insert_kb);

}

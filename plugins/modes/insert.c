#include "termgfx.h"
#include <buffer.h>
#include <editor.h>
#include <utf8.h>
#include <vec.h>

bool insert_handle_events(Mode* mode, tg_event* ev)
{
    (void)mode;
    (void)ev;
    Buffer* current_buffer = editor.views[editor.currentView].buffer;
    if (ev->type == TG_EV_KEY) {
        if (ev->data.key == TG_KEY_ESC) {
            editor_switch_mode("normal");
        } else {
            if (ev->data.key == TG_KEY_CHAR) {
                insert_char_bytes(current_buffer, ev->fullChar, CHAR_LEN(ev->fullChar[0]));
            }
        }
    }
    return true;
}

void go_to_insert(Mode* m, tg_event* ev)
{
    (void)m;
    (void)ev;
    editor_switch_mode("insert");
}

Mode insert_mode = DEFINE_MODE("insert", "INS", insert_handle_events);
KeyBinding go_to_insert_kb = { "i", go_to_insert };

void insert_init() { vec_push(&editor.modes, insert_mode); }

void insert_apply()
{
    editor_bind_key("normal", go_to_insert_kb);
}

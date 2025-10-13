#include "termgfx.h"
#include <editor.h>
#include <vec.h>

static bool handle_events(Mode* mode, tg_event* ev)
{
    (void)mode;
    if (ev->type != TG_EV_KEY)
        return false;

    if (ev->data.key == TG_KEY_CHAR) {
        switch (ev->ch) {
        case 'q':
            editor.shouldClose = true;
            return true;
        default:
            break;
        }
    }
    return false;
}

Mode normal_mode = DEFINE_MODE("normal", "NOR", handle_events);
void normal_init()
{
    vec_push(&editor.modes, normal_mode);
    editor.current_mode = editor.modes.length - 1;
}

void normal_apply() { }

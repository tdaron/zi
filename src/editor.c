#include "termgfx.h"
#include <editor.h>
#include <io.h>
#include <string.h>
#include <utf8.h>

void editor_init(Editor* editor) { vec_init(&editor->modes); }

bool editor_switch_mode(char* name)
{
    int idx;
    editor_mode_lookup(name, &idx);
    if (idx != -1) {
        editor.current_mode = idx;
        return true;
    } else
        return false;
}
bool editor_bind_key(char* name, KeyBinding binding)
{

    Mode* m = editor_mode_lookup(name, NULL);
    if (m == NULL)
        return false;
    vec_push(&m->keybindings, binding);
    return true;
}

void editor_new_view(Buffer* buf) { editor.views[editor.viewsCount++] = (View) { buf, 0, 0 }; }
void editor_set_current_view(int viewId) { editor.currentView = viewId; }
Mode* editor_mode_lookup(char* name, int* idx)
{
    for (int i = 0; i < editor.modes.length; i++) {
        if (strcmp(editor.modes.data[i].name, name) == 0) {
            if (idx != NULL)
                *idx = i;
            return &editor.modes.data[i];
        }
    }
    if (idx != NULL)
        *idx = -1;
    return NULL;
}

void free_editor()
{
    for (int i = 0; i < editor.viewsCount; i++) {
        // TODO: This will double free if multiple window
        //  on the same buffer.
        free_buffer(editor.views[i].buffer);
    }
    log_close();
}
void editor_next_buffer() { editor.currentView = (editor.currentView + 1) % editor.viewsCount; }

void command_callback(String_View command)
{
    if (sv_eq(command, SV("q"))) {
        editor.shouldClose = true;
        return;
    }

    editor.message = "Unknown command :(";
}

void editor_handle_event(tg_event* ev)
{
    Mode* mode = &editor.modes.data[editor.current_mode];
    if (!mode->events_handler(mode, ev)) {
        // TODO support binding events with other keys
        // like backspace, return, ...
        // Atm this will have to be implemented natively in events_handler
        
        if (ev->type == TG_EV_KEY && ev->data.key == TG_KEY_CHAR) {
            for (int i = 0; i < mode->keybindings.length; i++) {
                KeyBinding* kb = &mode->keybindings.data[i];
                //TODO proper comparaison
                if (*kb->key == ev->ch) {
                   kb->handler(mode, ev);     
                }
            }
            
        }
    }
}

void editor_input_mode(String_View prefix, CALLBACK_FN callback)
{
    // editor.mode = INPUT_MODE;
    editor.userInput.length = 0;
    editor.userInput.callback = callback;
    editor.userInput.prefix = prefix;
}

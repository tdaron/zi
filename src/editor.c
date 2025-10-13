#include <editor.h>
#include <string.h>
#include <utf8.h>
#include <io.h>


void editor_init(Editor* editor) {
    vec_init(&editor->modes);
}


void editor_new_view(Buffer* buf) { editor.views[editor.viewsCount++] = (View) { buf, 0, 0 }; }
void editor_set_current_view(int viewId) { editor.currentView = viewId; }
Mode* editor_mode_lookup(char* name, int* idx) {
    for (int i = 0; i < editor.modes.length; i++) {
        if (strcmp(editor.modes.data[i].name, name) == 0) {
            *idx = i;
            return &editor.modes.data[i];
        }
    }
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
    editor.modes.data[editor.current_mode].events_handler(&editor.modes.data[editor.current_mode], ev);
}

void editor_input_mode(String_View prefix, CALLBACK_FN callback)
{
    // editor.mode = INPUT_MODE;
    editor.userInput.length = 0;
    editor.userInput.callback = callback;
    editor.userInput.prefix = prefix;
}

#include <editor.h>
#include <nob.h>
#include <utf8.h>
#include <io.h>

void editor_new_view(Buffer* buf) { editor.views[editor.viewsCount++] = (View) { buf, 0, 0 }; }
void editor_set_current_view(int viewId) { editor.currentView = viewId; }

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
    Buffer* current_buffer = editor.views[editor.currentView].buffer;
    if (ev->type == TG_EV_KEY) {
        if (ev->data.key == TG_KEY_ESC) {
            if (editor.mode == NORMAL_MODE) {
                editor.shouldClose = true;
            } else {

                editor.mode = NORMAL_MODE;
            }
        }
        if (ev->data.key == TG_KEY_BACKSPACE) {
            if (editor.mode == INSERT_MODE) {
                delete_chars(current_buffer, 1);
            }
            if (editor.mode == INPUT_MODE) {
                editor.userInput.length-=utf8_prev_char_len(editor.userInput.input, editor.userInput.length-1);
            }
        }
        if (ev->data.key == TG_KEY_ENTER) {
            if (editor.mode == INPUT_MODE) {
                if (editor.userInput.callback)
                    editor.mode = NORMAL_MODE;
                editor.userInput.callback(sv_from_parts(editor.userInput.input, editor.userInput.length));
            }
            if (editor.mode == INSERT_MODE) {
                insert_char(current_buffer, '\n');
            }
        }
        if (ev->data.key == TG_KEY_CHAR) {
            if (editor.mode == INPUT_MODE) {
                for (int i = 0; i < CHAR_LEN(ev->fullChar[0]); i++) {
                    editor.userInput.input[editor.userInput.length++] = ev->fullChar[i];
                }
            }
            if (editor.mode == INSERT_MODE) {
                insert_char_bytes(current_buffer, ev->fullChar, CHAR_LEN(ev->fullChar[0]));
            }
            if (editor.mode == NORMAL_MODE) {
                if (ev->ch == 'n') {
                    editor_next_buffer();
                    editor.message = "";
                }
                if (ev->ch == 'i')
                    editor.mode = INSERT_MODE;
                if (ev->ch == ':') {
                    editor_input_mode(SV(":"), command_callback);
                }
            }
        }
    }
}

void editor_input_mode(String_View prefix, CALLBACK_FN callback)
{
    NOB_UNUSED(callback);
    editor.mode = INPUT_MODE;
    editor.userInput.length = 0;
    editor.userInput.callback = callback;
    editor.userInput.prefix = prefix;
}

#include <editor.h>
#include <nob.h>

void editor_open_buffer(char* name) { editor.buffers[editor.buffersCount++] = new_buffer(name, 0, 0); }
void editor_set_current_buffer(int buffer) { editor.currentBuffer = buffer; }

void free_editor()
{
    for (int i = 0; i < editor.buffersCount; i++) {
        free_buffer(editor.buffers[i]);
    }
    log_close(editor.logFileHandle);
}
void editor_next_buffer() { editor.currentBuffer = (editor.currentBuffer + 1) % editor.buffersCount; }

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
                delete_chars(editor.buffers[editor.currentBuffer], 1);
            }
            if (editor.mode == INPUT_MODE) {
                editor.userInput.length--;
            }
        }
        if (ev->data.key == TG_KEY_ENTER) {
            if (editor.mode == INPUT_MODE) {
                if (editor.userInput.callback)
                    editor.mode = NORMAL_MODE;
                    editor.userInput.callback(sv_from_parts(editor.userInput.input, editor.userInput.length));
            }
        }
        if (ev->data.key == TG_KEY_CHAR) {
            if (editor.mode == INPUT_MODE) {
                editor.userInput.input[editor.userInput.length++] = ev->ch;
            }
            if (editor.mode == INSERT_MODE) {
                log_print(LOG_INFO, "Inserting %d bytes.", ev->n_bytes);
                for (int i = 0; i < ev->n_bytes; i++) {
                    insert_char(editor.buffers[editor.currentBuffer], ev->fullChar[i]);
                }
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

#include "buffer.h"
#include "termgfx.h"
#include <editor.h>
void editor_open_buffer(char* name) {
    editor.buffers[editor.buffersCount++] = new_buffer(name, 0, 0);
}
void editor_set_current_buffer(int buffer) { editor.currentBuffer = buffer; }

void free_editor()
{
    for (int i = 0; i < editor.buffersCount; i++) {
        free_buffer(editor.buffers[i]);
    }
}
void editor_next_buffer() { editor.currentBuffer = (editor.currentBuffer + 1) % editor.buffersCount; }

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
        }
        if (ev->data.key == TG_KEY_CHAR) {
            if (editor.mode == INSERT_MODE) {
                insert_char(editor.buffers[editor.currentBuffer], ev->ch);
            } else {
                if (ev->ch == 'n') {
                    editor_next_buffer();
                    editor.message = "";
                }
                if (ev->ch == 'i')
                    editor.mode = INSERT_MODE;
                if (ev->ch == ':') {
                    editor.message = ":some-cmd-here";
                    editor.mode = COMMAND_MODE;
                }
            }
        }
    }
}


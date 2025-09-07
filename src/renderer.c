#include "editor.h"
#include <buffer.h>
#include <clay.h>
#include <renderer.h>
#include <termgfx.h>

void draw(Clay_RenderCommandArray renderCommands)
{
    tg_reset();
    tg_clear();
    tg_update_size();
    for (int i = 0; i < renderCommands.length; i++) {
        Clay_RenderCommand* renderCommand = &renderCommands.internalArray[i];
        switch (renderCommand->commandType) {
        case CLAY_RENDER_COMMAND_TYPE_RECTANGLE: {

            Clay_BoundingBox box = renderCommand->boundingBox;
            Clay_Color color = renderCommand->renderData.rectangle.backgroundColor;
            tg_set_bg(color.r, color.g, color.b);
            tg_draw_box(box.x, box.y, box.width, box.height, ' ');
            break;
        }

        case CLAY_RENDER_COMMAND_TYPE_TEXT:
            Clay_Color color = renderCommand->renderData.text.textColor;
            tg_set_fg(color.r, color.g, color.b);
            tg_print_text_with_length(renderCommand->boundingBox.x, renderCommand->boundingBox.y,
                renderCommand->renderData.text.stringContents.chars,
                renderCommand->renderData.text.stringContents.length);
            break;
        case CLAY_RENDER_COMMAND_TYPE_CUSTOM: {
            Clay_BoundingBox box = renderCommand->boundingBox;
            char *slice1, *slice2;
            int length1, length2;
            get_contents(editor.buffers[editor.currentBuffer], &slice1, &length1, &slice2, &length2);
            tg_set_fg(255, 255, 255);
            tg_print_text_with_length(box.x, box.y, slice1, length1);
            // TODO: Proper compute display size of slice1
            tg_print_text_with_length(box.x + length1, box.y, slice2, length2);
            if (editor.mode == INSERT_MODE) {
                tg_set_bg(255, 255, 255);
                tg_print_text(box.x + length1 + length2, box.y, " ");
                tg_reset();
            }

            break;
        }
        default:
            printf("Unsupported command type: %d\n", renderCommand->commandType);
        }
    }

    fflush(stdout);
}

bool render(Clay_RenderCommandArray commands)
{
    draw(commands);
    tg_wait_for_keypress(1000);
    tg_event ev;
    ev = tg_get_event();
    if (ev.type == TG_EV_KEY) {
        if (ev.data.key == TG_KEY_ESC) {
            if (editor.mode == INSERT_MODE) {
                editor.mode = NORMAL_MODE;
            } else {
                return false;
            }
        }
        if (ev.data.key == TG_KEY_BACKSPACE) {
            if (editor.mode == INSERT_MODE) {
                delete_chars(editor.buffers[editor.currentBuffer], 1);
            }
        }
        if (ev.data.key == TG_KEY_CHAR) {
            if (editor.mode == INSERT_MODE) {
                insert_char(editor.buffers[editor.currentBuffer], ev.ch);
            } else {
                if (ev.ch == 'n') {
                    editor_next_buffer(&editor);
                    editor.message = "";
                }
                if (ev.ch == 'q')
                    return false;
                if (ev.ch == 'i')
                    editor.mode = INSERT_MODE;
            }
        }
    }

    // 30 FPS (might adapt this)
    // usleep(16000 * 2);
    return true;
}

void init_renderer()
{
    tg_alt_screen_enable();
    tg_enable_raw();
    tg_set_nonblocking(1);
    tg_cursor_hide();
    tg_install_winch_handler();
    tg_mouse_enable();
}

void quit_renderer()
{
    tg_mouse_disable();
    tg_set_nonblocking(0);
    tg_disable_raw();
    tg_alt_screen_disable();
    tg_cursor_show();
}

int renderer_width() { return tg_width(); }

int renderer_height() { return tg_height(); }

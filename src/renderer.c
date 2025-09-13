#include "editor.h"
#include <buffer.h>
#include <clay.h>
#include <renderer.h>
#include <stdio.h>
#include <termgfx.h>
#include <palette.h>

void draw(Clay_RenderCommandArray renderCommands)
{
    tg_update_size();
    for (int i = 0; i < renderCommands.length; i++) {
        Clay_RenderCommand* renderCommand = &renderCommands.internalArray[i];
        switch (renderCommand->commandType) {
        case CLAY_RENDER_COMMAND_TYPE_RECTANGLE: {
            Clay_BoundingBox box = renderCommand->boundingBox;
            Clay_Color color = renderCommand->renderData.rectangle.backgroundColor;
            tg_set_bg(&color);
            tg_draw_box(box.x, box.y, box.width, box.height, ' ');
            break;
        }

        case CLAY_RENDER_COMMAND_TYPE_TEXT: {
            Clay_Color color = renderCommand->renderData.text.textColor;
            tg_set_bg(TRANSPARENT);
            tg_set_fg(&color);
            tg_print_text_with_length(renderCommand->boundingBox.x, renderCommand->boundingBox.y,
                renderCommand->renderData.text.stringContents.chars,
                renderCommand->renderData.text.stringContents.length);
            break;
        }
        case CLAY_RENDER_COMMAND_TYPE_CUSTOM: {

            Clay_BoundingBox box = renderCommand->boundingBox;
            char *slice1, *slice2;
            int length1, length2;
            get_contents(editor.buffers[editor.currentBuffer], &slice1, &length1, &slice2, &length2);
            tg_set_bg(TRANSPARENT);
            tg_set_fg(&(Clay_Color) { 255, 255, 255, 255 });
            int s1l = tg_print_text_with_length(box.x, box.y, slice1, length1);
            // TODO: Proper compute display size of slice1
            int s2l = tg_print_text_with_length(box.x + length1, box.y, slice2, length2);
            if (editor.mode == INSERT_MODE) {
                tg_set_bg(&(Clay_Color)CURSOR_COLOR);
                tg_print_text(box.x + s1l + s2l, box.y, " ");
            }

            break;
        }
        default:
            printf("Unsupported command type: %d\n", renderCommand->commandType);
        }
    }

    tg_flush();
}

bool render(Clay_RenderCommandArray commands)
{
    draw(commands);
    tg_wait_for_keypress(1000);
    tg_event ev;
    ev = tg_get_event();
    editor_handle_event(&ev);

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

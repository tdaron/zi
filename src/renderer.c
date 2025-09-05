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
            tg_print_text_with_length(renderCommand->boundingBox.x, renderCommand->boundingBox.y,
                renderCommand->renderData.text.stringContents.chars,
                renderCommand->renderData.text.stringContents.length);
            break;
        case CLAY_RENDER_COMMAND_TYPE_CUSTOM: {
            Clay_BoundingBox box = renderCommand->boundingBox;
            tg_draw_box(box.x, box.y, box.width, box.height, '#');
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
    tg_event ev;
    ev = tg_get_event();
    if (ev.type == TG_EV_KEY) {
        if (ev.data.key == TG_KEY_ESC)
            return false;
    }
    draw(commands);

    // 30 FPS (might adapt this)
    usleep(16000*2);
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

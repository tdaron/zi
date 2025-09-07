#include <buffer.h>
#include <editor.h>
#include <layout.h>
#include <renderer.h>

Editor editor;

int main()
{
    editor_open_buffer(&editor, "src/main.c");
    editor_open_buffer(&editor, "src/buffer.c");
    editor_open_buffer(&editor, "src/basic_hello.c");
    init_renderer();
    layout_init(renderer_width(), renderer_height());
    layout_set_dimension(renderer_width(), renderer_height());
    while (render(layout())) {
        layout_set_dimension(renderer_width(), renderer_height());
    }
    quit_renderer();
    free_editor(&editor);
    return 0;
}

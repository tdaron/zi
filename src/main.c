#include <buffer.h>
#include <editor.h>
#include <io.h>
#include <layout.h>
#include <renderer.h>
#define SV_IMPLEMENTATION
#include <locale.h>
#include <sv.h>
#include <generated_plugins.h>

Editor editor;

int main()
{
    setlocale(LC_ALL, "en_US.UTF-8");
    log_init();
    log_print(LOG_INFO, "zi is booting...");

    for (int i = 0; i < PLUGINS_COUNT; i++) {
        pluginsInitFunctions[i]();
    }
    
    init_renderer();
    log_print(LOG_INFO, "zi is set up !");
    editor_open_buffer("src/main.c");
    editor_open_buffer("src/buffer.c");
    editor_open_buffer("src/basic_hello.c");
    layout_init(renderer_width(), renderer_height());
    layout_set_dimension(renderer_width(), renderer_height());
    while (render(layout())) {
        layout_set_dimension(renderer_width(), renderer_height());
        if (editor.shouldClose) {
            break;
        }
    }
    quit_renderer();
    log_print(LOG_INFO, "goodbye !");
    free_editor(&editor);

    return 0;
}

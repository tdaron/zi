#include <buffer.h>
#include <layout.h>
#include <renderer.h>

int main()
{
    init_renderer();
    layout_init(renderer_width(), renderer_height());
    while (render(layout())) {
        layout_set_dimension(renderer_width(), renderer_height());
    }
    quit_renderer();
    return 0;
}

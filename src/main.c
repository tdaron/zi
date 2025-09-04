#include "renderer.h"
#include "buffer.h"

int main() {
  renderer_init();
  Buffer* b = new_buffer("", 0);
  while (renderer_draw(b)) {
    continue;
  }
  return 0;
}

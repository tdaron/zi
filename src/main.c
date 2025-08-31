#include "renderer.h"

int main() {
  initRenderer();
  int running = 1;
  while (running) {
    running = !renderFrame();
  }
  quit();
}

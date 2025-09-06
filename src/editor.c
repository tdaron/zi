#include "buffer.h"
#include <editor.h>
void editor_open_buffer(Editor* e, char* name) {
  e->buffers[e->buffersCount++] = new_buffer(name, 0, 0);
}
void editor_set_current_buffer(Editor* e, int buffer) {
  e->currentBuffer = buffer;
}

void free_editor(Editor *e) {
  for (int i = 0; i < e->buffersCount; i++) {
    free_buffer(e->buffers[i]);
  }
}
void editor_next_buffer(Editor* e) {
  e->currentBuffer = (e->currentBuffer+1) % e->buffersCount;
}

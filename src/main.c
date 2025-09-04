#include "buffer.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
  Buffer* b = new_buffer("defg", 4);
  print_buffer(b);
  insert_string(b, "bc");
  print_buffer(b);
  move_cursor(b, -30);
  insert_string(b, "a");
  print_buffer(b);
  move_cursor(b, 30);
  print_buffer(b);
  insert_string(b, "hijte");
  print_buffer(b);
  delete_chars(b, 2);
  print_buffer(b);
  move_cursor(b, -5);
  print_buffer(b);
  delete_chars(b, 1);
  print_buffer(b);
  char* o = get_raw_content(b);
  printf("RAW: %s\n", o);
  free(o);
  free_buffer(b);
  return 0;
}

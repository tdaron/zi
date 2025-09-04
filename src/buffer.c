#include "buffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define GAP_SIZE 25


Buffer* new_buffer(char* content, int length) {
  Buffer* b = malloc(sizeof(Buffer));
  b->buf = malloc((length + GAP_SIZE) * sizeof(char));
  b->ccur = b->buf;
  b->cend = b->buf + GAP_SIZE;
  b->length = length + GAP_SIZE;
  memcpy(b->cend, content, length * sizeof(char));
  return b;
}

void insert_string(Buffer* b, char* s) {
  int i = 0;
  while (s[i] != '\0') {
    insert_char(b, s[i]);
    i++;
  }
  
}

void move_cursor(Buffer* b, int offset) {
  char* new_ccur = b->ccur + offset;
  char* new_cend = b->cend + offset;
  if (offset > 0) {

    // Preventing overflow
    int max_offset = (b->buf + b->length) - b->cend;
    if (offset > max_offset) {
      new_cend = b->cend + max_offset;
      new_ccur = b->ccur +max_offset;
      offset = max_offset;
    }
    memcpy(b->ccur, b->cend, (size_t)(offset));
  }

  if (offset < 0) {
    // Preventing overflow
    if (new_ccur < b->buf) {
      int max_offset = b->ccur - b->buf;
      new_ccur = b->ccur - max_offset;
      new_cend = b->cend - max_offset;
      offset = -max_offset;
    }

    memcpy(new_cend, new_ccur, (-offset));
  }
  
  b->ccur = new_ccur;
  b->cend = new_cend;
}

void insert_char(Buffer* b, char c) {
  *(b->ccur++) = c;

  // Gap is filled
  if (b->ccur == b->cend) {
    int offset = b->ccur - b->buf;
    int to_cpy = (b->buf+b->length) - b->cend;
    b->buf = realloc(b->buf, b->length + GAP_SIZE);    
    b->ccur = b->buf + offset;
    b->cend = b->ccur + GAP_SIZE;
    memmove(b->cend, b->ccur, to_cpy);
    b->length += GAP_SIZE;
  }
}

void delete_chars(Buffer* b, int n) {
  // Avoid erasing in the infinity lol
  while (n > 0 && b->ccur > b->buf) {
    b->ccur --;
    n--;
  }
}

int content_length(Buffer* b) {
  return b->length - (b->cend - b->ccur);
}

void print_buffer(Buffer* b) {
  printf("%.*s#", (int)(b->ccur - b->buf), b->buf);
  printf("%.*s (%d)\n", (int)((b->buf+b->length) - b->cend), b->cend, content_length(b));
}



void free_buffer(Buffer* b) {
  free(b->buf);
  free(b);
}

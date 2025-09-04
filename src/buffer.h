#ifndef BUFFER_H
#define BUFFER_H

// Gap Buffer data structure
typedef struct {
  char *buf;  // beginning
  char *ccur; // gap beginning
  char *cend; // gap end
  int length; // buf + length is end
} Buffer;

Buffer *new_buffer(char *content, int length);
void print_buffer(Buffer *b);
void insert_char(Buffer *b, char c);
void insert_string(Buffer *b, char *s);
void move_cursor(Buffer *b, int offset);
void delete_chars(Buffer *b, int n);
char *get_raw_content(Buffer *b);
void free_buffer(Buffer *b);
void get_contents(Buffer *b, char **slice1, int *slice1_length, char **slice2,
                  int *slice2_length);

#endif

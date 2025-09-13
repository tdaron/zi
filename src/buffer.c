#include <buffer.h>
#include <editor.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <utf8.h>
#define GAP_SIZE 256

// This implementation follows the Gap Buffer datastructure.

Buffer* new_buffer(char* name, char* content, int length)
{
    Buffer* b = malloc(sizeof(Buffer));
    b->buf = malloc((length + GAP_SIZE) * sizeof(char));
    b->ccur = b->buf;
    b->cend = b->buf + GAP_SIZE;
    b->length = length + GAP_SIZE;
    b->raw = NULL;
    b->name = name;
    memcpy(b->cend, content, length * sizeof(char));
    return b;
}

void insert_string(Buffer* b, char* s)
{
    int i = 0;
    while (s[i] != '\0') {
        insert_char(b, s[i]);
        i++;
    }
}

void move_cursor(Buffer* b, int offset)
{
    // Clamping offset
    if (offset > 0) {
        int max_offset = (b->buf + b->length) - b->cend;
        if (offset > max_offset) {
            offset = max_offset;
        }
    } else if (offset < 0) {
        int max_offset = b->ccur - b->buf;
        if (-offset > max_offset) {
            offset = -max_offset;
        }
    }
    char* new_ccur = b->ccur + offset;
    char* new_cend = b->cend + offset;
    if (offset > 0) {
        memcpy(b->ccur, b->cend, (size_t)(offset));
    }

    if (offset < 0) {
        memcpy(new_cend, new_ccur, (-offset));
    }

    b->ccur = new_ccur;
    b->cend = new_cend;
}

void insert_char(Buffer* b, char c)
{
    *(b->ccur++) = c;

    // Gap is filled
    if (b->ccur == b->cend) {
        log_print(LOG_INFO, "realloc... current size: %d bytes", b->length);
        int offset = b->ccur - b->buf;
        int to_cpy = (b->buf + b->length) - b->cend;
        b->buf = realloc(b->buf, b->length + GAP_SIZE);
        b->ccur = b->buf + offset;
        b->cend = b->ccur + GAP_SIZE;
        memmove(b->cend, b->ccur, to_cpy);
        b->length += GAP_SIZE;
    }
}

void delete_chars(Buffer* b, int n)
{
    // Avoid erasing in the infinity lol
    while (n > 0 && b->ccur > b->buf) {
        int length = utf8_prev_char_len(b->buf, b->ccur - b->buf - 1);
        if (length == -1) break;
        b->ccur -= length;
        n -= 1;
    }
}

int content_length(Buffer* b) { return b->length - (b->cend - b->ccur); }

void print_buffer(Buffer* b)
{
    if (content_length(b) == 0) {
        return;
    }
    printf("%.*s#", (int)(b->ccur - b->buf), b->buf);
    printf("%.*s (%d)\n", (int)((b->buf + b->length) - b->cend), b->cend, content_length(b));
}

char* get_raw_content(Buffer* b)
{
    if (b->raw != NULL)
        free(b->raw);
    b->raw = malloc(content_length(b) * sizeof(char) + 1);
    sprintf(b->raw, "%.*s%.*s", (int)(b->ccur - b->buf), b->buf, (int)((b->buf + b->length) - b->cend), b->cend);
    return b->raw;
}

int get_cursor_position(Buffer* b) { return b->ccur - b->buf; }

void clear(Buffer* b)
{
    b->ccur = b->buf;
    b->cend = b->buf + b->length - 1;
}

void get_contents(Buffer* b, char** slice1, int* slice1_length, char** slice2, int* slice2_length)
{
    *slice1 = b->buf;
    *slice1_length = b->ccur - b->buf;
    *slice2 = b->cend;
    *slice2_length = (b->buf + b->length) - b->cend;
}

void free_buffer(Buffer* b)
{
    free(b->buf);
    if (b->raw != NULL)
        free(b->raw);
    free(b);
}

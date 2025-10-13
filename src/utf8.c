#include <stdio.h>
#include <utf8.h>

// Those providers are providers of char for the read_utf8_char function
// as it can consume a variable number of chars.

// Get chars from stdin (user input)
int getchar_provider(void* _) { (void)_; return getchar(); }

// Get chars from a buffer
int buffer_provider(void* d)
{
    bufferProviderData* data = d;
    return data->buf[data->pos++];
}

int read_utf8_char(char* buf, int (*provider)(void* userData), void* userData)
{
    int c = provider(userData);
    if (c == EOF) {
        return EOF;
    }

    unsigned char b = (unsigned char)c;
    buf[0] = b;

    if (b < 0x80) {
        // 1-byte ASCII
        buf[1] = '\0';
        return 0;
    }

    for (int i = 1; i < CHAR_LEN(b); i++) {
        int next = provider(userData);
        if (next == EOF)
            return EOF;
        unsigned char nb = (unsigned char)next;
        if ((nb & 0xC0) != 0x80)
            return -1; // invalid continuation
        buf[i] = nb;
    }
    return 0;
}

int utf8_prev_char_len(const char* buf, int pos)
{
    if (pos < 0)
        return -1;
    int i = pos;

    // Scan backward until we find a non-continuation byte
    while (i >= 0 && (buf[i] & 0xC0) == 0x80) {
        i--;
    }
    if (i < 0)
        return -1; // invalid UTF-8
    return pos - i + 1; // number of bytes in the previous character
}

typedef struct {
    const char* buf;
    int pos;
} bufferProviderData;

#define CHAR_LEN(b) \
    ((((unsigned char)b) < 0x80) ? 1 : \
    (((b) & 0xE0) == 0xC0) ? 2 : \
    (((b) & 0xF0) == 0xE0) ? 3 : \
    (((b) & 0xF8) == 0xF0) ? 4 : -1)

int getchar_provider(void* _);
// data is bufferProviderData
int buffer_provider(void* data);
int read_utf8_char(char* buf, int (*provider)(void* userData), void* userData);
int utf8_prev_char_len(const char* buf, int lastBytePos);

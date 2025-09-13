typedef struct {
  const char* buf;
  int pos;
} bufferProviderData;


int getchar_provider(void* _);
// data is bufferProviderData
int buffer_provider(void* data);
int read_utf8_char(char* buf, int (*provider)(void* userData), void* userData);

